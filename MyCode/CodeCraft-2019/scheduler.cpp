#include "scheduler.h"

static int SchedulerBaseTime=0;

//更新该车的下一结点，需车子已经经过该节点后调用
void UpdateNextNodeAndRoad(Car_t* car)
{
    int i,j,pos;
    int nowNode,nextNode;
    Cross_t *nowCross = nullptr,*nextCross = nullptr;
    queue<int>* planPath=&(car->selectPath);
    if(planPath->size()>0)
    {
        //获取汽车下一个结点ID,并从计划路径中弹出当前结点
        nowNode=planPath->front();
        planPath->pop();
        car->lastCross=car->nextCross;
        nextNode=car->nextCross=planPath->front();
        //获取去往下一个结点所过的道路编号。
        int finded=0;
//        for(auto it=crossSet.begin();it!=crossSet.end();it++)
//        {
//            //原理：左右结点相等可指定出道路
//            if(it->id == nowNode)
//            {    nowCross  = *it;finded++;}
//            if(it->id == nextNode)
//            {    nextCross = *it;finded++;}
//            if(finded==2)
//            {
//                for(int i=0;i<4;i++)
//                {
//                    for(int j=0;j<4;j++)
//                    {
//                        if(nowCross.RoadID[i]==nextCross.RoadID[j]&&nextCross.RoadID[j]!=NEG1)
//                        {
//                            car->lastRoad = static_cast<Road_t*>(car->nextRoad)->id;
//                            car->nextRoad = static_cast<void*>(&roadSet.at(nowCross.RoadID[i]-firstRoadID));
//                            return;
//                        }
//                    }
//                }
//            }
//        }
        for(pos=0;pos!=crossSet.size();pos++)
        {
            //原理：左右结点相等可指定出道路
            if(crossSet.at(pos).id == nowNode)
            {    nowCross  = &crossSet.at(pos);finded++;}
            if(crossSet.at(pos).id == nextNode)
            {    nextCross = &crossSet.at(pos);finded++;}
            if(finded==2)
            {
                for(i=0;i<4;i++)
                {
                    for(j=0;j<4;j++)
                    {
                        if(nowCross->RoadID[i]==nextCross->RoadID[j]&&nextCross->RoadID[j]!=NEG1)
                        {
                            if(car->nextRoad!=nullptr)
                                car->lastRoad = static_cast<Road_t*>(car->nextRoad)->id;
                            car->nextRoad = static_cast<void*>(&roadSet.at(nowCross->RoadID[i]-firstRoadID));
                            return;
                        }
                    }
                }
            }
        }
    }
}

//判断指定小车前往下一条道路的行进方向
//当小车位于路口时调用
void JudgeNextDirection(Cross_t* nowCross, Car_t* car)
{
    //i=当前道路在下一结点所处位置，j=下一道路在下一结点所处位置
    int i=0,j=0,k=0;
    for(i=0;i<4;i++)
        if(car->realRoute.top()==nowCross->RoadID[i])
            break;
    for(j=0;j<4;j++)
    {
        for(k=0;k<4;k++)
        {
            if(nowCross->RoadID[j]==crossSet.at(car->nextCross-1).RoadID[k]&&nowCross->RoadID[j]!=-1)
                goto over;
        }
    }
over:
//    int tmp=j-i;
    int tmp=i-j;
    if(tmp==-1||tmp== 3)car->nextDirection=TURNLEFT;
    if(tmp== 1||tmp==-3)car->nextDirection=TURNRIGHT;
    if(tmp== 2||tmp==-2)car->nextDirection=STRAIGHT;
}

//整体移动车道里的车。
void MoveCarOfChannel(std::vector<Car_t*>* channel,int roadSpeed)
{
    //循环移位，这个函数的调用前提是end端一定是空的
    int realDistant;//头车真实移动距离
    int newPos;//新坐标上限
    int maxPos=channel->size()-1;

    for(int pos=channel->size()-1;pos>=0;pos--)
    {
        if(channel->at(pos)==&emptyCar)//空车不管
            continue;
        else//找到离道路出口最近的车，注意是道路出口
        {
            realDistant=min(channel->at(pos)->topSpeed,roadSpeed);
            newPos=pos+realDistant;
            if(newPos>maxPos)//坐标修正
                newPos=maxPos;

            channel->at(newPos)=channel->at(pos);
            channel->at(pos) = &emptyCar;
            maxPos=newPos-1;
            pos=newPos;
        }
    }
}

