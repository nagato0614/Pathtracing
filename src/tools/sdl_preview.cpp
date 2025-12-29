#include <SDL3/SDL.h>
#include <SDL3/SDL_test_font.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "camera/PinholeCamera.hpp"
#include "core/Scene.hpp"
#include "film/Film.hpp"
#include "linearAlgebra/Vector3.hpp"
#include "material/Diffuse.hpp"
#include "material/DiffuseLight.hpp"
#include "material/Glass.hpp"
#include "material/Material.hpp"
#include "material/Mirror.hpp"
#include "object/Sphere.hpp"
#include "render/BidirectionalPathtracing.hpp"
#include "render/NormalRenderer.hpp"
#include "render/Pathtracing.hpp"
#include "sky/ImageBasedLighting.hpp"
#include "structure/BVH.hpp"

using namespace nagato;

namespace
{
constexpr float kPi = 3.14159265358979323846f;

struct SceneConfig
{
    std::string name;
    Vector3 eye;
    Vector3 target;
    float fov;
    bool hasSky;
    std::string skyPath;
    std::function<void(BVH &, std::vector<Material *> &)> setup;
};

void setupScene0(BVH &bvh, std::vector<Material *> &materials)
{
  auto white = new Diffuse(Spectrum(0.8f));
  auto red = new Diffuse(Spectrum::rgb2Spectrum({0.8f, 0.2f, 0.2f}));
  auto blue = new Diffuse(Spectrum::rgb2Spectrum({0.2f, 0.2f, 0.8f}));
  auto light = new DiffuseLight(Spectrum(1.0f), 20.0f);
  auto glass = new Glass(Spectrum(0.99f), 1.5f);
  materials.push_back(white);
  materials.push_back(red);
  materials.push_back(blue);
  materials.push_back(light);
  materials.push_back(glass);

  bvh.setObject(new Sphere(Vector3(0, -100.5f, -1), 100, white));
  bvh.setObject(new Sphere(Vector3(0, 102.5f, -1), 100, white));
  bvh.setObject(new Sphere(Vector3(-101, 1, -1), 100, red));
  bvh.setObject(new Sphere(Vector3(101, 1, -1), 100, blue));
  bvh.setObject(new Sphere(Vector3(0, 1, -103), 100, white));
  bvh.setObject(new Sphere(Vector3(0, 10, -1), 7.5f, light));
  bvh.setObject(new Sphere(Vector3(-0.5f, -0.2f, -1), 0.3f, glass));
  bvh.setObject(new Sphere(Vector3(0.5f, -0.2f, -1.5f), 0.3f, white));
}

void setupScene1(BVH &bvh, std::vector<Material *> &materials)
{
  bvh.loadObject("../models/living_room.obj", "../models/living_room.mtl", nullptr, 5);
}

void setupScene2(BVH &bvh, [[maybe_unused]] std::vector<Material *> &materials)
{
  auto mirror = new Mirror(Spectrum(0.99f));
  materials.push_back(mirror);
  bvh.loadObject("../models/low_poly_bunny.obj", "../models/low_poly_bunny.mtl", mirror);
  bvh.loadObject("../models/sponza/sponza.obj", "../models/sponza/sponza.mtl", nullptr);
}

void setupScene3(BVH &bvh, std::vector<Material *> &materials)
{
  auto d65_spd = loadSPDFile("../property/cie_si_d65.csv");
  auto red = new Diffuse(Spectrum("../property/macbeth_15_red.csv"));
  auto blue = new Diffuse(Spectrum("../property/macbeth_13_blue.csv"));
  auto white = new Diffuse(Spectrum("../property/macbeth_22_neutral_5.csv"));
  auto d65 = new DiffuseLight(d65_spd, 10);
  auto glass = new Glass(Spectrum(0.99f), 1.5f);
  materials.push_back(red);
  materials.push_back(blue);
  materials.push_back(white);
  materials.push_back(d65);
  materials.push_back(glass);

  bvh.loadObject("../models/left.obj", "../models/left.mtl", red);
  bvh.loadObject("../models/right.obj", "../models/right.mtl", blue);
  bvh.loadObject(
    "../models/back_ceil_floor_plane.obj", "../models/back_ceil_floor_plane.mtl", white);
  bvh.loadObject("../models/light_plane.obj", "../models/light_plane.mtl", d65);
  bvh.loadObject("../models/teapod.obj", "../models/teapod.mtl", glass, 2.f);
}

std::vector<SceneConfig> createScenes()
{
  return {
    {"Simple", Vector3(0, 0, 2), Vector3(0, 0, 0), 45.0f * kPi / 180.0f, false, "", setupScene0},
    {"living_room",
     Vector3(0, 8, 44),
     Vector3(0, 8, 0),
     55.0f * kPi / 180.0f,
     false,
     "../texture/uffizi-large.exr",
     setupScene1},
    {"Spheres IBL",
     Vector3(-10, 2, 0),
     Vector3(0, 2, 0),
     55.0f * kPi / 180.0f,
     true,
     "../texture/uffizi-large.exr",
     setupScene2},
    {"Cornell Bunny",
     Vector3(0, 5, 14),
     Vector3(0, 0, 0),
     55.0f * kPi / 180.0f,
     false,
     "",
     setupScene3}};
}
} // namespace

