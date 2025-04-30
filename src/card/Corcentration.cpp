#include "Concentration.h"

#include <format>
#include <iostream>
#include <ranges>

Concentration::Concentration() : Concentration(9, 6) {}

Concentration::Concentration(int32_t width, int32_t height) : CardGame(2), field_(width, height) {}

void Concentration::Initialize() {
  deck_.Shuffle();
  while (deck_.DeckSize() != 0) {
    field_.Add(deck_.DrawOne());
  }
}
void Concentration::Finalize() {}

void Concentration::Play() {
  for (;;) {
    std::cout << field_.ToString() << std::endl;

    while (field_.NumberOfSelected() != 2) {
      char col;
      int32_t row;
      std::cout << "Select Position[e.g. a3]" << std::endl;
      std::cout << "==> ";
      std::cin >> col >> row;
      bool selectable = field_.Selectable(col, row);
      if (selectable == true) {
        field_.Select(col, row);
        std::cout << field_.ToString() << std::endl;
      } else {
        std::cout << "Invalid Input" << std::endl;
      }
    }
    bool result = field_.Check();
    if (result == true) {
      std::cout << "Hit!" << std::endl;
    } else {
      std::cout << "Miss..." << std::endl;
    }
    int32_t number_of_hidden = field_.NumberOfHidden();
    if (number_of_hidden == 0) {
      break;
    }
    std::cout << std::endl;
  }
}