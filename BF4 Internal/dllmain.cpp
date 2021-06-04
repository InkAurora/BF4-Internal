#include "functions.h"
#include "aimbot.h"

int WINAPI Main(HMODULE hModule) {
  AllocConsole();
  FILE* f;
  freopen_s(&f, "CONOUT$", "w", stdout);

  cout << "BF4 Internal                 ;by INK" << endl << endl;

  loadGameContext();

  CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)mainFlow, NULL, NULL, NULL);

  string message;

  while (true) {
	if (GetAsyncKeyState(VK_NUMPAD1)) {
	  B_NORECOIL = !B_NORECOIL;
	  message = B_NORECOIL ? "No Recoil Enabled!" : "No Recoil Disabled!";
	  cout << message << endl;
	  while (GetAsyncKeyState(VK_NUMPAD1)) {}
	}
	if (GetAsyncKeyState(VK_NUMPAD2)) {
	  B_NOSPREAD = !B_NOSPREAD;
	  message = B_NOSPREAD ? "No Spread Enabled!" : "No Spread Disabled!";
	  cout << message << endl;
	  while (GetAsyncKeyState(VK_NUMPAD2)) {}
	}
	if (GetAsyncKeyState(VK_NUMPAD5)) {
	  AIMBOT_KEY++;
	  if (AIMBOT_KEY == 3) AIMBOT_KEY = 0;
	  if (AIMBOT_KEY == 1) {
		aimbotThreadHasClosed = false;
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)aimbotThread, NULL, NULL, NULL);
	  }
	  string key = (AIMBOT_KEY == 1) ? "Left Alt" : "Right Mouse Button";
	  if (AIMBOT_KEY > 0) cout << "Aimbot Key: " << key << endl;
	  while (GetAsyncKeyState(VK_NUMPAD5)) {}
	}
	if (GetAsyncKeyState(VK_NUMPAD6)) {
	  AIMBOT_TARGET++;
	  if (AIMBOT_TARGET == 5) AIMBOT_TARGET = 1;
	  switch (AIMBOT_TARGET) {
	  case 2:
		BONE_INDEX = 142;
		break;
	  case 3:
		BONE_INDEX = 7;
		break;
	  case 4:
		BONE_INDEX = 6;
		break;
	  default:
		BONE_INDEX = 104;
		break;
	  }
	  string bone = (AIMBOT_TARGET == 1) ? "Head" : (AIMBOT_TARGET == 2) ? "Neck" : (AIMBOT_TARGET == 3) ? "Spine 1" : "Spine 2";
	  cout << "Aimbot target changed to " << bone << "!" << endl;
	  while (GetAsyncKeyState(VK_NUMPAD6)) {}
	}
	if (GetAsyncKeyState(VK_NUMPAD7)) {
	  B_MINIMAPSPOT = !B_MINIMAPSPOT;
	  message = B_MINIMAPSPOT ? "Minimap Spotting Enabled!" : "Minimap Spotting Disabled!";
	  cout << message << endl;
	  while (GetAsyncKeyState(VK_NUMPAD7)) {}
	}

	if (GetAsyncKeyState(VK_END) & 1) {
	  closeThread = true;
	  AIMBOT_KEY = 0;
	  cout << "Exiting!" << endl;
	  Sleep(500);
	  while (!threadHasClosed || !aimbotThreadHasClosed) {}
	  break;
	}
  }

  fclose(f);
  FreeConsole();
  FreeLibraryAndExitThread(hModule, 0);

  return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
  switch (ul_reason_for_call)
  {
  case DLL_PROCESS_ATTACH:
    CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Main, hModule, NULL, NULL);
    break;
  }
  return true;
}
