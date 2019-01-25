#pragma once
#include <string>
#include <list>

class IServiceLocator
{
public:
	virtual ~IServiceLocator() = default;;

	virtual void RegisterService(std::string name, void* serviceObject, std::string tag = "") = 0;
	virtual void* Locate(std::string name, std::string tag = "") = 0;

	template<typename T>
	void RegisterService(T* serviceObject, std::string tag = "")
	{
		RegisterService(typeid(T).name(), reinterpret_cast<void*>(serviceObject), tag);
	}

	template<typename T>
	T* Locate(const std::string &tag = "")
	{
		return reinterpret_cast<T*>(Locate(typeid(T).name(), tag));
	}
};

class ServiceLocator : public IServiceLocator
{
public:
	void RegisterService(std::string name, void* serviceObject, std::string tag = "") override;
	void* Locate(std::string name, std::string tag = "") override;
	virtual ~ServiceLocator();

protected:
	struct Service final
	{
		void* ServiceObject;
		std::string Name;
		std::string Tag;
	};

private:
	std::list<struct Service> services_;
	typedef std::list<struct Service>::iterator ServiceIterator;
};