#include "stdafx.h"
#include "scene.hpp"
#include "mesh.hpp"

Scene::Scene()
	: _free_fly_camera(NULL)
	, _debug_renderer(NULL)
	, _extents(FLT_MAX, FLT_MAX, FLT_MAX)
{
}

Scene::~Scene()
{
	container_delete(_meshes);
}

void Scene::destroy()
{
	delete this;
}
