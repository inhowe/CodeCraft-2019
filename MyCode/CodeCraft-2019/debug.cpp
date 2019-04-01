#include "debug.h"
using namespace std;

void DBG_PRINT_CAR(Car_t car)
{
    cout<<"Car:"<<endl;
    cout<<"id:"<<car.id<<endl;
    cout<<"to:"<<car.to<<endl;
    cout<<"topSpeed:"<<car.topSpeed<<endl;
    cout<<"planTime:"<<car.planTime<<endl;
//    cout<<"currentSpeed:"<<car.nowSpeed<<endl;
    cout<<"currentPos:"<<car.nextCross<<endl;
    cout<<"runTime:"<<car.runTime<<endl;
}
