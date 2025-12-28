//
// Created by 長井亨 on 2018/07/18.
//

#ifndef PATHTRACING_SCENE_HPP
#define PATHTRACING_SCENE_HPP

#include <optional>
#include <vector>
#include "object/Object.hpp"
#include "sky/Sky.hpp"

namespace nagato
{
class Scene
{
  public:
    Scene();
    virtual ~Scene() = default;

    std::vector<Object *> objects;

    virtual std::optional<Hit> intersect(Ray &ray, float tmin, float tmax);

    void setObject(Object *object);

    void loadObject(
      const std::string &objfilename,
      const std::string &mtlfilename,
      Material *m,
      float scale = 1.0f);

    void freeObject();

    int getObjectCount() const;

    Spectrum directLight(Ray &ray, Hit info);

    /**
     * レイとオブジェクトの衝突が発生しない場合空の輝度を取得する
     * skyは唯一つだけ設定できる
     * @param sky
     */
    void setSky(Sky *sky);

    const Sky &getSky();

    bool hasSky();

  protected:
    std::vector<Object *> lights;

    Sky *sky = nullptr;

    int objectCount = 0;
};
} // namespace nagato

#endif // PATHTRACING_SCENE_HPP
