#include "relay_registry.h"

#include <algorithm>
#include <utility>

bool RelayRegistry::add(
    const std::string& name,
    std::shared_ptr<IRelay> relay)
{
    if (name.empty() || relay == nullptr)
    {
        return false;
    }

    const auto [iterator, inserted] =
        relays_.emplace(name, std::move(relay));

    static_cast<void>(iterator);
    return inserted;
}

IRelay* RelayRegistry::find(const std::string& name)
{
    const auto iterator = relays_.find(name);

    if (iterator == relays_.end())
    {
        return nullptr;
    }

    return iterator->second.get();
}

const IRelay* RelayRegistry::find(
    const std::string& name) const
{
    const auto iterator = relays_.find(name);

    if (iterator == relays_.end())
    {
        return nullptr;
    }

    return iterator->second.get();
}

bool RelayRegistry::contains(
    const std::string& name) const
{
    return relays_.contains(name);
}

std::size_t RelayRegistry::size() const
{
    return relays_.size();
}

std::vector<std::string> RelayRegistry::names() const
{
    std::vector<std::string> result;
    result.reserve(relays_.size());

    for (const auto& [name, relay] : relays_)
    {
        static_cast<void>(relay);
        result.push_back(name);
    }

    std::sort(result.begin(), result.end());
    return result;
}
