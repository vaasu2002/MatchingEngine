//
// Created by Vaasu Bisht on 27/09/25.
//

#ifndef ORDER_H
#define ORDER_H

#pragma once
#include <regex>
#include <sstream>

#include "OrderTypes.h"

namespace OrderEngine
{
    class Order{
    public:
        Order(Base::OrderId m_id, Base::Symbol  m_symbol, Base::OrderSide m_side,
            Base::Quantity m_oty,Base::Price m_price,Base::Price stopPrice)
            : mId(m_id),
              mSymbol(std::move(m_symbol)),
              mSide(m_side),
              mOty(m_oty), mType(),
              mOpenQty(m_oty),
              mPrice(m_price),
              mStopPrice(stopPrice),
              mStatus(Base::OrderStatus::PENDING),
              mCreatedAt(std::chrono::high_resolution_clock::now())
        {
        }


        Base::OrderId GetId() const
        {
            return mId;
        }

        Base::Symbol GetSymbol() const
        {
            return mSymbol;
        }

        Base::OrderSide GetSide() const
        {
            return mSide;
        }

        Base::Quantity GetQuantity() const
        {
            return mOty;
        }

        Base::Quantity GetOpenQuantity() const
        {
            return mOpenQty;
        }

        void SetOpenQuantity(Base::Quantity openQty)
        {
            mOpenQty = openQty;
        }

        Base::Price GetPrice() const
        {
            return mPrice;
        }

        Base::OrderStatus GetOrderStatus()
        {
            return mStatus;
        }

        void SetOrderStatus(Base::OrderStatus orderStatus)
        {
            mStatus = orderStatus;
        }

        Base::OrderType GetOrderType() const
        {
            return mType;
        }

        Base::Price GetStopPrice() const
        {
            return mStopPrice;
        }
        std::string ToString() const
        {
            std::ostringstream oss;

            oss << "Order["
                << "Id=" << mId
                << ", Symbol=" << mSymbol
                << ", Side=" << OrderSideToString(mSide)
                << ", Quantity=" << mOty
                << ", OpenQty=" << mOpenQty
                << ", Price=" << mPrice
                << ", Status=" << OrderStatusToString(mStatus)
                << "]";

            return oss.str();
        }

        bool iSBuy() const { return GetSide() == Base::OrderSide::BUY; }
        bool isSell() const { return GetSide() == Base::OrderSide::SELL; }
        bool isMarket() const { return GetOrderType() == Base::OrderType::MARKET; }
        bool isLimit() const { return GetOrderType() == Base::OrderType::LIMIT; }
        bool isStop() const { return GetOrderType() == Base::OrderType::STOP || GetOrderType() == Base::OrderType::STOP_LIMIT; }
        static bool isAllOrNone() { return false; }
        void set_open_quantity(uint64_t uint64);
        // bool isImmediateOrCancel() const { return time_in_force() == TimeInForce::IMMEDIATE_OR_CANCEL; }
        // bool isFillOrKill() const { return time_in_force() == TimeInForce::FILL_OR_KILL; }

    private:
        Base::OrderId mId;
        Base::Symbol mSymbol;
        Base::OrderSide mSide;
        Base::Quantity mOty;
        Base::OrderType mType;
        Base::Quantity mOpenQty;
        Base::Price mPrice;
        Base::OrderStatus mStatus;
        Base::Timestamp mCreatedAt;
        Base::Price mStopPrice;
        static std::string OrderSideToString(Base::OrderSide side)
        {
            switch (side)
            {
            case Base::OrderSide::BUY: return "BUY";
            case Base::OrderSide::SELL: return "SELL";
            default: return "UNKNOWN";
            }
        }

        static std::string OrderStatusToString(Base::OrderStatus status)
        {
            switch (status)
            {
            case Base::OrderStatus::PENDING: return "PENDING";
            case Base::OrderStatus::FILLED: return "FILLED";
            case Base::OrderStatus::ACCEPTED: return "ACCEPTED";
            case Base::OrderStatus::REJECTED: return "REJECTED";
            case Base::OrderStatus::REPLACED: return "REPLACED";
            case Base::OrderStatus::PARTIALLY_FILLED: return "PARTIALLY_FILLED";
            default: return "UNKNOWN";
            }
        }
    }; // class Order
} // namespace OrderEngine

#endif //ORDER_H
