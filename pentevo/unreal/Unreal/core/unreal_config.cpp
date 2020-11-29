#include "std.h"
#include "unreal_config.h"
#include "portable/pystring.h"
#include "dx.h"

#include "config.h"
#include "vars.h"
#include <util.h>
#include <string>

using namespace std;
using namespace pystring;

unreal_config::unreal_config() = default;

auto unreal_config::load(const char* filename, const std::string& appname) -> void
{
    auto fname = filename != nullptr ? filesystem::path(filename) : appname;

    if (fname.extension() == ".exe" || !fname.has_extension())
        fname.replace_extension(".ini");

    if (!filesystem::exists(fname))
        errexit("config file not found %ls", fname.c_str());

    app_path_ = fname.parent_path();
    INI::File file(fname.string());

    color(CONSCLR_DEFAULT); printf("ini: ");
    color(CONSCLR_INFO);    printf("%ls\n", fname.c_str());

    auto atm		= file.GetSection("ATM");
    auto zc			= file.GetSection("ZC");

    load_misc(file);
    load_roms(file);
    load_ula (file);
    load_video(file);
    load_beta128(file);
    load_leds(file);
    load_sound(file);
    load_ay(file);
    load_saa1099(file);
    load_ngs(file);
    load_input(file);
    load_colors(file);
    load_hdd(file);
}

auto unreal_config::load_misc(INI::File& file) -> void
{
	auto* const section = file. GetSection("MISC");

    helpname = get_as_path(section, "Help", "help_eng.html");

    if (section->GetValue("HideConsole", 0).AsBool())
    {
        FreeConsole();
        nowait = true;
    }

    ConfirmExit		= section->GetValue("ConfirmExit", 0).AsBool();
    sleepidle		= section->GetValue("ShareCPU", 0).AsBool();
    highpriority	= section->GetValue("HighPriority").AsBool();
    tape_traps		= section->GetValue("TapeTraps", 1).AsBool();
    vm1				= section->GetValue("Z80_VM1", 0).AsBool();
    outc0			= section->GetValue("OUT_C_0", 1).AsBool();
    tape_autostart	= section->GetValue("TapeAutoStart", 1).AsBool();
    EFF7_mask		= section->GetValue("EFF7mask", 0).AsInt();
    spg_mem_init	= section->GetValue("SPGMemInit", 0).AsInt();

    // CMOS
    auto line = upper(section->GetValue("CMOS", "").AsString());
    cmos = 0;
    if (line == "DALLAS") cmos = 1;
    if (line == "512BU1") cmos = 2;

	// ULA+
    line = upper(section->GetValue("ULAPLUS", "").AsString());
    ulaplus = UPLS_NONE;
    if (line == "TYPE1") ulaplus = UPLS_TYPE1;
    if (line == "TYPE2") ulaplus = UPLS_TYPE2;

    // TS VDAC
    line = upper(section->GetValue("TS_VDAC", "").AsString());
    vdac = TS_VDAC_OFF;
    for (auto i = 0; i < TS_VDAC_MAX; i++)
        if (line == ts_vdac_names[i].nick)
        {
            vdac = ts_vdac_names[i].value;
            break;
        }

    // TS VDAC2 (FT812)
    vdac2 = section->GetValue("TS_VDAC2", 0).AsBool();

    // Cache
    cache = section->GetValue("Cache", 0).AsInt();
    if (cache && cache != 16 && cache != 32)
        cache = 0;

    line = upper(section->GetValue("HIMEM", "").AsString());

    cur_mem_model = MM_PENTAGON;
    for (auto i = 0; i < N_MM_MODELS; i++)
        if (line == mem_model[i].shortname)
        {
            cur_mem_model = mem_model[i].Model;
            break;
        }

    ramsize = section->GetValue("RamSize", 128).AsInt();
    workdir = get_as_path(section, "DIR", "");

	// TODO какой то треш, но перенес как было
	const auto tmp_path = filesystem::current_path();
    filesystem::current_path(workdir);
    SnapDir = filesystem::current_path();
    filesystem::current_path(tmp_path);
	
    RomDir = SnapDir;
    HddDir = SnapDir;

    line = upper(section->GetValue("RESET", "").AsString());
    reset_rom = RM_SOS;
    if (line == "DOS")	reset_rom = RM_DOS;
    if (line == "MENU")	reset_rom = RM_128;
    if (line == "SYS")	reset_rom = RM_SYS;

    line = upper(section->GetValue("Modem", "").AsString());
    modem_port = (line == "NONE") ? 0 : std::stoi(slice(line,3));

    line = upper(section->GetValue("ZiFi", "").AsString());
    zifi_port = (line == "NONE") ? 0 : std::stoi(slice(line, 3));
}