//从车道中删除一辆最靠近进结点方向的车并返回，其他车不动位置。
Car_t* PopCarFromChannel(std::vector<Car_t*>*channel,int roadSpeed)
{
    int i=0;
    Car_t* tmpCar;
    int realSpeed;

    //倒序，因为channel增长方向是进结点方向
    for(int pos=channel->size()-1;pos>=0;pos--)
    {
        if(channel->at(pos)!=&emptyCar)
        {
            //剩余可行驶距离大于等于限速，不可能有车出车道，退出该函数
            if(i>=channel->at(pos)->topSpeed || i>=roadSpeed)
                break;
            //获取真实速度
            if(channel->at(pos)->topSpeed<=roadSpeed)realSpeed = channel->at(pos)->topSpeed;
            else realSpeed = roadSpeed;
            tmpCar = channel->at(pos);
            channel->at(pos) = &emptyCar;
            //标记该车剩余可行驶距离
            tmpCar->nextDistance = realSpeed - i;

            return tmpCar;
        }
        i++;
    }
    return &emptyCar;
}

//把调度队列车弹出并压入道路队列，其他车不动位置。
void PushCarToChannel(Cross_t* cross)
{
    int carID;
    int cnt;
    Car_t*  car;
    Road_t* nextRoad;
    bool first=true;
    //是不是认为进入队列的车一定能在单位时间内全走完？因为认为调度队列里同一个优先级的车其实没有先后顺序
    //根据直行、左转、右转优先级逐个调度
    for(int j=0;j<3;j++)
    {
start:
        if(cross->ScheduleQueue[0].size()!=0&&j==1)//进入左转调度但是直行调度还有车，则不应调度
            break;
        if(cross->ScheduleQueue[1].size()!=0&&j==2)//进入右转调度但是左转调度还有车，则不应调度
            break;
        if(cross->ScheduleQueue[j].size()==0)//空队列，跳过。
            continue;
        //提取被调度的车的信息,现在不能把车从调度队列中弹出
        carID = cross->ScheduleQueue[j].front();
        car = &carSet.at(carID-firstCarID);
        nextRoad = static_cast<Road_t*>(car->nextRoad);
        //该节点id属于下一道路的from，则应从0-2车道进
        if(nextRoad->from == cross->id)
        {
            for(int i=0;i<nextRoad->channelNbr;i++)
            {
again:
                cnt=0;
                if(i>=nextRoad->channelNbr)break;
                //遍历车道，找出空了多少个车位
                for(auto it=nextRoad->channel[i].begin();it!=nextRoad->channel[i].end();it++)
                {
                    if(first)//第一个位置就不是空车，直接换车道
                    {
                        first=false;
                        if((*it)!=&emptyCar)
                        {
                            i++;
                            goto again;
                        }
                    }
                    if((*it)!=&emptyCar)
                    {
                        //空车位数量大于能走的距离，可以行走
                        int realDist=min(cnt-1,car->nextDistance-1);
                        realDist=min(realDist,nextRoad->topSpeed-1);
                        if(cnt>=car->nextDistance)
                        {
                            if(cross->ScheduleQueue[j].size()==0)break;
                            carID = cross->ScheduleQueue[j].front();
                            car = &carSet.at(carID-firstCarID);
                            //修改指针
                            (*(it-cnt+realDist))=car;
                            //记录路径
                            car->realRoute.push(nextRoad->id);
                            //上路成功，弹出这辆车
                            cross->ScheduleQueue[j].pop_front();
                            cnt=realDist;
                        }
                        else//否则阻塞，切换车道
                        {
                            i++;
                            goto again;//
                        }
                        break;
                    }
                    cnt++;
                }
                if(cross->ScheduleQueue[j].size()==0)break;
                //空车道，直接上路
                if(cnt==nextRoad->channel[i].size())
                {
                    int realDist=min(car->topSpeed-1,nextRoad->topSpeed-1);
                    //修改车道内车的指针以修改信息。
                    nextRoad->channel[i].at(realDist)=car;
                    //记录路径
                    car->realRoute.push(nextRoad->id);
                    //调度成功，调度队列弹出一辆车
                    cross->ScheduleQueue[j].pop_front();
                    cnt = realDist+1;
                }
                //剩余距离大于下一辆的车速并且车道没填满，则可以再上路一辆
                if(cross->ScheduleQueue[j].size()==0)break;
                goto start;
//                carID = cross->ScheduleQueue[j].front();
//                car = &carSet.at(carID-firstCarID);
//                nextRoad = static_cast<Road_t*>(car->nextRoad);
//                if(cnt-1>=min(car->nextDistance,nextRoad->topSpeed)&&(nextRoad->channel[i].at(0))==&emptyCar)
//                    goto again;
            }
        }
        else if(nextRoad->to == cross->id)
        {
            for(int i=MAXCHANNEL;i<MAXCHANNEL+nextRoad->channelNbr;i++)
            {
again2:
                cnt=0;
                if(i>=(MAXCHANNEL+nextRoad->channelNbr))break;
                //遍历车道，找出空了多少个车位
                for(auto it=nextRoad->channel[i].begin();it!=nextRoad->channel[i].end();it++)
                {
                    if(first)//第一个位置就不是空车，直接换车道
                    {
                        first=false;
                        if((*it)!=&emptyCar)
                        {
                            i++;
                            goto again2;
                        }
                    }
                    if((*it)!=&emptyCar)
                    {
                        //空车位数量大于能走的距离，可以行走
                        int realDist=min(cnt-1,car->nextDistance-1);
                        realDist=min(realDist,nextRoad->topSpeed-1);
                        if(cnt>=car->nextDistance)
                        {
                            if(cross->ScheduleQueue[j].size()==0)break;
                            carID = cross->ScheduleQueue[j].front();
                            car = &carSet.at(carID-firstCarID);
                            //修改指针
                            (*(it-cnt+realDist))=car;
                            //记录路径
                            car->realRoute.push(nextRoad->id);
                            //上路成功，弹出这辆车
                            cross->ScheduleQueue[j].pop_front();
                            cnt=realDist;
                        }
                        else//否则阻塞，切换车道
                        {
                            i++;
                            goto again2;//
                        }
                        break;
                    }
                    cnt++;
                }
                if(cross->ScheduleQueue[j].size()==0)break;
                //空车道，直接上路
                if(cnt==nextRoad->channel[i].size())
                {
                    int realDist=min(car->topSpeed-1,nextRoad->topSpeed-1);
                    //修改车道内车的指针以修改信息。
                    nextRoad->channel[i].at(realDist)=car;
                    //记录路径
                    car->realRoute.push(nextRoad->id);
                    //调度成功，调度队列弹出一辆车
                    cross->ScheduleQueue[j].pop_front();
                    cnt = realDist+1;
                }
                //剩余距离大于下一辆的车速并且车道没填满，则可以再上路一辆,否则跳出循环
                if(cross->ScheduleQueue[j].size()==0)break;
                goto start;
//                carID = cross->ScheduleQueue[j].front();
//                car = &carSet.at(carID-firstCarID);
//                nextRoad = static_cast<Road_t*>(car->nextRoad);
//                if(cnt-1>=min(car->nextDistance,nextRoad->topSpeed)&&(nextRoad->channel[i].at(0))==&emptyCar)
//                    goto again2;
            }
        }
    }
}

