#include "mincostmaxflow.h"

vector<edge>edgeSet;//边的集合
int nodeNbr, edgeNbr;//结点数，边数
vector<queue<int>> allPath;//所有可行路径，调用提取路径函数后发生更新

static vector<int>Graph[maxn];//图
static int a[maxn];//找增广路每个点的水流量，状态变量，记录流量
static int p[maxn];//每次找增广路反向记录路径，不知道什么用
static int d[maxn];//SPFA算法的最短路，不懂
static int inq[maxn];//SPFA算法是否在队列中,中间变量，无用


//初始化，n=结点数
void init(int n)
{
    for(int i = 0; i <= n; i++)
    {
        Graph[i].clear();
//        a[i]=p[i]=d[i]=inq[i]=0;
    }
    edgeSet.clear();
}

//添加边，用于构造图
//u-边的左结点，v-边的右结点，c-边的容量，isDuplex-是否双向，cost-边的花费
void addedge(int u, int v, int c, bool isDuplex,int cost,int id)
{

    edgeSet.push_back(edge(u, v, c, 0, cost,id));//正向
    if(isDuplex)
        edgeSet.push_back(edge(v, u, c, 0, cost,id));//反向
    else
        edgeSet.push_back(edge(v, u, 0, 0, cost,id));//反向

    int m = static_cast<int>(edgeSet.size());
    Graph[u].push_back(m - 2);
    Graph[v].push_back(m - 1);
}

//s源点，t汇点，flow流结果，cost费用结果
//中间函数，不用管，但是是核心
bool bellman(int s, int t, int& flow, int& cost)
{
    for(int i = 0; i <= nodeNbr + 1; i++)d[i] = INF;//Bellman算法的初始化
    memset(inq, 0, sizeof(inq));
    d[s] = 0;inq[s] = 1;//源点s的距离设为0，标记入队
    p[s] = 0;a[s] = INF;//源点流量为INF（和之前的最大流算法是一样的）

    queue<int>q;//Bellman算法和增广路算法同步进行，沿着最短路拓展增广路，得出的解一定是最小费用最大流
    q.push(s);
    while(!q.empty())
    {
        int u = q.front();
        q.pop();
        inq[u] = 0;//入队列标记删除
        for(int i = 0; i < Graph[u].size(); i++)
        {
            edge & now = edgeSet[Graph[u][i]];
            int v = now.v;
            if(now.c > now.f && d[v] > d[u] + now.cost)
                //now.c > now.f表示这条路还未流满（和最大流一样）
                //d[v] > d[u] + e.cost Bellman 算法中边的松弛
            {
                d[v] = d[u] + now.cost;//Bellman 算法边的松弛
                p[v] = Graph[u][i];//反向记录边的编号
                a[v] = min(a[u], now.c - now.f);//到达v点的水量取决于边剩余的容量和u点的水量
                if(!inq[v]){q.push(v);inq[v] = 1;}//Bellman 算法入队
            }
        }
    }
    if(d[t] == INF)return false;//找不到增广路
    flow += a[t];//最大流的值，此函数引用flow这个值，最后可以直接求出flow
    cost += d[t] * a[t];//距离乘上到达汇点的流量就是费用
    for(int u = t; u != s; u = edgeSet[p[u]].u)//逆向存边
    {
        edgeSet[p[u]].f += a[t];//正向边加上流量
        edgeSet[p[u] ^ 1].f -= a[t];//反向边减去流量 （和增广路算法一样）
    }
    return true;
}

//最小最大流算法，构造图后调用它即可
//返回最大流的值
//s源点，t汇点，flow最大流，cost返回总费用
//该算法运行结束后，不直接输出路径，但是边集合e的f发生更新，可通过e集合寻找出路线
int MincostMaxflow(int s, int t, int& flow, int& cost)
{
    cost = 0;
    while(bellman(s, t, flow, cost));//由于Bellman函数用的是引用，所以只要一直调用就可以求出flow和cost
    return flow;//返回最大流，cost引用可以直接返回最小费用
}

//从边集合e中提取路径
//s-源点，t-汇点，e边集合
void ExtractPathFromEdgeSet(int s,int t,vector<edge> e)
{
    //清空路径
    allPath.clear();
    queue<int> path,backPath;
    bool ClrFlag=false;
    int nowNode=s,nextNode=s;//也可以理解为左结点和右结点
    path.push(s);
    //找出其中任意一个可行路径
again:
    for(auto it=e.begin();it!=e.end();++it)
    {
        if(it->u==nowNode && it->f>0)//当前结点有流量f>0，说明这是一个有流量的边
        {
            path.push(it->v);//记录该边的右结点
            if(it->v==t)//找到汇点，结束搜索
                break;//search over.
            nowNode=it->v;
            goto again;//记录完成后继续搜索再下一级结点
        }
    }
    allPath.push_back(path);//全部搜完，添加进路径集合

    //搜索其余的可行路径
    //原理：根据路径中的结点展开搜索，搜索目标是当前结点是否存在其他含流的结点
    //     如果有，则在表中把该结点与路径中该结点与下级结点的流归0，表发生更新，goto到前面重新搜索路径
    //     如果无，则把路径中的下级结点作为当前结点，重复这个原理。
    //NOTE：flow归0意味着阻断了这条道路，此时重新搜索路径就一定不会搜索到曾经搜到过的路径
    //      是否会一下子阻断过多道路呢？
    nowNode=path.front();
    path.pop();
    nextNode=path.front();
    backPath=path;
    path = queue<int>();
    path.push(s);
again2:
    for(auto it=e.begin();it!=e.end();++it)
    {
        if(it->u==nowNode && it->v == nextNode)//找到路径中一对上下结点
        {
            ClrFlag = false;
            for(auto it=e.begin();it!=e.end();++it)//判断是否还有其他的下结点也具有流
            {
                if(it->u==nowNode && it->v!=nextNode && it->f>0)
                {
                    ClrFlag = true;
                    break;
                }
            }
            if(ClrFlag)//找到其他结点
            {
                ClrFlag=false;
                it->f=0;//flow清0,边集合发生更新，开始重新搜索
                nowNode=s;
                goto again;
            }
            else //未找到其他结点，选择下级结点作为当前结点，重复这个原理
            {
               nowNode=nextNode;
               backPath.pop();
               nextNode=backPath.front();
               goto again2;
            }
        }
    }
}

//构造一个测试图
void ConstructTestGraph(void)
{
    edgeNbr=0;
    nodeNbr=9;init(nodeNbr);
//    addedge(1,2,1,1);edgeNbr++;
//    addedge(1,3,4,1);edgeNbr++;
//    addedge(3,4,1,1);edgeNbr++;
//    addedge(3,5,3,1);edgeNbr++;
//    addedge(5,6,1,1);edgeNbr++;
//    addedge(5,7,1,1);edgeNbr++;
//    addedge(5,8,1,1);edgeNbr++;
//    addedge(6,9,1,1);edgeNbr++;
//    addedge(7,9,1,1);edgeNbr++;
//    addedge(8,9,1,1);edgeNbr++;
//    addedge(4,9,1,1);edgeNbr++;
//    addedge(2,9,1,1);edgeNbr++;
}

//构造地图
void ConstructGraph(void)
{
    nodeNbr = static_cast<int>(crossSet.size()) ;
    init(nodeNbr);
    allPath.clear();
    edgeNbr=0;
    for(auto it=roadSet.begin();it!=roadSet.end();it++)
    {
        addedge(it->from,it->to,it->channelNbr,it->isDuplex,1,it->id);
        edgeNbr++;
    }
}
