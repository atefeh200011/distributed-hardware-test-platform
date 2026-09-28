#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "relay.h"

class RelayRegistry
{
public:
    bool add(
        const std::string& name,
        std::shared_ptr<IRelay> relay);

    IRelay* find(const std::string& name);
    const IRelay* find(const std::string& name) const;

    bool contains(const std::string& name) const;
    std::size_t size() const;
    std::vector<std::string> names() const;

private:
    std::unordered_map<
        std::string,
        std::shared_ptr<IRelay>> relays_;
};
