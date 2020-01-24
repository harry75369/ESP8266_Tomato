#include <initializer_list>
#include <vector>

#define NOTE_D0 -1
#define NOTE_D1 294
#define NOTE_D2 330
#define NOTE_D3 370
#define NOTE_D4 393
#define NOTE_D5 440
#define NOTE_D6 494
#define NOTE_D7 554

#define NOTE_DL1 147
#define NOTE_DL2 165
#define NOTE_DL3 185
#define NOTE_DL4 196
#define NOTE_DL5 220
#define NOTE_DL6 247
#define NOTE_DL7 277

#define NOTE_DH1 587
#define NOTE_DH2 659
#define NOTE_DH3 739
#define NOTE_DH4 784
#define NOTE_DH5 880
#define NOTE_DH6 988
#define NOTE_DH7 1108

class Music {
private:
  int pin;
  int state;
  int currNote;
  int len;
  std::vector<int> notes;
  std::vector<float> durations;
  unsigned long lastNoteStartTime;

  void playCurrNote() {
    lastNoteStartTime = millis();
    if (notes[currNote] == NOTE_D0) {
      noTone(pin);
    } else {
      tone(pin, notes[currNote]);
    }
  }

public:
  Music(const std::initializer_list<int>& notes, const std::initializer_list<float>& durations)
    : pin(D3)
    , state(0)
    , currNote(0)
    , lastNoteStartTime(0)
  {
    len = std::min(notes.size(), durations.size());
    this->notes.insert(this->notes.end(), notes.begin(), notes.end());
    this->durations.insert(this->durations.end(), durations.begin(), durations.end());
  }

  void setPin(int pin) {
    this->pin = pin;
  }

  void play() {
    state = 1;
    playCurrNote();
  }

  void playFromStart() {
    currNote = 0;
    play();
  }

  void stop() {
    state = 0;
    noTone(pin);
  }

  void toggle() {
    if (!state) {
      play();
    } else {
      stop();
    }
  }

  void update() {
    if (!state) {
      return;
    }

    if ((millis() - lastNoteStartTime) >= 400 * durations[currNote]) {
      noTone(pin);
      currNote = (currNote + 1) % len;
      playCurrNote();
    }
  }
};

static Music xiaoxingxing(
  {
    NOTE_D1,NOTE_D1,NOTE_D5,NOTE_D5,NOTE_D6,NOTE_D6,NOTE_D5,NOTE_D0,
    NOTE_D4,NOTE_D4,NOTE_D3,NOTE_D3,NOTE_D2,NOTE_D2,NOTE_D1,NOTE_D0,
    NOTE_D5,NOTE_D5,NOTE_D4,NOTE_D4,NOTE_D3,NOTE_D3,NOTE_D2,NOTE_D0,
    NOTE_D5,NOTE_D5,NOTE_D4,NOTE_D4,NOTE_D3,NOTE_D3,NOTE_D2,NOTE_D0,
    NOTE_D1,NOTE_D1,NOTE_D5,NOTE_D5,NOTE_D6,NOTE_D6,NOTE_D5,NOTE_D0,
    NOTE_D4,NOTE_D4,NOTE_D3,NOTE_D3,NOTE_D2,NOTE_D2,NOTE_D1,NOTE_D0,
    NOTE_D0,
  },
  {
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    5,
  });

static Music huluwa(
  {
    NOTE_DH1,NOTE_D6,NOTE_D5,NOTE_D6,NOTE_D0,
    NOTE_DH1,NOTE_D6,NOTE_D5,NOTE_DH1,NOTE_D6,NOTE_D0,NOTE_D6,
    NOTE_D6,NOTE_D6,NOTE_D5,NOTE_D6,NOTE_D0,NOTE_D6,
    NOTE_DH1,NOTE_D6,NOTE_D5,NOTE_DH1,NOTE_D6,NOTE_D0,
    NOTE_D1,NOTE_D1,NOTE_D3,
    NOTE_D1,NOTE_D1,NOTE_D3,NOTE_D0,
    NOTE_D6,NOTE_D6,NOTE_D6,NOTE_D5,NOTE_D6,
    NOTE_D5,NOTE_D1,NOTE_D3,NOTE_D0,
    NOTE_DH1,NOTE_D6,NOTE_D6,NOTE_D5,NOTE_D6,
    NOTE_D5,NOTE_D1,NOTE_D2,NOTE_D0,
    NOTE_D7,NOTE_D7,NOTE_D5,NOTE_D3,
    NOTE_D5,
    NOTE_DH1,NOTE_D0,NOTE_D6,NOTE_D6,NOTE_D5,NOTE_D5,NOTE_D6,NOTE_D6,
    NOTE_D0,NOTE_D5,NOTE_D1,NOTE_D3,NOTE_D0,
    NOTE_DH1,NOTE_D0,NOTE_D6,NOTE_D6,NOTE_D5,NOTE_D5,NOTE_D6,NOTE_D6,
    NOTE_D0,NOTE_D5,NOTE_D1,NOTE_D2,NOTE_D0,
    NOTE_D3,NOTE_D3,NOTE_D1,NOTE_DL6,
    NOTE_D1,
    NOTE_D3,NOTE_D5,NOTE_D6,NOTE_D6,
    NOTE_D3,NOTE_D5,NOTE_D6,NOTE_D6,
    NOTE_DH1,NOTE_D0,NOTE_D7,NOTE_D5,
    NOTE_D6,
    NOTE_D0,
  },
  {
    1,1,0.5,0.5,1,
    0.5,0.5,0.5,0.5,1,0.5,0.5,
    0.5,1,0.5,1,0.5,0.5,
    0.5,0.5,0.5,0.5,1,1,
    1,1,1+1,
    0.5,1,1+0.5,1,
    1,1,0.5,0.5,1,
    0.5,1,1+0.5,1,
    0.5,0.5,0.5,0.5,1+1,
    0.5,1,1+0.5,1,
    1+1,0.5,0.5,1,
    1+1+1+1,
    0.5,0.5,0.5+0.25,0.25,0.5+0.25,0.25,0.5+0.25,0.25,
    0.5,1,0.5,1,1,
    0.5,0.5,0.5+0.25,0.25,0.5+0.25,0.25,0.5+0.25,0.25,
    0.5,1,0.5,1,1,
    1+1,0.5,0.5,1,
    1+1+1+1,
    0.5,1,0.5,1+1,
    0.5,1,0.5,1+1,
    1+1,0.5,0.5,1,
    1+1+1+1,
    5,
  });

