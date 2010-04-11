#include "stdafx.h"
#include "scene.hpp"

Scene::Scene()
	: _free_fly_camera(NULL)
	, _debug_renderer(NULL)
	, _extents(FLT_MAX, FLT_MAX, FLT_MAX)
{
}
