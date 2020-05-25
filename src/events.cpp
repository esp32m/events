#include <string.h>

#include <events.hpp>

namespace esp32m
{

  bool Event::is(const char *type) const { return type && !strcmp(_type, type); }

  Subscription::~Subscription()
  {
    EventManager::instance().unsubscribe(this);
  }

  void EventManager::publish(Event &event)
  {
    for (size_t i = 0; i < _subscriptions.size(); i++)
    {
      Subscription *sub;
      {
        std::lock_guard<std::mutex> guard(_mutex);
        sub = _subscriptions[i];
        if (!sub)
          continue;
        sub->ref();
      }
      sub->_cb(&event);
      {
        std::lock_guard<std::mutex> guard(_mutex);
        sub->unref();
      }
    }
  }

  const Subscription *EventManager::subscribe(Subscription::Callback cb)
  {
    const auto result = new Subscription(cb);
    std::lock_guard<std::mutex> guard(_mutex);
    for (auto it = _subscriptions.begin(); it != _subscriptions.end(); ++it)
      if (*it == nullptr) // fill empty spots that may have appeared due to unsubscribe
      {
        *it = result;
        return result;
      }
    _subscriptions.push_back(result);
    return result;
  }

  void EventManager::unsubscribe(const Subscription *sub)
  {
    std::lock_guard<std::mutex> guard(_mutex);
    while (sub->_refcnt)
      sleep(1);
    for (auto it = _subscriptions.begin(); it != _subscriptions.end(); ++it)
      if (*it == sub)
        *it = nullptr; // we don't actually remove the element for thread safety
  }

  EventManager &EventManager::instance()
  {
    static EventManager i;
    return i;
  }

} // namespace esp32m