//把车ID压入结点的调度队列
//popedCar必须是进该node的车,且必须是未完成路线的车。
void EnterScheduleQueue(Cross_t* node,Car_t* popedCar)
{
    switch (popedCar->nextDirection) {
    case STRAIGHT :node->ScheduleQueue[STRAIGHT].push_back(popedCar->id);break;
    case TURNLEFT :node->ScheduleQueue[TURNLEFT].push_back(popedCar->id);break;
    case TURNRIGHT:node->ScheduleQueue[TURNRIGHT].push_back(popedCar->id);break;
    }
}

//车进结点并进入出结点调度队列或者到达目的地
void EnterCross(Cross_t* node)
{
    int roadID;
    Road_t* road;
    Car_t* car;

    //结点道路循环
    for(int i=0;i<4;i++)
    {
        roadID = node->RoadID[i];
        if(roadID == -1) continue;//无效道路
        road=&roadSet.at(roadID-firstRoadID);

        //结点ID等于道路的右结点，0-2车道是进结点车道
        if(node->id==road->to)
        {
            //车道循环
            for(int j=0;j<road->channelNbr;j++)
            {
                //从道路中弹出一辆车
                car = PopCarFromChannel(&road->channel[j],road->topSpeed);
                if(car==&emptyCar)continue;
                //该路口是否是该车终点，不是则判断方向并进入出结点调度队列
                if(car->to == node->id)
                {
                    car->isOver=true;
                    unfinishedCarCnt--;
                }
                else
                {
                    UpdateNextNodeAndRoad(car);//更新车辆的结点信息
                    JudgeNextDirection(node,car);//更新方向
                    EnterScheduleQueue(node,car);//进入调度队列
                }
            }
        }
        else if(node->id==road->from)//结点ID等于道路的左结点，3-5车道是进结点车道
        {
            //车道循环
            for(int j=MAXCHANNEL;j<MAXCHANNEL+road->channelNbr;j++)
            {
                //弹出一辆车
                car = PopCarFromChannel(&road->channel[j],road->topSpeed);
                if(car==&emptyCar)continue;
                //该路口是否是该车终点，不是则判断方向并进入出结点调度队列
                if(car->to == node->id)
                {
                    car->isOver=true;
                    unfinishedCarCnt--;
                }
                else
                {
                    UpdateNextNodeAndRoad(car);//更新车辆的结点信息
                    JudgeNextDirection(node,car);//更新方向
                    EnterScheduleQueue(node,car);//进入调度队列
                }
            }
        }
    }
}

