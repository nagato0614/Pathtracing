#include <SDL3/SDL.h>
#include <iostream>
#include <vector>
#include <memory>
#include <chrono>
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

using namespace nagato;

int main(int argc, char* argv[]) {
    const int width = 1000;
    const int height = 1000;
    const int samples = 1000;

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Pathtracing Preview", width, height, 0);
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

    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, width, height);
    if (!texture) {
        std::cerr << "SDL_CreateTexture failed: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    Spectrum::initSpectrum();

    // Scene Setup
    Diffuse whiteMaterial(Spectrum(0.8));
    Diffuse redMaterial(Spectrum::rgb2Spectrum({0.8, 0.2, 0.2}));
    Diffuse blueMaterial(Spectrum::rgb2Spectrum({0.2, 0.2, 0.8}));
    DiffuseLight light(Spectrum(1.0), 20.0);
    Glass glass(Spectrum(0.99), 1.5);

    BVH bvh;
    // Simple Cornell Box like scene
    bvh.setObject(new Sphere(Vector3(0, -100.5, -1), 100, &whiteMaterial)); // Floor
    bvh.setObject(new Sphere(Vector3(0, 102.5, -1), 100, &whiteMaterial));  // Ceiling
    bvh.setObject(new Sphere(Vector3(-101, 1, -1), 100, &redMaterial));     // Left wall
    bvh.setObject(new Sphere(Vector3(101, 1, -1), 100, &blueMaterial));    // Right wall
    bvh.setObject(new Sphere(Vector3(0, 1, -103), 100, &whiteMaterial));   // Back wall
    bvh.setObject(new Sphere(Vector3(0, 10, -1), 7.5, &light));            // Light
    
    bvh.setObject(new Sphere(Vector3(-0.5, -0.2, -1), 0.3, &glass));       // Object
    bvh.setObject(new Sphere(Vector3(0.5, -0.2, -1.5), 0.3, &whiteMaterial)); // Object

    bvh.constructBVH();

    Vector3 eye(0, 0, 2);
    Vector3 up(0, 1, 0);
    float fov = 45 * M_PI / 180;
    PinholeCamera camera(eye, up, fov, width, height);
    Film film(width, height);
    Pathtracing pathtracing(&bvh, &film, &camera, samples);

    bool quit = false;
    SDL_Event event;
    int current_pass = 0;

    std::vector<uint8_t> pixels(width * height * 3);

    auto start_time = std::chrono::high_resolution_clock::now();

    while (!quit && current_pass < samples) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                quit = true;
            }
        }

        // Render one pass
        pathtracing.render(current_pass);
        current_pass++;

        // Update texture
        Film temp_film(width, height);
        for (int i = 0; i < width * height; ++i) {
            temp_film[i] = film[i] / (float)current_pass;
        }
        auto rgb_data = temp_film.toRGB();
        for (int i = 0; i < width * height; ++i) {
            pixels[i * 3 + 0] = rgb_data[i].r255();
            pixels[i * 3 + 1] = rgb_data[i].g255();
            pixels[i * 3 + 2] = rgb_data[i].b255();
        }
        SDL_UpdateTexture(texture, nullptr, pixels.data(), width * 3);

        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);

        // Simple title update with progress
        auto current_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = current_time - start_time;
        double sps = (double)current_pass / elapsed.count();

        std::string title = "Pathtracing Preview - Pass: " + std::to_string(current_pass) + "/" + std::to_string(samples) +
                            " - " + std::to_string(sps) + " samples/sec";
        SDL_SetWindowTitle(window, title.c_str());

        // We don't strictly need to cap at 60fps here because rendering a pass
        // likely takes more than 16ms anyway. But if it's faster, it will just run faster.
        // SDL_Delay(16); // Optional: to cap frame rate if rendering is extremely fast
    }

    // Keep the window open after rendering finishes
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                quit = true;
            }
        }
        SDL_Delay(100);
    }

    bvh.freeObject();
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
