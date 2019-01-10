#include "std.h"
#include "resource.h"
#include "emul.h"
#include "vars.h"
#include "config.h"
#include "util.h"
#include "core.h"
#include "libs/cpu_manager.h"


char bpx_file_name[FILENAME_MAX];


INT_PTR CALLBACK watchdlg(HWND dlg, UINT msg, WPARAM wp, LPARAM lp)
{
	char tmp[0x200]; unsigned i;
	static const int ids1[] = { IDC_W1_ON, IDC_W2_ON, IDC_W3_ON, IDC_W4_ON };
	static const int ids2[] = { IDE_W1, IDE_W2, IDE_W3, IDE_W4 };
	if (msg == WM_INITDIALOG) {
		for (i = 0; i < 4; i++) {
			CheckDlgButton(dlg, ids1[i], watch_enabled[i] ? BST_CHECKED : BST_UNCHECKED);
			script2text(tmp, watch_script[i]); SetWindowText(GetDlgItem(dlg, ids2[i]), tmp);
		}
		CheckDlgButton(dlg, IDC_TR_RAM, trace_ram ? BST_CHECKED : BST_UNCHECKED);
		CheckDlgButton(dlg, IDC_TR_ROM, trace_ram ? BST_CHECKED : BST_UNCHECKED);
	reinit:
		for (i = 0; i < 4; i++)
			EnableWindow(GetDlgItem(dlg, ids2[i]), watch_enabled[i]);
		return 1;
	}
	if (msg == WM_COMMAND && (LOWORD(wp) == ids1[0] || LOWORD(wp) == ids1[1] || LOWORD(wp) == ids1[2] || LOWORD(wp) == ids1[3])) {
		for (i = 0; i < 4; i++)
			watch_enabled[i] = IsDlgButtonChecked(dlg, ids1[i]) == BST_CHECKED;
		goto reinit;
	}
	if ((msg == WM_SYSCOMMAND && (wp & 0xFFF0) == SC_CLOSE) || (msg == WM_COMMAND && LOWORD(wp) == IDCANCEL)) {
		trace_ram = IsDlgButtonChecked(dlg, IDC_TR_RAM) == BST_CHECKED;
		trace_rom = IsDlgButtonChecked(dlg, IDC_TR_ROM) == BST_CHECKED;
		for (i = 0; i < 4; i++)
			if (watch_enabled[i]) {
				SendDlgItemMessage(dlg, ids2[i], WM_GETTEXT, sizeof tmp, LPARAM(tmp));
				if (!toscript(tmp, watch_script[i])) {
					sprintf(tmp, "Watch %d: error in expression\nPlease do RTFM", i + 1);
					MessageBox(dlg, tmp, nullptr, MB_ICONERROR); watch_enabled[i] = 0;
					SetFocus(GetDlgItem(dlg, ids2[i]));
					return 0;
				}
			}
		EndDialog(dlg, 0);
	}
	return 0;
}

void mon_watchdialog()
{
	DebugCore::get_view()->show_dialog(MAKEINTRESOURCE(IDD_OSW), watchdlg);
}

void init_bpx(char* file)
{
	addpath(bpx_file_name, file ? file : "bpx.ini");
	const auto bpx_file = fopen(bpx_file_name, "rt");
	if (!bpx_file) return;
	if (file)
	{
		color(CONSCLR_DEFAULT); printf("bpx: ");
		color(CONSCLR_INFO);    printf("%s\n", bpx_file_name);
	}

	char line[100];
	while (!feof(bpx_file))
	{
		fgets(line, sizeof(line), bpx_file);
		line[sizeof(line) - 1] = 0;
		char type = -1;
		auto start = -1, end = -1, cpu_idx = -1;
		const auto n = sscanf(line, "%c%1d=%i-%i", &type, &cpu_idx, &start, &end);
		if (n < 3 || cpu_idx < 0 || cpu_idx >= int(TCpuMgr::get_count()) || start < 0)
			continue;

		if (end < 0)
			end = start;

		unsigned mask = 0;
		switch (type)
		{
		case 'r': mask |= MEMBITS_BPR; break;
		case 'w': mask |= MEMBITS_BPW; break;
		case 'x': mask |= MEMBITS_BPX; break;
		default: continue;
		}

		auto& cpu = TCpuMgr::get_cpu(cpu_idx);
		for (auto i = unsigned(start); i <= unsigned(end); i++)
			cpu.membits[i] |= mask;
		cpu.dbgchk = isbrk(cpu);
	}
	fclose(bpx_file);
}

void done_bpx()
{
	const auto bpx_file = fopen(bpx_file_name, "wt");
	if (!bpx_file) return;

	for (unsigned cpu_idx = 0; cpu_idx < TCpuMgr::get_count(); cpu_idx++)
	{
		auto& cpu = TCpuMgr::get_cpu(cpu_idx);

		for (auto i = 0; i < 3; i++)
		{
			for (unsigned start = 0; start < 0x10000; )
			{
				static const unsigned mask[] = { MEMBITS_BPR, MEMBITS_BPW, MEMBITS_BPX };
				if (!(cpu.membits[start] & mask[i]))
				{
					start++;
					continue;
				}
				const unsigned active = cpu.membits[start];
				unsigned end;
				for (end = start; end < 0xFFFF && !((active ^ cpu.membits[end + 1]) & mask[i]); end++) {}

				static const char type[] = { 'r', 'w', 'x' };
				if (active & mask[i])
				{
					if (start == end)
						fprintf(bpx_file, "%c%1d=0x%04X\n", type[i], cpu_idx, start);
					else
						fprintf(bpx_file, "%c%1d=0x%04X-0x%04X\n", type[i], cpu_idx, start, end);
				}

				start = end + 1;
			}
		}
	}

	fclose(bpx_file);
}
