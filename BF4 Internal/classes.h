#pragma once

#include "includes.h"

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

  DWORD64 SoldierPointer = 0;
};

class WeaponData {
public:
  string Name = "";
  BOOL successRead = false;
  float ZeroingRad = 0;
  vector<float> Velocity = vector<float>(3, 0);
  vector<float> Offset = vector<float>(3, 0);
  float Gravity = 0;
};