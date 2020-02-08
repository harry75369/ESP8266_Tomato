#ifndef __ONE_SHOT_H__
#define __ONE_SHOT_H__

#include <functional>

#define TRIGGER_BODY(...) \
  do {                    \
    if (triggered) {      \
      return;             \
    }                     \
    func(__VA_ARGS__);    \
    triggered = true;     \
  } while(0)

class OneShot {
  bool triggered;

public:
  OneShot() : triggered(false) {}

  void reset() {
    triggered = false;
  }

  void trigger(const std::function<void()>& func) {
    TRIGGER_BODY();
  }

  template<typename T0>
  void trigger(const std::function<void(const T0&)>& func, const T0& v0) {
    TRIGGER_BODY(v0);
  }

  template<typename T0, typename T1>
  void trigger(const std::function<void(const T0&, const T1&)>& func, const T0& v0, const T1& v1) {
    TRIGGER_BODY(v0, v1);
  }

  template<typename T0, typename T1, typename T2>
  void trigger(const std::function<void(const T0&, const T1&, const T2&)>& func, const T0& v0, const T1& v1, const T2& v2) {
    TRIGGER_BODY(v0, v1, v2);
  }
};

#endif // __ONE_SHOT_H__
