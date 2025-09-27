#include <iostream>

#include "Order.h"
#include "OrderTypes.h"
int main()
{
    OrderEngine::Base::Price price = 100;
    OrderEngine::Base::OrderSide side = OrderEngine::Base::OrderSide::BUY;
    OrderEngine::Base::OrderId id = 5;
    OrderEngine::Base::Symbol symbol = "find";
    OrderEngine::Base::Quantity q = 5000;
    const OrderEngine::Order order(id,symbol,side,q,price);
    std::cout<<"Printing: "<<order.ToString()<<std::endl;
    return 0;
}