#pragma once

namespace BoundingObject
{
  enum Enum {
    kSphere   = 1 << 0,
    kAABB     = 1 << 1,
    kOBB      = 1 << 2,
  };
}

struct Sphere
{
  Sphere() : radius(0), center(0,0,0) {}
  float radius;
  D3DXVECTOR3 center;
};

struct OBB
{
  D3DXVECTOR3 extents[3];
  D3DXVECTOR3 center;
};
/*
struct AABB
{
  D3DXVECTOR3 v0;
  D3DXVECTOR3 v1;
};
*/