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

    /**
     * @brief Fill order at this price level up a specified quantity 
     * @details
     * - Example:
     *   This takes buy order and tries to fill it by matching against sell orders.
     * - Tt follows FIFO rule, earlier order gets filled first.
     */
    template <typename OrderPtr> Base::Quantity PriceTracker<OrderPtr>::
    FillQuantity(Base::Quantity maxQty)
    {   
        // Tracks how many orders we have fulfilled so far
        Base::Quantity totalFilled = 0; 

        // Iterator to the first resting order in the orderbook
        auto it = mOrders.begin();

        while( it != mOrders.end() && totalFilled < maxQty )
        {
            auto currRestingOrder = *it;
            
            // Shares available in the resting order
            Base::Quantity sharesAvailable  = currRestingOrder->GetOpenQuantity();

            // Number of shares we can extract from current resting order
            Base::Quantity sharesToFill = std::min(sharesAvailable , maxQty - totalFilled);
            
            if (sharesToFill == 0)
            {
                break;
            }

            // Apply fill
            currRestingOrder->SetOpenQuantity(sharesAvailable - sharesToFill);
            totalFilled += sharesToFill;
            mTotalQuantity -= sharesToFill;
            

            if( currRestingOrder->GetOpenQuantity() == 0 )
            {
                // Incoming order is completely filled
                currRestingOrder->SetOrderStatus(Base::OrderStatus::FILLED);
                
                // Remove this resting order from the list and move to next  
                it = mOrders.erase(it);
                mOrderCount--; // 
            }
            else
            {
                // Incoming order is partially filled 
                currRestingOrder->SetOrderStatus(Base::OrderStatus::PARTIALLY_FILLED);
                it++;
            }
        }

        return totalFilled;
    }

    template class PriceTracker<Order*>;

} // namespace OrderEngine