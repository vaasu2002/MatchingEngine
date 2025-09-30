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

    // Explicit template instantiation
    template class OrderTracker<Order*>;
} // namespace OrderEngine