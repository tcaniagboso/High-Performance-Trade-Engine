//
// Created by tcani on 8/16/2025.
//

#include "order_entry.hpp"
#include <optional>

int main() {
    std::optional<int> x = 42;
    return x.has_value() ? 0 : 1;
}