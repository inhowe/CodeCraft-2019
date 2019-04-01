#include "route.h"

//把路径集合根据源点和汇点映射到各个车辆里
void MapPathSetToCar(void)
{
    int* lastSelect;
//    for(auto it=carSet.begin();it!=carSet.end();it++)
//    {
//        //尽可能的给车分配不同路径，不集中在一个路径
//        lastSelect = &pathSet[it->from][it->to].lastSelect;
//        it->selectPath = pathSet[it->from][it->to].allPath.at(*lastSelect);
//        (*lastSelect)++;
//        if((*lastSelect)==pathSet[it->from][it->to].nbr)
//            (*lastSelect)=0;
//        UpdateNextNodeAndRoad(&(*it));
//        std::cout<<cnt++<<std::endl;
//    }

    for(uint32_t it=0;it<carSet.size();it++)
    {
        //尽可能的给车分配不同路径，不集中在一个路径
//        lastSelect = &(pathSet[carSet.at(it).from][carSet.at(it).to].lastSelect);
//        carSet.at(it).selectPath = pathSet[carSet.at(it).from][carSet.at(it).to].allPath.at(*lastSelect);
//        (*lastSelect)++;
//        if((*lastSelect)==pathSet[carSet.at(it).from][carSet.at(it).to].nbr)
//            (*lastSelect)=0;
//        cout<<cnt++<<endl;
//        UpdateNextNodeAndRoad(&carSet.at(it));
        lastSelect = &(pathSet[carSet.at(it).from][carSet.at(it).to].lastSelect);
        if(pathSet[carSet.at(it).from][carSet.at(it).to].allPath[*lastSelect].size()!=0)
        {
            carSet.at(it).selectPath = pathSet[carSet.at(it).from][carSet.at(it).to].allPath[*lastSelect];
            (*lastSelect)++;
            if((*lastSelect)==4||pathSet[carSet.at(it).from][carSet.at(it).to].allPath[*lastSelect].size())
                (*lastSelect)=0;
            UpdateNextNodeAndRoad(&carSet.at(it));
        }
        else (*lastSelect)=0;
    }
}

//重新给指定车选择路径，通常发生在上路后重新规划路径的情况
void ReMapPathToCar(Car_t* car)
{
//    int* lastSelect;
//    //统计有多少个路径,重新指定路径可能不需要再次统计了不过还是留着先。
//    pathSet[car->from][car->to].nbr = pathSet[car->from][car->to].allPath.size();
//    //尽可能的给车分配不同路径，不集中在一个路径
//    lastSelect = &pathSet[car->from][car->to].lastSelect;
//    car->selectPath = pathSet[car->from][car->to].allPath.at((*lastSelect)++);
//    //所有路径都用过一次了，下标重新回到原点
//    if((*lastSelect)==pathSet[car->from][car->to].nbr)(*lastSelect)=0;

    int* lastSelect;
    //尽可能的给车分配不同路径，不集中在一个路径
    lastSelect = &pathSet[car->from][car->to].lastSelect;
    car->selectPath = pathSet[car->from][car->to].allPath[(*lastSelect)++];
    //所有路径都用过一次了，下标重新回到原点
    if((*lastSelect)==4)(*lastSelect)=0;
}

//生成所有结点的路径图
void GenerateAllPath(void)
{int i=0;
    for(auto it=crossSet.begin();it!=crossSet.end();it++)
        for(auto it2=crossSet.begin();it2!=crossSet.end();it2++)
        {
            //from,to,flow,cost
            if(it->id == it2->id)continue;
            ConstructGraph();
            MincostMaxflow(it->id,it2->id,pathSet[it->id][it2->id].flow,pathSet[it->id][it2->id].cost);
            ExtractPathFromEdgeSet(it->id,it2->id,edgeSet);
            i=0;
            while(!allPath.empty())
            {
                //只取前4个可行路径
                pathSet[it->id][it2->id].allPath[i++]=allPath.back();
                if(i==4)break;
                allPath.pop_back();
            }
        }
}
