#pragma once
#include <string>
#include <utility>

enum class device_type
{
	module,
	device,
	clone,
};

class device
{
public:
	const std::string& name;
	const device_type& type;

	virtual ~device() = default;

	virtual auto load_config() -> void = 0;
	virtual auto apply_config() -> void = 0;

	virtual auto init() -> void = 0;
	virtual auto done() -> void = 0;

protected:
	device(const std::string& name_param, const device_type& type_param)
		: name(name_param), type(type_param) {}
};