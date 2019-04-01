#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <queue>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <time.h>
#include <algorithm>
#include <deque>
#include <map>

#define MAXNODE 200 //最大结点数

#define NEG1 -1
#define ZERO 0

#define MAXCHANNEL  5

#define STRAIGHT    0
#define TURNLEFT    1
#define TURNRIGHT   2

typedef struct Car_s{
    int id=NEG1;
    int from=NEG1;
    int to=NEG1;
    int topSpeed=NEG1;
    int planTime=NEG1;
    void* nextRoad=nullptr;//下一条规划道路，用的时候要强制转换
    int nextCross=NEG1;//下一个结点
    int nextDirection=NEG1;//下一条规划道路是直行还是左转还是右转
    int nextDistance=ZERO;//下一个道路剩余可以走的距离
    int lastCross=NEG1;
    int lastRoad=NEG1;
    std::stack<int> realRoute;//真实路线
    std::queue<int> selectPath;//计划路线
    int runTime=NEG1;
    int flow=ZERO;
    int cost=ZERO;
    bool isOver=false;
}Car_t;


typedef struct Road_s{
    int id=NEG1;
    int length=NEG1;
    int topSpeed=NEG1;
//    int nowSpeed=NEG1;
    std::vector<Car_t*> channel[MAXCHANNEL*2];//0-2 表示from-to的方向的道路，3-5表示to-from的方向，数组增大方向为车辆方向
    int channelNbr=NEG1;
    int from=NEG1;
    int to=NEG1;
    bool isDuplex=false;//双向标志
    int time=ZERO;//同步时间，小于全局时间则该道路需要更新，且不应大于全局时间
}Road_t;

//道路在结点所处的位置，上右下左
#define TOPPOS     0
#define RIGHTPOS   1
#define DOWNPOS    2
#define LEFTPOS    3
typedef struct Cross_s{
    int id=NEG1;
    int RoadID[4]={NEG1,NEG1,NEG1,NEG1};
    std::queue<int> CarStation;//计划发车,存车的ID即可
    std::deque<int> ScheduleQueue[3];//临时缓存车站,3个缓冲分别表示直行，左拐，右拐
    int time=ZERO;//同步时间，小于全局时间则该结点需要更新，且不应大于全局时间
}Cross_t;

typedef struct Path_s{
//    std::vector<std::queue<int>> allPath;
    std::queue<int> allPath[4];
    int flow=ZERO;
    int cost=ZERO;
    int lastSelect=ZERO;//最后一次被选择的路径位置
//    int nbr;//路径数目统计=allPath.size()
}Path_t;

extern int unfinishedCarCnt;
extern Car_t emptyCar;
extern Road_t emptyRoad;
extern Path_t pathSet[MAXNODE][MAXNODE];//所有路径集合
extern std::vector<Car_t> carSet;
extern std::vector<Road_t> roadSet;
extern std::vector<Cross_t> crossSet;
extern int firstCarID,firstRoadID,firstCrossID;
