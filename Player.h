/*
 * Player.h
 *
 *  Created on: 7 Jun 2020
 *      Author: xxMrPHDxx
 */

#ifndef PLAYER_H_
#define PLAYER_H_

#include "pch.h"

class Player {
public:
	sf::Sprite sAura, sCockpit, sLwing, sRwing;

	// Physics attribute
	float x, y;
	float heading = 0.0f;

	bool moving;
	float vel_min = 8.0, vel_max = 800.0;
	sf::Vector2f speed;
	float acceleration, deceleration;
	float drag;

	Player();
	virtual ~Player();
	static int update(lua_State*);
	static int render(lua_State*);
	static int move(lua_State*);
};

#endif /* PLAYER_H_ */
