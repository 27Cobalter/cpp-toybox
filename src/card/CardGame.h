#pragma once

#include "Deck.h"

#include <string>

class CardGame {
protected:
  Deck deck_;

public:
  CardGame(int32_t number_of_joker = 0);
  virtual ~CardGame();
  virtual void Initialize() = 0;
  virtual void Finalize()   = 0;
  virtual void Play()       = 0;
};
