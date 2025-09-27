#include "PriceTracker.h"
#include "../Order.h" // so "Order" is known for explicit instantiation

namespace OrderEngine
{
    // ---- Template definitions ----
    template <typename OrderPtr>
    PriceTracker<OrderPtr>::PriceTracker(Base::Price price)
        : mPrice(price), mTotalQuantity(0), mOrderCount(0) {}

    template <typename OrderPtr>
    const typename PriceTracker<OrderPtr>::OrderList&
    PriceTracker<OrderPtr>::GetOrders() const
    {
        return mOrders;
    }

    template <typename OrderPtr>
    bool PriceTracker<OrderPtr>::IsEmpty() const
    {
        return mOrders.empty();
    }

    template <typename OrderPtr>
    Base::Quantity PriceTracker<OrderPtr>::GetTotalQuantity() const
    {
        return mTotalQuantity;
    }

    template <typename OrderPtr>
    Base::Price PriceTracker<OrderPtr>::GetPrice() const
    {
        return mPrice;
    }

    template <typename OrderPtr>
    uint64_t PriceTracker<OrderPtr>::GetOrderCount() const
    {
        return mOrderCount;
    }

    // ---- Explicit instantiations for the types you actually use ----
    template class PriceTracker<Order*>; // matches use in main()

    // If you also use smart pointers elsewhere, add them too:
    // template class PriceTracker<std::shared_ptr<Order>>;
}
