//
// Created by Vaasu Bisht on 29/09/25.
//

#pragma once
#ifndef ORDER_TRACKER_H
#define ORDER_TRACKER_H

#include <iostream>
#include "PriceTracker.h"
#include "../Order.h"

namespace OrderEngine{

    /**
     * @class OrderTracker 
     * @typedef OrderPtr
     * @brief Manages one side of the order book (all buys/all sells)
     * 
     * @details
     * - Organizes orders into PriceTracker (which we price levels) 
     *   keyed by price. 
     * - Ensures proper price-time priority as it tracks order locations
     *   for fast access and updates.
     * - Supports order matching again incoming trades, and provides quick
     *   access to the best price levels.
     */
    template<typename OrderPtr> class OrderTracker
    {
    public:
    using PriceTrackerPtr = std::shared_ptr<PriceTracker<OrderPtr>>;

        /**
        * @brief Custom comparator for price-based map ordering
        * 
        * Buy side: Higher prices have priority (descending order)
        * Sell side: Lower prices have priority (ascending order)
        */
        struct PriceComparator{
            bool mIsBuySide;
            
            explicit PriceComparator(bool isBuySide = false) 
                : mIsBuySide(isBuySide) {}

            bool operator()(Base::Price a, Base::Price b) const {
                return mIsBuySide ? a > b : a < b;  
            }
        }; // struct PriceComparator

        /**
         * Example:
         * - mPriceTrackerMap[15100] = PriceTracker containing [Order A, Order B, Order C]  // 151.00
         * - mPriceTrackerMap[15050] = PriceTracker containing [Order D, Order E]           // 150.50  
         * - mPriceTrackerMap[15000] = PriceTracker containing [Order F]                    // 150.00
        */
    using PriceTrackerMap = std::map<Base::Price, PriceTrackerPtr, PriceComparator>;
        
        /**
         * Cache for efficient order lookups
         * Location of order in the order book
         * - Key: OrderId
         * - Value: Pair of (Price, Iterator to order in PriceTracker's OrderList
         * 
         * Example:
         * - mOrderLocationMap[12345] = (15100, Iterator to Order A in PriceTracker at 15100)
         * - mOrderLocationMap[12346] = (15100, Iterator to Order B in PriceTracker at 15100)
         */
        using OrderLocationMap = 
            std::map<Base::OrderId, 
               std::pair<Base::Price, typename PriceTracker<OrderPtr>::OrderIterator>>;
        
        // Constructor
        explicit OrderTracker(bool isBuySide); 
        
        // Add an order to the tracker
        void AddOrder(OrderPtr order);
    private:
        PriceTrackerMap mPriceTrackerMap;
        OrderLocationMap mOrderLocationMap;
        bool mIsBuySide; // True if this tracker is for buy orders, false for sell orders

        PriceTrackerPtr getOrCreatePriceTracker(Base::Price price);
    };

    // Explicit template instantiation declaration
    extern template class OrderTracker<Order*>;

} // namespace OrderEngine
#endif // ORDER_TRACKER_H