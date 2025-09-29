#include <iostream>

#include "Order.h"
#include "OrderTypes.h"
#include "OrderTracker/PriceTracker.h"

int main()
{
    OrderEngine::Base::Price price = 100;
    const OrderEngine::Base::OrderSide side = OrderEngine::Base::OrderSide::BUY;
    const OrderEngine::Base::OrderId id = 5;
    OrderEngine::Base::Symbol symbol = "find";
    OrderEngine::Base::Quantity q = 5000;
    OrderEngine::Order* order = new OrderEngine::Order(id,symbol,side,q,price);
    std::cout<<"Printing: "<<order->ToString()<<std::endl;

    OrderEngine::PriceTracker<OrderEngine::Order*> priceTracker(price);
    auto orderIterator = priceTracker.AddOrder(order);
    std::cout<<priceTracker.GetOrderCount()<<std::endl;
    std::cout<<priceTracker.GetPrice()<<std::endl;
    priceTracker.RemoveOrder(orderIterator);
    std::cout<<priceTracker.GetOrderCount()<<std::endl;
    return 0;
}