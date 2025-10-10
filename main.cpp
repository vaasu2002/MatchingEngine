#include <iostream>

#include "Order.h"
#include "OrderTypes.h"
#include "OrderTracker/PriceTracker.h"
#include "OrderBook/OrderBook.h"

int main() {
    using namespace OrderEngine;
    // Creating order book for VAA symbol
    Base::Symbol symbol = "VAA";
    OrderBook<Order*> ob(symbol);
    
    // Creating a resting order, that will be sitting in the order book, waiting to be matched.
    const Base::OrderId rid = 42;
    Base::OrderSide rside = Base::OrderSide::SELL;
    Base::Quantity rqty = 4000;
    Base::Price rprice = 100;
    Base::Price rstopPrice = 100;
    Order* restingAsk = new Order(rid, symbol, rside, rqty, rprice,rstopPrice);
    restingAsk->SetType(Base::OrderType::LIMIT);
    ob.addOrder(restingAsk);
    std::cout << "[OrderBook] Seeded resting ASK: id=42, qty=4000 @100\n";

        // Making a MARKET BUY (BID) order with: id 43, at price 100, qty 4000
        // Place a MARKET BUY for qty 3000; should match at 100
        {
            const Base::OrderId bidId = 43;
            Base::OrderSide bside = Base::OrderSide::BUY;
            Base::Quantity bqty = 3000;
            Base::Price dummyPrice = 0; // ignored by market
            Base::Price stopDummyPrice = 0; // ignored by market
            Order* mktBuy = new Order(bidId, symbol, bside, bqty, dummyPrice,stopDummyPrice);
            mktBuy->SetType(Base::OrderType::MARKET);

            Base::OrderConditions conds = Base::NO_CONDITIONS; // Not using any conditions while matching
            bool filled = ob.addOrder(mktBuy, conds); // Trying to match order

            std::cout << "[OrderBook] Market BUY submitted: id=43, qty=3000\n";
            std::cout << "[OrderBook] Matched? " << std::boolalpha << filled << "\n";
            std::cout << "[OrderBook] Market BUY open qty now: " << mktBuy->GetOpenQuantity() << " (expected 0)\n";
            // If you expose getters on OrderBook for last trade price/qty, print them here.
            // e.g., std::cout << "[OrderBook] LastTradePrice: " << ob.GetLastTradePrice() << "\n";

            delete mktBuy;
        }

        // Place another MARKET BUY for qty 2000; should consume remaining resting ASK (1000 left) and cancel remainder
        {
            const Base::OrderId bidId = 44;
            Base::OrderSide bside = Base::OrderSide::BUY;
            Base::Quantity bqty = 2000;
            Base::Price dummyPrice = 0; // ignored by market
            Base::Price stopDummyPrice = 0; // ignored by market
            Order* mktBuy2 = new Order(bidId, symbol, bside, bqty, dummyPrice,stopDummyPrice);
            mktBuy2->SetType(Base::OrderType::MARKET);

            Base::OrderConditions conds = Base::NO_CONDITIONS; // no IOC/AON flags
            bool filled2 = ob.addOrder(mktBuy2, conds);

            std::cout << "[OrderBook] Market BUY submitted: id=44, qty=2000\n";
            std::cout << "[OrderBook] Matched? " << std::boolalpha << filled2 << "\n";
            std::cout << "[OrderBook] Market BUY open qty now: " << mktBuy2->GetOpenQuantity()
                      << " (expected > 0 due to remaining qty being cancelled)\n";

            delete mktBuy2;
        }

    std::cout << "[DONE] All sample flows executed.\n";
    return 0;
}