#pragma once

#include <cstdint>
#include <string>

class Card {
private:
  int32_t value_;

public:
  Card(int32_t value = 0);
  bool IsJoker() const;
  std::string Suit() const;
  std::string Rank() const;
  std::string ToString() const;
  Card& operator=(const Card& value);
};
