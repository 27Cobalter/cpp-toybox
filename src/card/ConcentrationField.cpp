#include "ConcentrationField.h"

#include <algorithm>
#include <format>
#include <ranges>
#include <sstream>

int32_t ConcentrationField::Index(char col, int32_t row) const {
  int32_t col_num = col - 'a';
  return row * width_ + col_num;
}

ConcentrationField::ConcentrationField(int32_t width, int32_t height) : width_(width), height_(height) {
  cards_.resize(0);
  opened_.resize(0);
  selected_.resize(0);
}

void ConcentrationField::Add(const Card& card) {
  cards_.emplace_back(card);
  opened_.emplace_back(false);
}

std::string ConcentrationField::ToString() const {
  std::stringstream ss;
  ss << std::format("{:4s}", "");
  for (auto x : std::views::iota(0, width_)) {
    ss << std::format("{:4s}", std::format("[{:c}]", 'a' + x));
  }
  ss << std::endl;

  for (auto y : std::views::iota(0, height_)) {
    ss << std::format("{:4s}", std::format("[{:d}]", y));
    for (auto x : std::views::iota(0, width_)) {
      if ((y * width_ + x) < cards_.size()) {
        std::string status;
        int32_t index = y * width_ + x;
        if (opened_[index] == true) {
          status = "---";
        } else if (std::ranges::find(selected_, index) != selected_.end()) {
          status = cards_[index].ToString();
        } else {
          status = "XXX";
        }
        ss << std::format("{:4s}", status);
      }
    }
    ss << std::endl;
  }
  return ss.str();
}

bool ConcentrationField::Selectable(char col, int32_t row) const {
  int32_t index = Index(col, row);
  if (index >= 0 && index < cards_.size()) {
    return selected_.size() < 2 && opened_[index] == false && std::ranges::find(selected_, index) == selected_.end();
  } else {
    return false;
  }
}
void ConcentrationField::Select(char col, int32_t row) {
  if (Selectable(col, row) == true) {
    int32_t index = Index(col, row);
    selected_.emplace_back(index);
  }
}
bool ConcentrationField::Check() {
  bool result = false;
  if (selected_.size() == 2) {
    result = (cards_[selected_[0]].Rank() == cards_[selected_[1]].Rank());
    if (result == true) {
      opened_[selected_[0]] = true;
      opened_[selected_[1]] = true;
    }
  }
  selected_.resize(0);
  return result;
}

int32_t ConcentrationField::NumberOfSelected() const {
  return selected_.size();
}

int32_t ConcentrationField::NumberOfHidden() const {
  return std::ranges::count(opened_, false);
}
