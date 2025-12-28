//
// Created by 長井亨 on 2018/07/28.
//

#include "structure/BVH.hpp"
#include <algorithm>
#include <queue>
#include <limits>

namespace nagato
{

BVH::BVH(std::vector<Object *> objects)
{
  this->objects.clear();
  for (Object *o : objects)
  {
    this->objects.push_back(o);
  }
}

float BVH::surfaceArea(const Aabb bbox) { return 2 * (bbox.max - bbox.min).norm(); }

Aabb BVH::mergeAABB(const Aabb a, const Aabb b)
{
  Aabb bbox;

  bbox.min.x = std::min(a.min.x, b.min.x);
  bbox.min.y = std::min(a.min.y, b.min.y);
  bbox.min.z = std::min(a.min.z, b.min.z);
  bbox.max.x = std::max(a.max.x, b.max.x);
  bbox.max.y = std::max(a.max.y, b.max.y);
  bbox.max.z = std::max(a.max.z, b.max.z);

  return bbox;
}

void BVH::constructBVH()
{
  if (objects.size() == 0)
  {
    fprintf(stderr, "[ERROR] オブジェクトがないためBVH構築できません");
    exit(CANNOT_CONSTRUCT_BVH);
  }
  nodeCount = 0;
  constructBVH_internal(objects, 0, nodeCount);
}

void BVH::constructBVH_internal(const std::vector<Object *> &objects, int splitAxis, int nodeIndex)
{
  // オブジェクトが１つだけの場合それを葉する
  if (objects.size() == 1)
  {
    auto *leaf = &nodes[nodeIndex];
    leaf->object = objects.front();
    leaf->bbox = leaf->object->getAABB();
    leaf->left = -1;
    leaf->right = -1;
    nodeCount++;
    return;
  }
  else if (objects.empty())
  {
    std::cerr << "empty" << std::endl;
    exit(EMPTY_OBJECT);
  }

  auto *node = &nodes[nodeIndex];
  node->object = nullptr;
  nodeCount++;

  // 大きなAABBの構築
  Aabb aabb;
  for (auto i : objects)
  {
    aabb = mergeAABB(aabb, i->getAABB());
  }
  node->bbox = aabb;

  std::vector<Object *> sorted_objects = objects;

  // オブジェクトをsplitAxisで指定する軸についてソートする
  std::sort(sorted_objects.begin(),
            sorted_objects.end(),
            [splitAxis](const Object *a, const Object *b)
            {
              auto aCenter = a->getAABB().getCenter();
              auto bCenter = b->getAABB().getCenter();
              return aCenter[splitAxis] < bCenter[splitAxis];
            });

  std::vector<Object *> left(0);
  std::vector<Object *> right(0);

  // オブジェクトの分割
  for (int i = 0; i < sorted_objects.size(); i++)
  {
    if (i < sorted_objects.size() / 2)
      left.push_back(sorted_objects[i]);
    else
      right.push_back(sorted_objects[i]);
  }

  // 子を作成
  node->left = nodeCount;
  constructBVH_internal(left, (splitAxis + 1) % 3, node->left);
  node->right = nodeCount;
  constructBVH_internal(right, (splitAxis + 1) % 3, node->right);
}

void BVH::showBVH()
{
  if (nodeCount == 0)
  {
    printf("empty");
    return;
  }
  for (int i = 0; i < nodeCount; i++)
  {
    printf("%03d : ", i);
    if (nodes[i].object != nullptr)
    {
      std::cout << nodes[i].object->toString() << std::endl;
    }
    else
    {
      printf("[node]\n");
    }
  }
}

std::optional<Hit> BVH::intersect(Ray &ray, float min, float max)
{
  if (nodeCount == 0)
  {
    fprintf(stderr, "ERROR : [BVH] BVHを構築していないかオブジェクトがありません");
    exit(-1);
  }

  std::optional<Hit> closestHit = std::nullopt;
  float closestDistance = max;

  std::vector<int> stack;
  stack.reserve(64);
  stack.push_back(0);

  const Vector3 &origin = ray.getOrigin();
  const Vector3 &direction = ray.getDirection();

  auto intersectBox = [&](const Aabb &box, float tMax, float &tNear) -> bool {
    float tminLocal = min;
    float tmaxLocal = tMax;

    for (int axis = 0; axis < 3; ++axis)
    {
      const float dirComponent = (&direction.x)[axis];
      const float oriComponent = (&origin.x)[axis];
      const float boxMin = (&box.min.x)[axis];
      const float boxMax = (&box.max.x)[axis];

      if (std::abs(dirComponent) < 1e-8f)
      {
        if (oriComponent < boxMin || oriComponent > boxMax)
          return false;
        continue;
      }

      float invDir = 1.0f / dirComponent;
      float t1 = (boxMin - oriComponent) * invDir;
      float t2 = (boxMax - oriComponent) * invDir;
      if (t1 > t2)
        std::swap(t1, t2);

      tminLocal = std::max(tminLocal, t1);
      tmaxLocal = std::min(tmaxLocal, t2);

      if (tminLocal > tmaxLocal)
        return false;
    }

    tNear = tminLocal;
    return tmaxLocal >= min;
  };

  while (!stack.empty())
  {
    int nodeIndex = stack.back();
    stack.pop_back();

    auto *node = &nodes[nodeIndex];
    float boxHitT = std::numeric_limits<float>::max();
    if (!intersectBox(node->bbox, closestDistance, boxHitT))
    {
      continue;
    }

    if (node->object != nullptr)
    {
      auto hit = node->object->intersect(ray, min, closestDistance);
      if (hit && hit->getDistance() >= min && hit->getDistance() <= closestDistance)
      {
        closestDistance = hit->getDistance();
        closestHit = hit;
      }
      continue;
    }

    const int left = node->left;
    const int right = node->right;

    if (left != -1 && right != -1)
    {
      float leftT, rightT;
      bool leftHit = intersectBox(nodes[left].bbox, closestDistance, leftT);
      bool rightHit = intersectBox(nodes[right].bbox, closestDistance, rightT);

      if (leftHit && rightHit)
      {
        if (leftT > rightT)
        {
          stack.push_back(left);
          stack.push_back(right);
        }
        else
        {
          stack.push_back(right);
          stack.push_back(left);
        }
      }
      else
      {
        if (leftHit)
          stack.push_back(left);
        if (rightHit)
          stack.push_back(right);
      }
    }
    else
    {
      if (left != -1)
      {
        float leftT;
        if (intersectBox(nodes[left].bbox, closestDistance, leftT))
          stack.push_back(left);
      }
      if (right != -1)
      {
        float rightT;
        if (intersectBox(nodes[right].bbox, closestDistance, rightT))
          stack.push_back(right);
      }
    }
  }

  return closestHit;
}

int BVH::getNodeCount() { return nodeCount; }

size_t BVH::getMemorySize() { return static_cast<size_t>(sizeof(BVHNode) * nodeCount * 1e-6); }

int BVH::makeNewNode()
{
  if (nodeCount >= BVH_NODE)
  {
    fprintf(stderr, "[ERROR] BVHノードが足りません");
    exit(EMPTY_NODE);
  }
  return nodeCount;
}

BVH::BVH() {}

std::optional<Hit> BVH::testIntersect(Ray &ray, float min, float max)
{
  std::optional<Hit> minh;
  for (int i = 0; i < nodeCount; i++)
  {
    auto *node = &nodes[i];
    if (node->object != nullptr)
    {
      auto h = node->object->intersect(ray, min, max);
      if (!h)
      {
        continue;
      }
      minh = h;
      max = minh->getDistance();
    }
  }
  return minh;
}

BVHNode *BVH::getRoot() const { return root; }

const std::vector<Object *> &BVH::getObjects() const { return objects; }

const BVHNode *BVH::getNodes() const { return nodes; }
} // namespace nagato