//小车上路,需要先调用更新结点函数
void DriveCarInChannel(Cross_t* cross)
{
    int carID;
    Car_t* car;
    Road_t* nextRoad;
    int emptyCnt=0;//空车位计数
start:
    //车站里没车则不用调度该函数
    if(cross->CarStation.size()==0)return;
    //获取需要上路的车
    carID = cross->CarStation.front();
    car = &carSet.at(carID-firstCarID);
    nextRoad = static_cast<Road_t*>(car->nextRoad);
    //获得方向
    //该节点id属于下一道路的from，则应从0-2车道进
    if(nextRoad->from == cross->id)
    {
        for(int i=0;i<nextRoad->channelNbr;i++)
        {
again:
            emptyCnt=0;
            if(i>=nextRoad->channelNbr)break;
            //遍历车道，找出空了多少个车位
            for(auto it=nextRoad->channel[i].begin();it!=nextRoad->channel[i].end();it++)
            {
                //第一个就不是空的那么该车道就肯定塞不进了,直接换车道
                if(nextRoad->channel[i].at(0)!=&emptyCar)
                {
                    i++;
                    goto again;
                }
                //找到第一个不为空的车位，前一个就是空车位
                if((*it)!=&emptyCar)
                {
                    //空车位数量大于能走的距离，可以行走
                    int realDist=min(emptyCnt-1,car->topSpeed-1);
                    realDist=min(realDist,nextRoad->topSpeed-1);
                    if(emptyCnt>=min(car->topSpeed,nextRoad->topSpeed))
                    {
                        if(cross->CarStation.size()==0)return;
                        carID = cross->CarStation.front();
                        car = &carSet.at(carID-firstCarID);
                        //修改指针
                        (*(it-emptyCnt+realDist))=car;
                        //记录出发时间
                        carSet.at(carID-firstCarID).runTime=SchedulerBaseTime;
                        //记录路径
                        car->realRoute.push(nextRoad->id);
                        //上路成功，弹出这辆车
                        cross->CarStation.pop();
                        emptyCnt=realDist;

                    }
                    else//否则阻塞，切换车道
                    {
                        i++;
                        goto again;
                    }
                    break;
                }
                emptyCnt++;
            }
            if(cross->CarStation.size()==0)return;
            //cnt等于车道长度则该车道无车，直接上路
            if(emptyCnt==nextRoad->channel[i].size())
            {
                int realDist=min(car->topSpeed-1,nextRoad->topSpeed-1);
                //记录出发时间
                carSet.at(carID-firstCarID).runTime=SchedulerBaseTime;
                //修改指针
                nextRoad->channel[i].at(realDist)=car;
                //记录路径
                car->realRoute.push(nextRoad->id);
                //弹出该车
                cross->CarStation.pop();
                //修改计数
                emptyCnt=car->topSpeed;
            }
            //剩余距离大于下一辆的车速并且车道没填满，则可以再上路一辆
            if(cross->CarStation.size()==0)return;
//            carID = cross->CarStation.front();
//            car = &carSet.at(carID-firstCarID);
//            nextRoad = static_cast<Road_t*>(car->nextRoad);
//            if(emptyCnt-1>=min(car->topSpeed,nextRoad->topSpeed)&&(nextRoad->channel[i].at(0))==&emptyCar)
//                goto again;
            goto start;
        }
    }
    else if(nextRoad->to == cross->id)
    {
        for(int i=MAXCHANNEL;i<MAXCHANNEL+nextRoad->channelNbr;i++)
        {
again2:
            emptyCnt=0;
            if(i>=(MAXCHANNEL+nextRoad->channelNbr))break;
            //遍历车道，找出空了多少个车位
            for(auto it=nextRoad->channel[i].begin();it!=nextRoad->channel[i].end();it++)
            {
                if(nextRoad->channel[i].at(0)!=&emptyCar)
                {
                    i++;
                    goto again2;
                }
                //找到第一个不为空的车位，前一个就是空车位
                if((*it)!=&emptyCar)
                {
                    //空车位数量大于能走的距离，可以行走
                    int realDist=min(emptyCnt-1,car->topSpeed-1);
                    realDist=min(realDist,nextRoad->topSpeed-1);
                    if(emptyCnt>=min(car->topSpeed,nextRoad->topSpeed))
                    {
                        if(cross->CarStation.size()==0)return;
                        carID = cross->CarStation.front();
                        car = &carSet.at(carID-firstCarID);
                        //修改指针
                        (*(it-emptyCnt+realDist))=car;
                        //记录出发时间
                        carSet.at(carID-firstCarID).runTime=SchedulerBaseTime;
                        //记录路径
                        car->realRoute.push(nextRoad->id);
                        //上路成功，弹出这辆车
                        cross->CarStation.pop();
                        emptyCnt=realDist;
                    }
                    else//否则阻塞，切换车道
                    {
                        i++;
                        goto again2;
                    }
                    break;
                }
                emptyCnt++;
            }
            if(cross->CarStation.size()==0)return;
            //cnt等于车道长度则该车道无车，直接上路
            if(emptyCnt==nextRoad->channel[i].size())
            {
                int realDist=min(car->topSpeed-1,nextRoad->topSpeed-1);
                //记录出发时间
                carSet.at(carID-firstCarID).runTime=SchedulerBaseTime;
                //修改指针
                nextRoad->channel[i].at(realDist)=car;
                //记录路径
                car->realRoute.push(nextRoad->id);
                //弹出该车
                cross->CarStation.pop();
                //修改计数
                emptyCnt=car->topSpeed;
            }
            //剩余距离大于车速并且车道没填满，则可以再上路一辆
            if(cross->CarStation.size()==0)return;
//            carID = cross->CarStation.front();
//            car = &carSet.at(carID-firstCarID);
//            nextRoad = static_cast<Road_t*>(car->nextRoad);
//            if(emptyCnt-1>=min(car->topSpeed,nextRoad->topSpeed)&&(nextRoad->channel[i].at(0))==&emptyCar)
//                goto again2;
            goto start;
        }
    }
}

