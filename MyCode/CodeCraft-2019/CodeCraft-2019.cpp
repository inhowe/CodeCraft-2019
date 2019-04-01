#include "common.h"
#include "parasefile.h"
#include "debug.h"
#include "mincostmaxflow.h"
#include "scheduler.h"
#include "route.h"

int unfinishedCarCnt;//未完成的车辆计数
Car_t emptyCar;//空的车辆变量，用于初始化
Road_t emptyRoad;//空的道路变量
Path_t pathSet[MAXNODE][MAXNODE];//所有可行路径 行=from结点，列=to结点
std::vector<Car_t> carSet;
std::vector<Road_t> roadSet;
std::vector<Cross_t> crossSet;
int firstCarID=0,firstRoadID=0,firstCrossID=0;//记录第一个ID号，用于检索访问代替遍历提高速度
clock_t clk;

int main(int argc, char *argv[])
{

    clk = clock();
    std::cout << "Begin" << std::endl;
#ifdef DEBUG
    std::string carPath("../../Materials/1-map-training-4/car.txt");
    std::string roadPath("../../Materials/1-map-training-4/road.txt");
    std::string crossPath("../../Materials/1-map-training-4/cross.txt");
    std::string answerPath("../../Materials/1-map-training-4/answer.txt");

//     carPath="../config_6/car.txt";
//     roadPath="../config_6/road.txt";
//     crossPath="../config_6/cross.txt";
//     answerPath="../config_6/answer.txt";

//     carPath="../../Materials/1-map-training-1/car.txt";
//     roadPath="../../Materials/1-map-training-1/road.txt";
//     crossPath="../../Materials/1-map-training-1/cross.txt";
//     answerPath="../../Materials/1-map-training-1/answer.txt";

//    std::cout << "carPath is " << carPath << std::endl;
//    std::cout << "roadPath is " << roadPath << std::endl;
//    std::cout << "crossPath is " << crossPath << std::endl;
//    std::cout << "answerPath is " << answerPath << std::endl;
#else
    if(argc < 5){
        std::cout << "please input args: carPath, roadPath, crossPath, answerPath" << std::endl;
        exit(1);
    }

    std::string carPath(argv[1]);
    std::string roadPath(argv[2]);
    std::string crossPath(argv[3]);
    std::string answerPath(argv[4]);
#endif

    paraseFile(carPath,roadPath,crossPath);
    std::cout<<"File parased."<<endl;
    ConstructGraph();
    std::cout<<"Graph constructed."<<endl;
    GenerateAllPath();
    std::cout<<"Path generated."<<endl;
    MapPathSetToCar();
    std::cout<<"Path mapped."<<endl;

    Scheduler();

    std::cout<<"Run Time(ms): "<<clock()-clk<<std::endl;

	// TODO:read input filebuf
	// TODO:process
	// TODO:write output file
	
    return 0;
}
