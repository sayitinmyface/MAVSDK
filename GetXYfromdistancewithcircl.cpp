#include <math.h>
#include <iostream>
#define PI 3.14159265358979323846
typedef double custem_t;
custem_t degreetoradian(custem_t degree){return ((PI/180)*degree);}
int main(int argc,char ** argv)
{
    if (argc<=1)
    {
        std::cout << "you have entered " << argc << " arguments " << std::endl;
        return -1;
    }
    // 
    custem_t current_x = 10,current_y = 20,distance = 30;
    custem_t radian = degreetoradian(std::stoi(argv[1]));
    custem_t target_x = current_x + distance * cos(radian);
    custem_t target_y = current_y + distance * sin(radian);
    std::cout << " target x :: " << target_x << ", target y :: " << target_y << std::endl;
    return 0;
}