auto unreal_config::load_roms(INI::File& file) -> void
{
    auto* const section = file.GetSection("ROM");

    auto list = {
    	"PENTAGON",
    	"ATM1", "ATM2", "ATM3",
    	"SCORP", "PROFROM", "GMX",
    	"PROFI", "KAY", "PLUS3", "QUORUM",
    	"TSL",
    	"LSY", "PHOENIX",
    	"GS", "MOONSOUND"
	};

    for (const auto* item : list)
        roms_path.insert({ item, get_as_path(section, item) });

    const auto line = section->GetValue("ROMSET", "").AsString();

    if (!line.empty()) {
        load_remset(file, line);
        use_romset = true;
    }
    else
        use_romset = false;
}

auto unreal_config::load_ula(INI::File& file) -> void
{
    auto* const section = file.GetSection("ULA");
	
    intstart			= section->GetValue("intstart", 0).AsInt();
    t_line				= section->GetValue("Line", 224).AsInt();
    intfq				= section->GetValue("int", 50).AsInt();
    intlen				= section->GetValue("intlen", 32).AsInt();
    frametime			= section->GetValue("Frame", 71680).AsInt();
    border_4T			= section->GetValue("4TBorder", 0).AsBool();
    even_M1				= section->GetValue("EvenM1", 0).AsBool();
    floatbus			= section->GetValue("FloatBus", 0).AsBool();
    floatdos			= section->GetValue("FloatDOS", 0).AsBool();
    portff				= section->GetValue("PortFF", 0).AsBool();

    mem_swap			= section->GetValue("AtmMemSwap", 0).AsBool();
    use_comp_pal		= section->GetValue("UsePalette", 1).AsBool();
    profi_monochrome	= section->GetValue("ProfiMonochrome", 0).AsBool();

    get_preset_list(section, "preset", ula_presets);    
}

