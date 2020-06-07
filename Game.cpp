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

	// Create a game table
	lua_newtable(L);
	lua_pushlightuserdata(L, this);
	lua_setfield(L, -2, "ptr");
	lua_setglobal(L, "game");

	// Create a player table
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
	luaL_Reg methods[] = {
		{ "update", Player::update },
		{ "render", Player::render },
		{ "move", Player::move }
	};
	for(luaL_Reg* reg=&methods[0]; reg<&methods[sizeof(methods)/sizeof(luaL_Reg)]; reg++){
		lua_pushcfunction(L, reg->func);
		lua_setfield(L, -2, reg->name);
	}
	lua_pop(L, 1);

	// Setup keyboard keys
	lua_newtable(L);
	std::ifstream ifs("Config/keyboard.cfg");
	std::string name, key, _buf;
	int code;
	if(!ifs.is_open()){
		printf("Failed to open keyboard config!\n");
		exit(-1);
	}
	for(;!ifs.eof() && ifs >> name && ifs >> _buf && ifs >> key;){
		try{
			lua_pushinteger(L, code = std::stoi(key));
			lua_setglobal(L, name.c_str());
			lua_pushboolean(L, false);
			lua_seti(L, -2, code);
		}catch(std::invalid_argument& e){
			std::cout << "Error: " << e.what() << std::endl;
			exit(-1);
		}
	}
	lua_setglobal(L, "keys");

	loadScripts();
}

Game::~Game() {
	delete window;
}

void Game::handleInput(){
	while(window->pollEvent(event)){
		switch(event.type){
			case sf::Event::Closed: window->close(); break;
			case sf::Event::KeyPressed: onKeyPressed(event.key); break;
			case sf::Event::KeyReleased: onKeyReleased(event.key); break;
			default: break;
		}
	}
}

void Game::loadScripts(){
	scr_update = ResourceManager::load_textfile("Scripts/update.lua");
	scr_render = ResourceManager::load_textfile("Scripts/render.lua");
	src_keypressed = ResourceManager::load_textfile("Scripts/key_pressed.lua");
	src_keyreleased = ResourceManager::load_textfile("Scripts/key_released.lua");
}

void Game::onKeyPressed(sf::Event::KeyEvent& e){
	lua_pushinteger(L, e.code);
	lua_setglobal(L, "key");
	if(luaL_dostring(L, src_keypressed.c_str())){
		printf("Error: %s\n", lua_tostring(L, -1));
	}
}

void Game::onKeyReleased(sf::Event::KeyEvent& e){
	lua_pushinteger(L, e.code);
	lua_setglobal(L, "key");
	if(luaL_dostring(L, src_keyreleased.c_str())){
		printf("Error: %s\n", lua_tostring(L, -1));
	}
}

void Game::update(){
	delta_time = dt_clock.restart().asMilliseconds() / 1000.f;
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
	sf::Sprite bg(*ResourceManager::textures["Backgrounds"][16]);
	window->draw(bg);
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
	p->heading = angle;
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

	// Wing animation direction
	float angle_prog = 10 * std::sin(timer / 200.f);
	sf::Vector2f dir(std::cos(p->heading)*angle_prog, std::sin(p->heading)*angle_prog);

	for(sf::Sprite* spr : {&p->sLwing, &p->sRwing}){
		// Set the origin
		sf::FloatRect bounds = spr->getLocalBounds();
		sf::Vector2f origin(bounds.width/2, bounds.height/2);
		spr->setOrigin(origin);

		// Set the wing movement
		spr->setPosition(p->x + dir.x, p->y + dir.y);
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
