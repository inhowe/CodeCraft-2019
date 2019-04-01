#pragma once

#include "common.h"

using namespace std;

const int INF = 0x3f3f3f3f;//定义一个大数表示无穷量，为什么是3f?
const int maxn = 5000;// 定义最大结点数，最好是n的两倍还是1倍呢？

//边
struct edge
{
    //前结点u,后结点v,容量c,流量f,费用cost,该边的id
    int u, v, c, f, cost,id;
    edge(int u, int v, int c, int f, int cost,int id):
        u(u), v(v), c(c), f(f), cost(cost),id(id){}
};

extern vector<edge>edgeSet;//边的集合
extern int nodeNbr, edgeNbr;//结点数，边数
extern vector<queue<int>> allPath;//所有可行路径

int MincostMaxflow(int s, int t, int& flow, int& cost);
void ExtractPathFromEdgeSet(int s,int t,vector<edge> e);
void ConstructGraph(void);
