//
// Created by 長井亨 on 2018/07/23.
//

#ifndef PATHTRACING_MATERIAL_HPP
#define PATHTRACING_MATERIAL_HPP

#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "BSDF/BSDF.hpp"
#include "color/Spectrum.hpp"
#include "core/SurfaceType.hpp"
#include "linearAlgebra/Vector2.hpp"

namespace nagato
{
class BSDF;
class Hit;

class Material
{
  public:
    Material(SurfaceType t, Spectrum c, Spectrum e = Spectrum(), float emitterL = 1.0);

    ~Material();

    BSDF &getBSDF() const;

    SurfaceType type() const;

    std::string typeName();

    const Spectrum &getColor() const;

    const Spectrum &getEmitter() const;

    void setDiffuseTexture(const std::string &path);

    bool hasDiffuseTexture() const;

    const std::string &getDiffuseTexture() const;

    Spectrum evaluateColor(const Hit *hit = nullptr) const;

    Spectrum getTextureColor(const Hit *hit = nullptr) const;

  protected:
    struct TextureImage
    {
        int width = 0;
        int height = 0;
        std::vector<Spectrum> texels;
    };

    void loadDiffuseTexture(const std::string &path);

    Spectrum sampleTexture(const Vector2 &uv) const;

    Spectrum color;
    Spectrum emitter;
    std::shared_ptr<BSDF> bsdf;
    SurfaceType surfaceType;
    std::optional<std::string> diffuseTexturePath;
    std::unique_ptr<TextureImage> diffuseTextureImage;
};
} // namespace nagato
#endif // PATHTRACING_MATERIAL_HPP
