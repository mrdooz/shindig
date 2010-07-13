#pragma once

class Mesh;

class Geometry
{
public:
  Geometry();
  ~Geometry();

  Mesh *mesh() { return _mesh.get(); }
  const Mesh *mesh() const { return _mesh.get(); }

  const Sphere& bounding_sphere() const { return _sphere; }

private:
  friend class ObjLoader;

  void debug_draw(DebugDraw *d);

  D3DXMATRIX _orientation;
  boost::scoped_ptr<Mesh> _mesh;
  Sphere _sphere;
  string2 _name;
};