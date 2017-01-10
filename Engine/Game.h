/******************************************************************************************
*    Chili DirectX Framework Version 16.07.20                                              *
*    Game.h                                                                                *
*    Copyright 2016 PlanetChili.net <http://www.planetchili.net>                           *
*                                                                                          *
*    This file is part of The Chili DirectX Framework.                                     *
*                                                                                          *
*    The Chili DirectX Framework is free software: you can redistribute it and/or modify   *
*    it under the terms of the GNU General Public License as published by                  *
*    the Free Software Foundation, either version 3 of the License, or                     *
*    (at your option) any later version.                                                   *
*                                                                                          *
*    The Chili DirectX Framework is distributed in the hope that it will be useful,        *
*    but WITHOUT ANY WARRANTY; without even the implied warranty of                        *
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                         *
*    GNU General Public License for more details.                                          *
*                                                                                          *
*    You should have received a copy of the GNU General Public License                     *
*    along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
******************************************************************************************/
#pragma once

#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"
#include <chrono>

class Game
{
public:
	Game( class MainWindow& wnd );
	Game( const Game& ) = delete;
	Game& operator=( const Game& ) = delete;
	void Go();
private:
	void ComposeFrame();
	void UpdateModel();
	/********************************/
	/*  User Functions              */
	/********************************/
private:
	MainWindow& wnd;
	Graphics gfx;


	// Temp ship variables
	float ship_x = 0.f;
	float ship_y = 0.f;
	const float ship_speed = 3.f;
	const float ship_size = 32.f;

	// Temp bullet variables
	constexpr static unsigned int max_bullets = 10u;
	unsigned int bullet_count = 0;
	float bullet_x[ max_bullets ];
	float bullet_y[ max_bullets ];
	float bullet_vx[ max_bullets ];
	float bullet_vy[ max_bullets ];
	const float bullet_length = 5.f;
	const float bullet_speed = 10.f;
	constexpr static float fire_rate = .0025f;
	float fire_rate_tracker = 0.f;

	// Temp timer variables
	std::chrono::time_point<std::chrono::steady_clock> start, end;
	float frame_time = 0.f;

	// Temp enemy variables
	// TODO: Create three enemies
	// TODO: Decide formula for ship speeds
	/*
	1) Moves straight (from any side ?)
	2) Moves straight at player's current position when spawned
	3) Chases player
	*/
};
