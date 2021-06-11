#pragma once

#pragma warning(disable : 4996)

#include "includes.h"
#include "globals.h"
#include "classes.h"

void clearConsole() {
  COORD topLeft = { 0, 0 };
  HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO screen;
  DWORD written;

  GetConsoleScreenBufferInfo(console, &screen);
  FillConsoleOutputCharacterA(
	console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
  );
  FillConsoleOutputAttribute(
	console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
	screen.dwSize.X * screen.dwSize.Y, topLeft, &written
  );
  SetConsoleCursorPosition(console, topLeft);
}

void printLogo() {
  cout << ":::::::::  ::::::::::     :::               :::::::::  :::    ::: :::::::::   ::::::::  :::::::::: " << endl;
  cout << ":+:    :+: :+:           :+:                :+:    :+: :+:    :+: :+:    :+: :+:    :+: :+:        " << endl;
  cout << "+:+    +:+ +:+          +:+ +:+             +:+    +:+ +:+    +:+ +:+    +:+ +:+        +:+   " << endl;
  cout << "+#++:++#+  :#::+::#    +#+  +:+             +#++:++#+  +#+    +:+ +#++:++#:  :#:        +#++:++#  " << endl;
  cout << "+#+    +#+ +#+        +#+#+#+#+#+           +#+        +#+    +#+ +#+    +#+ +#+   +#+# +#+  " << endl;
  cout << "#+#    #+# #+#              #+#             #+#        #+#    #+# #+#    #+# #+#    #+# #+#  " << endl;
  cout << "#########  ###              ###             ###         ########  ###    ###  ########  ########## " << endl;
  cout << "" << endl;
  cout << ":::::::::  :::   :::               ::::::::::: ::::    ::: :::    ::: " << endl;
  cout << ":+:    :+: :+:   :+:                   :+:     :+:+:   :+: :+:   :+:" << endl;
  cout << "+:+    +:+  +:+ +:+                    +:+     :+:+:+  +:+ +:+  +:+" << endl;
  cout << "+#++:++#+    +#++:                     +#+     +#+ +:+ +#+ +#++:++" << endl;
  cout << "+#+    +#+    +#+                      +#+     +#+  +#+#+# +#+  +#+   " << endl;
  cout << "#+#    #+#    #+#                      #+#     #+#   #+#+# #+#   #+#  " << endl;
  cout << "#########     ###                  ########### ###    #### ###    ### " << endl;
  cout << "" << endl;
  cout << "---------------------------------------------------------------------------------------------------" << endl;
  cout << "" << endl;
}

void updateConsole() {
  clearConsole();
  printLogo();
  cout << "NUMPAD 1: No Recoil      ::  " << (B_NORECOIL ? "ENABLED" : "DISABLED") << endl;
  cout << "NUMPAD 2: No Spread      ::  " << (B_NOSPREAD ? "ENABLED" : "DISABLED") << endl;
  cout << "NUMPAD 5: Aimbot Key     ::  " << (AIMBOT_KEY == 1 ? "LEFT ALT" : AIMBOT_KEY == 2 ? "RMB" : "DISABLED") << endl;
  cout << "NUMPAD 6: Aimbot Target  ::  " << (AIMBOT_TARGET == 1 ? "HEAD" : AIMBOT_TARGET == 2 ? "NECK" : AIMBOT_TARGET == 3 ? "SPINE 1" : "SPINE 2") << endl;
  cout << "NUMPAD 7: Minimpap Spot  ::  " << (B_MINIMAPSPOT ? "ENABLED" : "DISABLED") << endl;
  cout << "NUMPAD 8: Engine Chams   ::  " << (B_CHAMS ? "ENABLED" : "DISABLED") << endl;
  cout << "NUMPAD 9: Enable HUD     ::  " << (B_HARDCORE ? "ENABLED" : "DISABLED") << endl;
}

void readBytes(DWORD64 address, void* destination, SIZE_T size) {
  memcpy(destination, (void*)address, size);
}

void writeBytes(DWORD64 address, void* bytes, SIZE_T size, int debug = 0) {
  DWORD protect;
  VirtualProtect((LPVOID)address, size, PAGE_EXECUTE_READWRITE, &protect);
  memcpy((void*)address, &bytes, size);
  VirtualProtect((LPVOID)address, size, protect, &protect);
}

string ReadString(DWORD64 address, size_t size) {
  LPCVOID param = (LPCVOID)address;

  char* str = new char[size];
  readBytes(address, str, size);

  string temp(&str[0], &str[size]);
  char* t_str = strtok(&temp[0], "\0");
  if (t_str != nullptr) return t_str;
  return string("");
}

vector<float> ReadVector(DWORD64 vectorAddress) {
  vector<float> vector3;
  
  float temp;
  for (int i = 0; i < 3; i++) {
	readBytes(vectorAddress + (i * 0x4), &temp, 4);
	vector3.push_back(temp);
  }

  return vector3;
}

