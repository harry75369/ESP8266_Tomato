#ifndef __ONE_SHOT_H__
#define __ONE_SHOT_H__

#include <functional>

class OneShot {
  bool triggered;
  std::function<void()> func;

public:
  OneShot(const std::function<void()>& f) : triggered(false), func(f) {}

  void trigger() {
    if (triggered) {
      return;
    }
    func();
    triggered = true;
  }
};

#endif // __ONE_SHOT_H__
