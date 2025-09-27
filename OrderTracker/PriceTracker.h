#pragma once
#ifndef PRICE_TRACKER_H
#define PRICE_TRACKER_H

#include <vector>
#include "../OrderTypes.h"

namespace OrderEngine
{
    template<typename OrderPtr>
    class PriceTracker
    {
    public:
        using OrderList = std::vector<OrderPtr>;
        using OrderIterator = typename OrderList::iterator;

    private:
        Base::Price mPrice = 0;
        OrderList mOrders;
        Base::Quantity mTotalQuantity = 0;
        uint64_t mOrderCount = 0;

    public:
        explicit PriceTracker(Base::Price price);
        Base::Price GetPrice() const;
        Base::Quantity GetTotalQuantity() const;
        uint64_t GetOrderCount() const;
        bool IsEmpty() const;
        const OrderList& GetOrders() const;
    };

    // (Optional but recommended) If you know you’ll use Order* everywhere,
    // declare extern template to prevent implicit instantiation in other TUs.
    class Order; // forward declare
    extern template class PriceTracker<Order*>;
    // If you also use shared_ptr<Order>, add:
    // extern template class PriceTracker<std::shared_ptr<Order>>;
}

#endif // PRICE_TRACKER_H