/******************************************************************************************
*	Chili DirectX Framework Version 12.04.24											  *
*	Game.cpp																			  *
*	Copyright 2012 PlanetChili.net														  *
*																						  *
*	This file is part of The Chili DirectX Framework.									  *
*																						  *
*	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
*	it under the terms of the GNU General Public License as published by				  *
*	the Free Software Foundation, either version 3 of the License, or					  *
*	(at your option) any later version.													  *
*																						  *
*	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
*	GNU General Public License for more details.										  *
*																						  *
*	You should have received a copy of the GNU General Public License					  *
*	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
******************************************************************************************/
#include "Game.h"
#include <time.h>
#include <sstream>
#include <algorithm>
#include <d3d11_2.h>

Game::Game(HWND hWnd, const KeyboardServer& kServer, const MouseServer& mServer)
	: gfx(hWnd),
	audio(hWnd),
	kbd(kServer),
	mouse(mServer),
	enter_is_pressed(false),
	up_is_pressed(false),
	space_is_pressed(false)
{
	srand((unsigned int)time(NULL));
	game.is_over = false;
	game.score = 0;
	ship.x = 385;
	global_enemy.count = 0;
	global_laser.count = 0;

	enemy.reserve(MAX_ENEMIES);
	laser.reserve(MAX_LASERS);

	/*// Nullify enemy array
	for (int index_enemy = 0; index_enemy < MAX_ENEMIES; index_enemy++)
	{
	Null_Mem( index_enemy,ENEMY );
	}
	// Nullify laser array
	for (int index_laser = 0; index_laser < MAX_LASERS; index_laser++)
	{
	Null_Mem( index_laser,LASER );
	}*/
}

Game::~Game()
{
}

void Game::Go()
{
#ifdef _DEBUG
	float dt = 1.0f / 60.0f;
#else
	timer.StopWatch();
	float dt = timer.GetTimeMilli() * 0.001f;
	timer.StartWatch();
#endif

	if (kbd.SpaceIsPressed())
	{
		if (!space_is_pressed)
		{
			space_is_pressed = true;
			game.is_paused = !game.is_paused;
		}
	}
	else
	{
		space_is_pressed = false;
		if (!game.is_paused)
		{
			Update_Progression();
			Update_Keyboard_Input(dt);
			Update_Laser(dt);
			Update_Enemy(dt);

			UpdateGameObjectStates();
		}
	}
	gfx.BeginFrame();
	ComposeFrame();
	gfx.EndFrame();
}

void Game::Draw_Laser(int x, int y, LASER_DIRECTION direction) {
	float old_x = x;
	if (direction == MIDDLE)
	{
		for (int r = 0; r < 3; r++)
		{
			for (int c = 0; c < 10; c++)
			{
				gfx.PutPixel(x + r, y + c, 255, 50, 50);
			}
		}
	}
	else if (direction == LEFT)
	{
		for (int r = 0; r < 3; r++)
		{
			for (int c = 0; c < 10; c++)
			{
				gfx.PutPixel(x + r, y + c, 255, 50, 50);
				// Draws top to bottom so use +=
				x += 1.0f;
			}
			x = old_x;
		}
	}
	else if (direction == RIGHT)
	{
		for (int r = 0; r < 3; r++)
		{
			for (int c = 0; c < 10; c++)
			{
				gfx.PutPixel(x + r, y + c, 255, 50, 50);
				// Draws top to bottom so use -=
				x -= 1.0f;
			}
			x = old_x - 3;
		}
	}
}

void Game::Draw_Score(int x, int y) {
	std::stringstream ss;
	ss << game.score;
	std::string str(ss.str());

	for (int i = 0; i < str.size(); i++)
	{
		Draw_Digit(str[i] - 48, x + game.digit_width * i, y);
	}
}

void Game::Set_New_Lasers(LASER_DIRECTION* direction, const float* offset) {
	for (int i = 0; i < global_laser.multiple; ++i)
	{		
		Laser l;
		l.x = (float)ship.x + offset[i];
		l.direction = direction[i];
		l.y = 564;

		laser.emplace_back(l);
	}
	//for (int index_laser = 0; index_laser < MAX_LASERS; index_laser++)
	//{
	//	if (laser[index_laser].index == EMPTY &&
	//		index_laser + global_laser.multiple <= MAX_LASERS)
	//	{
	//		for (int laser_count = 0; laser_count < global_laser.multiple; laser_count++)
	//		{
	//			// shift a memory slot each loop while
	//			// using index_laser as the base starting point
	//			// since it was delcared outside of this loop
	//			laser[index_laser + laser_count].x = (float)ship.x + offset[laser_count];
	//			laser[index_laser + laser_count].direction = direction[laser_count];
	//			laser[index_laser + laser_count].y = 564;
	//			laser[index_laser + laser_count].index = USED;
	//			global_laser.count++;
	//		}
	//		break;
	//	}
	//}
}

