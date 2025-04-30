#pragma once

#include "CardGame.h"
#include "ConcentrationField.h"

class Concentration : public CardGame {
private:
  ConcentrationField field_;

public:
  Concentration();
  Concentration(int32_t width, int32_t height);
  void Initialize() override;
  void Finalize() override;
  void Play() override;
};
