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
    /**
     * Order: 
     * id: 5
     * symbol: find
     * side: BUY
     * quantity: 5000
     * price: 100
     */
    OrderEngine::Order* order = new OrderEngine::Order(id,symbol,side,q,price);
    std::cout<<"Printing [Order]: "<<order->ToString()<<std::endl;
    
    const OrderEngine::Base::OrderId id2 = 15;
    OrderEngine::Base::Symbol symbol2 = "find";
    const OrderEngine::Base::OrderSide side2 = OrderEngine::Base::OrderSide::SELL;
    OrderEngine::Base::Quantity q2 = 5000;
    OrderEngine::Base::Price price2 = 100;
    /**
     * Order: 
     * id: 5
     * symbol: find
     * side: SELL
     * quantity: 4000
     * price: 100
     */
    OrderEngine::Order* order2 = new OrderEngine::Order(id2,symbol2,side2,q2,price2);

    OrderEngine::PriceTracker<OrderEngine::Order*> priceTracker(price);
    auto orderIterator = priceTracker.AddOrder(order);
    std::cout<<"[Order:TotalQuantity]: "<<priceTracker.GetTotalQuantity()<<std::endl;
    std::cout<<"[Order:TotalOrder]: "<<priceTracker.GetOrderCount()<<std::endl;
    std::cout<<"[Order:Price] "<<priceTracker.GetPrice()<<std::endl;

    std::cout<<"[ACTION]: Filling 4000 shared from the order"<<std::endl;
    priceTracker.FillQuantity(4000);

    std::cout<<"[Order:TotalQuantity]: "<<priceTracker.GetTotalQuantity()<<std::endl;
    std::cout<<"[Order:TotalOrder]: "<<priceTracker.GetOrderCount()<<std::endl;
    std::cout<<"[Order:Price] "<<priceTracker.GetPrice()<<std::endl;

    std::cout<<"[ACTION]: Removing the current order"<<std::endl;
    priceTracker.RemoveOrder(orderIterator);
    std::cout<<"[Order:OrderCount] "<<priceTracker.GetOrderCount()<<std::endl;
    return 0;
}