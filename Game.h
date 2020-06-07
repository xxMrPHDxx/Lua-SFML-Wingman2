/*
 * Game.h
 *
 *  Created on: 7 Jun 2020
 *      Author: xxMrPHDxx
 */

#ifndef GAME_H_
#define GAME_H_

#include "pch.h"

class Player;

class Game {
private:
	sf::RenderWindow* window;
	sf::Event event;

	sf::Clock clock;
	float timer;

	Player* player;

	lua_State* L;

	// Scripts
	std::string scr_update, scr_render;

	// Private methods
	void handleInput();
	void loadScripts();
public:
	static constexpr float PI = 3.1415f;
	static constexpr float TO_DEGREES = 180.f / Game::PI;
	static constexpr float TO_RADIANS = Game::PI / 180.f;

	Game();
	virtual ~Game();

	// Getters
	const bool isRunning() const { return window->isOpen(); }

	// Methods
	void update();
	void render();

	// Lua methods
	void update_player_sprite(Player*, sf::Vector2f);
	void draw_player_aura(Player*, int);
	void draw_player_wing(Player*, int, int);
	void draw_player_cockpit(Player*, int);
};

#endif /* GAME_H_ */
