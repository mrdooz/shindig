#pragma once

class Mesh2;

class ObjLoader
{
public:
  bool load_from_file(const char *filename, Mesh2 *mesh);
private:
  struct Face
  {
    Face(int a, int b, int c) : a(a), b(b), c(c) {}
    int a, b, c;
  };

};
