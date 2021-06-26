#include "stdafx.h"
#include "Ray3D.h"

#include "Geometry/General/BasicOperations.h"

/// [Public methods]

Ray3D::Ray3D(const vec3& orig, const vec3& dest) :
	Edge3D(orig, dest)
{
}

Ray3D::Ray3D(const Ray3D& ray) :
	Edge3D(ray)
{
}

Ray3D::~Ray3D()
{
}

Ray3D& Ray3D::operator=(const Ray3D& ray)
{
	Edge3D::operator=(ray);

	return *this;
}

/// [Protected methods]

bool Ray3D::isParametricTValid(const float t) const
{
	return t >= 0.0f;
}

/// EISEMANN RAY

/// [Static members initialization]

const std::vector<uint16_t> EisemannRay::_xValue{ 2, 4, 8 };
const std::vector<uint16_t> EisemannRay::_yValue{ 16, 32, 64 };
const std::vector<uint16_t> EisemannRay::_zValue{ 128, 256, 512 };

EisemannRay::TestFunctionMap EisemannRay::_testFunction = std::move(EisemannRay::initializeTestFunctions());

/// [Public methods]

EisemannRay::EisemannRay(const Ray3D& ray) :
	Ray3D(ray)
{
	this->computeParameters();
}

EisemannRay::EisemannRay(const EisemannRay& ray) :
	Ray3D(ray)
{
	this->computeParameters();
}

EisemannRay EisemannRay::operator=(const EisemannRay& ray)
{
	if (this != &ray)
	{
		Ray3D::operator=(ray);
		this->computeParameters();
	}

	return *this;
}

bool EisemannRay::intersect(const AABB& aabb)
{
	auto function = _testFunction[_rayType];

	return function(*this, aabb.min(), aabb.max());
}

/// [Protected methods]

void EisemannRay::classifyRay()
{
	vec3 dir = _direction;
	for (int i = 0; i < 3; ++i)
	{
		if (BasicOperations::equal(dir[i], 0.0f)) dir[i] = 0.0f;
	}

	const int i = BasicOperations::sign(dir[0]) + 1;
	const int j = BasicOperations::sign(dir[1]) + 1;
	const int k = BasicOperations::sign(dir[2]) + 1;

	// b00110100 === MPO
	//    ||||||_ k non-zero (false)
	//    |||||_ k negative (false)
	//    ||||_ j non-zero (true)
	//    |||_ j negative (false)
	//    ||_ i non-zero (true)
	//    |_ i negative (true)
	const int rayType = _xValue[i] + _yValue[j] + _zValue[k];
	_rayType = static_cast<RayType>(rayType);
}

void EisemannRay::computeParameters()
{
	const float i = _direction[0], j = _direction[1], k = _direction[2];					// Ray direction
	const float x = _orig[0], y = _orig[1], z = _orig[2];									// Ray origin
	const float origLength = glm::length(_orig), dirLength = glm::length(_direction);

	if (BasicOperations::equal(origLength, 0.0f) && BasicOperations::equal(dirLength, 0.0f))
	{
		return;
	}

	const float ii = _ii = BasicOperations::equal(i, 0.0f) ? 0.0f : 1.0f / i;
	const float ij = _ij = BasicOperations::equal(j, 0.0f) ? 0.0f : 1.0f / j;
	const float ik = _ik = BasicOperations::equal(k, 0.0f) ? 0.0f : 1.0f / k;

	// Ray slope
	const float ibyj = _ibyj = i * ij;
	const float jbyi = _jbyi = j * ii;
	const float jbyk = _jbyk = j * ik;
	const float kbyj = _kbyj = k * ij;
	const float ibyk = _ibyk = i * ik;
	const float kbyi = _kbyi = k * ii;
	_cxy = y - jbyi * x;
	_cxz = z - kbyi * x;
	_cyx = x - ibyj * y;
	_cyz = z - kbyj * y;
	_czx = x - ibyk * z;
	_czy = y - jbyk * z;

	this->classifyRay();
}

