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

namespace OrderEngine::Base
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

}
#endif // ORDER_TYPES_H