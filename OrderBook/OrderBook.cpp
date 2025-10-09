//
// Created by Vaasu Bisht on 03/10/25.
//

#include "OrderBook.h"

#include <utility>

namespace OrderEngine {
    template <typename OrderPtr>
    OrderBook<OrderPtr>::OrderBook(Base::Symbol  symbol):
        mSymbol(std::move(symbol)),
        mBidTracker(true),
        mAskTracker(false),
        mStopBidTracker(true),
        mStopAskTracker(false),
        mMarketPrice(0),
        mLastTradePrice(0),
        mLastTradeQty(0){
            mPendingTrades.reserve(1000);
        }

    template <typename OrderPtr>
    void OrderBook<OrderPtr>::setMarketPrice(Base::Price price)
    {
        mMarketPrice.store(price);
    }

    // <===================================== addOrder Mathod =====================================>
    template <typename OrderPtr>
    bool OrderBook<OrderPtr>::addOrder(const OrderPtr& order, Base::OrderConditions conditions)
    {
        // Order* order = new Order();
        std::lock_guard<std::recursive_mutex> lock(mBookMutex); // acquire lock
        
        // todo: change design pattern to chain of responsibility
        if (!validateOrder(order)) {
            rejectOrder(order, "Invalid order parameters");
            return false;
        }

        bool filled = false;

        if(order->isMarket()){
            filled = processMarketOrder(order, conditions);
            std::cout<<"Order: "<<order->ToString()<<std::endl;
            std::cout<<"Filled Flag: "<<filled<<std::endl;
        }
        else if(order->isLimit()){
            filled = processLimitOrder(order, conditions);
            std::cout<<"[addOrder() method of OrderBook class]: This is a limit orer"<<std::endl;
        }
        // todo: add order processing for stop order and limit order
        // todo: add notification that order is accepted
        // todo: update market data and depth
        return filled;
    }

    template <typename OrderPtr>
    void OrderBook<OrderPtr>::rejectOrder(const OrderPtr& order, const std::string& reason)
    {
        order->SetOrderStatus(Base::OrderStatus::REJECTED);
        ++mStats.mTotalRejected;
        // for (const auto& listener : mOrderListeners) {
        //     listener->on_reject(order, reason);
        // }
        //todo: add warn log
    }

    template <typename OrderPtr>
    bool OrderBook<OrderPtr>::validateOrder(const OrderPtr& order) const
    {
        if(!order) return false;
        if(order->GetSymbol() != mSymbol) return false;
        if(order->GetQuantity() == 0) return false;
        if(order->GetOpenQuantity() > order->GetQuantity()) return false;
        if(!order->isMarket() && order->GetPrice() <= 0) return false;
        if(order->isStop() && order->GetStopPrice() <= 0) return false;
        return true;
    }

    template <typename OrderPtr>
    bool OrderBook<OrderPtr>::processMarketOrder(const OrderPtr& inBoundOrderPtr, const Base::OrderConditions conditions)
    {
        bool filled = false;
        if(inBoundOrderPtr->isBuy()){
            filled = matchMarketBuyOrder(inBoundOrderPtr, conditions);
        }
        else {
            std::cout<<"<========================== Custom =============================>"<<std::endl;
            std::cout<<"processMarketOrder:matchMarketSellOrder: "<<inBoundOrderPtr->ToString()<<std::endl;
            std::cout<<"<========================== Custom =============================>"<<std::endl;
            filled = matchMarketSellOrder(inBoundOrderPtr, conditions);
            // todo: implement matchMarketSellOrder
        }

        if (inBoundOrderPtr->GetOpenQuantity() > 0) {
            inBoundOrderPtr->SetOrderStatus(Base::OrderStatus::CANCELLED);
            // todo: notifyOrderCancelled
        }
        return filled;
    }

    template <typename OrderPtr>
    void OrderBook<OrderPtr>::addRestingOrder(const OrderPtr& order)
    {
        // Order* order = new Order();
        if(order->isBuy())
        {
            mBidTracker.AddOrder(order);
            order->SetOrderStatus(Base::OrderStatus::PENDING);
            mStats.mTotalOrdersAdded++;
        }
        else // Sell Order
        {
            mAskTracker.AddOrder(order);
            order->SetOrderStatus(Base::OrderStatus::PENDING);
            mStats.mTotalOrdersAdded++;
        }
    }