auto unreal_config::load_video(INI::File& file) -> void
{
    auto* const section = file.GetSection("VIDEO");
	
    flashcolor		= section->GetValue("FlashColor", 0).AsBool();
    frameskip		= section->GetValue("SkipFrame", 0).AsInt();
    flip			= section->GetValue("VSync", 0).AsBool();
    fullscr			= section->GetValue("FullScr", 1).AsBool();
    refresh			= section->GetValue("Refresh", 0).AsBool();
    frameskipmax	= section->GetValue("SkipFrameMaxSpeed", 33).AsInt();
    updateb			= section->GetValue("Update", 1).AsBool();
    ch_size			= section->GetValue("ChunkSize", 0).AsInt();
    noflic			= section->GetValue("NoFlic", 0).AsBool();
    alt_nf			= section->GetValue("AltNoFlic", 0).AsBool();
    scanbright		= section->GetValue("ScanIntens", 66).AsInt();
    pixelscroll		= section->GetValue("PixelScroll", 0).AsBool();
    detect_video	= section->GetValue("DetectModel", 1).AsBool();
    fontsize		= 8;

    ray_paint_mode = section->GetValue("raypaint_mode", 0).AsInt();
    if (ray_paint_mode > RAYDRAW_DIM) ray_paint_mode = RAYDRAW_DIM;

    videoscale = section->GetValue("scale", 2).AsInt();

    rsm_mix_frames = section->GetValue("rsm.frames", 8).AsInt();
	
    auto line = upper(section->GetValue("rsm.mode", "").AsString());
    rsm_mode = RSM_FIR0;
    if (line == "FULL")		rsm_mode = RSM_FIR0;
    if (line == "2C")		rsm_mode = RSM_FIR1;
    if (line == "3C")		rsm_mode = RSM_FIR2;
    if (line == "SIMPLE")	rsm_mode = RSM_SIMPLE;

    atariset = section->GetValue("AtariPreset", "").AsString();
	
    line = upper(section->GetValue("video", "").AsString());
    render = 0;
	
    for (auto i = 0; renders[i].func; i++)
        if (line == upper(renders[i].nick))
            render = i;

    line = upper(section->GetValue("driver", "").AsString());
    for (auto i = 0; i < countof(drivers); i++)
        if (line == upper(drivers[i].nick))
            video_driver = i;

    fast_sl = section->GetValue("fastlines", 0).AsBool();

    bordersize = section->GetValue("Border", 3).AsInt();
    if (bordersize > 5)
        bordersize = 3;
	
    minres = section->GetValue("MinRes", 0).AsInt();

    scrshot_path = section->GetValue("ScrShotDir", ".").AsString();
	
    line = section->GetValue("ScrShot", "").AsString();
    scrshot = SS_SCR;
    for (int i = 0; i < sizeof(SSHOT_EXT) / sizeof(SSHOT_EXT[0]); i++)
    {
        if (line == upper(SSHOT_EXT[i]))
        {
            scrshot = (SSHOT_FORMAT)i;
            break;
        }
    }

    ffmpeg_exec     = section->GetValue("ffmpeg.exec", "ffmpeg.exe").AsString();
    ffmpeg_parm     = section->GetValue("ffmpeg.parm", "").AsString();
    ffmpeg_vout     = section->GetValue("ffmpeg.vout", "video#.avi").AsString();
    ffmpeg_newcons  = section->GetValue("ffmpeg.newconsole", 1).AsBool();
}

auto unreal_config::load_beta128(INI::File& file) -> void
{
    auto* const section = file.GetSection("Beta128");
	
    trdos_present = section->GetValue("beta128", 1).AsBool();
    trdos_traps = section->GetValue("Traps", 1).AsBool();
    wd93_nodelay = section->GetValue("Fast", 1).AsBool();
    trdos_interleave = section->GetValue("IL", 1).AsInt() - 1;
    if (trdos_interleave > 2) trdos_interleave = 0;
	
    fdd_noise = section->GetValue("Noise", 0).AsBool();
    appendboot = get_as_path(section, "BOOT", "");
}

auto unreal_config::load_leds(INI::File& file) -> void
{
    auto* const section = file.GetSection("LEDS");

    auto parse_coords = [](const std::string& val) -> unsigned
    {
        int x, y, z;
    	
        if (sscanf(val.c_str(), "%d:%d,%d", &z, &x, &y) != 3)
            return  0;
        else
            return  (x & 0xFFFF) + ((y << 16) & 0x7FFFFFFF) + z * 0x80000000;
    };
	
    leds_enabled        = section->GetValue("leds", 1).AsBool();
    leds_status         = section->GetValue("status", 1).AsBool();
    led_flash_ay_kbd    = section->GetValue("KBD_AY", 1).AsBool();
    leds_perf_t         = section->GetValue("PerfShowT", 0).AsBool();
    leds_bandBpp        = section->GetValue("BandBpp", 512).AsInt();
	
    if (leds_bandBpp != 64 && leds_bandBpp != 128 && leds_bandBpp != 256 && leds_bandBpp != 512)
        leds_bandBpp = 512;

    led_ay      = parse_coords(section->GetValue("AY", "").AsString());
    led_perf    = parse_coords(section->GetValue("Perf", "").AsString());
    led_load    = parse_coords(section->GetValue("LOAD", "").AsString());
    led_input   = parse_coords(section->GetValue("Input", "").AsString());
    led_time    = parse_coords(section->GetValue("Time", "").AsString());
    led_osw     = parse_coords(section->GetValue("OSW", "").AsString());
    led_memband = parse_coords(section->GetValue("MemBand", "").AsString());
}

