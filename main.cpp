/*
 * main.cpp
 *
 *  Created on: 6 Jun 2020
 *      Author: xxMrPHDxx
 */

#include "pch.h"

#include "ResourceManager.h"
#include "Game.h"

int main(){
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	ResourceManager::load_texture("Textures/textures.txt");

	Game game;

	while(game.isRunning()){
		game.update();
		game.render();
	}

	return 0;
}
