#include <SDL3/SDL.h>
#include <SDL3/SDL_test_font.h>
#include <atomic>
#include <chrono>
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
#include "render/Pathtracing.hpp"
#include "sky/ImageBasedLighting.hpp"
#include "structure/BVH.hpp"

using namespace nagato;

struct SceneConfig
{
    std::string name;
    Vector3 eye;
    Vector3 target;
    float fov;
    bool hasSky;
    std::string skyPath;
    std::function<void(BVH *, std::vector<Material *> &)> setup;
};

void setupScene0(BVH *bvh, std::vector<Material *> &materials)
{
  auto white = new Diffuse(Spectrum(0.8));
  auto red = new Diffuse(Spectrum::rgb2Spectrum({0.8, 0.2, 0.2}));
  auto blue = new Diffuse(Spectrum::rgb2Spectrum({0.2, 0.2, 0.8}));
  auto light = new DiffuseLight(Spectrum(1.0), 20.0);
  auto glass = new Glass(Spectrum(0.99), 1.5);
  materials.push_back(white);
  materials.push_back(red);
  materials.push_back(blue);
  materials.push_back(light);
  materials.push_back(glass);

  bvh->setObject(new Sphere(Vector3(0, -100.5, -1), 100, white));
  bvh->setObject(new Sphere(Vector3(0, 102.5, -1), 100, white));
  bvh->setObject(new Sphere(Vector3(-101, 1, -1), 100, red));
  bvh->setObject(new Sphere(Vector3(101, 1, -1), 100, blue));
  bvh->setObject(new Sphere(Vector3(0, 1, -103), 100, white));
  bvh->setObject(new Sphere(Vector3(0, 10, -1), 7.5, light));
  bvh->setObject(new Sphere(Vector3(-0.5, -0.2, -1), 0.3, glass));
  bvh->setObject(new Sphere(Vector3(0.5, -0.2, -1.5), 0.3, white));
}

void setupScene1(BVH *bvh, std::vector<Material *> &materials)
{
  auto white = new Diffuse(Spectrum(0.8));
  auto glass = new Glass(Spectrum(0.99), 1.5);
  materials.push_back(white);
  materials.push_back(glass);

  bvh->loadObject("../models/floor.obj", "../models/floor.mtl", white);
  bvh->loadObject("../models/low_poly_bunny.obj", "../models/low_poly_bunny.mtl", glass);
}

void setupScene2(BVH *bvh, std::vector<Material *> &materials)
{
  auto white = new Diffuse(Spectrum(0.8));
  auto purple = new Diffuse(Spectrum::rgb2Spectrum({0.8, 0.2, 0.8}));
  auto glass = new Glass(Spectrum(0.99), 1.5);
  materials.push_back(white);
  materials.push_back(purple);
  materials.push_back(glass);

  bvh->setObject(new Sphere{Vector3(-2, 2, -1), 1.1, purple});
  bvh->setObject(new Sphere{Vector3(2, 2, -1), 1.5, glass});
  bvh->loadObject("../models/floor.obj", "../models/floor.mtl", white);
}

void setupScene3(BVH *bvh, std::vector<Material *> &materials)
{
  auto d65_spd = loadSPDFile("../property/cie_si_d65.csv");
  auto red = new Diffuse(Spectrum("../property/macbeth_15_red.csv"));
  auto blue = new Diffuse(Spectrum("../property/macbeth_13_blue.csv"));
  auto white = new Diffuse(Spectrum("../property/macbeth_22_neutral_5.csv"));
  auto d65 = new DiffuseLight(d65_spd, 10);
  auto glass = new Glass(Spectrum(0.99), 1.5);
  materials.push_back(red);
  materials.push_back(blue);
  materials.push_back(white);
  materials.push_back(d65);
  materials.push_back(glass);

  bvh->loadObject("../models/left.obj", "../models/left.mtl", red);
  bvh->loadObject("../models/right.obj", "../models/right.mtl", blue);
  bvh->loadObject(
    "../models/back_ceil_floor_plane.obj", "../models/back_ceil_floor_plane.mtl", white);
  bvh->loadObject("../models/light_plane.obj", "../models/light_plane.mtl", d65);
  bvh->loadObject("../models/low_poly_bunny.obj", "../models/low_poly_bunny.mtl", glass);
}