void Game::Deploy_Enemy(float delta_time) 
{
	if (global_enemy.wait_count >= global_enemy.wait_time)
	{
		if (enemy.size() < MAX_ENEMIES)
		{
			switch (game.level)
			{
			case 1:
				enemy.emplace_back<Enemy>({ global_enemy.level, 
					global_enemy.color, &global_enemy });
				break;
			case 2:
				enemy.emplace_back<Enemy>({ global_enemy.level2_hp,
					global_enemy.level2_color, &global_enemy });
				break;
			case 3:
				enemy.emplace_back<Enemy>({ global_enemy.level3_hp,
					global_enemy.level3_color, &global_enemy });
				break;
			case 4:
				enemy.emplace_back<Enemy>({ global_enemy.level4_hp,
					global_enemy.level4_color, &global_enemy });
				break;
			default:
				break;
			}
		}
		global_enemy.wait_count = 0.0f;
	}
	global_enemy.wait_count += delta_time;
}

void Game::Update_Keyboard_Input(float delta_time) {
	int frameStep = delta_time * ship.speed;
	global_laser.count += frameStep;

	// Shoot lasers
	if (kbd.UpIsPressed())
	{
		if (laser.size() < MAX_LASERS)
		{
			if (global_laser.count >= global_laser.fireRate)
			{
				global_laser.count = 0;
				switch (game.level)
				{
				case 1:
					Set_New_Lasers(global_laser.level1_direction,
						global_laser.level1_x_offset);
					break;
				case 2:
					Set_New_Lasers(global_laser.level2_direction,
						global_laser.level2_x_offset);
					break;
				case 3:
					Set_New_Lasers(global_laser.level3_direction,
						global_laser.level3_x_offset);
					break;
				case 4:
					Set_New_Lasers(global_laser.level4_direction,
						global_laser.level4_x_offset);
					break;
				default:
					break;
				}
			}
		}
	}
	// Ship movement
	if (kbd.LeftIsPressed())
	{
		if (ship.x - frameStep <= 0)
		{
			ship.x = 0;
		}
		else
		{
			ship.x += -frameStep;
		}
	}
	else if (kbd.RightIsPressed())
	{
		if (ship.x + frameStep > 799 - 30)
		{
			ship.x = 799 - 30;
		}
		else
		{
			ship.x += frameStep;
		}
	}
	// Restart game
	if (kbd.EnterIsPressed())
	{
		if (!enter_is_pressed)
		{
			if (game.is_over)
			{
				enter_is_pressed = true;
				Restart_Game();
			}
		}
	}
	else
	{
		enter_is_pressed = false;
	}
}

void Game::Update_Progression() {
	// Check for Level 1
	if (game.score >= 0 && game.score < game.level2)
	{
		game.level = 1;
		for (int index_color = 0; index_color < 3; index_color++)
		{
			global_enemy.color[index_color] = global_enemy.level1_color[index_color];
		}
		global_enemy.hp = global_enemy.level1_hp;
		global_laser.speed = global_laser.level1_speed;
		global_laser.multiple = 1;
	}
	else if (game.score >= game.level2 && game.score < game.level3)
	{
		game.level = 2;
		for (int index_color = 0; index_color < 3; index_color++)
		{
			global_enemy.color[index_color] = global_enemy.level2_color[index_color];
		}
		global_enemy.hp = global_enemy.level2_hp;
		global_laser.speed = global_laser.level2_speed;
		global_laser.multiple = 2;
	}
	else if (game.score >= game.level3 && game.score < game.level4)
	{
		game.level = 3;
		for (int index_color = 0; index_color < 3; index_color++)
		{
			global_enemy.color[index_color] = global_enemy.level3_color[index_color];
		}
		global_enemy.hp = global_enemy.level3_hp;
		global_laser.speed = global_laser.level3_speed;
		global_laser.multiple = 3;
	}
	else if (game.score >= game.level4)
	{
		game.level = 4;
		for (int index_color = 0; index_color < 3; index_color++)
		{
			global_enemy.color[index_color] = global_enemy.level4_color[index_color];
		}
		global_enemy.hp = global_enemy.level4_hp;
		global_laser.speed = global_laser.level3_speed;
		global_laser.multiple = 4;
	}
}

