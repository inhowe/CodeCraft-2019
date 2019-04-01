#pragma once

#include "common.h"

#define BYPLANTIME  0
#define BYID        1
#define BYFROM      2
#define BYTO        3

void paraseFile(std::string carPath,std::string roadPath,std::string crossPath);

void LessSortCar(uint8_t type);
void paraseCarFile(std::string addr);
void paraseCrossFile(std::string addr);
void paraseRoadFile(std::string addr);
void EnterCarToStation();


