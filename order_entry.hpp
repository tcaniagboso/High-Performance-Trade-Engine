#pragma once

#include <cctype>
#include <chrono>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

namespace order_entry {
    // -------------- Enums ----------------------
    enum class OrderSide {BUY, SELL};
    enum class OrderType{LIMIT, MARKET};

    // ---------------- Helpers ---------------------
    inline std::string toLower(std::string s) {
        for (char& c : s) {
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }

        return s;
    }

    inline std::string toUpper(std::string s) {
        for (char& c: s) {
            c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        }

        return s;
    }

    inline OrderSide parseOrderSide(std::string_view orderSide) {
        auto lowerOrderSide = toLower(std::string{orderSide});
        if (lowerOrderSide == "buy") return OrderSide::BUY;
        if (lowerOrderSide == "sell") return OrderSide::SELL;
        throw std::invalid_argument("Invalid Order Side: " + std::string{orderSide});
    }

    inline OrderType parseOrderType(std::string_view orderType) {
        auto lowerOrderType = toLower(std::string{orderType});
        if (lowerOrderType == "limit") return OrderType::LIMIT;
        if (lowerOrderType == "market") return OrderType::MARKET;
        throw std::invalid_argument("Invalid Order Type: " + std::string{orderType});
    }

    inline uint64_t parseQuantity(const std::string& quantity) {
        try {
            long long qty = std::stoll(quantity);
            if (qty <= 0) {
                throw std::invalid_argument("Quantity must be positive: " + quantity);
            }
            return static_cast<uint64_t>(qty);
        } catch (...) {
            throw;
        }
    }

    inline int64_t parsePriceTicks(const std::string& priceTicks) {
        try {
            long long priceT = std::stoll(priceTicks);
            return static_cast<int64_t>(priceT);
        } catch (...) {
            throw;
        }
    }

    // ------------------ Requests -------------------
    using Symbol = std::string;
    using OrderId = uint64_t;
    using Quantity = uint64_t;
    using PriceT = int64_t;
    using TimePt = std::chrono::steady_clock::time_point;

    struct NewOrder {
        OrderId _orderId;
        Symbol _symbol;
        OrderSide _orderSide;
        OrderType _orderType;
        Quantity _quantity;
        std::optional<PriceT> _priceTicks;
        TimePt _timePt {std::chrono::steady_clock::now()};

        // Constructor for Market Order
        NewOrder(uint64_t orderId, std::string symbol, const std::string& orderSide, const std::string& orderType, const std::string& quantity)
            : _orderId{orderId},
            _symbol{std::move(symbol)},
            _orderSide{parseOrderSide(orderSide)},
            _orderType{parseOrderType(orderType)},
            _quantity{parseQuantity(quantity)} {}

        // Constructor for LIMIT order
        NewOrder(uint64_t orderId, std::string symbol, const std::string& orderSide, const std::string& orderType, const std::string& quantity, const std::string& priceTicks)
        : NewOrder(orderId, std::move(symbol), orderSide, orderType, quantity){
            _priceTicks = parsePriceTicks(priceTicks);
        }
    };

    struct CancelOrder {
        OrderId _orderId;
        TimePt _timePt {std::chrono::steady_clock::now()};

        explicit CancelOrder(uint64_t orderId) : _orderId{orderId} {}
    };

    struct OrderRequest {
        enum class Kind {NEW, CANCEL};

        std::optional<NewOrder> _newOrder;
        std::optional<CancelOrder> _cancelOrder;

        static OrderRequest makeNewOrder(NewOrder order) {
            OrderRequest orderRequest;
            orderRequest._newOrder = std::move(order);
            return orderRequest;
        }

        static OrderRequest makeCancelOrder(CancelOrder order) {
            OrderRequest orderRequest;
            orderRequest._cancelOrder = order;
            return orderRequest;
        }
    };
} // namespace order_entry
