// HostMonitorDll.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <assert.h>
#include <shellapi.h>
#include <wchar.h>

typedef int(*SYNC_DATA)(int hr, int spo2, int rr, int nibp_sys, int nibp_dia, int nibp_mean);
extern SYNC_DATA gSyncData;
extern int startHostMonitor(int main_cnt, int main_width, int main_height, int sub_cnt, int sub_width, int sub_height, int color_bytes);
extern void* getUiOfHostMonitor(int display_id, int* width, int* height, bool force_update);
extern void sendTouch2HostMonitor(int x, int y, bool is_down, int display_id);

int sync_data(int hr, int spo2, int rr, int nibp_sys, int nibp_dia, int nibp_mean)
{
	wchar_t arguments[128];
	memset(arguments, 0, sizeof(arguments));
	swprintf(arguments, 128, L"/C sync_data.bat %d %d %d %d %d %d WinForms",
		hr, spo2, rr, nibp_sys, nibp_dia, nibp_mean);
	HINSTANCE ret = ShellExecute(NULL, L"open", L"cmd.exe", arguments, NULL, SW_HIDE);
	return 0;
}

DWORD WINAPI threadHostMonitor(LPVOID lpParam)
{
	gSyncData = sync_data;
	sync_data(60, 98, 30, 120, 80, 100);//Ping cloud
	startHostMonitor(1, 1024, 768, 8, 1024, 370, 4);
	return 0;
}

extern "C" __declspec(dllexport) int startHostMonitorFromDll()
{
	ShellExecute(0, L"open", L"https://github.com/idea4good/GuiLite", L"", L"", SW_SHOWNORMAL);
	DWORD pid;
	CreateThread(NULL, 0, threadHostMonitor, NULL, 0, &pid);
	return 0;
}

extern "C" __declspec(dllexport) HBITMAP getHbitmapOfHostMonitorUiFromDll(int display_id, int width, int height, bool force_update)
{
	static HBITMAP s_bitmap;
	BYTE* buffer = (BYTE*)getUiOfHostMonitor(display_id, NULL, NULL, force_update);
	if (buffer)
	{
		if (s_bitmap)
		{
			DeleteObject(s_bitmap);
		}
		s_bitmap = CreateBitmap(width, height, 1, 32, buffer);
		return s_bitmap;
	}
	return 0;
}

extern "C" __declspec(dllexport) void sendTouch2HostMonitorFromDll(int display_id, int x, int y, bool is_down)
{
	sendTouch2HostMonitor(x, y, is_down, display_id);
}
