#include "game.hpp"
#include "state_menu.hpp"


int main() {
	Game game;
	game.pushState(new StateMenu());

	game.gameLoop();
	return 0;
}