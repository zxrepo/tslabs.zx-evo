#pragma once
#include <string>
#include <filesystem>
#include <map>
#include <functional>

#include "emul.h"
#include <sndchip.h>
#include "portable/iniparser.h"

class ide_config
{
public:
	std::filesystem::path image{};
	unsigned c, h, s, lba;
	bool readonly;
	bool cd;
};

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
	std::vector<std::string> ula_presets{};

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
	std::filesystem::path scrshot_path{};

	std::string ffmpeg_exec{};
	std::string ffmpeg_parm{};
	std::string ffmpeg_vout{};
	bool ffmpeg_newcons{};

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
	int scrshot{};
	int video_driver{};

	int rsm_mix_frames{};
	int rsm_mode{};
	int render{};

	// beta128 section
	std::filesystem::path appendboot{};
	bool trdos_present{};
	bool trdos_traps{};
	bool wd93_nodelay{};
	int trdos_interleave{};
	bool fdd_noise{};

	// leds section
	bool leds_enabled{};
	bool leds_status{};
	bool led_flash_ay_kbd{};
	bool leds_perf_t{};
	int leds_bandBpp{};

	unsigned led_ay{};
	unsigned led_perf{};
	unsigned led_load{};
	unsigned led_input{};
	unsigned led_time{};
	unsigned led_osw{};
	unsigned led_memband{};

	// sound section
	int gs_type{};

	std::function<void()> do_sound{};
	int soundbuffer{};
	bool sound_enabled{};
	bool sound_gsreset{};
	int sound_fq{};
	bool sound_dsprimary{};

	int sound_beeper_vol{};
	int sound_micout_vol{};
	int sound_micin_vol{};
	int sound_ay_vol{};
	bool sound_covoxFB{};
	int sound_covoxFB_vol{};
	bool sound_covoxDD{};
	int sound_covoxDD_vol{};
	int sound_covoxProfi_vol{};
	int sound_sd{};
	int sound_sd_vol{};
	bool sound_saa1099{};
	int sound_saa1099_vol{};
	bool sound_moonsound{};
	int sound_moonsound_vol{};
	int sound_gs_vol{};
	int sound_bass_vol{};
	bool sound_filter{};
	bool sound_reject_dc{};

	// saa1099 section
	int sound_saa1099fq{};

	// ngs section
	int gs_ramsize{};

	// ay section
	std::vector<std::string> sound_ay_vols{};
	std::vector<std::string> sound_ay_stereo{};
	
	SNDCHIP::CHIP_TYPE sound_ay_chip{};
	AY_SCHEME sound_ay_scheme{};
	int sound_ayfq{};
	bool sound_ay_samples{};

	// input section
	std::string input_zxkeymap{};
	std::string input_keyset{};
	zxkeymap* input_active_zxk{};
	int input_mouse{};
	int input_mousewheel{};
	bool input_joymouse{};
	int input_mousescale{};
	bool input_mouseswap{};
	bool input_kjoy{};
	bool input_keymatrix{};
	int input_firedelay{};
	bool input_altlock{};
	int input_paste_hold{};
	int input_paste_release{};
	int input_paste_newline{};
	int input_keybpcmode{};
	bool atm_xt_kbd{};
	int input_joy_id{};
	int input_firenum{};
	
	unreal_config();
	auto load(const char* filename, const std::string& appname) -> void;

	// color section
	int color_pal{};
	int color_num_pals{};

	// hdd section
	int ide_scheme{};
	bool ide_skip_real{};
	bool cd_aspi{};
	std::vector<ide_config> ide{ {},{} };

private:
	int load_errors_{};
	std::filesystem::path app_path_{};
	
	auto load_misc(INI::File& file) -> void;
	auto load_roms(INI::File& file) -> void;
	auto load_ula (INI::File& file) -> void;
	auto load_video(INI::File& file) -> void;
	auto load_beta128(INI::File& file) -> void;
	auto load_leds(INI::File& file) -> void;
	auto load_sound(INI::File& file) -> void;
	auto load_ngs(INI::File& file) -> void;
	auto load_ay(INI::File& file) -> void;
	auto load_saa1099(INI::File& file) -> void;
	auto load_input(INI::File& file) -> void;
	auto load_colors(INI::File& file) -> void;
	auto load_hdd(INI::File& file) -> void;

	
	auto load_remset(INI::File& file, const std::string& romset) -> void;

	// helpers
	[[nodiscard]]
	auto get_fullname(const std::string& fname) const ->std::filesystem::path;
	[[nodiscard]]
	auto get_as_path(const INI::Section* section, const std::string& key, const std::string& defval = "") const -> std::filesystem::path;

	static auto get_preset_list(const INI::Section* section, const std::string& prefix, std::vector<std::string>& result) -> void;
};
