#include "PriceTracker.h"
#include "../Order.h" 

namespace OrderEngine
{

    template <typename OrderPtr> PriceTracker<OrderPtr>::PriceTracker(Base::Price price)
        : mPrice(price), mTotalQuantity(0), mOrderCount(0) {}

    template <typename OrderPtr> const typename PriceTracker<OrderPtr>::OrderList& PriceTracker<OrderPtr>::
    GetOrders() const
    {
        return mOrders;
    }

    template <typename OrderPtr> bool PriceTracker<OrderPtr>::
    IsEmpty() const
    {
        return mOrders.empty();
    }

    template <typename OrderPtr>Base::Quantity PriceTracker<OrderPtr>::
    GetTotalQuantity() const
    {
        return mTotalQuantity;
    }

    template <typename OrderPtr>Base::Price PriceTracker<OrderPtr>::
    GetPrice() const
    {
        return mPrice;
    }

    template <typename OrderPtr> uint64_t PriceTracker<OrderPtr>::
    GetOrderCount() const
    {
        return mOrderCount;
    }

    template <typename OrderPtr> typename PriceTracker<OrderPtr>::OrderIterator PriceTracker<OrderPtr>::
    AddOrder(const OrderPtr& order)
    {
        mTotalQuantity += order->GetOpenQuantity();
        mOrderCount++;
        return mOrders.insert(mOrders.end(), order);
    }

    template <typename OrderPtr> void PriceTracker<OrderPtr>::
    RemoveOrder(const OrderIterator& itr)
    {
        if(itr != mOrders.end())
        {
            mTotalQuantity -= (*itr)->GetOpenQuantity();
            mOrderCount--;
            mOrders.erase(itr);
        }
    }

    template <typename OrderPtr> void PriceTracker<OrderPtr>::
    UpdateQuantity(const OrderPtr& order, Base::Quantity oldQty, Base::Quantity newQty)
    {
        mTotalQuantity += (newQty-oldQty); // O(1)
    }

    template <typename OrderPtr>
    OrderPtr PriceTracker<OrderPtr>::FrontOrder() const
    {
        return mOrders.empty() ? nullptr : mOrders.front();
    }

    template class PriceTracker<Order*>;

} // namespace OrderEngine