vector<vector<float>> ReadMatrix(DWORD64 mAddress) {
  vector<vector<float>> matrix(4, vector<float>(4));

  __int64 offset = 0;
  float temp;
  for (__int64 i = 0; i < 4; i++) {
	for (__int64 j = 0; j < 4; j++) {
	  readBytes(mAddress + offset, &temp, 4);
	  matrix.at(i).at(j) = temp;
	  offset += 0x4;
	}
  }

  return matrix;
}

float Distance(vector<float> v1, vector<float> v2) {
  vector<float> fV;
  
  fV = { v1[0] - v2[0], v1[1] - v2[1], v1[2] - v2[2] };
  float result = sqrt(fV[0] * fV[0] + fV[1] * fV[1] + fV[2] * fV[2]);
  
  return result;
}

float RadToDeg(float rad) {
  return (rad * (180 / PI_F));
}

float CrosshairDistance(vector<float> pos, vector<vector<float>> vMat) {
  vector<float> front = { -vMat.at(2).at(0) + vMat.at(3).at(0), -vMat.at(2).at(1) + vMat.at(3).at(1), -vMat.at(2).at(2) + vMat.at(3).at(2) };
  vector<float> orig = { vMat.at(3).at(0), vMat.at(3).at(1), vMat.at(3).at(2) };
  vector<float> diff = { front[0] - orig[0], front[1] - orig[1], front[2] - orig[2] };
  float ShootspaceDist = Distance(front, orig);
  float TotalDist = Distance(front, pos);
  float DistDiff = TotalDist / ShootspaceDist;
  front[0] += diff[0] * DistDiff;
  front[1] += diff[1] * DistDiff;
  front[2] += diff[2] * DistDiff;
  float LengthH = Distance(orig, front);
  float LengthO = Distance(pos, front);
  return (RadToDeg(sin(LengthO / LengthH)));
}

vector<float> WorldToScreen(vector<float> enemy, vector<vector<float>> vProj, __int64 pose = -1) {
  enemy[1] += (pose == -1) ? 0 : (pose == 0) ? 1.7f : (pose == 1) ? 1.15f : (pose == 2) ? 0.4f : 0;
  vector<float> result(3, 0);

  result[2] = ((vProj.at(0).at(3) * enemy[0]) + (vProj.at(1).at(3) * enemy[1]) + (vProj.at(2).at(3) * enemy[2] + vProj.at(3).at(3)));
  
  if (result[2] < 0.000001) return { 0, 0, 0 };

  result[0] = (SCREEN_X / 2) + (SCREEN_X / 2) * ((vProj.at(0).at(0) * enemy[0]) + (vProj.at(1).at(0) * enemy[1]) + (vProj.at(2).at(0) * enemy[2] + vProj.at(3).at(0))) / result[2];
  result[1] = (SCREEN_Y / 2) - (SCREEN_Y / 2) * ((vProj.at(0).at(1) * enemy[0]) + (vProj.at(1).at(1) * enemy[1]) + (vProj.at(2).at(1) * enemy[2] + vProj.at(3).at(1))) / result[2];

  if (result[0] > 0 && result[0] < SCREEN_X && result[1] > 0 && result[1] < SCREEN_Y) return result;
  return { 0, 0, 0 };
}

BOOL Valid(DWORD64 pointer) {
  return (pointer >= 0x10000 && pointer <= 0x0F000000000000);
};

BOOL ValidSlot(int ActiveSlot) {
  return (ActiveSlot >= 0 && ActiveSlot <= 6 && (ActiveSlot <= 2 || ActiveSlot >= 5));
}

void readData() {
  readBytes(moduleBase + 0x2670D80, &GameContext, 8);
  if (Valid(GameContext)) {
	readBytes(GameContext + 0x60, &ClientPlayerManager, 8);
	if (Valid(ClientPlayerManager)) {
	  readBytes(ClientPlayerManager + 0x540, &ClientPlayer, 8);
	  readBytes(ClientPlayerManager + 0x548, &IdToPlayerMapOffset, 8);
	}
  }
}

void loadGameContext() {
  moduleBase = (DWORD64)GetModuleHandle(NULL);
  while (true) {
	readData();
	if (Valid(IdToPlayerMapOffset)) {
	  return;
	}
	Sleep(200);
  }
}

