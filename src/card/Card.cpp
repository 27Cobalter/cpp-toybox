#include "Card.h"

#include <cassert>
#include <format>

Card::Card(int32_t value) : value_(value) {}

bool Card::IsJoker() const {
  return value_ >= 52;
}

std::string Card::Suit() const {
  if (IsJoker() == true) {
    return "";
  }

  int32_t suit = value_ / 13;
  switch (suit) {
  case (0):
    return "S";
  case (1):
    return "H";
  case (2):
    return "D";
  case (3):
    return "C";
  default:
    assert(false);
    return "";
  }
}

std::string Card::Rank() const {
  if (IsJoker() == true) {
    return "";
  }

  int32_t rank = (value_ % 13) + 1;
  switch (rank) {
  case (1):
    return "A";
  case (11):
    return "J";
  case (12):
    return "Q";
  case (13):
    return "K";
  default:
    return std::format("{}", rank);
  }
}

std::string Card::ToString() const {
  if (IsJoker() == true) {
    return "JK";
  }

  return Suit() + Rank();
}

Card& Card::operator=(const Card& card) {
  this->value_ = card.value_;
  return *this;
}
