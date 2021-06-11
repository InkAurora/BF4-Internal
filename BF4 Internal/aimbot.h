#pragma once

#include "includes.h"
#include "globals.h"
#include "classes.h"
#include "functions.h"
#include "updater.h"

__int64 selectTarget()
{
  __int64 i = 0;
  __int64 selectedIndex = -1;
  float maxAngle = 12;

  SCREEN_X = GetSystemMetrics(SM_CXSCREEN);
  SCREEN_Y = GetSystemMetrics(SM_CYSCREEN);

  updateRenderer();
  updateLocalPlayer();
  updateEnemyList();
  updateWeaponData();

  for (i; i < EnemyList.size(); i++) {
    if (EnemyList[i].Occluded == 0 && EnemyList[i].Health >= 0.000001) {
      if (EnemyList[i].DistanceCH >= 0 && EnemyList[i].DistanceCH < maxAngle) {
        selectedIndex = i;
        maxAngle = EnemyList[i].DistanceCH;
      }
    }
  }

  return selectedIndex;
}

void aimbotThread() {
  while (AIMBOT_KEY > 0) {
    __int64 index = selectTarget();
    if (index >= 0) {
      vector<float> WorldVector = getWorldVector(EnemyList[index]);
      vector<int> Move = getMoveVector(WorldVector);
      DWORD vKey = (AIMBOT_KEY == 1) ? VK_LMENU : (AIMBOT_KEY == 2) ? VK_RBUTTON : 0;
      if (GetAsyncKeyState(vKey)) {
        mouse_event(1, Move[0], Move[1], 0, 0);
      }
    }
    Sleep(1000 / 60);
  }

  aimbotThreadHasClosed = true;
}
