#pragma once

#pragma warning(disable : 4996)

#include <iostream>
#include <Windows.h>
#include <future>
#include <math.h>

using namespace std;

DWORD64 GameContext, ClientPlayerManager, ClientPlayer, IdToPlayerMapOffset;
DWORD64 GameRenderer, RenderView;
__int64 ZeroingDistanceLevel;
float ClientFovX, ClientFovY;
vector<vector<float>> ViewMatrixInverse(4, vector<float>(4));
vector<vector<float>> ViewProjection(4, vector<float>(4));

__int64 SCREEN_X;
__int64 SCREEN_Y;

const float  PI_F = 3.14159265358979f;

HANDLE hRead, hWrite;

BOOL B_MINIMAPSPOT(0), B_NORECOIL(0), B_NOSPREAD(0);

BOOL closeThread(0), threadHasClosed(0), aimbotThreadHasClosed(1);

class WeaponData {
public:
  float ADSRecoil = 0;
  float HipRecoil = 0;
  float WeaponSpread = 0;
  int Ammo = 0;
  int AmmoInMag = 0;
  string Name = "";
  BOOL successRead = false;
  float ZeroingRad = 0;
  vector<float> Velocity = vector<float>(3, 0);
  vector<float> Offset = vector<float>(3, 0);
  float Gravity = 0;
};

vector<WeaponData> WeaponList;
vector<WeaponData> BackupWeaponList;

string ReadString(DWORD64 address, size_t size) {
  LPCVOID param = (LPCVOID)address;

  char* str = new char[size];
  str = (char*)address;

  string temp(&str[0], &str[size]);
  char* t_str = strtok(&temp[0], "\0");
  if (t_str != nullptr) return t_str;
  return string("");
}

vector<float> ReadVector(DWORD64 vectorAddress) {
  vector<float> vector3;

  vector3.push_back(*(float*)(vectorAddress + 0x0));
  vector3.push_back(*(float*)(vectorAddress + 0x4));
  vector3.push_back(*(float*)(vectorAddress + 0x8));

  return vector3;
}

