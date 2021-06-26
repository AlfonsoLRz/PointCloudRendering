#include "stdafx.h"
#include "Vector3.h"

#include "Geometry/General/BasicOperations.h"

bool Vector3::equal(const vec3& v1, const vec3& v2)
{
	return BasicOperations::equal(v1.x, v2.x) && BasicOperations::equal(v1.y, v2.y) && BasicOperations::equal(v1.z, v2.z);
}