class PreviewApp
{
  public:
    int run();

  private:
    enum class AppState
    {
      Running,
      ResetRequested,
      SceneChangeRequested,
      Quitting
    };

    enum class IntegratorType
    {
      PathTracing,
      Bidirectional,
      Normal
    };

    struct RenderParams
    {
        int width = 0;
        int height = 0;
        Vector3 eye = Vector3(0);
        Vector3 target = Vector3(0);
        Vector3 up = Vector3(0, 1, 0);
        float fov = 0;
        int sceneIdx = 0;
        int polygonCount = 0;
        float orbitRadius = 0;
        float orbitTheta = 0;
        float orbitPhi = 0;
        IntegratorType integrator = IntegratorType::PathTracing;
    };

    bool initSDL();
    void mainLoop();
    void managerLoop();
    void renderLoop();
    void cleanup();
    void startManagerThread();
    void stopManagerThread();
    void clearMaterials();
    void loadScene(int idx);
    void rebuildResources(const RenderParams &params, bool sceneChanged);

    void handleEvent(const SDL_Event &event);
    void handleWindowResized(int width, int height);
    void handleSceneSelection(float x, float y);
    void handleKeyDown(const SDL_KeyboardEvent &key);
    void updateTextureIfNeeded();
    void applyLatestPixels();
    void renderFrame(const RenderParams &snapshot);
    void drawSceneButtons(int displayWidth, int displayHeight, int currentSceneIdx);
    void drawStatusPanel(const RenderParams &snapshot, int displayWidth, int displayHeight);
    void drawSaveButton();

    void requestReset();
    void requestSceneChange(int sceneIdx);
    void requestIntegratorChange(IntegratorType type);
    void saveCurrentFrame();

    void commitCameraUpdate(const Vector3 &eye,
                            const Vector3 &target,
                            float orbitRadius,
                            float orbitTheta,
                            float orbitPhi);
    static const char *toString(IntegratorType type);

    int width_ = 500;
    int height_ = 500;
    int displayWidth_ = width_;
    int displayHeight_ = height_;
    const int samples_ = 10000;
    const Vector3 up_ = Vector3(0, 1, 0);

    SDL_Window *window_ = nullptr;
    SDL_Renderer *renderer_ = nullptr;
    SDL_Texture *texture_ = nullptr;

    std::vector<uint8_t> pixels_;
    std::vector<uint8_t> sharedPixels_;
    SDL_FRect saveButtonRect_{0.0f, 0.0f, 0.0f, 0.0f};

    std::unique_ptr<BVH> bvh_ = std::make_unique<BVH>();
    std::vector<Material *> materials_;
    std::unique_ptr<ImageBasedLighting> sky_;
    std::unique_ptr<PinholeCamera> camera_;
    std::unique_ptr<Film> film_;
    std::unique_ptr<RenderBase> integrator_;