void Game::Update_Laser(float delta_time) {
	int frameStep = delta_time * global_laser.speed;
	
	for (auto &l : laser)
	{
		if (l.direction == LEFT)
		{
			l.x -= frameStep + 3.f;
		}
		else if (l.direction == RIGHT)
		{
			l.x += frameStep + 3.f;
		}
		l.y -= frameStep;
	}
}

void Game::Update_Enemy(float delta_time) 
{
	Deploy_Enemy(delta_time);
	for (auto &e : enemy)
	{
		e.Update(delta_time);
	}
}

void Game::UpdateGameObjectStates()
{
	for (auto &e : enemy)
	{
		CheckEnemyCollisions(e);
	}

	for (auto &l : laser)
	{
		CheckLaserOnScreen(l);
	}

	CleanVectors();
}

void Game::CheckLaserOnScreen(Laser & ThisLaser)
{	
	// Check if laser hits screen edges
	if (ThisLaser.y <= 0 || ThisLaser.x < 0 || ThisLaser.x + global_laser.width >= SCREENWIDTH)
	{
		ThisLaser.isAlive = false;
	}
}

void Game::CheckEnemyCollisions(Enemy & ThisEnemy)
{
	// Check if enemy hits player ship
	if (ship.x + ship.width > ThisEnemy.x &&
		ship.x < ThisEnemy.x + global_enemy.width &&
		ship.y + ship.height > ThisEnemy.y &&
		ship.y < ThisEnemy.y + global_enemy.height)
	{
		game.is_over = true;
	}

	// Check if enemy is hit by laser
	bool accounted_for = false;
	for (int index_laser = 0; index_laser < laser.size(); ++index_laser)
	{
		if ((int)laser[index_laser].x + global_laser.width > ThisEnemy.x &&
			(int)laser[index_laser].x < ThisEnemy.x + global_enemy.width &&
			laser[index_laser].y + global_laser.height > ThisEnemy.y &&
			laser[index_laser].y < ThisEnemy.y + global_enemy.height)
		{
			--ThisEnemy.hp;
			laser[index_laser].isAlive = false;
			if (ThisEnemy.hp <= 0 && !accounted_for)
			{
				accounted_for = true;
				game.score += 1;
			}
		}
	}

	// Check if enemy is still on screen
	if (ThisEnemy.y + global_enemy.height >= SCREENHEIGHT)
	{
		ThisEnemy.hp = 0;
	}
}

void Game::CleanVectors()
{
	/*
	Two things are happening here.  
	
	One std::remove_if is testing each object using the lambda function.  
	If the function returns true, the element after the current is moved into
	the current position, and when it is finished with the entire list, the 
	remove_if function returns an iterator or pointer to the new end of the array.
	
	Two, the erase function, the version we use here anyway, erases from the returned
	iterator to the end of the array, which now makes the vector go from 0 to 
	whatever position is the new end.  Basically, it replaces the Null_Mem and 
	Shift_Memory functions you created, again brilliant of you to have done this
	on your own.
	*/

	auto enemyEraseBegin = std::remove_if(enemy.begin(), enemy.end(),
		[](Enemy &E)
	{
		bool remove = E.hp <= 0;
		return remove;

	});
	auto enemyEraseEnd = enemy.end();

	auto laserEraseBegin = std::remove_if(laser.begin(), laser.end(),
		[](const Laser &L)
	{
		bool remove = !L.isAlive;
		return remove;
	});
	auto laserEraseEnd = laser.end();

	enemy.erase(enemyEraseBegin, enemyEraseEnd);
	laser.erase(laserEraseBegin, laserEraseEnd);
}

void Game::Restart_Game() {
	enemy.clear();
	laser.clear();
	game.is_over = false;
	game.score = 0;
	ship.x = 385;
	global_enemy.count = 0;
	global_enemy.wait_count = 0;
	global_laser.count = 0;
}

void Game::ComposeFrame() {
	if (!game.is_over)
	{
		// Draw ship stuff
		ship.Draw(gfx);

		// Draw laser stuff
		for (int index_laser = 0; index_laser < laser.size(); index_laser++)
		{
			Draw_Laser((int)laser[index_laser].x,
				laser[index_laser].y,
				laser[index_laser].direction);
		}

		// Draw enemy stuff
		for (auto &e : enemy)
		{
			e.Draw(gfx);
		}		
	}

	Draw_Score(game.score_x, game.score_y);
}
