#include "stdafx.h"
#include "geometry.hpp"
#include "debug_renderer.hpp"
#include "mesh2.hpp"

Geometry::Geometry()
{
	DebugRenderer::instance().add_debug_render_delegate(fastdelegate::MakeDelegate(this, &Geometry::debug_draw), true);
}

Geometry::~Geometry()
{
	DebugRenderer::instance().add_debug_render_delegate(fastdelegate::MakeDelegate(this, &Geometry::debug_draw), false);
}

void Geometry::debug_draw(DebugDraw *d)
{
  d->name = _name;
  d->bounding_flags = BoundingObject::kSphere;
  d->sphere = _sphere;
  d->orientation = _orientation;
}
