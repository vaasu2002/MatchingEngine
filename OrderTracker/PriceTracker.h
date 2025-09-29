#pragma once
#ifndef PRICE_TRACKER_H
#define PRICE_TRACKER_H

#include <vector>
#include "../OrderTypes.h"

namespace OrderEngine
{
    /**
     * @brief Represents a single price point in the order book. 
     * 
     * @details
     * - PriceTracker groups all the active orderes submitted at that price.
     * - It maintains both the list of orders (FIFO by entry time) and 
     *   aggregate statistics like total open quantity and order count. 
     * - Think of an orderbook like a building with floors, where each floor represents a different price.
     */
    template<typename OrderPtr> class PriceTracker
    {

    public:
        using OrderList = std::vector<OrderPtr>; 
        using OrderIterator = typename OrderList::iterator;

    private:
        Base::Price mPrice = 0; // Price to which this tracker(OrderList) corresponds 
        OrderList mOrders; 
        Base::Quantity mTotalQuantity = 0; // Total quantity of all orders at this price
        uint64_t mOrderCount = 0; // Total number of orders at this price

    public:
        explicit PriceTracker(Base::Price price);
        Base::Price GetPrice() const;
        Base::Quantity GetTotalQuantity() const;
        uint64_t GetOrderCount() const;
        bool IsEmpty() const;
        // Returns the list of orders at this price
        const OrderList& GetOrders() const;

        /**
         * @brief Adds a new order to the list of tracked orders.
         */
        OrderIterator AddOrder(const OrderPtr& order);
        
        /**
         * @brief Removes an order from the list of tracked orders.
         * 
         * @details
         * - This is typically called when an order is fully filled or cancelled.
         * - It updates the total quantity and order count accordingly.
         */
        void RemoveOrder(const OrderIterator& it);

        void UpdateQuantity(const OrderPtr& order, Base::Quantity oldQty, Base::Quantity newQty);

        OrderPtr FrontOrder() const;
    };

    class Order; // forward declare
    extern template class PriceTracker<Order*>;
}

#endif // PRICE_TRACKER_H