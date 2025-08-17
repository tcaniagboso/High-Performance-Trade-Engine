#pragma once

#include <functional>
#include <list>
#include <map>
#include <optional>
#include <unordered_map>
#include <vector>

#include "order_entry.hpp"

namespace order_book {
    using PriceT = order_entry::PriceT;
    using Quantity = order_entry::Quantity;
    using OrderId = order_entry::OrderId;
    using TimePt = order_entry::TimePt;

    struct BestQuote {
        std::optional<PriceT> _bid;
        std::optional<PriceT> _ask;
    };

    enum class Side { BID, ASK };
    enum class CancelResult { OK, NOT_FOUND, ALREADY_CLOSED};

    struct Level {
        PriceT _price;
        Quantity _quantity;
    };

    struct Snapshot {
        std::vector<Level> _bids;
        std::vector<Level> _asks;
    };

    struct RestingOrder {
        OrderId _orderId;
        Quantity _remaining;
        TimePt _timestamp;
        // maybe pointer back to client/session

        RestingOrder(OrderId orderId, Quantity remaining, TimePt timeStamp)
        : _orderId{orderId},
        _remaining{remaining},
        _timestamp{timeStamp} {}
    };

    using Queue = std::list<RestingOrder>;

    struct Locator {
        Side side;
        PriceT price;
        Queue::iterator position;
    };

    class IOrderBook {
    public:
        virtual ~IOrderBook() = default;

        // inserts
        virtual void addMarketBuy(OrderId orderId, Quantity quantity) = 0;
        virtual void addMarketSell(OrderId orderId, Quantity quantity) = 0;
        virtual void addLimitBuy(OrderId orderId, PriceT priceT, Quantity quantity) = 0;
        virtual void addLimitSell(OrderId orderId, PriceT priceT, Quantity quantity) = 0;

        // cancels
        virtual CancelResult cancel(OrderId orderId) = 0;

        // Queries
        virtual BestQuote bestQuote() const = 0;
        virtual Quantity depthQuantity(Side side, PriceT price) const = 0;
        virtual Snapshot snapshot(std::size_t levels) const = 0;
    };

    class SimplePriceTimeBook : public IOrderBook {

    private:
        std::map<PriceT, Queue, std::greater<PriceT>> _bids;
        std::map<PriceT, Queue, std::less<PriceT>> _asks;
        std::unordered_map<OrderId, Locator> _liveOrders;

    public:
        SimplePriceTimeBook();

        void addMarketBuy(OrderId orderId, Quantity quantity) override;
        void addMarketSell(OrderId orderId, Quantity quantity) override;
        void addLimitBuy(OrderId orderId, PriceT priceT, Quantity quantity) override;
        void addLimitSell(OrderId orderId, PriceT priceT, Quantity quantity) override;

        CancelResult cancel(OrderId orderId) override;

        BestQuote bestQuote() const override;
        Quantity depthQuantity(Side side, PriceT price) const override;
        Snapshot snapshot(std::size_t levels) const override;

    };
} // namespace order_book
