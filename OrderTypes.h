/**
* @file OrderTypes.h
* @brief This contains all the common data-types and enumeration
*/

#pragma once

#ifndef ORDER_TYPES_H

#include <cstdint>
#include <string>
#include <memory>
#include <chrono>

namespace OrderEngine
{
    namespace Base
    {
        using Price = int64_t; // Price in the smallest currency unit
        using Quantity = uint64_t;
        using OrderId = uint64_t;
        using Symbol = std::string;
        using Timestamp = std::chrono::high_resolution_clock::time_point;

        /*
         * Represents which side of financial order the trade is on
        */
        enum class OrderSide:char
        {
            BUY = 'B',
            SELL = 'S'
        };

        enum class OrderType : char {
            LIMIT = 'L',
            MARKET = 'M',
            STOP = 'T',
            STOP_LIMIT = 'S'
        };

        enum class OrderStatus:char
        {
            PENDING = 'P',
            ACCEPTED = 'A',
            PARTIALLY_FILLED = 'F',
            FILLED = 'C',
            REJECTED = 'R',
            REPLACED = 'E', CANCELLED
        };

        /* Bitmask flags describing the characteristics of a trade fill.
         * Flags can be combined to capture both execution role and completion status.
         * - FILL_NORMAL    : Default, no special flags.
         * - FILL_AGGRESSIVE: The order actively removed liquidity (aggressor).
         * - FILL_PASSIVE   : The order provided liquidity by resting in the book.
         * - FILL_PARTIAL   : Fill covered only part of the order’s quantity.
         * - FILL_COMPLETE  : Fill fully satisfied the order’s quantity.
        */
        enum FillFlags : uint32_t {
            FILL_NORMAL = 0,
            FILL_AGGRESSIVE = 1 << 0,
            FILL_PASSIVE = 1 << 1,
            FILL_PARTIAL = 1 << 2,
            FILL_COMPLETE = 1 << 3,
            Any
        };
        inline FillFlags operator|(FillFlags a, FillFlags b) {
            return static_cast<FillFlags>(
                static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
        }

        inline FillFlags& operator|=(FillFlags& a, FillFlags b) {
            a = a | b;
            return a;
        }

        enum OrderConditions : uint32_t {
            NO_CONDITIONS = 0,
            ALL_OR_NONE = 1 << 0,
            IMMEDIATE_OR_CANCEL = 1 << 1,
            FILL_OR_KILL = (ALL_OR_NONE | IMMEDIATE_OR_CANCEL),
            HIDDEN = 1 << 2,
            ICEBERG = 1 << 3
        };
        inline OrderConditions operator|(OrderConditions a, OrderConditions b) {
            return static_cast<OrderConditions>(
                static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
        }

        inline OrderConditions& operator|=(OrderConditions& a, OrderConditions b) {
            a = a | b;
            return a;
        }
    }
}
#endif // ORDER_TYPES_H