    std::vector<SceneConfig> scenes_ = createScenes();

    std::atomic<AppState> appState_{AppState::SceneChangeRequested};
    std::atomic<bool> quitRender_{false};
    std::atomic<int> currentPass_{0};
    std::atomic<bool> newDataAvailable_{false};
    std::atomic<bool> textureUpdateNeeded_{false};
    double lastPassDuration_ = 0.0;
    double totalRenderTime_ = 0.0;

    RenderParams params_;
    std::mutex paramsMutex_;
    std::mutex filmMutex_;
    std::mutex statsMutex_;

    std::thread managerThread_;
};

bool PreviewApp::initSDL()
{
  if (!SDL_Init(SDL_INIT_VIDEO))
  {
    std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
    return false;
  }

  window_ = SDL_CreateWindow("Pathtracing Preview", width_, height_, SDL_WINDOW_RESIZABLE);
  if (!window_)
  {
    std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return false;
  }

  renderer_ = SDL_CreateRenderer(window_, nullptr);
  if (!renderer_)
  {
    std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
    SDL_DestroyWindow(window_);
    window_ = nullptr;
    SDL_Quit();
    return false;
  }

  texture_ = SDL_CreateTexture(
    renderer_, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, width_, height_);
  if (!texture_)
  {
    std::cerr << "SDL_CreateTexture failed: " << SDL_GetError() << std::endl;
    SDL_DestroyRenderer(renderer_);
    SDL_DestroyWindow(window_);
    renderer_ = nullptr;
    window_ = nullptr;
    SDL_Quit();
    return false;
  }

  displayWidth_ = width_;
  displayHeight_ = height_;
  pixels_.resize(static_cast<size_t>(width_) * height_ * 3);
  sharedPixels_ = pixels_;

  return true;
}

void PreviewApp::clearMaterials()
{
  for (auto *mat : materials_)
  {
    delete mat;
  }
  materials_.clear();
}

void PreviewApp::loadScene(int idx)
{
  bvh_->freeObject();
  clearMaterials();
  sky_.reset();

  const auto &config = scenes_[idx];
  config.setup(*bvh_, materials_);

  if (config.hasSky)
  {
    sky_ = std::make_unique<ImageBasedLighting>(config.skyPath);
    bvh_->setSky(sky_.get());
  }

  bvh_->constructBVH();
}

void PreviewApp::rebuildResources(const RenderParams &params, bool sceneChanged)
{
  Vector3 eye = params.eye;
  Vector3 target = params.target;
  float fov = params.fov;
  int width = params.width;
  int height = params.height;

  if (sceneChanged)
  {
    const auto &scene = scenes_[params.sceneIdx];
    eye = scene.eye;
    target = scene.target;
    fov = scene.fov;

    float orbitRadius = std::sqrt((eye - target).norm());
    float orbitTheta = 0.0f;
    float orbitPhi = 0.0f;
    if (orbitRadius > 0.0f)
    {
      orbitTheta = std::acos(std::clamp((eye.y - target.y) / orbitRadius, -1.0f, 1.0f));
      orbitPhi = std::atan2(eye.z - target.z, eye.x - target.x);
    }

    {
      std::lock_guard<std::mutex> lock(paramsMutex_);
      params_.eye = eye;
      params_.target = target;
      params_.fov = fov;
      params_.orbitRadius = orbitRadius;
      params_.orbitTheta = orbitTheta;
      params_.orbitPhi = orbitPhi;
      params_.polygonCount = bvh_->getObjectCount();
    }
  }

  camera_ = std::make_unique<PinholeCamera>(eye, target, up_, fov, width, height);
  film_ = std::make_unique<Film>(width, height);

  switch (params.integrator)
  {
    case IntegratorType::PathTracing:
      integrator_ = std::make_unique<Pathtracing>(bvh_.get(), film_.get(), camera_.get(), samples_);
      break;
    case IntegratorType::Bidirectional:
      integrator_ = std::make_unique<BidirectionalPathtracing>(
        bvh_.get(), film_.get(), camera_.get(), samples_);
      break;
    case IntegratorType::Normal:
      integrator_ = std::make_unique<NormalRenderer>(bvh_.get(), film_.get(), camera_.get());
      break;
  }

  width_ = width;
  height_ = height;

  {
    std::lock_guard<std::mutex> lock(paramsMutex_);
    params_.polygonCount = bvh_->getObjectCount();
  }

  {
    std::lock_guard<std::mutex> lock(filmMutex_);
    sharedPixels_.assign(static_cast<size_t>(width_) * height_ * 3, 0);
    textureUpdateNeeded_.store(true);
  }

  currentPass_.store(0);
  newDataAvailable_.store(false);
  {
    std::lock_guard<std::mutex> lock(statsMutex_);
    lastPassDuration_ = 0.0;
    totalRenderTime_ = 0.0;
  }
}