void Scheduler(void)
{
    cout<<"Start scheduler."<<endl;
    cout<<"Size of car:"<<unfinishedCarCnt<<endl;
    while(unfinishedCarCnt)
    {
        SchedulerBaseTime++;
        cout<<"ScheduleTimes:"<<SchedulerBaseTime<<" "<<"UnfinishedCar:"<<unfinishedCarCnt<<endl;
        //车进结点
        for(auto it=crossSet.begin();it!=crossSet.end();it++)
        {
            EnterCross(&(*it));
        }
        //移动道路的车
        for(auto it=roadSet.begin();it!=roadSet.end();it++)
        {
            for(int i=0;i<it->channelNbr;i++)
                MoveCarOfChannel(&it->channel[i],it->topSpeed);
            if(it->isDuplex)//双向
            {
                for(int i=MAXCHANNEL;i<MAXCHANNEL+it->channelNbr;i++)
                    MoveCarOfChannel(&it->channel[i],it->topSpeed);
            }
        }
        //车进道路
        for(auto it=crossSet.begin();it!=crossSet.end();it++)
        {
            PushCarToChannel(&(*it));
            DriveCarInChannel(&(*it));
        }
    }
    cout<<"ScheduleTimes:"<<SchedulerBaseTime++<<" "<<"UnfinishedCar:"<<unfinishedCarCnt<<endl;
}
