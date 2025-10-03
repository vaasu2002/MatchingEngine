#include <iostream>

#include "Order.h"
#include "OrderTypes.h"
#include "OrderTracker/PriceTracker.h"
#include "OrderTracker/OrderTracker.h"
#include "OrderBook/OrderBook.h"

int main() {
    using namespace OrderEngine;

    {
        Base::Price price = 100;
        Base::Price stopPrice = 100;
        const Base::OrderSide side = Base::OrderSide::BUY;
        const Base::OrderId id = 5;
        Base::Symbol symbol = "find";
        Base::Quantity qty = 5000;

        Order* order = new Order(id,symbol,side,qty,price,stopPrice);
        std::cout << "Printing [Order]: " << order->ToString() << std::endl;

        delete order;
    }

    {
        Base::Price price = 100;
        Base::Price stopPrice = 100;
        Base::OrderSide side = Base::OrderSide::BUY;
        Base::OrderId id = 6;
        Base::Symbol symbol = "find";
        Base::Quantity qty = 5000;

        Order* order = new Order(id, symbol, side, qty, price,stopPrice);

        PriceTracker<Order*> priceTracker(price);
        auto it = priceTracker.AddOrder(order);
        std::cout << "[PriceTracker] TotalQuantity: " << priceTracker.GetTotalQuantity() << "\n";
        std::cout << "[PriceTracker] OrderCount:    " << priceTracker.GetOrderCount()   << "\n";
        std::cout << "[PriceTracker] Price:         " << priceTracker.GetPrice()        << "\n";

        std::cout << "[ACTION] Filling 4000 shares from the order\n";
        priceTracker.FillQuantity(4000);

        std::cout << "[PriceTracker] TotalQuantity: " << priceTracker.GetTotalQuantity() << "\n";
        std::cout << "[PriceTracker] OrderCount:    " << priceTracker.GetOrderCount()   << "\n";
        std::cout << "[PriceTracker] Price:         " << priceTracker.GetPrice()        << "\n";

        std::cout << "[ACTION] Removing the current order\n";
        priceTracker.RemoveOrder(it);
        std::cout << "[PriceTracker] OrderCount:    " << priceTracker.GetOrderCount()   << "\n";

        delete order;
    }

    // ---------- OrderTracker quick exercise ----------
    {
        Base::Price price = 99;
        Base::Price stopPrice = 100;
        Base::OrderSide side = Base::OrderSide::BUY;
        Base::OrderId id = 7;
        Base::Symbol symbol = "find";
        Base::Quantity qty = 1000;

        Order* order = new Order(id, symbol, side, qty, price,stopPrice);
        OrderTracker<Order*> orderTrackerBuySide(/*isBuySide=*/true);
        orderTrackerBuySide.AddOrder(order);

        delete order;
    }

    // ---------- OrderBook: seed a resting ASK and hit it with a MARKET BUY ----------
    {
        Base::Symbol symbol = "find";
        OrderBook<Order*> ob(symbol);

        // Seed a resting SELL (ASK) at price 100, qty 4000
        {
            const Base::OrderId rid = 42;
            Base::OrderSide rside = Base::OrderSide::SELL;
            Base::Quantity rqty = 4000;
            Base::Price rprice = 100;
            Base::Price rstopPrice = 100;
            Order* restingAsk = new Order(rid, symbol, rside, rqty, rprice,rstopPrice);
            // If your Order exposes setters for type:
            // restingAsk->SetOrderType(Base::OrderType::LIMIT);
            ob.addOrder(restingAsk);
            std::cout << "[OrderBook] Seeded resting ASK: id=42, qty=4000 @100\n";
        }

        // Place a MARKET BUY for qty 3000; should match at 100
        {
            const Base::OrderId bidId = 43;
            Base::OrderSide bside = Base::OrderSide::BUY;
            Base::Quantity bqty = 3000;
            Base::Price dummyPrice = 0; // ignored by market
            Base::Price stopDummyPrice = 0; // ignored by market
            Order* mktBuy = new Order(bidId, symbol, bside, bqty, dummyPrice,stopDummyPrice);
            // mktBuy->SetOrderType(Base::OrderType::MARKET);

            Base::OrderConditions conds = Base::NO_CONDITIONS; // no IOC/AON flags
            bool filled = ob.addOrder(mktBuy, conds);

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
            // mktBuy2->SetOrderType(Base::OrderType::MARKET);

            Base::OrderConditions conds = Base::NO_CONDITIONS; // no IOC/AON flags
            bool filled2 = ob.addOrder(mktBuy2, conds);

            std::cout << "[OrderBook] Market BUY submitted: id=44, qty=2000\n";
            std::cout << "[OrderBook] Matched? " << std::boolalpha << filled2 << "\n";
            std::cout << "[OrderBook] Market BUY open qty now: " << mktBuy2->GetOpenQuantity()
                      << " (expected > 0 due to remaining qty being cancelled)\n";

            delete mktBuy2;
        }
    }

    std::cout << "[DONE] All sample flows executed.\n";
    return 0;
}