auto unreal_config::load_sound(INI::File& file) -> void
{
    auto* const section = file.GetSection("SOUND");
    
    auto line = upper(section->GetValue("SoundDrv", "").AsString());
    do_sound = do_sound_none;
    if (line == "WAVE") {
        do_sound = do_sound_wave;
        soundbuffer = section->GetValue("SoundBuffer", 0).AsInt();
        if (soundbuffer == 0)
            soundbuffer = 6;
        else if (soundbuffer >= MAXWQSIZE)
            soundbuffer = MAXWQSIZE - 1;
    }
    if (line == "DS") {
        do_sound = do_sound_ds;
    }

    sound_enabled   = section->GetValue("Enabled", 1).AsBool();
    sound_gsreset   = section->GetValue("GSReset", 0).AsBool();
    sound_fq        = section->GetValue ("Fq", 44100).AsInt();
    sound_dsprimary = section->GetValue("DSPrimary", 0).AsBool();

    gs_type = 0;
    line = upper(section->GetValue("GSTYPE", "").AsString());
    if (line == "Z80")  gs_type = 1;
    if (line == "BASS") gs_type = 2;

    sound_filter            = section->GetValue("SoundFilter", 0).AsBool();
    sound_reject_dc         = section->GetValue("RejectDC", 1).AsBool();

    sound_beeper_vol        = section->GetValue("BeeperVol", 4000).AsInt();
    sound_micout_vol        = section->GetValue("MicOutVol", 1000).AsInt();
    sound_micin_vol         = section->GetValue("MicInVol", 1000).AsInt();
    sound_ay_vol            = section->GetValue("AYVol", 4000).AsInt();
    sound_covoxFB           = section->GetValue("CovoxFB", 0).AsBool();
    sound_covoxFB_vol       = section->GetValue("CovoxFBVol", 8192).AsInt();
    sound_covoxDD           = section->GetValue("CovoxDD", 0).AsBool();
    sound_covoxDD_vol       = section->GetValue("CovoxDDVol", 4000).AsInt();
    sound_covoxProfi_vol    = section->GetValue("CovoxProfiVol", 4000).AsInt();
    sound_sd                = section->GetValue("SD", 0).AsBool();
    sound_sd_vol            = section->GetValue("SDVol", 4000).AsInt();
    sound_saa1099           = section->GetValue("Saa1099", 0).AsBool();
    sound_saa1099_vol       = section->GetValue("Saa1099Vol", 4000).AsInt();
    sound_moonsound         = section->GetValue("MoonSound", 0).AsBool();
    sound_moonsound_vol     = section->GetValue("MoonSoundVol", 4000).AsInt();
    sound_gs_vol            = section->GetValue("GSVol", 8000).AsInt();
    sound_bass_vol          = section->GetValue("BASSVol", 8000).AsInt();
    
}

auto unreal_config::load_ngs(INI::File& file) -> void
{
    auto* const section = file.GetSection("NGS");

    gs_ramsize = section->GetValue("RamSize", 2048).AsInt();
}

auto unreal_config::load_ay(INI::File& file) -> void
{
    auto* const section = file.GetSection("AY");

    get_preset_list(section, "VOLTAB", sound_ay_vols);
    get_preset_list(section, "VOLTAB", sound_ay_stereo);
	
    sound_ayfq = section->GetValue("Fq", 1774400).AsInt();

    auto line = upper(section->GetValue("Chip", "").AsString());
    sound_ay_chip = SNDCHIP::CHIP_YM;
    if (line == "YM2203")   sound_ay_chip = SNDCHIP::CHIP_YM2203;
    else if (line == "YM")  sound_ay_chip = SNDCHIP::CHIP_YM;
    else if (line == "AY")  sound_ay_chip = SNDCHIP::CHIP_AY;

    sound_ay_samples = section->GetValue("UseSamples", 0).AsBool();

    line = section->GetValue("Scheme", "").AsString();
    sound_ay_scheme = AY_SCHEME_NONE;
    if (line == "default")      sound_ay_scheme = AY_SCHEME_SINGLE;
    else if (line == "PSEUDO")  sound_ay_scheme = AY_SCHEME_PSEUDO;
    else if (line == "QUADRO")  sound_ay_scheme = AY_SCHEME_QUADRO;
    else if (line == "AYX32")   sound_ay_scheme = AY_SCHEME_AYX32;
    else if (line == "CHRV")    sound_ay_scheme = AY_SCHEME_CHRV;
    else if (line == "POS")     sound_ay_scheme = AY_SCHEME_POS;
}

