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

unreal_config::unreal_config()
{
}

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

    auto beta128	= file.GetSection("Beta128");
    auto leds		= file.GetSection("LEDS");
    auto sound		= file.GetSection("SOUND");
    auto input		= file.GetSection("INPUT");
    auto colors		= file.GetSection("COLORS");
    auto ay			= file.GetSection("AY");
    auto saa1099	= file.GetSection("SAA1099");
    auto atm		= file.GetSection("ATM");
    auto hdd		= file.GetSection("HDD");
    auto ngs		= file.GetSection("NGS");
    auto zc			= file.GetSection("ZC");

    load_misc(file);
    load_roms(file);
    load_ula (file);
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

    get_preset_list(section, "preset", ulapresets);    
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
    if (ray_paint_mode > RAYDRAW_DIM) conf.ray_paint_mode = RAYDRAW_DIM;

    videoscale = section->GetValue("scale", 2).AsInt();

    rsm_mix_frames = section->GetValue("rsm.frames", 8).AsInt();
	
    auto line = upper(section->GetValue("rsm.mode", "").AsString());
    rsm_mode = RSM_FIR0;
    if (line == "FULL")		conf.rsm.mode = RSM_FIR0;
    if (line == "2C")		conf.rsm.mode = RSM_FIR1;
    if (line == "3C")		conf.rsm.mode = RSM_FIR2;
    if (line == "SIMPLE")	conf.rsm.mode = RSM_SIMPLE;

    atariset = section->GetValue("AtariPreset", "").AsString();
	
    line = upper(section->GetValue("video", "").AsString());
    render = 0;
	
    for (auto i = 0; renders[i].func; i++)
        if (line == upper(renders[i].nick))
            conf.render = i;

    line = upper(section->GetValue("driver", "").AsString());
    for (auto i = 0; i < countof(drivers); i++)
        if (line == upper(drivers[i].nick))
            conf.driver = i;

    fast_sl = section->GetValue("fastlines", 0).AsBool();

    bordersize = section->GetValue("Border", 3).AsInt();
    if (bordersize > 5)
        bordersize = 3;
	
    minres = section->GetValue("MinRes", 0).AsInt();
	
    line = section->GetValue("Hide", "").AsString();

    /*char* ptr = strchr(line, ';'); if (ptr) *ptr = 0;
    for (ptr = line;;)
    {
        unsigned max = renders_count - 1;
        for (i = 0; renders[i].func; i++)
        {
            unsigned sl = strlen(renders[i].nick);
            if (!strnicmp(ptr, renders[i].nick, sl) && !isalnum(ptr[sl]))
            {
                ptr += sl;
                memcpy(&renders[i], &renders[i + 1], (sizeof * renders) * (max - i));
                break;
            }
        }
        if (!*ptr++)
            break;
    }

    GetPrivateProfileString(video, "ScrShotDir", ".", conf.scrshot_path, sizeof conf.scrshot_path, ininame);
    // addpath(conf.scrshot_path);
    GetPrivateProfileString(video, "ScrShot", nil, line, sizeof line, ininame);
    conf.scrshot = SS_SCR;
    for (int i = 0; i < sizeof(SSHOT_EXT) / sizeof(SSHOT_EXT[0]); i++)
    {
        if (line == upper(SSHOT_EXT[i]))
        {
            conf.scrshot = (SSHOT_FORMAT)i;
            break;
        }
    }

    GetPrivateProfileString(video, "ffmpeg.exec", "ffmpeg.exe", conf.ffmpeg.exec, sizeof conf.ffmpeg.exec, ininame);
    GetPrivateProfileString(video, "ffmpeg.parm", nil, conf.ffmpeg.parm, sizeof conf.ffmpeg.parm, ininame);
    GetPrivateProfileString(video, "ffmpeg.vout", "video#.avi", conf.ffmpeg.vout, sizeof conf.ffmpeg.vout, ininame);
    conf.ffmpeg.newcons = GetPrivateProfileInt(video, "ffmpeg.newconsole", 1, ininame);
    */
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
