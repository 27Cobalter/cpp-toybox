#include "Deck.h"

#include <cassert>
#include <numeric>
#include <random>
#include <ranges>

Deck::Deck(int32_t number_of_joker) {
  int32_t count = 52 + number_of_joker;
  deck_.reserve(count);
  for (auto i : std::views::iota(0, count)) {
    deck_.emplace_back(Card{i});
  }
}

void Deck::Shuffle() {
  const int32_t max_index = deck_.size() - 1;

  std::random_device seed_gen;
  std::default_random_engine engine(seed_gen());

  std::uniform_int_distribution<> uniform(0, deck_.size());
  for (auto i : std::views::iota(0, max_index - 1)) {
    std::uniform_int_distribution<>::param_type param(i, max_index);
    uniform.param(param);
    int32_t rand = uniform(engine);

    std::swap(deck_[i], deck_[rand]);
  }
}

Card Deck::DrawOne() {
  Card back = deck_.back();
  deck_.pop_back();
  return back;
}

size_t Deck::DeckSize() const {
  return deck_.size();
}