auto unreal_config::load_saa1099(INI::File& file) -> void
{
    auto* const section = file.GetSection("SAA1099");

    sound_saa1099fq = section->GetValue("Fq", 8000000).AsInt();
}

auto unreal_config::load_input(INI::File& file) -> void
{
    auto* const section = file.GetSection("INPUT");

    input_zxkeymap = upper(section->GetValue("ZXKeyMap", "default").AsString());
    input_active_zxk = &zxk_maps[0];
    for (auto i = 0; i < zxk_maps_count; i++)
    {
        if (input_zxkeymap == upper(zxk_maps[i].name))
        {
            input_active_zxk = &zxk_maps[i];
            break;
        }
    }

    input_keyset = section->GetValue("KeybLayout", "default").AsString();

    auto line = upper(section->GetValue("Mouse", "").AsString());
    input_mouse = 0;
    if (line == "KEMPSTON") input_mouse = 1;
    if (line == "AY")       input_mouse = 2;

    line = section->GetValue("Wheel", "").AsString();
    input_mousewheel = MOUSE_WHEEL_NONE;
    if (line == "KEMPSTON") input_mousewheel = MOUSE_WHEEL_KEMPSTON;
    if (line == "KEYBOARD") input_mousewheel = MOUSE_WHEEL_KEYBOARD;

    input_joymouse      = section->GetValue("JoyMouse", 0).AsBool();
    input_mousescale    = section->GetValue("MouseScale", 0).AsInt();
    input_mouseswap     = section->GetValue("SwapMouse", 0).AsBool();
    input_kjoy          = section->GetValue("KJoystick", 1).AsBool();
    input_keymatrix     = section->GetValue("Matrix", 1).AsBool();
    input_firedelay     = section->GetValue("FireRate", 1).AsInt();
    input_altlock       = section->GetValue("AltLock", 1).AsBool();
    input_paste_hold    = section->GetValue("HoldDelay", 2).AsInt();
    input_paste_release = section->GetValue("ReleaseDelay", 5).AsInt();
    input_paste_newline = section->GetValue("NewlineDelay", 20).AsInt();
    input_keybpcmode    = section->GetValue("KeybPCMode", 0).AsInt();
    atm_xt_kbd          = section->GetValue("ATMKBD", 0).AsBool();
    input_joy_id        = section->GetValue("Joy", 0).AsInt();

    line = upper(section->GetValue("Fire", "0").AsString());
    input_firenum = 0;
    zxkeymap* active_zxk = input_active_zxk;
    for (auto i = 0; i < active_zxk->zxk_size; i++)
        if (line == upper(active_zxk->zxk[i].name))
        {
            input_firenum = i;
        	break;
        }
}

auto unreal_config::load_colors(INI::File& file) -> void
{
    auto* const section = file.GetSection("COLORS");
    const string defpal = section->GetValue("color").AsString();
    auto i = 0;

	for(auto&key: section->GetSectionKeys())
	{
		if (key == "color")
			continue;

        auto line = section->GetValue(key).AsString();
        auto& pal = (pals[i]);

        sscanf(line.c_str(), "%02X,%02X,%02X,%02X,%02X,%02X:%X,%X,%X;%X,%X,%X;%X,%X,%X",
            &pal.ZZ,  &pal.ZN,  &pal.NN,
            &pal.NB,  &pal.BB,  &pal.ZB,
            &pal.r11, &pal.r12, &pal.r13,
            &pal.r21, &pal.r22, &pal.r23,
            &pal.r31, &pal.r32, &pal.r33);

        pal.r11 = min(pal.r11, 256U);
        pal.r12 = min(pal.r12, 256U);
        pal.r13 = min(pal.r13, 256U);

        pal.r21 = min(pal.r21, 256U);
        pal.r22 = min(pal.r22, 256U);
        pal.r23 = min(pal.r23, 256U);

        pal.r31 = min(pal.r31, 256U);
        pal.r32 = min(pal.r32, 256U);
        pal.r33 = min(pal.r33, 256U);
		
        if (upper(key) == upper(defpal))
            color_pal = i;

        i++;
	}

    color_num_pals = i;
}

