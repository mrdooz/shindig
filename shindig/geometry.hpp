#pragma once
#include "bounding.hpp"

class Mesh2;
struct DebugDraw;

class Geometry
{
public:
  Geometry();
  ~Geometry();

  Mesh2 *mesh() { return _mesh.get(); }
  const Mesh2 *mesh() const { return _mesh.get(); }

  const Sphere& bounding_sphere() const { return _sphere; }

private:
  friend class ObjLoader;

  void debug_draw(DebugDraw *d);

  D3DXMATRIX _orientation;
  boost::scoped_ptr<Mesh2> _mesh;
  Sphere _sphere;
  string2 _name;
};
