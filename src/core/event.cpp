#include "event.h"

namespace exd::core {

void EventBus::on(const std::string& event, Callback cb) {
    listeners_[event].push_back(std::move(cb));
}

void EventBus::emit(const std::string& event, const void* payload) {
    auto it = listeners_.find(event);
    if (it != listeners_.end())
        for (auto& cb : it->second) cb(payload);
}

void EventBus::clear() { listeners_.clear(); }

// Global singleton
static EventBus& bus() {
    static EventBus instance;
    return instance;
}

void event_on(const char* event, EventBus::Callback cb) { bus().on(event, std::move(cb)); }
void event_emit(const char* event, const void* payload) { bus().emit(event, payload); }

} // namespace exd::core