DWORD64 getWeaponPointer() {
  DWORD64 Character, ClientSoldier, SoldierWeaponComponent, CurrentAnimatedWeaponHandler;
  DWORD64 CurrentWeapon = 0;
  int ActiveSlot;

  if (Valid(ClientPlayer)) {
	readBytes(ClientPlayer + 0x14b0, &Character, 8);
	if (Valid(Character)) {
	  readBytes(Character, &ClientSoldier, 8);
	  if (Valid(ClientSoldier)) {
		readBytes(ClientSoldier + 0x568, &SoldierWeaponComponent, 8);
		if (Valid(SoldierWeaponComponent)) {
		  readBytes(SoldierWeaponComponent + 0x890, &CurrentAnimatedWeaponHandler, 8);
		  readBytes(SoldierWeaponComponent + 0xa98, &ActiveSlot, 4);
		  readBytes(SoldierWeaponComponent + 0xac8, &ZeroingDistanceLevel, 4);
		  if (Valid(CurrentAnimatedWeaponHandler) && ValidSlot(ActiveSlot)) {
			readBytes(CurrentAnimatedWeaponHandler + (ActiveSlot * 0x8), &CurrentWeapon, 8);
		  }
		}
	  }
	}
  }

  return CurrentWeapon;
}

vector<float> getBonePosition(DWORD64 address, __int64 boneIndex) {
  DWORD64 RagdollComponent, pQuatTransform;
  char ValidTransform;
  vector<float> Position = vector<float>(3, 0);

  readBytes(address + 0x580, &RagdollComponent, 8);
  if (Valid(RagdollComponent)) {
	readBytes(RagdollComponent + 0xc8, &ValidTransform, 1);
	if (ValidTransform > 0) {
	  readBytes(RagdollComponent + 0xb0, &pQuatTransform, 8);
	  if (Valid(pQuatTransform)) {
		Position = ReadVector(pQuatTransform + (boneIndex * 0x20));
		return Position;
	  }
	}
  }

  return { 0, 0, 0 };
}

vector<float> getWorldVector(EnemyData Enemy) {
  if (!myWeapon.successRead || !Valid(Enemy.SoldierPointer)) {
	return { 0, 0, 0 };
  }
  vector<float> AimVector = getBonePosition(Enemy.SoldierPointer, BONE_INDEX);
  vector<float> CamPos = { ViewMatrixInverse.at(3).at(0), ViewMatrixInverse.at(3).at(1) - myWeapon.Offset[1], ViewMatrixInverse.at(3).at(2) };
  float Dist = Distance(CamPos, AimVector);
  float Time = Dist / myWeapon.Velocity[2];
  // Dist = Dist - myWeapon.ZeroingRad;
  float DropG = 0.5f * myWeapon.Gravity * Time * Time;
  float Pitch = atan2(DropG, Dist);
  float ZeroPitch = atan2(myWeapon.Velocity[1], myWeapon.Velocity[2]);
  ZeroPitch += myWeapon.ZeroingRad;
  Pitch += ZeroPitch;
  AimVector = { AimVector[0] + (Enemy.Velocity[0] * Time) - (LocalPlayer.Velocity[0] * Time), AimVector[1] + (Enemy.Velocity[1] * Time) - (LocalPlayer.Velocity[1] * Time), AimVector[2] + (Enemy.Velocity[2] * Time) - (LocalPlayer.Velocity[2] * Time) };
  vector<float> WorldVector = WorldToScreen(AimVector, ViewProjection);
  float PixPerDeg = SCREEN_Y / RadToDeg(ClientFovY);
  WorldVector[1] += PixPerDeg * RadToDeg(Pitch);
  return WorldVector;
}

vector<int> getMoveVector(vector<float> WorldVector) {
  vector<int> Move = { 0 , 0 };

  if (WorldVector[0] != 0) {
	if (WorldVector[0] > (SCREEN_X / 2)) {
	  Move[0] = -(SCREEN_X / 2 - WorldVector[0]);
	  if (Move[0] + (SCREEN_X / 2) > SCREEN_X) Move[0] = 0;
	}
	else {
	  Move[0] = WorldVector[0] - SCREEN_X / 2;
	  if (Move[0] + (SCREEN_X / 2) <= 0) Move[0] = 0;
	}
  }
  if (WorldVector[1] != 0) {
	if (WorldVector[1] > (SCREEN_Y / 2)) {
	  Move[1] = -(SCREEN_Y / 2 - WorldVector[1]);
	  if (Move[1] + (SCREEN_Y / 2) > SCREEN_Y) Move[1] = 0;
	}
	else {
	  Move[1] = WorldVector[1] - SCREEN_Y / 2;
	  if (Move[1] + (SCREEN_Y / 2) <= 0) Move[1] = 0;
	}
  }

  return Move;
}

DWORD64 getFunctionCallAddress(DWORD64 baseAddress, DWORD64 functionAddress) {
  DWORD64 bytes = 0x00;
  bytes += 0xE8;
  DWORD64 sub = functionAddress - baseAddress - 0x5;
  bytes += sub << 8 & 0xff00;
  bytes += sub << 8 & 0xff0000;
  bytes += sub << 8 & 0xff000000;
  bytes += sub << 8 & 0xff00000000;
  return bytes;
}

DWORD64 byteToMem(DWORD64 byte, SIZE_T size) {
  DWORD64 mem = 0x00;
  for (__int64 i = 0; i < 8; i++) {
	mem = mem << 8;
	mem += byte >> (i * 8) & 0xff;
  }

  return mem;
}
