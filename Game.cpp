/*
 * Game.cpp
 *
 *  Created on: 7 Jun 2020
 *      Author: xxMrPHDxx
 */

#include <cmath>

#include "Game.h"

#include "Player.h"
#include "ResourceManager.h"

Game::Game() {
	window = new sf::RenderWindow(sf::VideoMode(640, 480), "Wingman 2!", sf::Style::Close | sf::Style::Titlebar);
	window->setFramerateLimit(30);
	timer = 0.0f;
	clock.restart();

	player = new Player();

	L = luaL_newstate();
	luaL_openlibs(L);

	lua_newtable(L);
	lua_pushlightuserdata(L, this);
	lua_setfield(L, -2, "ptr");
	lua_setglobal(L, "game");

	lua_newtable(L);
	lua_pushlightuserdata(L, player);
	lua_setfield(L, -2, "ptr");
	lua_setglobal(L, "player");

	// Register variables and methods for player
	lua_getglobal(L, "player");

	for(std::string idx : { "idx_auras", "idx_cpits", "idx_lwing", "idx_rwing" }){
		lua_pushinteger(L, 0);
		lua_setfield(L, -2, idx.c_str());
	}

	lua_pushcfunction(L, Player::update);
	lua_setfield(L, -2, "update");
	lua_pushcfunction(L, Player::render);
	lua_setfield(L, -2, "render");

	lua_pop(L, 1);

	loadScripts();
}

Game::~Game() {
	delete window;
}

void Game::handleInput(){
	while(window->pollEvent(event)){
		switch(event.type){
			case sf::Event::Closed: window->close(); break;
			default: break;
		}
	}
}

void Game::loadScripts(){
	scr_update = ResourceManager::load_textfile("Scripts/update.lua");
	scr_render = ResourceManager::load_textfile("Scripts/render.lua");
}

void Game::update(){
	handleInput();
	timer += clock.restart().asMilliseconds();

	lua_pushnumber(L, timer/100.0f);
	lua_setglobal(L, "time");

	// Set the mouse vector
	lua_newtable(L);
	sf::Vector2i m_pixel = sf::Mouse::getPosition(*window);
	sf::Vector2f mouse = window->mapPixelToCoords(m_pixel);
	lua_pushnumber(L, mouse.x);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L, mouse.y);
	lua_setfield(L, -2, "y");
	lua_setglobal(L, "mouse_pos");

	if(luaL_dostring(L, scr_update.c_str())){
		printf("Error: %s\n", lua_tostring(L, -1));
	}
}

void Game::render(){
	window->clear();
	if(luaL_dostring(L, scr_render.c_str())){
		printf("Error: %s\n", lua_tostring(L, -1));
	}
	window->display();
}

void Game::update_player_sprite(Player* p, sf::Vector2f mouse){
	// Calculate player sprites rotation
	sf::Vector2f d = sf::Vector2f(p->x, p->y) - mouse;
	float angle = std::atan2(d.y, d.x);
	if(angle < 0.0f) angle += PI;
	player->heading = angle;
}

void Game::draw_player_aura(Player* p, int idx_auras){
	Textures& texs = ResourceManager::textures["Accessories"];
	sf::Texture& tex_auras = *texs[idx_auras%11];
	p->sAura.setTexture(tex_auras);

	// Set the origin
	sf::FloatRect bounds = p->sAura.getLocalBounds();
	sf::Vector2f origin(bounds.width/2, bounds.height/2);
	p->sAura.setOrigin(origin);

	// Rotate the aura continuously
	p->sAura.setRotation(timer / 20.0);

	window->draw(p->sAura);
}

void Game::draw_player_wing(Player* p, int idx_lwing, int idx_rwing){
	Textures& texs = ResourceManager::textures["Accessories"];
	sf::Texture& tex_lwing = *texs[27+idx_lwing % 13];
	sf::Texture& tex_rwing = *texs[40+idx_rwing % 13];
	p->sLwing.setTexture(tex_lwing);
	p->sRwing.setTexture(tex_rwing);

	// Set the origin
	for(sf::Sprite* spr : {&p->sLwing, &p->sRwing}){
		sf::FloatRect bounds = spr->getLocalBounds();
		sf::Vector2f origin(bounds.width/2, bounds.height/2);
		spr->setOrigin(origin);
	}

	window->draw(p->sLwing);
	window->draw(p->sRwing);
}

void Game::draw_player_cockpit(Player* p, int idx_cpits){
	Textures& texs = ResourceManager::textures["Accessories"];
	sf::Texture& tex_cpits = *texs[11+idx_cpits % 16];
	p->sCockpit.setTexture(tex_cpits);

	// Set the origin
	sf::FloatRect bounds = p->sCockpit.getLocalBounds();
	sf::Vector2f origin(bounds.width/2, bounds.height/2);
	p->sCockpit.setOrigin(origin);

	window->draw(p->sCockpit);
}