vector<vector<float>> ReadMatrix(DWORD64 mAddress) {
  vector<vector<float>> matrix(4, vector<float>(4));

  __int64 offset = 0;
  for (__int64 i = 0; i < 4; i++) {
	for (__int64 j = 0; j < 4; j++) {
	  matrix.at(i).at(j) = *(float*)(mAddress + offset);
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

BOOL ValidSlot(__int64 ActiveSlot) {
  return (ActiveSlot >= 0 && ActiveSlot <= 1);
}

void readData(DWORD64 moduleBase) {
  GameContext = *(DWORD64*)(moduleBase + 0x2670D80);
  if (Valid(GameContext)) {
	ClientPlayerManager = *(DWORD64*)(GameContext + 0x60);
	if (Valid(ClientPlayerManager)) {
	  ClientPlayer = *(DWORD64*)(ClientPlayerManager + 0x540);
	  IdToPlayerMapOffset = *(DWORD64*)(ClientPlayerManager + 0x548);
	}
  }
}

void loadGameContext() {
  DWORD64 moduleBase = (DWORD64)GetModuleHandle(NULL);
  while (true) {
	readData(moduleBase);
	if (Valid(IdToPlayerMapOffset)) {
	  cout << "Library Loaded! Game Context: 0x0" << hex << GameContext << endl;
	  return;
	}
	Sleep(200);
  }
}

void minimapSpot() {
  DWORD64 pPlayer, pCharacter, pSoldier, spottingTarget;
  char spotType;

  for (__int64 i = 0; i < 70; i++) {
	pPlayer = *(DWORD64*)(IdToPlayerMapOffset + (i * 0x8));
	if (Valid(pPlayer)) {
	  pCharacter = *(DWORD64*)(pPlayer + 0x14b0);
	  if (Valid(pCharacter)) {
		pSoldier = *(DWORD64*)(pCharacter);
		if (Valid(pSoldier)) {
		  spottingTarget = *(DWORD64*)(pSoldier + 0xbe8);
		  if (Valid(spottingTarget)) {
			spotType = *(char*)(spottingTarget + 0x50);
			if (spotType != 1) *(char*)(spottingTarget + 0x50) = 1;
		  }
		}
	  }
	}
  }
}

int findWeapon(string weaponName) {
  if (WeaponList.size() == 0) return -1;
  for (int i = 0; i < WeaponList.size(); i++) {
	if (WeaponList[i].Name == weaponName) {
	  return i;
	}
  }

  return -1;
}

DWORD64 getWeaponPointer() {
  DWORD64 ClientPlayer, Character, ClientSoldier, SoldierWeaponComponent, CurrentAnimatedWeaponHandler;
  DWORD64 CurrentWeapon = 0;
  __int64 ActiveSlot;
  ClientPlayer = *(DWORD64*)(ClientPlayerManager + 0x540);
  if (Valid(ClientPlayer)) {
	Character = *(DWORD64*)(ClientPlayer + 0x14b0);
	if (Valid(Character)) {
	  ClientSoldier = *(DWORD64*)(Character);
	  if (Valid(ClientSoldier)) {
		SoldierWeaponComponent = *(DWORD64*)(ClientSoldier + 0x568);
		if (Valid(SoldierWeaponComponent)) {
		  CurrentAnimatedWeaponHandler = *(DWORD64*)(SoldierWeaponComponent + 0x890);
		  ActiveSlot = *(int*)(SoldierWeaponComponent + 0xa98);
		  ZeroingDistanceLevel = *(int*)(SoldierWeaponComponent + 0xac8);
		  if (Valid(CurrentAnimatedWeaponHandler) && ValidSlot(ActiveSlot)) {
			CurrentWeapon = *(DWORD64*)(CurrentAnimatedWeaponHandler + (ActiveSlot * 0x8));
		  }
		}
	  }
	}
  }

  return CurrentWeapon;
}

WeaponData getWeaponData(DWORD64 ptr = 0) {
  DWORD64 CurrentWeapon, CFiring, WeaponSway, WeaponSwayData, WeaponName, CurrentWeaponName;
  DWORD64 PrimaryFire, FiringFunctionData, BulletEntityData, WeaponModifier, isSilenced, WeaponZeroingModifier, pModes;

  CurrentWeapon = ptr;
  if (!ptr) CurrentWeapon = getWeaponPointer();
  WeaponData myWeapon;
  if (CurrentWeapon == 0xcccccccc || !Valid(CurrentWeapon)) return myWeapon;
  if (Valid(CurrentWeapon)) {
	WeaponName = *(DWORD64*)(CurrentWeapon + 0x30);
	if (Valid(WeaponName)) {
	  CurrentWeaponName = *(DWORD64*)(WeaponName + 0x130);
	  if (Valid(CurrentWeaponName)) {
		myWeapon.Name = ReadString(CurrentWeaponName, 100);
	  }
	}

	CFiring = *(DWORD64*)(CurrentWeapon + 0x49c0);
	if (Valid(CFiring)) {
	  WeaponSway = *(DWORD64*)(CFiring + 0x78);
	  if (Valid(WeaponSway)) {
		WeaponSwayData = *(DWORD64*)(WeaponSway + 0x8);
		if (Valid(WeaponSwayData)) {
		  myWeapon.ADSRecoil = *(float*)(WeaponSwayData + 0x444);
		  myWeapon.HipRecoil = *(float*)(WeaponSwayData + 0x440);
		  myWeapon.WeaponSpread = *(float*)(WeaponSwayData + 0x430);
		}
	  }

	  myWeapon.Ammo = *(int*)(CFiring + 0x1a0);
	  myWeapon.AmmoInMag = *(int*)(CFiring + 0x1a4);

	  PrimaryFire = *(DWORD64*)(CFiring + 0x128);
	  if (Valid(PrimaryFire)) {
		FiringFunctionData = *(DWORD64*)(PrimaryFire + 0x10);
		if (Valid(FiringFunctionData)) {
		  myWeapon.Offset = ReadVector(FiringFunctionData + 0x60);
		  myWeapon.Velocity = ReadVector(FiringFunctionData + 0x80);

		  BulletEntityData = *(DWORD64*)(FiringFunctionData + 0xb0);
		  if (Valid(BulletEntityData)) {
			myWeapon.Gravity = *(float*)(BulletEntityData + 0x130);
			if (myWeapon.Gravity != 0) myWeapon.successRead = true;
		  }
		}
	  }

	  WeaponModifier = *(DWORD64*)(CFiring + 0x1f0);
	  if (Valid(WeaponModifier)) {
		isSilenced = *(DWORD64*)(WeaponModifier + 0x68);
		if (Valid(isSilenced)) {
		  myWeapon.Velocity = ReadVector(isSilenced + 0x20);
		}

		WeaponZeroingModifier = *(DWORD64*)(WeaponModifier + 0xc0);
		if (Valid(WeaponZeroingModifier)) {
		  pModes = *(DWORD64*)(WeaponZeroingModifier + 0x18);
		  if (Valid(pModes)) {
			if (ZeroingDistanceLevel < 5 && ZeroingDistanceLevel >= 0) {
			  vector<float> zeroing = ReadVector(pModes + (ZeroingDistanceLevel * 0x8));
			  myWeapon.ZeroingRad = zeroing[1];
			}
			else {
			  myWeapon.ZeroingRad = 0;
			}
		  }
		}
	  }
	}
  }

  if (myWeapon.successRead) {
	if (findWeapon(myWeapon.Name) == -1) {
	  WeaponList.push_back(myWeapon);
	  BackupWeaponList.push_back(myWeapon);
	}
	else {
	  int i = findWeapon(myWeapon.Name);
	  WeaponList[i] = myWeapon;
	}
  }

  return myWeapon;
}

void noRecoil(BOOL removeRecoil) {
  DWORD64 CurrentWeapon, CFiring, WeaponSway, WeaponSwayData;

  CurrentWeapon = getWeaponPointer();
  if (Valid(CurrentWeapon)) {
	CFiring = *(DWORD64*)(CurrentWeapon + 0x49c0);
	if (Valid(CFiring)) {
	  WeaponSway = *(DWORD64*)(CFiring + 0x78);
	  if (Valid(WeaponSway)) {
		WeaponSwayData = *(DWORD64*)(WeaponSway + 0x8);
		if (Valid(WeaponSwayData)) {
		  if (removeRecoil) {
			*(float*)(WeaponSwayData + 0x444) = 0;
			*(float*)(WeaponSwayData + 0x440) = 100;
		  }
		  else {
			WeaponData myWeapon = getWeaponData(CurrentWeapon);
			if (!myWeapon.successRead) return;
			int i = findWeapon(myWeapon.Name);
			*(float*)(WeaponSwayData + 0x444) = BackupWeaponList[i].ADSRecoil;
			*(float*)(WeaponSwayData + 0x440) = BackupWeaponList[i].HipRecoil;
		  }
		}
	  }
	}
  }
}

void noSpread(BOOL removeSpread) {
  DWORD64 CurrentWeapon, CFiring, WeaponSway, WeaponSwayData;

  CurrentWeapon = getWeaponPointer();
  CFiring = *(DWORD64*)(CurrentWeapon + 0x49c0);
  WeaponSway = *(DWORD64*)(CFiring + 0x78);
  WeaponSwayData = *(DWORD64*)(WeaponSway + 0x8);
  if (removeSpread) {
	*(float*)(WeaponSwayData + 0x430) = 0;
	*(float*)(WeaponSwayData + 0x434) = 0;
	*(float*)(WeaponSwayData + 0x438) = 0;
	*(float*)(WeaponSwayData + 0x43c) = 0;
  }
  else {
	*(float*)(WeaponSwayData + 0x430) = 0;
	*(float*)(WeaponSwayData + 0x434) = 0;
	*(float*)(WeaponSwayData + 0x438) = 0;
	*(float*)(WeaponSwayData + 0x43c) = 0;
  }
}

void updateWeapon() {
  WeaponData myWeapon = getWeaponData();
  if (!myWeapon.successRead) return;
  if (B_NORECOIL) {
	if (myWeapon.ADSRecoil != 0 || myWeapon.HipRecoil != 100) {
	  noRecoil(true);
	}
  }
  else {
	if (myWeapon.ADSRecoil == 0 && myWeapon.HipRecoil == 100) {
	  noRecoil(false);
	}
  }
  if (B_NOSPREAD) {
	if (myWeapon.WeaponSpread == 1) {
	  noSpread(true);
	}
  }
  else {
	if (myWeapon.WeaponSpread == 0) {
	  noSpread(false);
	}
  }
}

void mainFlow() {
  while (1) {
	if (B_MINIMAPSPOT) minimapSpot();
	updateWeapon();
	if (closeThread) {
	  threadHasClosed = true;
	  return;
	}
	Sleep(16);
  }
}
