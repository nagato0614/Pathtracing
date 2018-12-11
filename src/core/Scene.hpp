//
// Created by 長井亨 on 2018/07/18.
//

#ifndef PATHTRACING_SCENE_HPP
#define PATHTRACING_SCENE_HPP

#include <optional>
#include <vector>
#include "../object/Object.hpp"

namespace nagato {
class Scene {
 public:
  Scene();

  std::vector<Object *> objects;

  virtual std::optional<Hit> intersect(Ray &ray, float tmin, float tmax);

  void setObject(Object *object);

  void loadObject(const std::string &objfilename,
                  const std::string &mtlfilename,
                  Material *m);

  void freeObject();

  int getObjectCount() const;

  Spectrum directLight(Ray &ray, Hit info);



 protected:

  std::vector<Object *> lights;

  int objectCount = 0;
};
}

#endif //PATHTRACING_SCENE_HPP
