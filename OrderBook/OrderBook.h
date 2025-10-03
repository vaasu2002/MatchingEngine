//
// Created by Vaasu Bisht on 03/10/25.
//
#pragma once
#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include "../OrderTypes.h"
#include "../OrderTracker/OrderTracker.h"

namespace OrderEngine {
    /**
     * @struct TradeExecution
     * @tparam OrderPtr
     * @brief Structure representing a trade execution event.
     * @param OrderPtr Smart pointer type for Order objects (e.g., std::shared_ptr<Order>).
     * @details
     * TradeExecution encapsulates details of a trade execution. This is official record of when
     * two orders match and trade. This can be thought of as the “birth certificate” of every trade.
     */
    template<typename OrderPtr> struct TradeExecution
    {
        OrderPtr mInBoundOrder; // The "aggressive" order that just arrived (initiator of the trade)
        OrderPtr mRestingOrder; // The "passive" order that was already in the book (the one being hit or lifted)
        Base::Quantity mQuantity{};
        Base::Price mPrice{};
        Base::Timestamp mTimestamp;
        Base::FillFlags mFlags;

        TradeExecution(const OrderPtr& inBoundOrder,const OrderPtr& restingOrder, Base::Quantity qty,
            Base::Price p, Base::FillFlags f = Base::FILL_NORMAL)
                :mInBoundOrder(inBoundOrder),mRestingOrder(restingOrder),mQuantity(qty),mPrice(p),
        mTimestamp(std::chrono::high_resolution_clock::now()),mFlags(f){}
    };

    /**
     * @brief Structure for tracking order book statistics.
     * @details
     * OrderBookStats maintains atomic counters for various order book events, allowing
     * for thread-safe tracking of metrics such as total orders added, cancelled, replaced,
     * trades executed, volume traded, and orders rejected.
     */
    struct OrderBookStats
    {
        std::atomic<uint64_t> mTotalOrdersAdded{0};
        std::atomic<uint64_t> mTotalOrdersCancelled{0};
        std::atomic<uint64_t> mTotalOrdersReplaced{0};
        std::atomic<uint64_t> mTotalTrades{0};
        std::atomic<uint64_t> mTotalVolume{0};
        std::atomic<uint64_t> mTotalRejected{0};

        void reset()
        {
            mTotalTrades = 0;
            mTotalRejected = 0;
            mTotalVolume=0;
            mTotalOrdersReplaced=0;
            mTotalOrdersAdded=0;
            mTotalOrdersCancelled=0;
        }
    };

    /**
     * @class OrderBook
     * @tparam OrderPtr
     * @brief The OrderBook class manages buy and sell orders, matches trades, and notifies listeners of events.
     * @remarks
     * The architecture design decision is to keep one order book instance per stock
     * 1. Stocks Are Independent
     * 2. Provides performance isolation
     * 3. Circuit breakers can be implemented per stock
     *
     */
    template<typename OrderPtr> class OrderBook
    {
    public:
        using OrderTracker = OrderTracker<OrderPtr>;
        using TradeExecution = TradeExecution<OrderPtr>;
    private:
        Base::Symbol mSymbol;
        OrderTracker mBidTracker;
        OrderTracker mAskTracker;
        OrderTracker mStopBidTracker;
        OrderTracker mStopAskTracker;

        // Market States
        std::atomic<Base::Price> mMarketPrice{};
        std::atomic<Base::Price> mLastTradePrice{};
        std::atomic<Base::Quantity> mLastTradeQty{};

        // Statistics
        OrderBookStats mStats;

        // Thread safety
        mutable std::recursive_mutex mBookMutex;

        // Trade execution queue for batch processing
        std::vector<TradeExecution> mPendingTrades;

    public:
        explicit OrderBook(Base::Symbol  symbol);
        ~OrderBook() = default;

        // ========== Configuration ==========

        void setMarketPrice(Base::Price price);

        bool addOrder(const OrderPtr& order, Base::OrderConditions conditions = Base::NO_CONDITIONS);
    private:
        void rejectOrder(const OrderPtr& order, const std::string& reason);
        bool validateOrder(const OrderPtr& order) const;
        bool processMarketOrder(const OrderPtr& inBoundOrderPtr, Base::OrderConditions conditions);
        bool matchMarketBuyOrder(const OrderPtr& order, Base::OrderConditions conditions);
        bool matchBuyOrder(const OrderPtr& inBoundOrderPtr, Base::OrderConditions conditions, Base::Price limitPrice);
        void executeTrade(const OrderPtr& inBoundOrderPtr, const OrderPtr& restingOrderPtr, Base::Quantity quantity, Base::Price price);
        static bool IsAllOrNone(Base::OrderConditions conditions);
        static bool isImmediateOrCancel(Base::OrderConditions conditions);
    };

    extern template class OrderBook<Order*>;
};


#endif //ORDERBOOK_H