int main(int argc, char *argv[])
{
  int width = 500;
  int height = 500;
  const int samples = 10000;

  if (!SDL_Init(SDL_INIT_VIDEO))
  {
    std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
    return 1;
  }

  SDL_Window *window = SDL_CreateWindow("Pathtracing Preview", width, height, SDL_WINDOW_RESIZABLE);
  if (!window)
  {
    std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return 1;
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);
  if (!renderer)
  {
    std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  Spectrum::initSpectrum();

  std::vector<SceneConfig> scenes = {
    {"Simple", Vector3(0, 0, 2), Vector3(0, 0, 0), 45 * M_PI / 180, false, "", setupScene0},
    {"Bunny IBL",
     Vector3(0, 5, 14),
     Vector3(0, 0, 0),
     55 * M_PI / 180,
     true,
     "../texture/playa.exr",
     setupScene1},
    {"Spheres IBL",
     Vector3(0, 5, 14),
     Vector3(0, 0, 0),
     55 * M_PI / 180,
     true,
     "../texture/playa.exr",
     setupScene2},
    {"Cornell Bunny",
     Vector3(0, 5, 14),
     Vector3(0, 0, 0),
     55 * M_PI / 180,
     false,
     "",
     setupScene3}};

  int currentSceneIdx = 0;
  auto bvh = std::make_unique<BVH>();
  std::vector<Material *> currentMaterials;
  std::unique_ptr<ImageBasedLighting> currentSky;

  // レンダリングループを制御するためのフラグと同期機構
  std::atomic<bool> quitRender{false};
  std::atomic<int> current_pass{0};
  std::atomic<bool> newDataAvailable{false};
  std::mutex filmMutex;

  // シーンのロード処理（オブジェクトの解放と再構築）
  auto loadScene = [&](int idx)
  {
    bvh->freeObject();
    for (auto m : currentMaterials)
      delete m;
    currentMaterials.clear();
    currentSky.reset();

    scenes[idx].setup(bvh.get(), currentMaterials);
    if (scenes[idx].hasSky)
    {
      currentSky = std::make_unique<ImageBasedLighting>(scenes[idx].skyPath);
      bvh->setSky(currentSky.get());
    }
    bvh->constructBVH();
  };

  loadScene(currentSceneIdx);

  auto eye = scenes[currentSceneIdx].eye;
  auto target = scenes[currentSceneIdx].target;
  auto up = Vector3(0, 1, 0);
  auto fov = scenes[currentSceneIdx].fov;

  // カメラ回転制御用
  float orbitRadius = (eye - target).norm();
  float orbitTheta =
    std::acos(std::clamp((eye.y - target.y) / orbitRadius, -1.0f, 1.0f)); // ピッチ (0 to PI)
  float orbitPhi = std::atan2(eye.z - target.z, eye.x - target.x); // ヨー (-PI to PI)

  std::unique_ptr<PinholeCamera> camera =
    std::make_unique<PinholeCamera>(eye, target, up, fov, width, height);
  std::unique_ptr<Film> film = std::make_unique<Film>(width, height);
  std::unique_ptr<Pathtracing> pathtracing =
    std::make_unique<Pathtracing>(bvh.get(), film.get(), camera.get(), samples);

  std::vector<uint8_t> pixels(width * height * 3);
  std::vector<uint8_t> shared_pixels(width * height * 3);

  enum class AppState
  {
    Running,
    ResetRequested,
    SceneChangeRequested,
    Quitting
  };

  std::atomic<AppState> appState{AppState::SceneChangeRequested};
  std::atomic<bool> textureUpdateNeeded{false};

  // カメラ位置や設定を管理スレッドへ渡すための共有変数
  struct RenderParams
  {
      int width, height;
      Vector3 eye, target, up;
      float fov;
      int sceneIdx;
      int polygonCount;
      float orbitRadius, orbitTheta, orbitPhi;
  };
  RenderParams sharedParams = {width,
                               height,
                               eye,
                               target,
                               up,
                               fov,
                               currentSceneIdx,
                               bvh->getObjectCount(),
                               orbitRadius,
                               orbitTheta,
                               orbitPhi};
  std::mutex paramsMutex;

  // 統計情報の同期用
  std::atomic<double> shared_last_pass_time{0};
  std::chrono::high_resolution_clock::time_point shared_start_time =
    std::chrono::high_resolution_clock::now();
  std::mutex timeMutex;

  // レンダリング用スレッドの関数定義
  auto renderFunc = [&]()
  {
    while (!quitRender)
    {
      if (current_pass < samples)
      {
        // パストレーシングの計算を実行
        pathtracing->render(current_pass);
        current_pass++;

        // メインスレッドの負荷を軽減するため、レンダリングスレッド側で
        // Filmからピクセルデータ（RGB）への変換を行う
        int pass = current_pass;
        Film temp_film(width, height);
        for (int i = 0; i < width * height; ++i)
        {
          temp_film[i] = (*film)[i] / (float) pass;
        }
        auto rgb_data = temp_film.toRGB();

        {
          // 表示用共有バッファへコピー（排他制御）
          std::lock_guard<std::mutex> lock(filmMutex);
          if (shared_pixels.size() == (size_t) width * height * 3)
          {
            for (int i = 0; i < width * height; ++i)
            {
              shared_pixels[i * 3 + 0] = rgb_data[i].r255();
              shared_pixels[i * 3 + 1] = rgb_data[i].g255();
              shared_pixels[i * 3 + 2] = rgb_data[i].b255();
            }
            newDataAvailable = true;
          }
        }

        // 1パス終了した時点で停止要求があればループを抜ける
        if (quitRender)
          break;
      }
      else
      {
        // すべてのサンプルが終了した場合は待機
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }
    }
  };

  // 管理スレッド: レンダリングスレッドのライフサイクルを管理する
  auto managerFunc = [&]()
  {
    std::thread renderThread;
    auto last_pass_finish_time = std::chrono::high_resolution_clock::now();

    while (appState != AppState::Quitting)
    {
      AppState currentState = appState.load();

      if (currentState == AppState::ResetRequested ||
          currentState == AppState::SceneChangeRequested)
      {
        bool isSceneChange = (currentState == AppState::SceneChangeRequested);

        // 実行中のスレッドがあれば停止を待機
        quitRender = true;
        if (renderThread.joinable())
        {
          renderThread.join();
        }

        // パラメータの取得
        RenderParams p;
        {
          std::lock_guard<std::mutex> lock(paramsMutex);
          p = sharedParams;
        }

        // 管理スレッド内部の作業用変数を更新
        int localWidth = p.width;
        int localHeight = p.height;
        Vector3 localEye = p.eye;
        Vector3 localTarget = p.target;
        float localFov = p.fov;

        if (isSceneChange)
        {
          loadScene(p.sceneIdx);

          // 新しいシーンの初期カメラ設定を反映
          localEye = scenes[p.sceneIdx].eye;
          localTarget = scenes[p.sceneIdx].target;
          localFov = scenes[p.sceneIdx].fov;

          // カメラ回転制御用のパラメータをリセット
          float localOrbitRadius = (localEye - localTarget).norm();
          float localOrbitTheta =
            std::acos(std::clamp((localEye.y - localTarget.y) / localOrbitRadius, -1.0f, 1.0f));
          float localOrbitPhi = std::atan2(localEye.z - localTarget.z, localEye.x - localTarget.x);

          // メインスレッドと共有するために sharedParams を更新
          {
            std::lock_guard<std::mutex> lock(paramsMutex);
            sharedParams.polygonCount = bvh->getObjectCount();
            sharedParams.eye = localEye;
            sharedParams.target = localTarget;
            sharedParams.fov = localFov;
            sharedParams.orbitRadius = localOrbitRadius;
            sharedParams.orbitTheta = localOrbitTheta;
            sharedParams.orbitPhi = localOrbitPhi;
            sharedParams.sceneIdx = p.sceneIdx;
          }
        }

        // リソースの再構築
        camera = std::make_unique<PinholeCamera>(
          localEye, localTarget, up, localFov, localWidth, localHeight);
        film = std::make_unique<Film>(localWidth, localHeight);
        pathtracing = std::make_unique<Pathtracing>(bvh.get(), film.get(), camera.get(), samples);

        {
          std::lock_guard<std::mutex> lock(filmMutex);
          shared_pixels.resize((size_t) localWidth * localHeight * 3);
          textureUpdateNeeded = true;
        }

        // メインスレッド側で使用する解像度変数を更新
        width = localWidth;
        height = localHeight;

        current_pass = 0;
        newDataAvailable = false;
        {
          std::lock_guard<std::mutex> lock(timeMutex);
          shared_start_time = std::chrono::high_resolution_clock::now();
        }
        last_pass_finish_time = std::chrono::high_resolution_clock::now();

        quitRender = false;
        appState = AppState::Running;
        renderThread = std::thread(renderFunc);
      }

      if (newDataAvailable && appState == AppState::Running)
      {
        auto now = std::chrono::high_resolution_clock::now();
        shared_last_pass_time = std::chrono::duration<double>(now - last_pass_finish_time).count();
        last_pass_finish_time = now;
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    quitRender = true;
    if (renderThread.joinable())
    {
      renderThread.join();
    }
  };

  std::thread managerThread(managerFunc);

  SDL_Texture *texture =
    SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, width, height);
  int displayWidth = width;
  int displayHeight = height;

  SDL_Event event;
  double last_pass_time_display = 0;

  // マウス操作状態の管理
  bool isMouseDown = false;
  float lastMouseX = 0;
  float lastMouseY = 0;

  while (appState != AppState::Quitting)
  {
    auto frame_start = std::chrono::high_resolution_clock::now();

    // 管理スレッドがシーン切り替えなどでパラメータを更新した可能性があるため同期
    if (appState == AppState::Running)
    {
      std::lock_guard<std::mutex> lock(paramsMutex);
      currentSceneIdx = sharedParams.sceneIdx;
      eye = sharedParams.eye;
      target = sharedParams.target;
      fov = sharedParams.fov;
      orbitRadius = sharedParams.orbitRadius;
      orbitTheta = sharedParams.orbitTheta;
      orbitPhi = sharedParams.orbitPhi;
    }

    // SDLイベントのポーリング（UI操作の処理）
    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_EVENT_QUIT)
      {
        appState = AppState::Quitting;
      }
      else if (event.type == SDL_EVENT_WINDOW_RESIZED)
      {
        SDL_Rect displayBounds;
        int windowWidth = event.window.data1;
        int windowHeight = event.window.data2;

        if (SDL_GetDisplayBounds(SDL_GetDisplayForWindow(window), &displayBounds))
        {
          windowWidth = std::min(windowWidth, displayBounds.w);
          windowHeight = std::min(windowHeight, displayBounds.h);
        }

        {
          std::lock_guard<std::mutex> lock(paramsMutex);
          sharedParams.width = windowWidth;
          sharedParams.height = windowHeight;
        }
        if (appState != AppState::Quitting)
        {
          appState = AppState::ResetRequested;
        }
      }
      else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
      {
        if (event.button.button == SDL_BUTTON_LEFT)
        {
          isMouseDown = true;
          lastMouseX = event.button.x;
          lastMouseY = event.button.y;
        }
      }
      else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP)
      {
        if (event.button.button == SDL_BUTTON_LEFT)
        {
          isMouseDown = false;
        }
        float x = event.button.x;
        float y = event.button.y;
        if (x >= (float) displayWidth - 120 && x <= (float) displayWidth - 10 && y >= 10 &&
            y <= 10 + (int) scenes.size() * 40)
        {
          int clickedIdx = (y - 10) / 40;
          if (clickedIdx >= 0 && clickedIdx < (int) scenes.size() && clickedIdx != currentSceneIdx)
          {
            {
              std::lock_guard<std::mutex> lock(paramsMutex);
              sharedParams.sceneIdx = clickedIdx;
            }
            if (appState != AppState::Quitting)
            {
              appState = AppState::SceneChangeRequested;
            }
          }
        }
      }
      else if (event.type == SDL_EVENT_MOUSE_MOTION)
      {
        if (isMouseDown)
        {
          float dx = event.motion.x - lastMouseX;
          float dy = event.motion.y - lastMouseY;
          lastMouseX = event.motion.x;
          lastMouseY = event.motion.y;

          orbitPhi -= dx * 0.01f;
          orbitTheta -= dy * 0.01f;
          orbitTheta = std::clamp(orbitTheta, 0.01f, (float) M_PI - 0.01f);

          eye.x = target.x + orbitRadius * std::sin(orbitTheta) * std::cos(orbitPhi);
          eye.y = target.y + orbitRadius * std::cos(orbitTheta);
          eye.z = target.z + orbitRadius * std::sin(orbitTheta) * std::sin(orbitPhi);

          {
            std::lock_guard<std::mutex> lock(paramsMutex);
            sharedParams.eye = eye;
            sharedParams.orbitTheta = orbitTheta;
            sharedParams.orbitPhi = orbitPhi;
          }
          if (appState != AppState::Quitting)
          {
            appState = AppState::ResetRequested;
          }
        }
      }
      else if (event.type == SDL_EVENT_MOUSE_WHEEL)
      {
        float scrollAmount = event.wheel.y;
        // SDL3ではyがfloatで、慣性スクロールなどで非常に小さい値が来ることがある
        if (std::abs(scrollAmount) > 0.01f)
        {
          orbitRadius -= scrollAmount * 0.5f;
          orbitRadius = std::max(0.1f, orbitRadius);

          eye.x = target.x + orbitRadius * std::sin(orbitTheta) * std::cos(orbitPhi);
          eye.y = target.y + orbitRadius * std::cos(orbitTheta);
          eye.z = target.z + orbitRadius * std::sin(orbitTheta) * std::sin(orbitPhi);

          {
            std::lock_guard<std::mutex> lock(paramsMutex);
            sharedParams.eye = eye;
            sharedParams.orbitRadius = orbitRadius;
          }
          if (appState != AppState::Quitting)
          {
            appState = AppState::ResetRequested;
          }
        }
      }
    }

    if (textureUpdateNeeded)
    {
      std::lock_guard<std::mutex> lock(filmMutex);
      if (texture)
        SDL_DestroyTexture(texture);
      texture = SDL_CreateTexture(
        renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, width, height);
      displayWidth = width;
      displayHeight = height;
      pixels.resize((size_t) displayWidth * displayHeight * 3);
      textureUpdateNeeded = false;
    }

    // 新しいレンダリング結果があれば反映
    if (newDataAvailable)
    {
      {
        std::lock_guard<std::mutex> lock(filmMutex);
        if (pixels.size() == shared_pixels.size())
        {
          pixels = shared_pixels;
          newDataAvailable = false;
        }
      }
      last_pass_time_display = shared_last_pass_time.load();
      SDL_UpdateTexture(texture, nullptr, pixels.data(), displayWidth * 3);
    }

    // 画面描画処理
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, nullptr, nullptr);

    // シーン切り替えボタンの描画
    for (int i = 0; i < (int) scenes.size(); ++i)
    {
      SDL_FRect rect = {(float) displayWidth - 120, 10.0f + i * 40.0f, 110.0f, 30.0f};
      if (i == currentSceneIdx)
        SDL_SetRenderDrawColor(renderer, 100, 255, 100, 255);
      else
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
      SDL_RenderFillRect(renderer, &rect);
    }

    // ステータス表示エリアの描画
    SDL_FRect statusRect = {(float) displayWidth - 250, (float) displayHeight - 95, 240.0f, 85.0f};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderFillRect(renderer, &statusRect);

    // ステータス情報の描画
    auto current_time = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point s_time;
    int polygon_count_display = 0;
    {
      std::lock_guard<std::mutex> lock(timeMutex);
      s_time = shared_start_time;
    }
    {
      std::lock_guard<std::mutex> lock(paramsMutex);
      polygon_count_display = sharedParams.polygonCount;
    }
    std::chrono::duration<double> elapsed = current_time - s_time;
    int pass = current_pass;
    double sps = (double) pass / (elapsed.count() > 0 ? elapsed.count() : 1.0);

    std::string line1 = "Pass: " + std::to_string(pass);
    std::string line2 = std::to_string(sps) + " SPS";
    std::string line3 = std::to_string(last_pass_time_display) + " s/pass";
    std::string line4 = std::to_string(displayWidth) + "x" + std::to_string(displayHeight);
    std::string line5 = "Polygons: " + std::to_string(polygon_count_display);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDLTest_DrawString(
      renderer, (float) displayWidth - 240, (float) displayHeight - 90, line1.data());
    SDLTest_DrawString(
      renderer, (float) displayWidth - 240, (float) displayHeight - 75, line2.data());
    SDLTest_DrawString(
      renderer, (float) displayWidth - 240, (float) displayHeight - 60, line3.data());
    SDLTest_DrawString(
      renderer, (float) displayWidth - 240, (float) displayHeight - 45, line4.data());
    SDLTest_DrawString(
      renderer, (float) displayWidth - 240, (float) displayHeight - 30, line5.data());

    SDL_RenderPresent(renderer);

    // 約60fpsを維持するための待機処理
    auto frame_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> frame_duration = frame_end - frame_start;
    if (frame_duration.count() < 16.666)
    {
      SDL_Delay(16.666 - frame_duration.count());
    }
  }

  appState = AppState::Quitting;
  if (managerThread.joinable())
  {
    managerThread.join();
  }

  bvh->freeObject();
  for (auto m : currentMaterials)
    delete m;
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
