#pragma once

#include "stdafx.h"

/**
*	@file Singleton.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 07/09/2019
*/

/**
*	@brief Singleton pattern as a template for any class.
*/
template<typename T>
class Singleton
{
protected:
	static std::unique_ptr<T> _instance;							//!< Singleton instance

protected:
	/**
	*	@brief Protected constructor as this class implements the Singleton pattern.
	*/
	Singleton() {};

	/**
	*	@brief Protected destructor as noone has the right to construct or delete an instance.
	*/
	~Singleton() {};

public:
	/**
	*	@brief Invalidated copy constructor for the base Singleton class.
	*/
	Singleton(const Singleton&) = delete;

	/**
	*	@brief Invalidated assigment operator for the base Singleton class.
	*/
	Singleton& operator=(const Singleton) = delete;

	/**
	*	@brief Returns the Singleton instance of the selected class. Constructor is called only if instance is not initialized.
	*	@return Class single instance.
	*/
	static T* getInstance();
};

// [Static members initialization]

template<typename T>
std::unique_ptr<T> Singleton<T>::_instance;

/// [Public methods]

template<typename T>
T* Singleton<T>::getInstance()
{
	if (!_instance.get())
	{
		_instance = std::unique_ptr<T>(new T());
	}

	return _instance.get();
}

