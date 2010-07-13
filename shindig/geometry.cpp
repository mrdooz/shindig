#include "stdafx.h"
#include "geometry.hpp"

Geometry::Geometry()
{

}

Geometry::~Geometry()
{

}


void Geometry::debug_draw(DebugDraw *d)
{
  d->name = _name;
  d->bounding_flags = BoundingObject::kSphere;
  d->sphere = _sphere;
  d->orientation = _orientation;
}
