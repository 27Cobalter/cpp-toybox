#pragma once

#include <vector>

#include "Card.h"

class Deck {
private:
  std::vector<Card> deck_;

public:
  Deck(int32_t number_of_joker = 0);
  void Shuffle();
  Card DrawOne();
  size_t DeckSize() const;
};
