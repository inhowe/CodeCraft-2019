#include "parasefile.h"

void paraseCarFile(std::string addr)
{
    //#(id,from,to,topspeed,planTime)
    std::ifstream File;
    std::string buff;
    File.open(addr);
    Car_t tmpCar;
    bool first=false;
    while(std::getline(File,buff))
    {
        if(buff[0]=='#')
            continue;
        if(buff.size()==0)break;
        buff=buff.substr(1);
        tmpCar.id = std::atoi(buff.c_str());
        buff=buff.substr(buff.find(',')+1);
        tmpCar.from = std::atoi(buff.c_str());
        buff=buff.substr(buff.find(',')+1);
        tmpCar.to = std::atoi(buff.c_str());
        buff=buff.substr(buff.find(',')+1);
        tmpCar.topSpeed = std::atoi(buff.c_str());
        buff=buff.substr(buff.find(',')+1);
        tmpCar.planTime = std::atoi(buff.c_str());

        if(first==false)//记录第一个ID号
        {
            firstCarID = tmpCar.id;
            first=true;
        }

        carSet.push_back(tmpCar);
//        std::cout<<tmpCar.id<<" "<<tmpCar.from<<" "<<tmpCar.to<<" "<<tmpCar.topSpeed<<" "<<tmpCar.planTime<<std::endl;
    }
    unfinishedCarCnt = carSet.size();
    File.close();
}

void paraseCrossFile(std::string addr)
{
    //#(id,roadId,roadId,roadId,roadId)
    std::ifstream File;
    std::string buff;
    File.open(addr);
    Cross_t tmpCross;
    bool first=false;
    while(std::getline(File,buff))
    {
        if(buff[0]=='#')
            continue;
        if(buff.size()==0)break;
        buff=buff.substr(1);
        tmpCross.id = std::atoi(buff.c_str());
        buff=buff.substr(buff.find(',')+1);
        tmpCross.RoadID[TOPPOS] = std::atoi(buff.c_str());
        buff=buff.substr(buff.find(',')+1);
        tmpCross.RoadID[RIGHTPOS] = std::atoi(buff.c_str());
        buff=buff.substr(buff.find(',')+1);
        tmpCross.RoadID[DOWNPOS] = std::atoi(buff.c_str());
        buff=buff.substr(buff.find(',')+1);
        tmpCross.RoadID[LEFTPOS] = std::atoi(buff.c_str());

        if(first==false)//记录第一个ID号
        {
            firstCrossID = tmpCross.id;
            first=true;
        }

        crossSet.push_back(tmpCross);
    }

    File.close();
}

void paraseRoadFile(std::string addr)
{
    //#(id,length,speed,channel,from,to,isDuplex)
    std::ifstream File;
    std::string buff;
    File.open(addr);
    Road_t tmpRoad;
    bool first=false;
    while(std::getline(File,buff))
    {
        if(buff[0]=='#')
            continue;
        if(buff.size()==0)break;
        buff=buff.substr(1);
        tmpRoad.id = std::atoi(buff.c_str());
        buff=buff.substr(buff.find(',')+1);
        tmpRoad.length = std::atoi(buff.c_str());
        buff=buff.substr(buff.find(',')+1);
        tmpRoad.topSpeed = std::atoi(buff.c_str());
        buff=buff.substr(buff.find(',')+1);
        tmpRoad.channelNbr = std::atoi(buff.c_str());
        buff=buff.substr(buff.find(',')+1);
        tmpRoad.from = std::atoi(buff.c_str());
        buff=buff.substr(buff.find(',')+1);
        tmpRoad.to = std::atoi(buff.c_str());
        buff=buff.substr(buff.find(',')+1);
        tmpRoad.isDuplex = std::atoi(buff.c_str())==1?true:false;

        if(first==false)//记录第一个ID号
        {
            firstRoadID = tmpRoad.id;
            first=true;
        }

        for(int i=0;i<MAXCHANNEL*2;i++)
            tmpRoad.channel[i].clear();

        for(int i=0;i<tmpRoad.length;i++)//填充空的车辆信息表示道路为空
        {
            for(int j=0;j<tmpRoad.channelNbr;j++)
                tmpRoad.channel[j].push_back(&emptyCar);
            if(tmpRoad.isDuplex)
            {
                for(int j=MAXCHANNEL;j<MAXCHANNEL+tmpRoad.channelNbr;j++)
                    tmpRoad.channel[j].push_back(&emptyCar);
            }
        }
        roadSet.push_back(tmpRoad);
    }

    File.close();
}

void paraseFile(std::string carPath,std::string roadPath,std::string crossPath)
{
    paraseCarFile(carPath);
    std::cout<<"Car parased."<<std::endl;
    paraseCrossFile(crossPath);
    std::cout<<"Cross parased."<<std::endl;
    paraseRoadFile(roadPath);
    std::cout<<"Road parased."<<std::endl;
    EnterCarToStation();
    std::cout<<"Car enter station."<<std::endl;
}

//小车分类排序，增序
bool LessSortCarByPlanTime_Functor (Car_t Car1,Car_t Car2) { return (Car1.planTime<Car2.planTime); }
bool LessSortCarByID_Functor (Car_t Car1,Car_t Car2) { return (Car1.id<Car2.id); }
bool LessSortCarByFrom_Functor (Car_t Car1,Car_t Car2) { return (Car1.from<Car2.from); }
bool LessSortCarByTo_Functor (Car_t Car1,Car_t Car2) { return (Car1.to<Car2.to); }
void LessSortCar(uint8_t type)
{
    switch (type) {
        case BYPLANTIME:
        std::sort(carSet.begin(),carSet.end(),LessSortCarByPlanTime_Functor);break;
        case BYID:
        std::sort(carSet.begin(),carSet.end(),LessSortCarByID_Functor);break;
        case BYFROM:
        std::sort(carSet.begin(),carSet.end(),LessSortCarByFrom_Functor);break;
        case BYTO:
        std::sort(carSet.begin(),carSet.end(),LessSortCarByTo_Functor);break;
    }
}

//把车辆放入各自的出发结点的神奇车库里,并按发车时间增序排列,id没有增序排列！
void EnterCarToStation()
{
    LessSortCar(BYPLANTIME);
    for(auto it=carSet.begin();it!=carSet.end();it++)
    {
        crossSet.at(static_cast<uint32_t>(it->from-1)).CarStation.push(it->id);
    }
    LessSortCar(BYID);
}

