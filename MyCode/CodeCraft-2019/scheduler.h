#pragma once

#include "common.h"
#include "mincostmaxflow.h"
#include "debug.h"
#include "route.h"

void Scheduler(void);
void UpdateNextNodeAndRoad(Car_t* car);
void JudgeNextDirection(Cross_t* nowCross, Car_t* car);
void MoveCarOfChannel(std::vector<Car_t*>* channel,int roadSpeed);
Car_t* PopCarFromChannel(std::vector<Car_t*>* channel,int roadSpeed);
void PushCarToChannel(Cross_t* cross);
void EnterScheduleQueue(Cross_t* node,Car_t* popedCar);
void EnterCross(Cross_t* node);
void DriveCarInChannel(Cross_t* cross);

