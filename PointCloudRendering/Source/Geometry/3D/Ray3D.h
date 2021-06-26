#pragma once

#include "Geometry/3D/AABB.h"
#include "Geometry/3D/Edge3D.h"

/**
*	@file Ray3D.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 08/05/2019
*/

/**
*	@brief Ray that starts from a point and continues with no destination.
*/
class Ray3D: public Edge3D
{
protected:
	/**
	*	@return True if t is in the interval we expect from a certain type of line.
	*/
	virtual bool isParametricTValid(const float t) const;

public:
	/**
	*	@brief Constructor.
	*/
	Ray3D(const vec3& orig, const vec3& dest);

	/**
	*	@brief Copy constructor.
	*/
	Ray3D(const Ray3D& ray);

	/**
	*	@brief Destructor.
	*/
	virtual ~Ray3D();

	/**
	*	@brief Assignment operator overriding.
	*/
	virtual Ray3D& operator=(const Ray3D& ray);
};

/**
*	@brief Class which supports the ray-AABB intersection test (Eisemann et al.).
*/
class EisemannRay: public Ray3D
{
	typedef std::unordered_map<uint16_t, std::function<bool(const EisemannRay&, const vec3&, const vec3&)>> TestFunctionMap;				//!< Hash table of functions for each ray type

protected:
	static const std::vector<uint16_t> _xValue, _yValue, _zValue;				//!< ID for each direction and for each vec3 position

	enum RayType : uint16_t
	{
		MMM = 146,																//!< These values are computed with _xValue, _yValue and _zValue
		MMP = 530,				
		MPM = 194,				
		MPP = 578,				
		PMM = 152,				
		PMP = 536,				
		PPM = 200,			
		PPP = 584,			
		POO = 296,				
		MOO = 290,				
		OPO = 324,			
		OMO = 276,				
		OOP = 512,			
		OOM = 164,				
		OMM = 148,			
		OMP = 532,				
		OPM = 196,				
		OPP = 580,				
		MOM = 162,			
		MOP = 546,			
		POM = 168,			
		POP = 552,			
		MMO = 274,			
		MPO = 322,		
		PMO = 280,			
		PPO = 328			
	};

protected:
	static TestFunctionMap _testFunction;

protected:
	// All these values could be computed only once! It is not needed for each intersection
	RayType		_rayType;												
	float		_ii, _ij, _ik, _ibyj, _jbyi, _jbyk, _kbyj, _ibyk, _kbyi;
	float		_cxy, _cxz, _cyx, _cyz, _czx, _czy;

protected:
	/**
	*	@brief Computes the ray type according to its direction.
	*/
	void classifyRay();

	/**
	*	@brief Computes ray parameters which are needed for the intersection test.
	*/
	void computeParameters();

	/**
	*	@brief Initializes the hash map with test functions for each ray type.
	*/
	static TestFunctionMap initializeTestFunctions();

public:
	/**
	*	@brief Constructor from a base instance.
	*/
	EisemannRay(const Ray3D& ray);

	/**
	*	@brief Copy constructor.
	*/
	EisemannRay(const EisemannRay& ray);

	/**
	*	@brief Assignment operator overriding.
	*/
	EisemannRay operator=(const EisemannRay& ray);

	/**
	*	@return True if AABB and ray intersect.
	*/
	bool intersect(const AABB& aabb);
};