    template <typename OrderPtr>
    bool OrderBook<OrderPtr>::matchMarketBuyOrder(const OrderPtr& order, Base::OrderConditions conditions)
    {
        // Order* order = new Order();
        Base::Price limitPrice = std::numeric_limits<Base::Price>::max(); // No price limit for market orders
        return matchBuyOrder(order,conditions,limitPrice);
    }

    template <typename OrderPtr>
    bool OrderBook<OrderPtr>::matchBuyOrder(const OrderPtr& inBoundOrderPtr, Base::OrderConditions conditions, Base::Price limitPrice)
    {
        Base::Quantity inBoundOrderRemaining = inBoundOrderPtr->GetOpenQuantity();
        bool anyFill = false;

        // Get matching sell(ask) orders from the ask-order tracker, format: std::vector<std::pair<OrderPtr, Quantity>>
        // These are order lying in sell section of order booking waiting to be matched with buy orders
        // These are resting order (orders lying in order book to be matched)
        auto matches = mAskTracker.MatchQuantity(limitPrice, inBoundOrderRemaining);

        for (const auto& [restingOrderPtr, restingOrderRemainingQty] : matches) {

            if (inBoundOrderRemaining == 0){
                break;
            }

            // Check all-or-none conditions
            if (IsAllOrNone(conditions) && restingOrderRemainingQty < inBoundOrderRemaining) {
                continue;
            }

            Base::Quantity fillQty = std::min(restingOrderRemainingQty, inBoundOrderRemaining);
            Base::Price fillPrice = restingOrderPtr->GetPrice();

            // Execute the trade
            executeTrade(inBoundOrderPtr, restingOrderPtr, fillQty, fillPrice);

            inBoundOrderRemaining -= fillQty;
            anyFill = true;

            // Update order quantities
            inBoundOrderPtr->SetOpenQuantity(inBoundOrderRemaining);

            if (inBoundOrderRemaining == 0) {
                inBoundOrderPtr->SetOrderStatus(Base::OrderStatus::FILLED);
                break;
            } else {
                inBoundOrderPtr->SetOrderStatus(Base::OrderStatus::PARTIALLY_FILLED);
            }
        }
        return anyFill;
    }

    template <typename OrderPtr>
    bool OrderBook<OrderPtr>::matchMarketSellOrder(const OrderPtr& order, Base::OrderConditions conditions)
    {
        // Order* order = new Order();
        Base::Price limitPrice = 0; // No price limit for market orders
        return matchSellOrder(order,conditions,limitPrice);
    }

    template <typename OrderPtr>
    bool OrderBook<OrderPtr>::matchSellOrder(const OrderPtr& inBoundOrderPtr, Base::OrderConditions conditions, Base::Price limitPrice)
    {
        // Order* inBoundOrderPtr = new Order();
        Base::Quantity inBoundOrderRemaining = inBoundOrderPtr->GetOpenQuantity();
        bool anyFill = false;

        // Get matching sell(ask) orders from the ask-order tracker, format: format: std::vector<std::pair<OrderPtr, Quantity>>
        // Thses orders are lying in the buy side of the order booking waiting to be matched with sell order.
        // These are resting orders waiting to be matched lying in order book.
        auto matches = mBidTracker.MatchQuantity(limitPrice, inBoundOrderRemaining);

        // std::cout<<"matchSellOrder method, size of matches: "<<matches.size()<<std::endl;
        for (const auto& [restingOrderPtr, restingOrderRemainingQty] : matches) {
            if (inBoundOrderRemaining == 0)
            {
                break;
            }
            
            // Check all-or-none conditions
            if (IsAllOrNone(conditions) && restingOrderRemainingQty < inBoundOrderRemaining) {
                continue;
            }
            
            Base::Quantity fillQty = std::min(restingOrderRemainingQty, inBoundOrderRemaining);
            Base::Price fillPrice = restingOrderPtr->GetPrice();
            // Execute the trade
            executeTrade(inBoundOrderPtr, restingOrderPtr, fillQty, fillPrice);
            
            inBoundOrderRemaining -= fillQty;
            anyFill = true;
            
            // Update order quantities
            inBoundOrderPtr->SetOpenQuantity(inBoundOrderRemaining);
            
            if (inBoundOrderRemaining == 0) {
                inBoundOrderPtr->SetOrderStatus(Base::OrderStatus::FILLED);
                break;
            } else {
                inBoundOrderPtr->SetOrderStatus(Base::OrderStatus::PARTIALLY_FILLED);
            }
        }
        return anyFill;
    }

