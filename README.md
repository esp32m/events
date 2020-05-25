# ESP32 events

Lightweight event manager for ESP-32

## Features

* Publish events and subscribe for events
* Thread-safe
* Modular and extensible, Event may be subclassed to define custom events

## Usage

```cpp
#include <Arduino.h>
#include <events.hpp>

using namespace esp32m;

void setup()
{
  Serial.begin(115200);
  EventManager::instance().subscribe([](Event *ev) {
    if (ev->is("notify"))
    {
      Serial.println("notified!");
    }
  });
}

void loop()
{
  Event ev("notify");
  EventManager::instance().publish(ev);
  sleep(1000);
}
```

## Usage - advanced

```cpp
#include <Arduino.h>
#include <events.hpp>

using namespace esp32m;

class CustomEvent : public Event
{
public:
  CustomEvent(int data) : Event("custom"), _data(data) {}
  int data() { return _data; }

private:
  int _data;
};

void setup()
{
  Serial.begin(115200);
  EventManager::instance().subscribe([](Event *ev) {
    if (ev->is("custom"))
    {
      Serial.println(((CustomEvent *)ev)->data());
    }
  });
}

int c = 0;
void loop()
{
  CustomEvent ev(c++);
  EventManager::instance().publish(ev);
  sleep(1000);
}
```
