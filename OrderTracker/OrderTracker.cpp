#include "OrderTracker.h"

namespace OrderEngine{

    template <typename OrderPtr> OrderTracker<OrderPtr>::OrderTracker(bool isBuySide)
        : mIsBuySide(isBuySide), mPriceTrackerMap(PriceComparator(isBuySide)) {}

    template<typename OrderPtr> void OrderTracker<OrderPtr>::
    AddOrder(OrderPtr order)
    {
        if(!order)
        {
            // todo: log
            return;
        }

        Base::OrderId orderId = order->GetId();
        Base::Price price = order->GetPrice();

        // Check if the order already exists.
        if( mOrderLocationMap.find(orderId) != mOrderLocationMap.end() )
        {   
            // Order already exsits
            // todo: log
            return;
        }

        PriceTrackerPtr priceTracker = getOrCreatePriceTracker(price);

        // Add order to the  PriceTracker and get the iterator
        auto orderIterator = priceTracker->AddOrder(order);

        // Cache the order's location
        mOrderLocationMap[orderId] = std::make_pair(price,orderIterator);

        std::cout<<"[INFO][OrderTracker][AddOrder]: Size of mOrderLocationMap= "<<mOrderLocationMap.size()<<std::endl;
    }

    template <typename OrderPtr> typename 
    OrderTracker<OrderPtr>::PriceTrackerPtr
    OrderTracker<OrderPtr>:: getOrCreatePriceTracker(Base::Price price)
    {
        // Finding existing PriceTracker
        auto it = mPriceTrackerMap.find(price);


        if (it != mPriceTrackerMap.end()) 
        {
            // Found the PriceTracker in map
            std::cout<<"[INFO][OrderTracker][getOrCreatePriceTracker]: PriceTracker already exsits.  "<<std::endl;
            return it->second;
        }

        std::cout<<"[INFO][OrderTracker][getOrCreatePriceTracker]: PriceTracker created.  "<<std::endl;

        // Not able to find PriceTracker, creating a new one
        auto newPriceTracker = std::make_shared<PriceTracker<OrderPtr>>(price);
        
        // Storing the newly created PriceTracker in map
        mPriceTrackerMap[price] = newPriceTracker;
        
        return newPriceTracker;
    }


    template <typename OrderPtr>
    void OrderTracker<OrderPtr>::RemoveOrder(OrderPtr order)
    {
        if (!order)
        {
            // todo: log error
            return;
        }

        Base::OrderId orderId = order->GetId();
        // Find the order's location in the cache
        auto locationIt = mOrderLocationMap.find(orderId);

        if (locationIt == mOrderLocationMap.end())
        {
            // Order not found in tracker
            // todo: log warning - trying to remove a non-existent order
            return;
        }
        // Extract price and order iterator from the cached location
        Base::Price price = locationIt->second.first;
        auto orderIterator = locationIt->second.second;

        // Find the PriceTracker at this price level
        auto priceTrackerIt = mPriceTrackerMap.find(price);
        mOrderLocationMap.erase(locationIt);

        if (priceTrackerIt == mPriceTrackerMap.end())
        {
            // Price level not found (should never happen if cache is consistent)
            // todo: log error - inconsistent state
            mOrderLocationMap.erase(locationIt);
            return;
        }

        PriceTrackerPtr priceTracker = priceTrackerIt->second;

        // Remove the order from the PriceTracker's order list
        priceTracker->RemoveOrder(orderIterator);

        // Remove from location cache
        mOrderLocationMap.erase(locationIt);

        // 
        if (priceTracker->IsEmpty())
        {
            mPriceTrackerMap.erase(priceTrackerIt);
        }
    }

    template <typename OrderPtr>
    std::vector<std::pair<OrderPtr, Base::Quantity>> OrderTracker<OrderPtr>::MatchQuantity(Base::Price limitPrice, Base::Quantity maxQty)
    {
        std::vector<std::pair<OrderPtr, Base::Quantity>> matches;
        Base::Quantity remaining = maxQty;

        auto it = mPriceTrackerMap.begin();
        while (it != mPriceTrackerMap.end() && remaining > 0) {
            Base::Price level_price = it->first;

            // Check if this price level can match
            bool can_match = mIsBuySide ? (level_price >= limitPrice) : (level_price <= limitPrice);
            if (!can_match) break;

            auto level = it->second;
            auto& orders = level->GetOrders();
            auto order_it = orders.begin();

            while (order_it != orders.end() && remaining > 0) {
                auto order = *order_it;
                Base::Quantity available = order->GetOpenQuantity();
                Base::Quantity matchQty = std::min(available, remaining);

                matches.emplace_back(order, matchQty);
                remaining -= matchQty;

                ++order_it;
            }

            ++it;
        }

        return matches;
    }


    template <typename OrderPtr>
    void OrderTracker<OrderPtr>::UpdateOrderQuantity(OrderPtr order, Base::Quantity newQty)
    {
        if (!order) {
            // todo: log error
            return;
        }

        Base::OrderId orderId = order->GetId();
        // Find the order's location in the cache
        auto locationIt = mOrderLocationMap.find(orderId);

        if (locationIt == mOrderLocationMap.end()) {
            // Order not found in tracker
            // todo: log warning - trying to update non-existent order
            return;
        }

        // Extract price and order iterator from the cached location
        Base::Price price = locationIt->second.first;
        auto orderIterator = locationIt->second.second;

        // Find the PriceTracker at this price level
        auto priceTrackerIt = mPriceTrackerMap.find(price);

        if (priceTrackerIt == mPriceTrackerMap.end()) {
            // Price level not found (should never happen if cache is consistent)
            // todo: log error - inconsistent state
            mOrderLocationMap.erase(locationIt);
            return;
        }   

        PriceTrackerPtr priceTracker = priceTrackerIt->second;

        // Update the order's quantity
        order->SetOpenQuantity(newQty);

        if (newQty == 0) {
            // Remove from PriceTracker
            priceTracker->RemoveOrder(orderIterator);
            
            // Remove from location cache
            mOrderLocationMap.erase(locationIt);
            
            // If PriceTracker is now empty, remove it from the map
            if (priceTracker->IsEmpty()) {
                mPriceTrackerMap.erase(priceTrackerIt);
            }
            
            std::cout << "[INFO][OrderTracker][UpdateOrderQuantity]: Order " << orderId 
                    << " removed (qty=0)" << std::endl;
        } 
        else {
            // Just log the update
            std::cout << "[INFO][OrderTracker][UpdateOrderQuantity]: Order " << orderId 
                    << " updated to qty=" << newQty << std::endl;
        }
    }
    // Explicit template instantiation
    template class OrderTracker<Order*>;
} // namespace OrderEngine