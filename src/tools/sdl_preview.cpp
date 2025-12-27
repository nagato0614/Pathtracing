#include <SDL3/SDL.h>
#include <SDL3/SDL_test_font.h>
#include <iostream>
#include <vector>
#include <memory>
#include <chrono>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include "linearAlgebra/Vector3.hpp"
#include "material/Material.hpp"
#include "core/Scene.hpp"
#include "object/Sphere.hpp"
#include "structure/BVH.hpp"
#include "film/Film.hpp"
#include "material/Diffuse.hpp"
#include "material/DiffuseLight.hpp"
#include "material/Glass.hpp"
#include "material/Mirror.hpp"
#include "camera/PinholeCamera.hpp"
#include "render/Pathtracing.hpp"
#include "sky/ImageBasedLighting.hpp"

using namespace nagato;

struct SceneConfig {
    std::string name;
    Vector3 eye;
    float fov;
    bool hasSky;
    std::string skyPath;
    std::function<void(BVH*, std::vector<Material*>&)> setup;
};

void setupScene0(BVH* bvh, std::vector<Material*>& materials) {
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

void setupScene1(BVH* bvh, std::vector<Material*>& materials) {
    auto white = new Diffuse(Spectrum(0.8));
    auto glass = new Glass(Spectrum(0.99), 1.5);
    materials.push_back(white);
    materials.push_back(glass);

    bvh->loadObject("../models/floor.obj", "../models/floor.mtl", white);
    bvh->loadObject("../models/low_poly_bunny.obj", "../models/low_poly_bunny.mtl", glass);
}

void setupScene2(BVH* bvh, std::vector<Material*>& materials) {
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

void setupScene3(BVH* bvh, std::vector<Material*>& materials) {
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
    bvh->loadObject("../models/back_ceil_floor_plane.obj", "../models/back_ceil_floor_plane.mtl", white);
    bvh->loadObject("../models/light_plane.obj", "../models/light_plane.mtl", d65);
    bvh->loadObject("../models/low_poly_bunny.obj", "../models/low_poly_bunny.mtl", glass);
}

int main(int argc, char* argv[]) {
    int width = 500;
    int height = 500;
    const int samples = 10000;

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Pathtracing Preview", width, height, SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    Spectrum::initSpectrum();

    std::vector<SceneConfig> scenes = {
        {"Simple", Vector3(0, 0, 2), 45 * M_PI / 180, false, "", setupScene0},
        {"Bunny IBL", Vector3(0, 1.5, 4), 55 * M_PI / 180, true, "../texture/playa.exr", setupScene1},
        {"Spheres IBL", Vector3(0, 1.5, 4), 55 * M_PI / 180, true, "../texture/playa.exr", setupScene2},
        {"Cornell Bunny", Vector3(0, 1.5, 4), 55 * M_PI / 180, false, "", setupScene3}
    };

    int currentSceneIdx = 0;
    auto bvh = std::make_unique<BVH>();
    std::vector<Material*> currentMaterials;
    std::unique_ptr<ImageBasedLighting> currentSky;

    // レンダリングループを制御するためのフラグと同期機構
    std::atomic<bool> quitRender{false};
    std::atomic<int> current_pass{0};
    std::atomic<bool> newDataAvailable{false};
    std::mutex filmMutex;

    // シーンのロード処理（オブジェクトの解放と再構築）
    auto loadScene = [&](int idx) {
        bvh->freeObject();
        for (auto m : currentMaterials) delete m;
        currentMaterials.clear();
        currentSky.reset();

        scenes[idx].setup(bvh.get(), currentMaterials);
        if (scenes[idx].hasSky) {
            currentSky = std::make_unique<ImageBasedLighting>(scenes[idx].skyPath);
            bvh->setSky(currentSky.get());
        }
        bvh->constructBVH();
    };

    loadScene(currentSceneIdx);

    auto eye = scenes[currentSceneIdx].eye;
    auto up = Vector3(0, 1, 0);
    auto fov = scenes[currentSceneIdx].fov;

    std::unique_ptr<PinholeCamera> camera = std::make_unique<PinholeCamera>(eye, up, fov, width, height);
    std::unique_ptr<Film> film = std::make_unique<Film>(width, height);
    std::unique_ptr<Pathtracing> pathtracing = std::make_unique<Pathtracing>(bvh.get(), film.get(), camera.get(), samples);

    std::vector<uint8_t> pixels(width * height * 3);
    std::vector<uint8_t> shared_pixels(width * height * 3);

    // レンダリング用スレッドの関数定義
    auto renderFunc = [&]() {
        while (!quitRender) {
            if (current_pass < samples) {
                // パストレーシングの計算を実行
                pathtracing->render(current_pass);
                current_pass++;
                
                // メインスレッドの負荷を軽減するため、レンダリングスレッド側で
                // Filmからピクセルデータ（RGB）への変換を行う
                int pass = current_pass;
                Film temp_film(width, height);
                for (int i = 0; i < width * height; ++i) {
                    temp_film[i] = (*film)[i] / (float)pass;
                }
                auto rgb_data = temp_film.toRGB();
                
                {
                    // 表示用共有バッファへコピー（排他制御）
                    std::lock_guard<std::mutex> lock(filmMutex);
                    for (int i = 0; i < width * height; ++i) {
                        shared_pixels[i * 3 + 0] = rgb_data[i].r255();
                        shared_pixels[i * 3 + 1] = rgb_data[i].g255();
                        shared_pixels[i * 3 + 2] = rgb_data[i].b255();
                    }
                    newDataAvailable = true;
                }

                // 1パス終了した時点で停止要求があればループを抜ける
                if (quitRender) break;
            } else {
                // すべてのサンプルが終了した場合は待機
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    };

    // レンダリングスレッドの開始
    std::thread renderThread(renderFunc);

    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, width, height);

    enum class AppState {
        Running,
        ResetRequested,
        SceneChangeRequested,
        Quitting
    };

    AppState state = AppState::Running;
    SDL_Event event;
    auto start_time = std::chrono::high_resolution_clock::now();
    double last_pass_time = 0;
    auto last_pass_finish_time = std::chrono::high_resolution_clock::now();

    int nextWidth = width;
    int nextHeight = height;

    while (state != AppState::Quitting) {
        auto frame_start = std::chrono::high_resolution_clock::now();

        // SDLイベントのポーリング（UI操作の処理）
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                state = AppState::Quitting;
            } else if (event.type == SDL_EVENT_WINDOW_RESIZED) {
                // ウィンドウサイズ変更時のリセット
                // ウィンドウサイズが異常な値（ディスプレイサイズ超など）を取得することがあるため、
                // ディスプレイの境界サイズでクランプする
                SDL_Rect displayBounds;
                int windowWidth = event.window.data1;
                int windowHeight = event.window.data2;
                
                if (SDL_GetDisplayBounds(SDL_GetDisplayForWindow(window), &displayBounds)) {
                    windowWidth = std::min(windowWidth, displayBounds.w);
                    windowHeight = std::min(windowHeight, displayBounds.h);
                }
                
                nextWidth = windowWidth;
                nextHeight = windowHeight;
                if (state != AppState::Quitting) {
                    state = AppState::ResetRequested;
                }
            } else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                // シーン切り替えボタンの判定（ボタンを離した時に確定）
                float x = event.button.x;
                float y = event.button.y;
                if (x >= width - 120 && x <= width - 10 && y >= 10 && y <= 10 + (int)scenes.size() * 40) {
                    int clickedIdx = (y - 10) / 40;
                    if (clickedIdx >= 0 && clickedIdx < (int)scenes.size() && clickedIdx != currentSceneIdx) {
                        currentSceneIdx = clickedIdx;
                        if (state != AppState::Quitting) {
                            state = AppState::SceneChangeRequested;
                        }
                    }
                }
            }
        }

        // レンダリングのリセット処理（サイズ変更やシーン切り替え時）
        if (state == AppState::ResetRequested || state == AppState::SceneChangeRequested) {
            bool isSceneChange = (state == AppState::SceneChangeRequested);
            
            // 1. まず実行中のレンダリングスレッドを停止要求し、完全に終了するのを待機する
            quitRender = true;
            if (renderThread.joinable()) {
                renderThread.join();
            }

            // スレッド停止後に安全にサイズを更新する
            width = nextWidth;
            height = nextHeight;

            // 2. スレッド停止後に最新の完了サンプルがあれば反映させる
            if (newDataAvailable) {
                std::lock_guard<std::mutex> lock(filmMutex);
                pixels = shared_pixels;
                newDataAvailable = false;
                SDL_UpdateTexture(texture, nullptr, pixels.data(), width * 3);
            }

            // 3. シーンやフィルム、カメラなどのリソースを安全に変更・再構築する
            if (isSceneChange) {
                loadScene(currentSceneIdx);
                eye = scenes[currentSceneIdx].eye;
                fov = scenes[currentSceneIdx].fov;
            }

            // カメラ、Film、パストレーサーを再構築
            camera = std::make_unique<PinholeCamera>(eye, up, fov, width, height);
            film = std::make_unique<Film>(width, height);
            pathtracing = std::make_unique<Pathtracing>(bvh.get(), film.get(), camera.get(), samples);
            
            // SDLテクスチャとバッファの更新
            if (texture) SDL_DestroyTexture(texture);
            texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, width, height);
            pixels.resize(width * height * 3);
            shared_pixels.resize(width * height * 3);
            current_pass = 0;
            newDataAvailable = false;
            start_time = std::chrono::high_resolution_clock::now();
            
            // 4. すべての準備が整った後で、新しいスレッドを開始する
            quitRender = false;
            state = AppState::Running;
            renderThread = std::thread(renderFunc);
        }

        // 新しいレンダリング結果があれば反映
        if (newDataAvailable) {
            int pass = current_pass;
            if (pass > 0) {
                auto now = std::chrono::high_resolution_clock::now();
                last_pass_time = std::chrono::duration<double>(now - last_pass_finish_time).count();
                last_pass_finish_time = now;

                {
                    // 共有バッファから表示用バッファへコピー
                    std::lock_guard<std::mutex> lock(filmMutex);
                    pixels = shared_pixels;
                    newDataAvailable = false;
                }
                SDL_UpdateTexture(texture, nullptr, pixels.data(), width * 3);
            }
        }

        // 画面描画処理
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, nullptr, nullptr);

        // シーン切り替えボタンの描画
        for (int i = 0; i < (int)scenes.size(); ++i) {
            SDL_FRect rect = {(float)width - 120, 10.0f + i * 40.0f, 110.0f, 30.0f};
            if (i == currentSceneIdx) SDL_SetRenderDrawColor(renderer, 100, 255, 100, 255);
            else SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
            SDL_RenderFillRect(renderer, &rect);
        }

        // ステータス表示エリアの描画
        SDL_FRect statusRect = {(float)width - 250, (float)height - 80, 240.0f, 70.0f};
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_RenderFillRect(renderer, &statusRect);

        // ステータス情報の描画
        auto current_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = current_time - start_time;
        int pass = current_pass;
        double sps = (double)pass / elapsed.count();
        
        std::string line1 = "Pass: " + std::to_string(pass);
        std::string line2 = std::to_string(sps) + " SPS";
        std::string line3 = std::to_string(last_pass_time) + " s/pass";
        std::string line4 = std::to_string(width) + "x" + std::to_string(height);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDLTest_DrawString(renderer, (float)width - 240, (float)height - 75, line1.data());
        SDLTest_DrawString(renderer, (float)width - 240, (float)height - 60, line2.data());
        SDLTest_DrawString(renderer, (float)width - 240, (float)height - 45, line3.data());
        SDLTest_DrawString(renderer, (float)width - 240, (float)height - 30, line4.data());

        SDL_RenderPresent(renderer);

        // 約60fpsを維持するための待機処理
        auto frame_end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> frame_duration = frame_end - frame_start;
        if (frame_duration.count() < 16.666) {
            SDL_Delay(16.666 - frame_duration.count());
        }
    }

    quitRender = true;
    if (renderThread.joinable()) {
        renderThread.join();
    }

    bvh->freeObject();
    for (auto m : currentMaterials) delete m;
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