EisemannRay::TestFunctionMap EisemannRay::initializeTestFunctions()
{
	TestFunctionMap map;

	map[MMM] = [] (const EisemannRay& ray, const vec3& lb, const vec3& ub) -> int {
		const vec3 ro = ray._orig;

		return !(ro[0] < lb[0] ||
				 ro[1] < lb[1] ||
				 ro[2] < lb[2] ||
				 ray._jbyi * lb[0] - ub[1] + ray._cxy > 0 ||
				 ray._ibyj * lb[1] - ub[0] + ray._cyx > 0 ||
				 ray._jbyk * lb[2] - ub[1] + ray._czy > 0 ||
				 ray._kbyj * lb[1] - ub[2] + ray._cyz > 0 ||
				 ray._kbyi * lb[0] - ub[2] + ray._cxz > 0 ||
				 ray._ibyk * lb[2] - ub[0] + ray._czx > 0);
	};

	map[MMP] = [](const EisemannRay& ray, const vec3& lb, const vec3& ub) -> int {
		const vec3 ro = ray._orig;

		return !(ro[0] < lb[0] ||
				 ro[1] < lb[1] ||
				 ro[2] > ub[2] ||
				 ray._jbyi * lb[0] - ub[1] + ray._cxy > 0 ||
				 ray._ibyj * lb[1] - ub[0] + ray._cyx > 0 ||
				 ray._jbyk * ub[2] - ub[1] + ray._czy > 0 ||
				 ray._kbyj * lb[1] - lb[2] + ray._cyz < 0 ||
				 ray._kbyi * lb[0] - lb[2] + ray._cxz < 0 ||
				 ray._ibyk * ub[2] - ub[0] + ray._czx > 0);
	};

	map[MPM] = [](const EisemannRay& ray, const vec3& lb, const vec3& ub) -> int {
		const vec3 ro = ray._orig;

		return !(ro[0] < lb[0] ||
				 ro[1] > ub[1] ||
				 ro[2] < lb[2] ||
				 ray._jbyi * lb[0] - lb[1] + ray._cxy < 0 ||
				 ray._ibyj * ub[1] - ub[0] + ray._cyx > 0 ||
				 ray._jbyk * lb[2] - lb[1] + ray._czy < 0 ||
				 ray._kbyj * ub[1] - ub[2] + ray._cyz > 0 ||
				 ray._kbyi * lb[0] - ub[2] + ray._cxz > 0 ||
				 ray._ibyk * lb[2] - ub[0] + ray._czx > 0);
	};

	map[MPP] = [](const EisemannRay& ray, const vec3& lb, const vec3& ub) -> int {
		const vec3 ro = ray._orig;

		return !(ro[0] < lb[0] ||
				 ro[1] > ub[1] ||
				 ro[2] > ub[2] ||
				 ray._jbyi * lb[0] - lb[1] + ray._cxy < 0 ||
				 ray._ibyj * ub[1] - ub[0] + ray._cyx > 0 ||
				 ray._jbyk * ub[2] - lb[1] + ray._czy < 0 ||
				 ray._kbyj * ub[1] - lb[2] + ray._cyz < 0 ||
				 ray._kbyi * lb[0] - lb[2] + ray._cxz < 0 ||
				 ray._ibyk * ub[2] - ub[0] + ray._czx > 0);
	};

	map[PMM] = [](const EisemannRay& ray, const vec3& lb, const vec3& ub) -> int {
		const vec3 ro = ray._orig;

		return !(ro[0] > ub[0] ||
				 ro[1] < lb[1] ||
				 ro[2] < lb[2] ||
				 ray._jbyi * ub[0] - ub[1] + ray._cxy > 0 ||
				 ray._ibyj * lb[1] - lb[0] + ray._cyx < 0 ||
				 ray._jbyk * lb[2] - ub[1] + ray._czy > 0 ||
				 ray._kbyj * lb[1] - ub[2] + ray._cyz > 0 ||
				 ray._kbyi * ub[0] - ub[2] + ray._cxz > 0 ||
				 ray._ibyk * lb[2] - lb[0] + ray._czx < 0);
	};

	map[PMP] = [](const EisemannRay& ray, const vec3& lb, const vec3& ub) -> int {
		const vec3 ro = ray._orig;

		return !(ro[0] > ub[0] ||
				 ro[1] < lb[1] ||
				 ro[2] > ub[2] ||
				 ray._jbyi * ub[0] - ub[1] + ray._cxy > 0 ||
				 ray._ibyj * lb[1] - lb[0] + ray._cyx < 0 ||
				 ray._jbyk * ub[2] - ub[1] + ray._czy > 0 ||
				 ray._kbyj * lb[1] - lb[2] + ray._cyz < 0 ||
				 ray._kbyi * ub[0] - lb[2] + ray._cxz < 0 ||
				 ray._ibyk * ub[2] - lb[0] + ray._czx < 0);
	};

	map[PPM] = [](const EisemannRay& ray, const vec3& lb, const vec3& ub) -> int {
		const vec3 ro = ray._orig;

		return !(ro[0] > ub[0] ||
				 ro[1] > ub[1] ||
				 ro[2] < lb[2] ||
				 ray._jbyi * ub[0] - lb[1] + ray._cxy < 0 ||
				 ray._ibyj * ub[1] - lb[0] + ray._cyx < 0 ||
				 ray._jbyk * lb[2] - lb[1] + ray._czy < 0 ||
				 ray._kbyj * ub[1] - ub[2] + ray._cyz > 0 ||
				 ray._kbyi * ub[0] - ub[2] + ray._cxz > 0 ||
				 ray._ibyk * lb[2] - lb[0] + ray._czx < 0);
	};

	map[PPP] = [](const EisemannRay& ray, const vec3& lb, const vec3& ub) -> int {
		const vec3 ro = ray._orig;

		return !(ro[0] > ub[0] ||
				 ro[1] > ub[1] ||
				 ro[2] > ub[2] ||
				 ray._jbyi * ub[0] - lb[1] + ray._cxy < 0 ||
				 ray._ibyj * ub[1] - lb[0] + ray._cyx < 0 ||
				 ray._jbyk * ub[2] - lb[1] + ray._czy < 0 ||
				 ray._kbyj * ub[1] - lb[2] + ray._cyz < 0 ||
				 ray._kbyi * ub[0] - lb[2] + ray._cxz < 0 ||
				 ray._ibyk * ub[2] - lb[0] + ray._czx < 0);
	};

	map[POO] = [](const EisemannRay& ray, const vec3& lb, const vec3& ub) -> int {
		const vec3 ro = ray._orig;
		const float ro1 = ro[1];
		const float ro2 = ro[2];

		return !(ro[0] > ub[0] ||
				 ro1 < lb[1] ||
				 ro1 > ub[1] ||
				 ro2 < lb[2] ||
				 ro2 > ub[2]);
	};

	map[MOO] = [](const EisemannRay& ray, const vec3& lb, const vec3& ub) -> int {
		const vec3 ro = ray._orig;

		return !(ro[0] < lb[0] ||
				 ro[1] < lb[1] ||
				 ro[1] > ub[1] ||
				 ro[2] < lb[2] ||
				 ro[2] > ub[2]);
	};

	map[OPO] = [](const EisemannRay& ray, const vec3& lb, const vec3& ub) -> int {
		const vec3 ro = ray._orig;

		return !(ro[1] > ub[1] ||
				 ro[0] < lb[0] ||
				 ro[0] > ub[0] ||
				 ro[2] < lb[2] ||
				 ro[2] > ub[2]);
	};

	map[OMO] = [](const EisemannRay& ray, const vec3& lb, const vec3& ub) -> int {
		const vec3 ro = ray._orig;

		return !(ro[1] < lb[1] ||
				 ro[0] < lb[0] ||
				 ro[0] > ub[0] ||
				 ro[2] < lb[2] ||
				 ro[2] > ub[2]);
	};

	map[OOP] = [](const EisemannRay& ray, const vec3& lb, const vec3& ub) -> int {
		const vec3 ro = ray._orig;

		return !(ro[2] > ub[2] ||
				 ro[0] < lb[0] ||
				 ro[0] > ub[0] ||
				 ro[1] < lb[1] ||
				 ro[1] > ub[1]);
	};

	map[OOM] = [](const EisemannRay& ray, const vec3& lb, const vec3& ub) -> int {
		const vec3 ro = ray._orig;

		return !(ro[2] < lb[2] ||
				 ro[0] < lb[0] ||
				 ro[0] > ub[0] ||
				 ro[1] < lb[1] ||
				 ro[1] > ub[1]);
	};

	map[OMM] = [](const EisemannRay& ray, const vec3& lb, const vec3& ub) -> int {
		const vec3 ro = ray._orig;

		return !(ro[0] < lb[0] ||
				 ro[0] > ub[0] ||
				 ro[1] < lb[1] ||
				 ro[2] < lb[2] ||
				 ray._jbyk * lb[2] - ub[1] + ray._czy > 0 ||
				 ray._kbyj * lb[1] - ub[2] + ray._cyz > 0);
	};

	map[OMP] = [](const EisemannRay& ray, const vec3& lb, const vec3& ub) -> int {
		const vec3 ro = ray._orig;

		return !(ro[0] < lb[0] ||
				 ro[0] > ub[0] ||
				 ro[1] < lb[1] ||
				 ro[2] > ub[2] ||
				 ray._jbyk * ub[2] - ub[1] + ray._czy > 0 ||
				 ray._kbyj * lb[1] - lb[2] + ray._cyz < 0);
	};

	map[OPM] = [](const EisemannRay& ray, const vec3& lb, const vec3& ub) -> int {
		const vec3 ro = ray._orig;

		return !(ro[0] < lb[0] ||
				 ro[0] > ub[0] ||
				 ro[1] > ub[1] ||
				 ro[2] < lb[2] ||
				 ray._jbyk * lb[2] - lb[1] + ray._czy < 0 ||
				 ray._kbyj * ub[1] - ub[2] + ray._cyz > 0);
	};

	map[OPP] = [](const EisemannRay& ray, const vec3& lb, const vec3& ub) -> int {
		const vec3 ro = ray._orig;

		return !(ro[0] < lb[0] ||
				 ro[0] > ub[0] ||
				 ro[1] > ub[1] ||
				 ro[2] > ub[2] ||
				 ray._jbyk * ub[2] - lb[1] + ray._czy < 0 ||
				 ray._kbyj * ub[1] - lb[2] + ray._cyz < 0);
	};

	map[MOM] = [](const EisemannRay& ray, const vec3& lb, const vec3& ub) -> int {
		const vec3 ro = ray._orig;

		return !(ro[1] < lb[1] ||
				 ro[1] > ub[1] ||
				 ro[0] < lb[0] ||
				 ro[2] < lb[2] ||
				 ray._kbyi * lb[0] - ub[2] + ray._cxz > 0 ||
				 ray._ibyk * lb[2] - ub[0] + ray._czx > 0);
	};

	map[MOP] = [](const EisemannRay& ray, const vec3& lb, const vec3& ub) -> int {
		const vec3 ro = ray._orig;

		return !(ro[1] < lb[1] ||
				 ro[1] > ub[1] ||
				 ro[0] < lb[0] ||
				 ro[2] > ub[2] ||
				 ray._kbyi * lb[0] - lb[2] + ray._cxz < 0 ||
				 ray._ibyk * ub[2] - ub[0] + ray._czx > 0);
	};

	map[POM] = [](const EisemannRay& ray, const vec3& lb, const vec3& ub) -> int {
		const vec3 ro = ray._orig;

		return !(ro[1] < lb[1] ||
				 ro[1] > ub[1] ||
				 ro[0] > ub[0] ||
				 ro[2] < lb[2] ||
				 ray._kbyi * ub[0] - ub[2] + ray._cxz > 0 ||
				 ray._ibyk * lb[2] - lb[0] + ray._czx < 0);
	};

	map[POP] = [](const EisemannRay& ray, const vec3& lb, const vec3& ub) -> int {
		const vec3 ro = ray._orig;

		return !(ro[1] < lb[1] ||
				 ro[1] > ub[1] ||
				 ro[0] > ub[0] ||
				 ro[2] > ub[2] ||
				 ray._kbyi * ub[0] - lb[2] + ray._cxz < 0 ||
				 ray._ibyk * ub[2] - lb[0] + ray._czx < 0);
	};

	map[MMO] = [](const EisemannRay& ray, const vec3& lb, const vec3& ub) -> int {
		const vec3 ro = ray._orig;

		return !(ro[2] < lb[2] ||
				 ro[2] > ub[2] ||
				 ro[0] < lb[0] ||
				 ro[1] < lb[1] ||
				 ray._jbyi * lb[0] - ub[1] + ray._cxy > 0 ||
				 ray._ibyj * lb[1] - ub[0] + ray._cyx > 0);
	};

	map[MPO] = [](const EisemannRay& ray, const vec3& lb, const vec3& ub) -> int {
		const vec3 ro = ray._orig;

		return !(ro[2] < lb[2] ||
				 ro[2] > ub[2] ||
				 ro[0] < lb[0] ||
				 ro[1] > ub[1] ||
				 ray._jbyi * lb[0] - lb[1] + ray._cxy < 0 ||
				 ray._ibyj * ub[1] - ub[0] + ray._cyx > 0);
	};

	map[PMO] = [](const EisemannRay& ray, const vec3& lb, const vec3& ub) -> int {
		const vec3 ro = ray._orig;

		return !(ro[2] < lb[2] ||
				 ro[2] > ub[2] ||
				 ro[0] > ub[0] ||
				 ro[1] < lb[1] ||
				 ray._jbyi * ub[0] - ub[1] + ray._cxy > 0 ||
				 ray._ibyj * lb[1] - lb[0] + ray._cyx < 0);
	};

	map[PPO] = [](const EisemannRay& ray, const vec3& lb, const vec3& ub) -> int {
		const vec3 ro = ray._orig;

		return !(ro[2] < lb[2] ||
				 ro[2] > ub[2] ||
				 ro[0] > ub[0] ||
				 ro[1] > ub[1] ||
				 ray._jbyi * ub[0] - lb[1] + ray._cxy < 0 ||
				 ray._ibyj * ub[1] - lb[0] + ray._cyx < 0);
	};

	return map;
}

