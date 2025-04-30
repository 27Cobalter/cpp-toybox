#pragma once

#include <vector>

#include "Card.h"

class ConcentrationField {
private:
  std::vector<Card> cards_;
  std::vector<bool> opened_;
  int32_t width_;
  int32_t height_;
  std::vector<int32_t> selected_;

  private:
  int32_t Index(char col, int32_t row) const;

public:
  ConcentrationField(int32_t width = 9, int32_t height = 6);
  void Add(const Card& card);
  std::string ToString() const;
  bool Selectable(char col, int32_t row) const;
  void Select(char col, int32_t row);
  bool Check();
  int32_t NumberOfSelected() const;
  int32_t NumberOfHidden() const;
};
