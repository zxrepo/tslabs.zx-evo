#pragma once
#include <string>
#include <filesystem>
#include <map>
#include "portable/iniparser.h"

class unreal_config
{
public:
	// misc section
	std::filesystem::path helpname{};
	std::filesystem::path workdir{};
	std::filesystem::path SnapDir{};
	std::filesystem::path RomDir{};
	std::filesystem::path HddDir{};
	
	bool nowait{};
	bool ConfirmExit{};
	bool sleepidle{};
	bool highpriority{};
	bool tape_traps{};
	bool vm1{};
	bool outc0{};
	bool tape_autostart{};
	int EFF7_mask{};
	int spg_mem_init{};
	int cmos{};
	int ulaplus{};
	int vdac{};
	bool vdac2{};
	int cache{};
	int cur_mem_model{};
	int ramsize{};
	int reset_rom{};
	int modem_port{};
	int zifi_port{};

	// rom section
	std::map<std::string, std::filesystem::path> roms_path{};
	bool use_romset{};
	
	// ula section
	std::vector<std::string> ulapresets{};
	
	int intstart{};
	int t_line{};
	int intfq{};
	int intlen{};
	int frametime{};
	bool border_4T{};
	bool even_M1{};
	bool floatbus{};
	bool floatdos{};
	bool portff{};
	bool mem_swap{};
	bool use_comp_pal{};
	bool profi_monochrome{};

	// video section
	std::string atariset{};
	
	bool flashcolor{};
	int frameskip{};
	bool flip{};
	bool fullscr{};
	bool refresh{};
	int frameskipmax{};
	bool updateb{};
	int ch_size{};
	bool noflic{};
	bool alt_nf{};
	int scanbright{};
	bool pixelscroll{};
	bool detect_video{};
	int fontsize{};
	int ray_paint_mode{};
	int videoscale{};
	bool fast_sl{};
	int bordersize{};
	int minres{};

	int rsm_mix_frames{};
	int rsm_mode{};

	int render{};
	
	unreal_config();
	auto load(const char* filename, const std::string& appname) -> void;

private:
	int load_errors_{};
	std::filesystem::path app_path_{};
	
	auto load_misc(INI::File& file) -> void;
	auto load_roms(INI::File& file) -> void;
	auto load_ula (INI::File& file) -> void;
	auto load_video(INI::File& file) -> void;

	auto load_remset(INI::File& file, const std::string& romset) -> void;

	// helpers
	[[nodiscard]]
	auto get_fullname(const std::string& fname) const ->std::filesystem::path;
	[[nodiscard]]
	auto get_as_path(const INI::Section* section, const std::string& key, const std::string& defval = "") const -> std::filesystem::path;

	static auto get_preset_list(const INI::Section* section, const std::string& prefix, std::vector<std::string>& result) -> void;
};