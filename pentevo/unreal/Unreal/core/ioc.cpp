#include "std.h"
#include "ioc.h"

ServiceLocator::~ServiceLocator()
{
	for (auto i = services_.begin(); i != services_.end(); ++i)
	{
		if (i->ServiceObject != nullptr)
		{
			delete i->ServiceObject;
			i->ServiceObject = nullptr;
		}
	}
	services_.clear();
}

void ServiceLocator::RegisterService(std::string name, void* serviceObject, std::string tag)
{
	if (serviceObject == nullptr)
		throw std::invalid_argument("serviceObject");

	for (auto i = services_.begin(); i != services_.end(); ++i)
	{
		if ((i->Name == name) && (i->Tag == tag))
		{
			throw std::runtime_error(std::string("Duplicate service registration for '") + name +
				std::string("' and tag '") + tag + std::string("'"));
		}
	}

	struct Service service;
	service.Name = name;
	service.ServiceObject = serviceObject;
	service.Tag = tag;

	services_.push_back(service);
}

void* ServiceLocator::Locate(std::string name, std::string tag)
{
	for (auto i = services_.begin(); i != services_.end(); ++i)
	{
		if ((i->Name == name) && (i->Tag == tag))
		{
			return reinterpret_cast<void*>(i->ServiceObject);
		}
	}
	throw std::runtime_error(std::string("Service not found for '") + name +
		std::string("' and tag '") + tag + std::string("'"));
}