auto unreal_config::load_hdd(INI::File& file) -> void
{
    auto* const section = file.GetSection("HDD");

    auto line = upper(section->GetValue("SCHEME", "").AsString());

    ide_scheme = IDE_NONE;
    if (line == "ATM")
        ide_scheme = IDE_ATM;
    else if (line == "NEMO-DIVIDE")
        ide_scheme = IDE_NEMO_DIVIDE;
    else if (line == "NEMO-A8")
        ide_scheme = IDE_NEMO_A8;
    else if (line == "NEMO")
        ide_scheme = IDE_NEMO;
    else if (line == "SMUC")
        ide_scheme = IDE_SMUC;
    else if (line == "PROFI")
        ide_scheme = IDE_PROFI;
    else if (line == "DIVIDE")
        ide_scheme = IDE_DIVIDE;

    ide_skip_real = section->GetValue("SkipReal", 0).AsBool();
	
    line =  upper(section->GetValue("CDROM", "SPTI").AsString());
    cd_aspi = line == "ASPI";

    for (auto i = 0; i < 2; i++)
    {
        auto& ide_device = ide[i];
    	
        auto param = "LBA" + std::to_string(i);
        ide_device.lba = section->GetValue(param).AsInt();

        param = "CHS" + std::to_string(i);
        line = section->GetValue(param, "0/0/0").AsString();

    	unsigned c, h, s;
        sscanf(line.c_str(), "%u/%u/%u", &c, &h, &s);

        char errbuf[64];
    	
        if (h > 16)
        {
            sprintf(errbuf, "HDD%d heads count > 16 : %u\n", i, h);
            errmsg(errbuf);
            continue;
        }
        if (s > 63)
        {
            sprintf(errbuf, "error HDD%d sectors count > 63 : %u\n", i, s);
            errmsg(errbuf);
            continue;
        }
        if (c > 16383)
        {
            sprintf(errbuf, "error HDD%d cylinders count > 16383 : %u\n", i, c);
            errmsg(errbuf);
            continue;
        }

        ide_device.c = c;
        ide_device.h = h;
        ide_device.s = s;

        param = "Image" + std::to_string(i);
        ide_device.image = get_as_path(section, param, "");

        param = "HD" + std::to_string(i) + "RO";
        ide_device.readonly = section->GetValue(param, 0).AsBool();

        param = "CD" + std::to_string(i);
        ide_device.cd = section->GetValue(param, 0).AsBool();

        if (!ide_device.cd && ide_device.lba == 0 && !ide_device.image.empty())
        {
	        const auto size = filesystem::file_size(ide_device.image);
            ide_device.lba = size / 512;
        }
    }
}

auto unreal_config::load_remset(INI::File& file, const std::string& romset)
-> void
{
    auto* const section = file.GetSection("ROM." + romset);
    auto list = { "sos", "dos", "128", "sys"};

    for (const auto* item : list)
        roms_path.insert({ item, get_as_path(section, item) });
}

auto unreal_config::get_fullname(const std::string& fname) const
-> std::filesystem::path
{
    auto res = app_path_;
    return res.append(fname);
}

auto unreal_config::get_as_path(const INI::Section* section, const std::string& key, const std::string& defval) const
-> std::filesystem::path
{
    return get_fullname(section->GetValue(key, defval).AsString());
}

auto unreal_config::get_preset_list(const INI::Section* section, const std::string& prefix, std::vector<std::string>& result)
-> void
{
    auto parts = vector<string>();
	
    for(auto& key: section->GetSectionKeys())
    {
        parts.clear();
        split(key, parts, ".", 1);

        if (parts.size() == 2 && upper(parts[0]) == upper(prefix)) {
            result.push_back(parts[1]);
        }
    }
}
