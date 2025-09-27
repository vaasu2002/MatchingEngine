#include <iostream>
#include "OrderTypes.h"
int main()
{
    OrderEngine::Base::Price price = 100;
    std::cout<<price<<std::endl;
    std::cout<<"Order matching is running"<<std::endl;
    return 0;
}