#include <iostream>
#include <memory>
#include <string_view>

#include "CardGame.h"
#include "Concentration.h"
#include "Config.h"

auto main() -> int32_t {
  Config config;
  config.Load("./Config.toml");

  while (true) {
    std::shared_ptr<CardGame> game = nullptr;
    while (game == nullptr) {
      std::cout << "Input Game Type [0: Concentration, 1: ...]" << std::endl;
      std::cout << "==> ";
      int32_t selected_id;
      std::cin >> selected_id;

      switch (selected_id) {
      case (0): {
        std::string width_key  = "Concentration.Width";
        std::string height_key = "Concentration.Height";
        if (config.Contains(width_key) == true && config.Contains(height_key) == true) {
          game = std::make_shared<Concentration>(std::stoi(config.Get(width_key)), std::stoi(config.Get(height_key)));
        } else {
          game = std::make_shared<Concentration>();
        }
        break;
      }
      default:
        std::cout << "Invalid Number" << std::endl;
        game = nullptr;
        break;
      }
    }

    game->Initialize();
    game->Play();
    game->Finalize();

    std::cout << "Continue?: [y, n]" << std::endl;
    std::cout << "==> ";
    char input;
    std::cin >> input;
    if (input == 'n') {
      break;
    }
  }

  return 0;
}