/*
 * Player.cpp
 *
 *  Created on: 7 Jun 2020
 *      Author: xxMrPHDxx
 */

#include "Player.h"

#include "Game.h"

Player::Player() {
	x = y = 100.f;
}

Player::~Player() {

}

int Player::update(lua_State* L){
	lua_getglobal(L, "player");
	lua_getfield(L, -1, "ptr");
	Player* player = (Player*) lua_touserdata(L, -1);

	lua_getglobal(L, "game");
	lua_getfield(L, -1, "ptr");
	Game* game = (Game*) lua_touserdata(L, -1);
	lua_pop(L, 4);

	// Get the mouse position
	lua_getglobal(L, "mouse_pos");
	lua_getfield(L, -1, "x");
	float mx = lua_tonumber(L, -1);
	lua_getfield(L, -2, "y");
	float my = lua_tonumber(L, -1);
	lua_pop(L, 3);
	sf::Vector2f mouse(mx, my);

	game->update_player_sprite(player, mouse);

	return 1;
}

int Player::render(lua_State* L){
	// Get the player
	lua_getfield(L, -1, "ptr");
	Player* player = (Player*) lua_touserdata(L, -1);

	// Get the game
	lua_getglobal(L, "game");
	lua_getfield(L, -1, "ptr");
	Game* game = (Game*) lua_touserdata(L, -1);
	lua_pop(L, 3);

	// Sprite transform stuff
	for(sf::Sprite* spr : {&player->sAura, &player->sCockpit, &player->sLwing, &player->sRwing}){
		// Scale down the sprites
		spr->setScale(0.6, 0.6);
		// Offset the player sprite position
		spr->setPosition(player->x, player->y);
		// Set the sprite rotation except the aura sprite
		if(spr == &player->sAura) continue;
		spr->setRotation(player->heading * Game::TO_DEGREES + 90.f);
	}

	// Draw the player aura sprite
	lua_getfield(L, -1, "idx_auras");
	int idx_auras = lua_tointeger(L, -1);
	game->draw_player_aura(player, idx_auras);
	lua_pop(L, 1);

	// Draw the player left and right wing sprites
	lua_getfield(L, -1, "idx_lwing");
	int idx_lwing = lua_tointeger(L, -1);
	lua_pop(L, 1);
	lua_getfield(L, -1, "idx_rwing");
	int idx_rwing = lua_tointeger(L, -1);
	lua_pop(L, 1);
	game->draw_player_wing(player, idx_lwing, idx_rwing);

	// Draw the player cockpit
	lua_getfield(L, -1, "idx_cpits");
	int idx_cpits = lua_tointeger(L, -1);
	lua_pop(L, 1);
	game->draw_player_cockpit(player, idx_cpits);

	return 1;
}
