#pragma once

#include "includes.h"
#include "globals.h"
#include "classes.h"
#include "functions.h"

void minimapSpot(BOOL enable = false) {
  DWORD64 address1 = moduleBase + 0x292AEC;
  DWORD64 address2 = moduleBase + 0x2943BE;
  if (enable) {
	writeBytes(address1, (void*)0x9090909090, 5);
	writeBytes(address2, (void*)0x9090909090, 5);
  }
  else {
	writeBytes(address1, (void*)getFunctionCallAddress(address1, moduleBase + 0x2AD530), 5);
	writeBytes(address2, (void*)getFunctionCallAddress(address2, moduleBase + 0x2A95D0), 5);
  }
}

void engineChams(BOOL enable = false) {
  DWORD64 address = moduleBase + 0xF8CE0F;
  DWORD64 address2 = moduleBase + 0x8BB8EB;
  if (enable) {
	writeBytes(address, (void*)0x9090, 2);
	writeBytes(address2, (void*)0x9090, 2);
  } else {
	writeBytes(address, (void*)0x0774, 2);
	writeBytes(address2, (void*)0x0374, 2);
  }
}

void noRecoil(BOOL enable = false) {
  DWORD64 address = moduleBase + 0x2D6DE0;
  if (enable) {
	writeBytes(address, (void*)0x9090909090, 5);
  } else {
	writeBytes(address, (void*)getFunctionCallAddress(address, moduleBase + 0x2D8FD0), 5);
  }
}

void noSpread(BOOL enable = false) {
  DWORD64 address = moduleBase + 0x2D84B8;
  if (enable) {
	writeBytes(address, (void*)byteToMem(0xF30F119BC0010000, 8), 8);
  } else {
	writeBytes(address, (void*)byteToMem(0xF30F1183C0010000, 8), 8);
  }
}

void noHardcoreHUD(BOOL enable = false) {
  DWORD64 base = moduleBase + 0x23717C0;
  readBytes(base, &base, 8);
  if (enable) {
	writeBytes(base + 0x55, (void*)0, 1);
	writeBytes(base + 0x56, (void*)0, 1);
	writeBytes(base + 0x57, (void*)0, 1);
	writeBytes(base + 0x58, (void*)0, 1);
	writeBytes(base + 0x59, (void*)0, 1);
	writeBytes(base + 0x5A, (void*)0, 1);
	writeBytes(base + 0x5D, (void*)0, 1);
  } else {
	writeBytes(base + 0x55, (void*)1, 1);
	writeBytes(base + 0x56, (void*)1, 1);
	writeBytes(base + 0x57, (void*)1, 1);
	writeBytes(base + 0x58, (void*)1, 1);
	writeBytes(base + 0x59, (void*)1, 1);
	writeBytes(base + 0x5A, (void*)1, 1);
	writeBytes(base + 0x5D, (void*)1, 1);
  }
}
