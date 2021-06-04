#pragma once

#include "functions.h"

__int64 AIMBOT_KEY = 0;
__int64 AIMBOT_TARGET = 1;
__int64 BONE_INDEX = 104;

class PlayerData {
public:
  string Name = "";
  char TeamID = 0;
  float Yaw = 0;
  vector<float> Position = vector<float>(3, 0);
  vector<float> Velocity = vector<float>(3, 0);
  float Health = 0;
  float HealthMax = 0;
  vector<float> VehicleVelocity = vector<float>(3, 0);
  float VehicleHealth = 0;
  float VehicleHealthMax = 0;
};

class EnemyData {
public:
  string Name = "";
  char TeamID = 0;
  char Occluded = 0;
  int PoseType = 0;
  vector<float> Position = vector<float>(3, 0);
  vector<float> Velocity = vector<float>(3, 0);
  float Health = 0;
  float HealthMax = 0;
  float DistanceCH = 0;
  BOOL IsDead = true;
  __int64 index = -1;

  DWORD64 SoldierPointer = 0;
};

PlayerData LocalPlayer;
EnemyData Target;
WeaponData Weapon;
vector<EnemyData> EnemyList = vector<EnemyData>(70);

vector<float> getBonePosition(DWORD64 address, __int64 boneIndex) {
  DWORD64 RagdollComponent, pQuatTransform;
  char ValidTransform;
  vector<float> Position = vector<float>(3, 0);

  RagdollComponent = *(DWORD64*)(address + 0x580);
  if (Valid(RagdollComponent)) {
    ValidTransform = *(char*)(RagdollComponent + 0xc8);
    if (ValidTransform > 0) {
      pQuatTransform = *(DWORD64*)(RagdollComponent + 0xb0);
      if (Valid(pQuatTransform)) {
        Position = ReadVector(pQuatTransform + (boneIndex * 0x20));
        return Position;
      }
    }
  }

  return { 0, 0, 0 };
}

void updateLocalPlayer() {
  DWORD64 Character, ClientSoldier, ClientReplication, HealthComponent, ClientVehicleEntity, VehicleHealthData, VehicleEntityData;

  if (Valid(ClientPlayer)) {
    Character = *(DWORD64*)(ClientPlayer + 0x14b0);
    ClientVehicleEntity = *(DWORD64*)(ClientPlayer + 0x14c0);
    if (Valid(Character)) {
      ClientSoldier = *(DWORD64*)(Character)-0x8;
      if (Valid(ClientSoldier)) {
        ClientReplication = *(DWORD64*)(ClientSoldier + 0x490);
        HealthComponent = *(DWORD64*)(ClientSoldier + 0x140);
        if (Valid(ClientReplication) && Valid(HealthComponent)) {
          LocalPlayer.Name = ReadString(ClientPlayer + 0x40, 64);
          LocalPlayer.TeamID = *(char*)(ClientPlayer + 0x13cc);
          LocalPlayer.Yaw = *(float*)(ClientSoldier + 0x4d8);
          LocalPlayer.Position = ReadVector(ClientReplication + 0x30);
          LocalPlayer.Velocity = ReadVector(ClientReplication + 0x50);
          LocalPlayer.Health = *(float*)(HealthComponent + 0x20);
          LocalPlayer.HealthMax = *(float*)(HealthComponent + 0x24);
        }
      }
    }

    if (Valid(ClientVehicleEntity)) {
      VehicleHealthData = *(DWORD64*)(ClientVehicleEntity + 0x140);
      VehicleEntityData = *(DWORD64*)(ClientVehicleEntity + 0x30);
      if (Valid(VehicleHealthData) && Valid(VehicleEntityData)) {
        LocalPlayer.VehicleVelocity = ReadVector(ClientVehicleEntity + 0x280);
        LocalPlayer.VehicleHealth = *(float*)(VehicleHealthData + 0x38);
        LocalPlayer.VehicleHealthMax = *(float*)(VehicleEntityData + 0x148);
      }
    }
  }
}