void PreviewApp::startManagerThread()
{
  managerThread_ = std::thread(&PreviewApp::managerLoop, this);
}

void PreviewApp::stopManagerThread()
{
  appState_.store(AppState::Quitting);
  quitRender_.store(true);
  if (managerThread_.joinable())
  {
    managerThread_.join();
  }
}

void PreviewApp::managerLoop()
{
  std::thread renderThread;
  auto lastPassFinish = std::chrono::high_resolution_clock::now();

  while (appState_.load() != AppState::Quitting)
  {
    AppState state = appState_.load();
    if (state == AppState::ResetRequested || state == AppState::SceneChangeRequested)
    {
      bool sceneChanged = (state == AppState::SceneChangeRequested);

      quitRender_.store(true);
      if (renderThread.joinable())
      {
        renderThread.join();
      }

      RenderParams snapshot;
      {
        std::lock_guard<std::mutex> lock(paramsMutex_);
        snapshot = params_;
      }

      if (sceneChanged)
      {
        loadScene(snapshot.sceneIdx);
      }

      rebuildResources(snapshot, sceneChanged);

      quitRender_.store(false);

      AppState expected = state;
      if (appState_.compare_exchange_strong(expected, AppState::Running))
      {
        renderThread = std::thread(&PreviewApp::renderLoop, this);
      }

      lastPassFinish = std::chrono::high_resolution_clock::now();
    }

    if (newDataAvailable_.load() && appState_.load() == AppState::Running)
    {
      auto now = std::chrono::high_resolution_clock::now();
      double passTime = std::chrono::duration<double>(now - lastPassFinish).count();
      {
        std::lock_guard<std::mutex> lock(statsMutex_);
        lastPassDuration_ = passTime;
        totalRenderTime_ += passTime;
      }
      lastPassFinish = now;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  quitRender_.store(true);
  if (renderThread.joinable())
  {
    renderThread.join();
  }
}

void PreviewApp::renderLoop()
{
  while (!quitRender_.load())
  {
    int passIndex = currentPass_.load();
    if (passIndex < samples_ && integrator_)
    {
      integrator_->render(passIndex);
      int passCount = currentPass_.fetch_add(1) + 1;

      if (dynamic_cast<NormalRenderer *>(integrator_.get()) != nullptr)
      {
        currentPass_.store(samples_);
      }

      Film tempFilm(width_, height_);
      for (int i = 0; i < width_ * height_; ++i)
      {
        tempFilm[i] = (*film_)[i] / static_cast<float>(passCount);
      }
      auto rgb = tempFilm.toRGB();

      std::lock_guard<std::mutex> lock(filmMutex_);
      if (sharedPixels_.size() == static_cast<size_t>(width_) * height_ * 3)
      {
        for (int i = 0; i < width_ * height_; ++i)
        {
          sharedPixels_[i * 3 + 0] = rgb[i].r255();
          sharedPixels_[i * 3 + 1] = rgb[i].g255();
          sharedPixels_[i * 3 + 2] = rgb[i].b255();
        }
        newDataAvailable_.store(true);
      }

      if (quitRender_.load())
      {
        break;
      }
    }
    else
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }
}

void PreviewApp::handleWindowResized(int width, int height)
{
  SDL_Rect displayBounds;
  if (SDL_GetDisplayBounds(SDL_GetDisplayForWindow(window_), &displayBounds))
  {
    width = std::min(width, displayBounds.w);
    height = std::min(height, displayBounds.h);
  }

  {
    std::lock_guard<std::mutex> lock(paramsMutex_);
    params_.width = width;
    params_.height = height;
  }

  requestReset();
}

void PreviewApp::handleSceneSelection(float x, float y)
{
  const float left = saveButtonRect_.x;
  const float right = saveButtonRect_.x + saveButtonRect_.w;
  const float top = saveButtonRect_.y;
  const float bottom = saveButtonRect_.y + saveButtonRect_.h;
  if (x >= left && x <= right && y >= top && y <= bottom)
  {
    saveCurrentFrame();
    return;
  }

  if (x < static_cast<float>(displayWidth_) - 120 || x > static_cast<float>(displayWidth_) - 10)
    return;

  int sceneIdx = static_cast<int>((y - 10.0f) / 40.0f);
  if (sceneIdx < 0 || sceneIdx >= static_cast<int>(scenes_.size()))
    return;

  int currentScene;
  {
    std::lock_guard<std::mutex> lock(paramsMutex_);
    currentScene = params_.sceneIdx;
  }

  if (sceneIdx != currentScene)
  {
    requestSceneChange(sceneIdx);
  }
}

void PreviewApp::commitCameraUpdate(
  const Vector3 &eye, const Vector3 &target, float orbitRadius, float orbitTheta, float orbitPhi)
{
  {
    std::lock_guard<std::mutex> lock(paramsMutex_);
    params_.eye = eye;
    params_.target = target;
    params_.orbitRadius = orbitRadius;
    params_.orbitTheta = orbitTheta;
    params_.orbitPhi = orbitPhi;
  }

  requestReset();
}

void PreviewApp::handleKeyDown(const SDL_KeyboardEvent &key)
{
  RenderParams snapshot;
  {
    std::lock_guard<std::mutex> lock(paramsMutex_);
    snapshot = params_;
  }

  bool cameraMoved = false;
  Vector3 eye = snapshot.eye;
  Vector3 target = snapshot.target;
  float orbitRadius = snapshot.orbitRadius;
  float orbitTheta = snapshot.orbitTheta;
  float orbitPhi = snapshot.orbitPhi;

  const float moveSpeed = 0.5f;
  const float rotateSpeed = 0.05f;

  Vector3 forward = normalize(target - eye);
  Vector3 right = normalize(cross(forward, snapshot.up));

  switch (key.key)
  {
    case SDLK_V:
      requestIntegratorChange(IntegratorType::PathTracing);
      return;
    case SDLK_B:
      requestIntegratorChange(IntegratorType::Bidirectional);
      return;
    case SDLK_N:
      requestIntegratorChange(IntegratorType::Normal);
      return;
    case SDLK_W:
      eye = eye + forward * moveSpeed;
      target = target + forward * moveSpeed;
      cameraMoved = true;
      break;
    case SDLK_S:
      eye = eye - forward * moveSpeed;
      target = target - forward * moveSpeed;
      cameraMoved = true;
      break;
    case SDLK_A:
      eye = eye - right * moveSpeed;
      target = target - right * moveSpeed;
      cameraMoved = true;
      break;
    case SDLK_D:
      eye = eye + right * moveSpeed;
      target = target + right * moveSpeed;
      cameraMoved = true;
      break;
    case SDLK_Q:
      orbitPhi -= rotateSpeed;
      cameraMoved = true;
      break;
    case SDLK_E:
      orbitPhi += rotateSpeed;
      cameraMoved = true;
      break;
    case SDLK_R:
      orbitTheta = std::max(0.1f, orbitTheta - rotateSpeed);
      cameraMoved = true;
      break;
    case SDLK_F:
      orbitTheta = std::min(kPi - 0.1f, orbitTheta + rotateSpeed);
      cameraMoved = true;
      break;
    default:
      break;
  }

  if (!cameraMoved)
    return;

  orbitRadius = std::max(0.1f, orbitRadius);

  Vector3 orbitOffset(orbitRadius * std::sin(orbitTheta) * std::cos(orbitPhi),
                      orbitRadius * std::cos(orbitTheta),
                      orbitRadius * std::sin(orbitTheta) * std::sin(orbitPhi));
  eye = target + orbitOffset;

  commitCameraUpdate(eye, target, orbitRadius, orbitTheta, orbitPhi);
}

void PreviewApp::handleEvent(const SDL_Event &event)
{
  switch (event.type)
  {
    case SDL_EVENT_QUIT:
      appState_.store(AppState::Quitting);
      break;
    case SDL_EVENT_WINDOW_RESIZED:
      handleWindowResized(event.window.data1, event.window.data2);
      break;
    case SDL_EVENT_MOUSE_BUTTON_UP:
      handleSceneSelection(static_cast<float>(event.button.x), static_cast<float>(event.button.y));
      break;
    case SDL_EVENT_KEY_DOWN:
      handleKeyDown(event.key);
      break;
    default:
      break;
  }
}

void PreviewApp::updateTextureIfNeeded()
{
  if (!textureUpdateNeeded_.load())
    return;

  std::lock_guard<std::mutex> lock(filmMutex_);
  if (texture_)
  {
    SDL_DestroyTexture(texture_);
  }
  texture_ = SDL_CreateTexture(
    renderer_, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, width_, height_);
  displayWidth_ = width_;
  displayHeight_ = height_;
  pixels_.assign(static_cast<size_t>(displayWidth_) * displayHeight_ * 3, 0);
  textureUpdateNeeded_.store(false);
}

void PreviewApp::applyLatestPixels()
{
  if (!newDataAvailable_.load())
    return;

  std::lock_guard<std::mutex> lock(filmMutex_);
  if (sharedPixels_.size() == pixels_.size())
  {
    pixels_ = sharedPixels_;
    newDataAvailable_.store(false);
  }

  if (texture_)
  {
    SDL_UpdateTexture(texture_, nullptr, pixels_.data(), displayWidth_ * 3);
  }
}

void PreviewApp::drawSceneButtons(int displayWidth, int displayHeight, int currentSceneIdx)
{
  (void) displayHeight;
  for (int i = 0; i < static_cast<int>(scenes_.size()); ++i)
  {
    SDL_FRect rect = {static_cast<float>(displayWidth) - 120.0f, 10.0f + i * 40.0f, 110.0f, 30.0f};
    if (i == currentSceneIdx)
      SDL_SetRenderDrawColor(renderer_, 100, 255, 100, 255);
    else
      SDL_SetRenderDrawColor(renderer_, 200, 200, 200, 255);
    SDL_RenderFillRect(renderer_, &rect);
    SDLTest_DrawString(renderer_, rect.x + 5.0f, rect.y + 10.0f, scenes_[i].name.c_str());
  }
}

void PreviewApp::drawStatusPanel(const RenderParams &snapshot, int displayWidth, int displayHeight)
{
  SDL_FRect statusRect = {static_cast<float>(displayWidth) - 250.0f,
                          static_cast<float>(displayHeight) - 140.0f,
                          240.0f,
                          135.0f};
  SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 128);
  SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
  SDL_RenderFillRect(renderer_, &statusRect);

  int pass = currentPass_.load();
  double totalRender = 0.0;
  double avgPass = 0.0;
  double sps = 0.0;
  {
    std::lock_guard<std::mutex> lock(statsMutex_);
    totalRender = totalRenderTime_;
    avgPass = (pass > 0) ? (totalRender / static_cast<double>(pass)) : 0.0;
    sps = (totalRender > 0.0) ? (static_cast<double>(pass) / totalRender) : 0.0;
  }

  std::string line1 = "Pass: " + std::to_string(pass);
  std::string line2 = std::to_string(sps).substr(0, 6) + " SPS (Avg)";
  std::string line3 = std::to_string(avgPass).substr(0, 6) + " s/pass (Avg)";
  std::string line4 = std::to_string(displayWidth_) + "x" + std::to_string(displayHeight_);
  std::string line5 = "Polygons: " + std::to_string(snapshot.polygonCount);
  std::string line6 = "Eye: (" + std::to_string(snapshot.eye.x).substr(0, 5) + ", " +
    std::to_string(snapshot.eye.y).substr(0, 5) + ", " +
    std::to_string(snapshot.eye.z).substr(0, 5) + ")";
  std::string line7 = "Target: (" + std::to_string(snapshot.target.x).substr(0, 5) + ", " +
    std::to_string(snapshot.target.y).substr(0, 5) + ", " +
    std::to_string(snapshot.target.z).substr(0, 5) + ")";
  std::string line8 =
    std::string("Integrator: ") + toString(snapshot.integrator) + " [V:PT B:BDPT N:Normal]";

  SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 255);
  SDLTest_DrawString(renderer_, statusRect.x + 10.0f, statusRect.y + 10.0f, line1.c_str());
  SDLTest_DrawString(renderer_, statusRect.x + 10.0f, statusRect.y + 25.0f, line2.c_str());
  SDLTest_DrawString(renderer_, statusRect.x + 10.0f, statusRect.y + 40.0f, line3.c_str());
  SDLTest_DrawString(renderer_, statusRect.x + 10.0f, statusRect.y + 55.0f, line4.c_str());
  SDLTest_DrawString(renderer_, statusRect.x + 10.0f, statusRect.y + 70.0f, line5.c_str());
  SDLTest_DrawString(renderer_, statusRect.x + 10.0f, statusRect.y + 85.0f, line6.c_str());
  SDLTest_DrawString(renderer_, statusRect.x + 10.0f, statusRect.y + 100.0f, line7.c_str());
  SDLTest_DrawString(renderer_, statusRect.x + 10.0f, statusRect.y + 115.0f, line8.c_str());

  SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_NONE);
}

