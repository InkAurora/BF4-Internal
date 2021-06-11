#include "includes.h"
#include "globals.h"
#include "classes.h"
#include "functions.h"
#include "aimbot.h"
#include "mainFlow.h"

void testFunc() {
  DWORD64 offset;
  for (int i = 0; i < 1000; i++) {
	readBytes(moduleBase + 0x2670D80, &offset, 8);
  }
  return;
}

int WINAPI Main(HMODULE hModule) {
  AllocConsole();
  FILE* f;
  freopen_s(&f, "CONOUT$", "w", stdout);

  loadGameContext();

  cout << "Game Context loaded!    Initializing..." << endl;
  Sleep(1000);

  updateConsole();

  // CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)mainFlow, NULL, NULL, NULL);

  while (true) {
	if (GetAsyncKeyState(VK_NUMPAD1)) {
	  B_NORECOIL = !B_NORECOIL;
	  updateConsole();
	  noRecoil(B_NORECOIL);
	  while (GetAsyncKeyState(VK_NUMPAD1)) {}
	}
	if (GetAsyncKeyState(VK_NUMPAD2)) {
	  B_NOSPREAD = !B_NOSPREAD;
	  updateConsole();
	  noSpread(B_NOSPREAD);
	  while (GetAsyncKeyState(VK_NUMPAD2)) {}
	}
	if (GetAsyncKeyState(VK_NUMPAD5)) {
	  AIMBOT_KEY++;
	  if (AIMBOT_KEY == 3) AIMBOT_KEY = 0;
	  if (AIMBOT_KEY == 1) {
		aimbotThreadHasClosed = false;
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)aimbotThread, NULL, NULL, NULL);
	  }
	  updateConsole();
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
	  updateConsole();
	  while (GetAsyncKeyState(VK_NUMPAD6)) {}
	}
	if (GetAsyncKeyState(VK_NUMPAD7)) {
	  B_MINIMAPSPOT = !B_MINIMAPSPOT;
	  updateConsole();
	  minimapSpot(B_MINIMAPSPOT);
	  while (GetAsyncKeyState(VK_NUMPAD7)) {}
	}
	if (GetAsyncKeyState(VK_NUMPAD8)) {
	  B_CHAMS = !B_CHAMS;
	  updateConsole();
	  engineChams(B_CHAMS);
	  while (GetAsyncKeyState(VK_NUMPAD8)) {}
	 }
	if (GetAsyncKeyState(VK_NUMPAD9)) {
	  B_HARDCORE = !B_HARDCORE;
	  updateConsole();
	  noHardcoreHUD(B_HARDCORE);
	  while (GetAsyncKeyState(VK_NUMPAD9)) {}
	}

	if (GetAsyncKeyState(VK_END) & 1) {
	  clearConsole();
	  AIMBOT_KEY = 0;
	  minimapSpot();
	  engineChams();
	  noRecoil();
	  noSpread();
	  cout << "Exiting!" << endl;
	  Sleep(500);
	  while (!aimbotThreadHasClosed) {}
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
