#pragma once
#include <functional>
#include <string>
#include <vector>
#include <unordered_map>

namespace exd::core {

class EventBus {
public:
    using Callback = std::function<void(const void* payload)>;
    void on(const std::string& event, Callback cb);
    void emit(const std::string& event, const void* payload = nullptr);
    void clear();

private:
    std::unordered_map<std::string, std::vector<Callback>> listeners_;
};

} // namespace exd::core
