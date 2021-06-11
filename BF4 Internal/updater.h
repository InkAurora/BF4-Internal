#pragma once

#include "includes.h"
#include "globals.h"
#include "classes.h"
#include "functions.h"

void updateRenderer() {
  readBytes(0x142672378, &GameRenderer, 8);
  if (Valid(GameRenderer)) {
    readBytes(GameRenderer + 0x60, &RenderView, 8);
    if (Valid(RenderView)) {
      readBytes(RenderView + 0x250, &ClientFovX, 4);
      readBytes(RenderView + 0xb4, &ClientFovY, 4);
      ViewMatrixInverse = ReadMatrix(RenderView + 0x2e0);
      ViewProjection = ReadMatrix(RenderView + 0x420);
    }
  }
}

void updateLocalPlayer() {
  DWORD64 Character, ClientSoldier, ClientReplication, HealthComponent, ClientVehicleEntity, VehicleHealthData, VehicleEntityData;

  readData();

  PlayerData reset;
  LocalPlayer = reset;

  if (Valid(ClientPlayer)) {
    readBytes(ClientPlayer + 0x14b0, &Character, 8);
    readBytes(ClientPlayer + 0x14c0, &ClientVehicleEntity, 8);
    if (Valid(Character)) {
      readBytes(Character, &ClientSoldier, 8);
      ClientSoldier -= 0x8;
      if (Valid(ClientSoldier)) {
        readBytes(ClientSoldier + 0x490, &ClientReplication, 8);
        readBytes(ClientSoldier + 0x140, &HealthComponent, 8);
        if (Valid(ClientReplication) && Valid(HealthComponent)) {
          LocalPlayer.Name = ReadString(ClientPlayer + 0x40, 64);
          readBytes(ClientPlayer + 0x13cc, &LocalPlayer.TeamID, 1);
          readBytes(ClientSoldier + 0x4d8, &LocalPlayer.Yaw, 4);
          LocalPlayer.Position = ReadVector(ClientReplication + 0x30);
          LocalPlayer.Velocity = ReadVector(ClientReplication + 0x50);
          readBytes(HealthComponent + 0x20, &LocalPlayer.Health, 4);
          readBytes(HealthComponent + 0x24, &LocalPlayer.HealthMax, 4);
        }
      }
    }

    if (Valid(ClientVehicleEntity)) {
      readBytes(ClientVehicleEntity + 0x140, &VehicleHealthData, 8);
      readBytes(ClientVehicleEntity + 0x30, &VehicleEntityData, 8);
      if (Valid(VehicleHealthData) && Valid(VehicleEntityData)) {
        LocalPlayer.VehicleVelocity = ReadVector(ClientVehicleEntity + 0x280);
        readBytes(VehicleHealthData + 0x38, &LocalPlayer.VehicleHealth, 4);
        readBytes(VehicleEntityData + 0x148, &LocalPlayer.VehicleHealthMax, 4);
      }
    }
  }
}

void updateEnemyList() {
  DWORD64 pPlayer, pCharacter, pSoldier, pReplication, pHealthComponent;

  if (EnemyList.size() > 0) EnemyList.clear();

  EnemyData enemy;
  for (__int64 i = 0; i < 70; i++) {
    readBytes(IdToPlayerMapOffset + (i * 0x8), &pPlayer, 8);
    if (Valid(pPlayer)) {
      readBytes(pPlayer + 0x13cc, &enemy.TeamID, 1);

      if (enemy.TeamID > 0 && enemy.TeamID != LocalPlayer.TeamID) {
        readBytes(pPlayer + 0x14b0, &pCharacter, 8);
        if (Valid(pCharacter)) {
          readBytes(pCharacter, &pSoldier, 8);
          pSoldier -= 0x8;
          if (Valid(pSoldier)) {
            readBytes(pSoldier + 0x490, &pReplication, 8);
            readBytes(pSoldier + 0x140, &pHealthComponent, 8);
            if (Valid(pReplication) && Valid(pHealthComponent)) {
              enemy.Name = ReadString(pPlayer + 0x40, 64);
              enemy.Position = ReadVector(pReplication + 0x30);
              enemy.Velocity = ReadVector(pReplication + 0x50);
              enemy.SoldierPointer = pSoldier;
              readBytes(pSoldier + 0x5b1, &enemy.Occluded, 1);
              readBytes(pSoldier + 0x4f0, &enemy.PoseType, 4);
              readBytes(pHealthComponent + 0x20, &enemy.Health, 4);
              enemy.DistanceCH = CrosshairDistance(enemy.Position, ViewMatrixInverse);
              EnemyList.push_back(enemy);
            }
          }
        }
      } else {
        continue;
      }
    }
  }
}

void updateWeaponData(DWORD64 ptr = 0) {
  DWORD64 CurrentWeapon, CFiring, WeaponName, CurrentWeaponName;
  DWORD64 PrimaryFire, FiringFunctionData, BulletEntityData, WeaponModifier, isSilenced, WeaponZeroingModifier, pModes;

  CurrentWeapon = ptr;
  if (!ptr) CurrentWeapon = getWeaponPointer();
  WeaponData weapon;
  if (CurrentWeapon == 0xcccccccc || !Valid(CurrentWeapon)) return;
  if (Valid(CurrentWeapon)) {
    readBytes(CurrentWeapon + 0x30, &WeaponName, 8);
	if (Valid(WeaponName)) {
      readBytes(WeaponName + 0x130, &CurrentWeaponName, 8);
	  if (Valid(CurrentWeaponName)) {
		weapon.Name = ReadString(CurrentWeaponName, 100);
        weapon.successRead = true;
	  }
	}

    readBytes(CurrentWeapon + 0x49c0, &CFiring, 8);
	if (Valid(CFiring)) {
      readBytes(CFiring + 0x128, &PrimaryFire, 8);
	  if (Valid(PrimaryFire)) {
        readBytes(PrimaryFire + 0x10, &FiringFunctionData, 8);
		if (Valid(FiringFunctionData)) {
		  weapon.Offset = ReadVector(FiringFunctionData + 0x60);
		  weapon.Velocity = ReadVector(FiringFunctionData + 0x80);

          readBytes(FiringFunctionData + 0xb0, &BulletEntityData, 8);
		  if (Valid(BulletEntityData)) {
            readBytes(BulletEntityData + 0x130, &weapon.Gravity, 4);
		  }
		}
	  }

      readBytes(CFiring + 0x1f0, &WeaponModifier, 8);
	  if (Valid(WeaponModifier)) {
        readBytes(WeaponModifier + 0x68, &isSilenced, 8);
		if (Valid(isSilenced)) {
		  weapon.Velocity = ReadVector(isSilenced + 0x20);
		}

        readBytes(WeaponModifier + 0xc0, &WeaponZeroingModifier, 8);
		if (Valid(WeaponZeroingModifier)) {
          readBytes(WeaponZeroingModifier + 0x18, &pModes, 8);
		  if (Valid(pModes)) {
			if (ZeroingDistanceLevel < 5 && ZeroingDistanceLevel >= 0) {
			  vector<float> zeroing = ReadVector(pModes + (ZeroingDistanceLevel * 0x8));
			  weapon.ZeroingRad = zeroing[1];
			}
			else {
			  weapon.ZeroingRad = 0;
			}
		  }
		}
	  }
	}
  }

  myWeapon = weapon;
}