void PreviewApp::drawSaveButton()
{
  saveButtonRect_.w = 120.0f;
  saveButtonRect_.h = 35.0f;
  saveButtonRect_.x = 10.0f;
  saveButtonRect_.y = static_cast<float>(displayHeight_) - saveButtonRect_.h - 10.0f;

  SDL_SetRenderDrawColor(renderer_, 80, 160, 255, 255);
  SDL_RenderFillRect(renderer_, &saveButtonRect_);
  SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
  SDLTest_DrawString(renderer_, saveButtonRect_.x + 20.0f, saveButtonRect_.y + 10.0f, "Save");
}

void PreviewApp::renderFrame(const RenderParams &snapshot)
{
  SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
  SDL_RenderClear(renderer_);

  SDL_RenderTexture(renderer_, texture_, nullptr, nullptr);

  drawSceneButtons(displayWidth_, displayHeight_, snapshot.sceneIdx);
  drawStatusPanel(snapshot, displayWidth_, displayHeight_);
  drawSaveButton();

  SDL_RenderPresent(renderer_);
}

void PreviewApp::requestReset()
{
  AppState current = appState_.load();
  while (current != AppState::Quitting)
  {
    if (current == AppState::SceneChangeRequested)
      return;
    if (appState_.compare_exchange_weak(current, AppState::ResetRequested))
      return;
  }
}

