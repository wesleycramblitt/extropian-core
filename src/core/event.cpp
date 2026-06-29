#include <vector>
#include <functional>
#include <string>
#include <unordered_map>

namespace exd::core {

/// Simple type-erased event bus. Listeners register callbacks by event name.
/// Thread-unsafe — use on the main thread only.
class EventBus {
public:
    using Callback = std::function<void(const void* payload)>;

    void on(const std::string& event, Callback cb) {
        listeners_[event].push_back(std::move(cb));
    }

    void emit(const std::string& event, const void* payload = nullptr) {
        auto it = listeners_.find(event);
        if (it != listeners_.end())
            for (auto& cb : it->second) cb(payload);
    }

    void clear() { listeners_.clear(); }

private:
    std::unordered_map<std::string, std::vector<Callback>> listeners_;
};

// Global singleton
static EventBus& bus() {
    static EventBus instance;
    return instance;
}

void event_on(const char* event, EventBus::Callback cb) { bus().on(event, std::move(cb)); }
void event_emit(const char* event, const void* payload) { bus().emit(event, payload); }

} // namespace exd::core
