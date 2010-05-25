#include "stdafx.h"
#include "test_effect3.hpp"
#include "redux_loader.hpp"
#include "system.hpp"
#include "scene.hpp"
#include "mesh.hpp"
#include "dynamic_vb.hpp"
#include <celsus/text_scanner.hpp>
#include <celsus/D3D11Descriptions.hpp>

// wavefront obj loader



TestEffect3::TestEffect3()
{
}

TestEffect3::~TestEffect3()
{
}

bool TestEffect3::init()
{
  using namespace fastdelegate;
  using namespace rt;

  System& s = System::instance();

  s.add_file_changed(s.convert_path("sculptris/blob1.obj", System::kDirDropBox), MakeDelegate(this, &TestEffect3::load_mesh), true);

  return true;
}

bool TestEffect3::close()
{
  return true;
}


bool TestEffect3::render()
{
  return true;
}

struct Tri
{
	Tri(int a, int b, int c) : a(a), b(b), c(c) {}
	int a, b, c;
};

bool TestEffect3::load_mesh(const std::string& filename)
{
	TextScanner scanner;
	if (!scanner.load(filename.c_str()))
		return false;


	std::vector<D3DXVECTOR3> verts;
	std::vector<Tri> tris;
	std::set<int> invalid_verts;
	std::set<int> invalid_tris;

	verts.reserve(10000);
	tris.reserve(10000);

	int tri_idx = 0;
	int vert_idx = 0;

	while (true)
	{
		char ch;
		if (!scanner.peek(&ch))
			break;
		switch(ch)
		{
		case 'v':
			{
				scanner.skip_chars("v \t");
				std::vector<float> f;
				scanner.read_floats(&f);
				if (f.size() == 3)
					verts.push_back(D3DXVECTOR3(f[0], f[1], f[2]));
				else
					invalid_verts.insert(vert_idx);
				vert_idx++;
			}
			break;
		case 'f':
			{
				scanner.skip_chars("f \t");
				std::vector<int> i;
				scanner.read_ints(&i);
				if (i.size() == 3)
					tris.push_back(Tri(i[0], i[1], i[2]));
				else
					invalid_tris.insert(tri_idx);
				tri_idx++;
			}
			break;
		default:
			break;
		}
		scanner.skip_to_next_line();
	}

  return true;
}