void PreviewApp::requestSceneChange(int sceneIdx)
{
  {
    std::lock_guard<std::mutex> lock(paramsMutex_);
    params_.sceneIdx = sceneIdx;
  }

  AppState current = appState_.load();
  while (current != AppState::Quitting)
  {
    if (appState_.compare_exchange_weak(current, AppState::SceneChangeRequested))
      return;
  }
}

void PreviewApp::requestIntegratorChange(IntegratorType type)
{
  {
    std::lock_guard<std::mutex> lock(paramsMutex_);
    if (params_.integrator == type)
    {
      return;
    }
    params_.integrator = type;
  }

  requestReset();
}

void PreviewApp::saveCurrentFrame()
{
  std::lock_guard<std::mutex> lock(filmMutex_);
  if (!film_)
  {
    return;
  }

  const int passCount = std::max(1, currentPass_.load());
  Film tempFilm(film_->getWidth(), film_->getHeight());
  const size_t pixelCount = film_->getPixelSize();
  for (size_t i = 0; i < pixelCount; ++i)
  {
    tempFilm[i] = (*film_)[i] / static_cast<float>(passCount);
  }

  RenderParams snapshot;
  {
    std::lock_guard<std::mutex> lock(paramsMutex_);
    snapshot = params_;
  }

  std::string prefix;
  switch (snapshot.integrator)
  {
    case IntegratorType::PathTracing:
      prefix = "pt_";
      break;
    case IntegratorType::Bidirectional:
      prefix = "bdpt_";
      break;
    case IntegratorType::Normal:
      prefix = "normal_";
      break;
  }

  std::string filename = prefix + "output.png";
  tempFilm.outputImage(filename);
  std::cout << "Saved current frame to " << filename << std::endl;
}

