#pragma once

#include "includes.h"
#include "classes.h"

const float  PI_F = 3.14159265358979f;

BOOL B_MINIMAPSPOT(0), B_NORECOIL(0), B_NOSPREAD(0), B_CHAMS(0), B_HARDCORE(0);
BOOL closeThread(0), threadHasClosed(0), aimbotThreadHasClosed(1);

char minimapCall1[5], minimapCall2[5];

__int64 SCREEN_X;
__int64 SCREEN_Y;
__int64 ZeroingDistanceLevel;
__int64 AIMBOT_KEY = 0;
__int64 AIMBOT_TARGET = 1;
__int64 BONE_INDEX = 104;

DWORD64 moduleBase;
DWORD64 GameContext, ClientPlayerManager, ClientPlayer, IdToPlayerMapOffset;
DWORD64 GameRenderer, RenderView;

float ClientFovX, ClientFovY;

vector<vector<float>> ViewMatrixInverse(4, vector<float>(4));
vector<vector<float>> ViewProjection(4, vector<float>(4));
vector<WeaponData> WeaponList;
vector<WeaponData> BackupWeaponList;
vector<EnemyData> EnemyList;

PlayerData LocalPlayer;
WeaponData myWeapon;
