//
// Created by OpenAI Codex.
//

#ifndef PATHTRACING_BIDIRECTIONALPATHTRACING_HPP
#define PATHTRACING_BIDIRECTIONALPATHTRACING_HPP

#include <string>
#include "render/RenderBase.hpp"

namespace nagato
{
class BidirectionalPathtracing : public RenderBase
{
  public:
    BidirectionalPathtracing(
      Scene *scene,
      Film *film,
      Camera *camera,
      int spp,
      int maxDepth = 5,
      int lightDepth = 5);

    void render() override;

    void render(const std::string &outputFilename);

    void render(int current_pass) override;

  private:
    // サブパス上の1頂点の情報を保持する構造体
    struct PathVertex
    {
        Hit hit;                       // ヒット情報（位置・法線など）
        Spectrum throughput;           // ここまでのスループット（寄与）
        const Material *material = nullptr; // 頂点のマテリアル
        Vector3 wo;                    // 事前の入射方向（カメラ側は視線方向）
        bool isEmitter = false;        // 面光源かどうか
        bool isDelta = false;          // 鏡・ガラスなどデルタ分布かどうか
    };

    Spectrum Li(size_t x, size_t y, const std::vector<PathVertex> &lightPath);

    std::vector<PathVertex> generateCameraSubpath(
      size_t x,
      size_t y,
      Spectrum &directLighting,
      Spectrum &emissionContribution);

    std::vector<PathVertex> generateLightSubpath();

    Spectrum connectPaths(
      const std::vector<PathVertex> &cameraPath,
      const std::vector<PathVertex> &lightPath);

    Spectrum connectVertices(const PathVertex &cameraVertex, const PathVertex &lightVertex) const;

    static bool isDeltaSurface(const Material *material);

    static Vector3 sampleCosineHemisphere(const Vector3 &normal);

    static void buildOrthonormalBasis(const Vector3 &n, Vector3 &t, Vector3 &b);

    int spp;
    int maxDepth;
    int lightDepth;
};
} // namespace nagato

#endif // PATHTRACING_BIDIRECTIONALPATHTRACING_HPP