const char *PreviewApp::toString(IntegratorType type)
{
  switch (type)
  {
    case IntegratorType::PathTracing:
      return "Path Tracing";
    case IntegratorType::Bidirectional:
      return "Bidirectional PT";
    case IntegratorType::Normal:
      return "Normal";
  }
  return "Unknown";
}

void PreviewApp::mainLoop()
{
  SDL_Event event;

  while (appState_.load() != AppState::Quitting)
  {
    auto frameStart = std::chrono::high_resolution_clock::now();

    while (SDL_PollEvent(&event))
    {
      handleEvent(event);
    }

    updateTextureIfNeeded();
    applyLatestPixels();

    RenderParams snapshot;
    {
      std::lock_guard<std::mutex> lock(paramsMutex_);
      snapshot = params_;
    }

    renderFrame(snapshot);

    auto frameEnd = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> frameDuration = frameEnd - frameStart;
    if (frameDuration.count() < 16.666)
    {
      Uint32 delay = static_cast<Uint32>(16.666 - frameDuration.count());
      SDL_Delay(delay);
    }
  }
}

void PreviewApp::cleanup()
{
  stopManagerThread();

  clearMaterials();
  bvh_->freeObject();

  if (texture_)
    SDL_DestroyTexture(texture_);
  if (renderer_)
    SDL_DestroyRenderer(renderer_);
  if (window_)
    SDL_DestroyWindow(window_);

  SDL_Quit();
}

int PreviewApp::run()
{
  if (!initSDL())
    return 1;

  Spectrum::initSpectrum();

  params_.width = width_;
  params_.height = height_;
  params_.up = up_;
  params_.sceneIdx = 0;
  params_.eye = scenes_.front().eye;
  params_.target = scenes_.front().target;
  params_.fov = scenes_.front().fov;
  params_.integrator = IntegratorType::PathTracing;
  params_.orbitRadius = std::sqrt((params_.eye - params_.target).norm());
  if (params_.orbitRadius > 0.0f)
  {
    params_.orbitTheta =
      std::acos(std::clamp((params_.eye.y - params_.target.y) / params_.orbitRadius, -1.0f, 1.0f));
    params_.orbitPhi =
      std::atan2(params_.eye.z - params_.target.z, params_.eye.x - params_.target.x);
  }
  else
  {
    params_.orbitTheta = 0.0f;
    params_.orbitPhi = 0.0f;
  }

  startManagerThread();
  mainLoop();
  cleanup();

  return 0;
}

int main(int argc, char *argv[])
{
  (void) argc;
  (void) argv;

  PreviewApp app;
  return app.run();
}