void updateEnemyList() {
  DWORD64 pPlayer, pCharacter, pSoldier, pReplication, pHealthComponent;

  for (__int64 i = 0; i < 70; i++) {
    pPlayer = *(DWORD64*)(IdToPlayerMapOffset + (i * 0x8));
    if (Valid(pPlayer)) {
      EnemyList[i].TeamID = *(char*)(pPlayer + 0x13cc);

      if (EnemyList[i].TeamID > 0 && EnemyList[i].TeamID != LocalPlayer.TeamID) {
        pCharacter = *(DWORD64*)(pPlayer + 0x14b0);
        if (Valid(pCharacter)) {
          pSoldier = *(DWORD64*)(pCharacter) - 0x8;
          if (Valid(pSoldier)) {
            pReplication = *(DWORD64*)(pSoldier + 0x490);
            pHealthComponent = *(DWORD64*)(pSoldier + 0x140);
            if (Valid(pReplication) && Valid(pHealthComponent)) {
              EnemyList[i].Name = ReadString(pPlayer + 0x40, 64);
              EnemyList[i].Position = ReadVector(pReplication + 0x30);
              EnemyList[i].Velocity = ReadVector(pReplication + 0x50);
              EnemyList[i].Occluded = *(char*)(pSoldier + 0x5b1);
              EnemyList[i].SoldierPointer = pSoldier;
              EnemyList[i].PoseType = *(int*)(pSoldier + 0x4f0);
              EnemyList[i].DistanceCH = CrosshairDistance(EnemyList[i].Position, ViewMatrixInverse);
              EnemyList[i].Health = *(float*)(pHealthComponent + 0x20);
              if (EnemyList[i].DistanceCH != Target.DistanceCH) EnemyList[i].IsDead = false;
            }
          }
        }
      }
    }
  }
}

void updateRenderer() {
  GameRenderer = *(DWORD64*)(0x142672378);
  if (Valid(GameRenderer)) {
    RenderView = *(DWORD64*)(GameRenderer + 0x60);
    if (Valid(RenderView)) {
      ClientFovX = *(float*)(RenderView + 0x250);
      ClientFovY = *(float*)(RenderView + 0xb4);
      ViewMatrixInverse = ReadMatrix(RenderView + 0x2e0);
      ViewProjection = ReadMatrix(RenderView + 0x420);
    }
  }
}

__int64 selectTarget()
{
  __int64 i = 0;
  __int64 selectedIndex = -1;
  float maxAngle = 12;

  SCREEN_X = GetSystemMetrics(SM_CXSCREEN);
  SCREEN_Y = GetSystemMetrics(SM_CYSCREEN);

  readData((DWORD64)GetModuleHandle(NULL));
  updateRenderer();
  updateLocalPlayer();
  updateEnemyList();
  for (i; i < EnemyList.size(); i++) {
    if (EnemyList[i].Occluded == 0 && EnemyList[i].Health >= 0.000001) {
      if (EnemyList[i].DistanceCH == Target.DistanceCH) {
        EnemyList[i].IsDead = true;
        Target.IsDead = true;
      }
      if (EnemyList[i].IsDead == false && EnemyList[i].DistanceCH >= 0 && EnemyList[i].DistanceCH < maxAngle) {
        selectedIndex = i;
        maxAngle = EnemyList[i].DistanceCH;
      }
    }
  }

  return selectedIndex;
}

vector<float> getWorldVector(EnemyData Enemy) {
  vector<float> AimVector = getBonePosition(Enemy.SoldierPointer, BONE_INDEX);
  WeaponData myWeapon = getWeaponData();
  if (!myWeapon.successRead) {
    return { 0, 0, 0 };
  }
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
    } else {
      Move[0] = WorldVector[0] - SCREEN_X / 2;
      if (Move[0] + (SCREEN_X / 2) <= 0) Move[0] = 0;
    }
  }
  if (WorldVector[1] != 0) {
    if (WorldVector[1] > (SCREEN_Y / 2)) {
      Move[1] = -(SCREEN_Y / 2 - WorldVector[1]);
      if (Move[1] + (SCREEN_Y / 2) > SCREEN_Y) Move[1] = 0;
    } else {
      Move[1] = WorldVector[1] - SCREEN_Y / 2;
      if (Move[1] + (SCREEN_Y / 2) <= 0) Move[1] = 0;
    }
  }

  return Move;
}

void aimbotThread() {
  while (AIMBOT_KEY > 0) {
    __int64 index = selectTarget();
    if (index >= 0) {
      Target = EnemyList[index];
      Target.index = index;
      vector<float> WorldVector = getWorldVector(Target);
      vector<int> Move = getMoveVector(WorldVector);
      DWORD vKey = (AIMBOT_KEY == 1) ? VK_LMENU : (AIMBOT_KEY == 2) ? VK_RBUTTON : 0;
      if (GetAsyncKeyState(VK_NUMPAD9)) {
        cout << Target.DistanceCH << "v0  " << Target.Velocity[0] << "  v1  " << Target.Velocity[1] << "  v2  " << Target.Velocity[2] << endl;
      }
      if (GetAsyncKeyState(vKey)) {
        mouse_event(1, Move[0], Move[1], 0, 0);
      }
    }
    Sleep(1000 / 60);
  }
  cout << "Aimbot Disabled!" << endl;
  aimbotThreadHasClosed = true;
}