    template <typename OrderPtr>
    void OrderBook<OrderPtr>::executeTrade(const OrderPtr& inBoundOrderPtr, const OrderPtr& restingOrderPtr, Base::Quantity quantity, Base::Price price)
    {
        Base::FillFlags flags = Base::FILL_NORMAL;
        if (inBoundOrderPtr->GetOpenQuantity() == quantity){
            flags |= Base::FILL_COMPLETE;
        }
        else{
            flags |= Base::FILL_PARTIAL;
        }

        // Create trade execution record
        TradeExecution trade(inBoundOrderPtr, restingOrderPtr, quantity, price, flags);
        mPendingTrades.push_back(trade);

        // ==== Updating Meta Data ====

        // Update statistics
        mStats.mTotalTrades++;
        mStats.mTotalVolume += quantity;
        // Update market price
        mLastTradePrice.store(price);
        mLastTradeQty.store(quantity);
        mMarketPrice.store(price);

        // Update resting order
        Base::Quantity restingRemainingQty = restingOrderPtr->GetOpenQuantity() - quantity;
        restingOrderPtr->SetOpenQuantity(restingRemainingQty);

        if (restingRemainingQty == 0) 
        {
            restingOrderPtr->SetOrderStatus(Base::OrderStatus::FILLED);

            // Remove the order from the order tracker
            if (restingOrderPtr->isBuy())
            {
                mBidTracker.RemoveOrder(restingOrderPtr);
            }
            else // Sell
            {
                mAskTracker.RemoveOrder(restingOrderPtr);
            }
        } 
        else 
        {
            restingOrderPtr->SetOrderStatus(Base::OrderStatus::PARTIALLY_FILLED);

            if(restingOrderPtr->isBuy())
            {
                mBidTracker.UpdateOrderQuantity(restingOrderPtr, restingRemainingQty);
            }
            else 
            {
                mAskTracker.UpdateOrderQuantity(restingOrderPtr, restingRemainingQty);
            }
        }

        // todo: log the trade
        // todo: notify trade listeners that trade is executed
    }

    template <typename OrderPtr>
    bool OrderBook<OrderPtr>::isImmediateOrCancel(const Base::OrderConditions conditions)
    {
        return (conditions & Base::IMMEDIATE_OR_CANCEL) == 1;
    }

    template <typename OrderPtr>
    bool OrderBook<OrderPtr>::IsAllOrNone(const Base::OrderConditions conditions)
    {
        return (conditions & Base::ALL_OR_NONE) == 1;
    }

    /**
     * @method processLimitOrder
     * @details
     * - Attemps to match order, if unmatched add remaining quantity to the order book.
     */
    template <typename OrderPtr>
    bool OrderBook<OrderPtr>::processLimitOrder(const OrderPtr& inBoundOrderPtr, const Base::OrderConditions conditions)
    {
        // Order* inBoundOrderPtr = new Order();
        bool isFilled = false;
        if(inBoundOrderPtr->isBuy()){
            isFilled = matchBuyOrder(inBoundOrderPtr, conditions, inBoundOrderPtr->GetPrice());
        }
        else {
            isFilled = matchSellOrder(inBoundOrderPtr, conditions, inBoundOrderPtr->GetPrice());
        }

        // If order has remaining quantity and is not IOC (Immediate or Cancel) : Add in order book
        if (inBoundOrderPtr->GetOpenQuantity() > 0) 
        {
            if (isImmediateOrCancel(conditions)) 
            // IOC orders are cancelled if not fully filled
            {
                /** <================================== Doubt ==================================>
                 *  If order is cancelled should we return isFlled as true ever or always false??
                 */
                inBoundOrderPtr->SetOrderStatus(Base::OrderStatus::CANCELLED);
                // todo: notifyOrderCancelled
            } 
            else 
            {
                // Add remaining quantity to the order book
                addRestingOrder(inBoundOrderPtr);
            }
        }
        
        return isFilled;
    }
    template class OrderBook<Order*>;
} // OrderEngine