static Music huanlesong(
  {
    NOTE_D3,NOTE_D3,NOTE_D4,NOTE_D5,
    NOTE_D5,NOTE_D4,NOTE_D3,NOTE_D2,
    NOTE_D1,NOTE_D1,NOTE_D2,NOTE_D3,
    NOTE_D3,NOTE_D2,NOTE_D2,
    NOTE_D3,NOTE_D3,NOTE_D4,NOTE_D5,
    NOTE_D5,NOTE_D4,NOTE_D3,NOTE_D2,
    NOTE_D1,NOTE_D1,NOTE_D2,NOTE_D3,
    NOTE_D2,NOTE_D1,NOTE_D1,
    NOTE_D2,NOTE_D2,NOTE_D3,NOTE_D1,
    NOTE_D2,NOTE_D3,NOTE_D4,NOTE_D3,NOTE_D1,
    NOTE_D2,NOTE_D3,NOTE_D4,NOTE_D3,NOTE_D2,
    NOTE_D1,NOTE_D2,NOTE_DL5,NOTE_D0,
    NOTE_D3,NOTE_D3,NOTE_D4,NOTE_D5,
    NOTE_D5,NOTE_D4,NOTE_D3,NOTE_D4,NOTE_D2,
    NOTE_D1,NOTE_D1,NOTE_D2,NOTE_D3,
    NOTE_D2,NOTE_D1,NOTE_D1,
    NOTE_D0,
  },
  {
    1,1,1,1,
    1,1,1,1,
    1,1,1,1,
    1+0.5,0.5,1+1,
    1,1,1,1,
    1,1,1,1,
    1,1,1,1,
    1+0.5,0.5,1+1,
    1,1,1,1,
    1,0.5,0.5,1,1,
    1,0.5,0.5,1,1,
    1,1,1,1,
    1,1,1,1,
    1,1,1,0.5,0.5,
    1,1,1,1,
    1+0.5,0.5,1+1,
    5,
  });

static Music taiyangzhaochangshengqi(
  {
    NOTE_DL6,NOTE_D3,NOTE_D3,NOTE_D3,NOTE_D4,
    NOTE_D3,NOTE_D3,NOTE_D4,NOTE_D5,
    NOTE_D6,NOTE_D6,NOTE_D5,NOTE_D5,
    NOTE_D3,
    NOTE_DL6,NOTE_D2,NOTE_D2,NOTE_D2,NOTE_D3,
    NOTE_D2,NOTE_D3,
    NOTE_D5,NOTE_D3,NOTE_D5,NOTE_DL7,NOTE_D1,
    NOTE_DL6,NOTE_DL6,NOTE_D1,NOTE_D3,
    NOTE_D6,NOTE_D5,NOTE_D6,NOTE_D5,
    NOTE_D6,NOTE_D5,NOTE_D6,NOTE_D5,NOTE_D5,
    NOTE_D3,NOTE_D2,NOTE_D6,NOTE_D5,
    NOTE_D3,NOTE_D2,NOTE_D3,
    NOTE_D2,NOTE_D6,NOTE_D4,
    NOTE_D3,NOTE_D2,NOTE_D3,NOTE_D5,NOTE_DL7,NOTE_D1,
    NOTE_DL6,
    NOTE_D0,
  },
  {
    1,1,1,0.75,0.25,
    2.5,0.5,0.5,0.5,
    1.5,0.5,1,1,
    4,
    1,1,1,0.75,0.25,
    2,2,
    0.75,0.25,2,0.75,0.25,
    5,1,1,1,
    0.75,0.25,2.75,0.25,
    0.75,0.25,2,0.75,0.25,
    5,1,1,1,
    0.75,0.25,4,
    1,1,1,
    0.75,0.25,1,1,0.75,0.25,
    4,
    5,
  });
