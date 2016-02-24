/******************************************************************************************
*  Chili DirectX Framework Version 12.04.24                                              *
*  Game.h                                                                                *
*  Copyright 2012 PlanetChili.net                                                        *
*                                                                                        *
*  This file is part of The Chili DirectX Framework.                                     *
*                                                                                        *
*  The Chili DirectX Framework is free software: you can redistribute it and/or modify   *
*  it under the terms of the GNU General Public License as published by                  *
*  the Free Software Foundation, either version 3 of the License, or                     *
*  (at your option) any later version.                                                   *
*                                                                                        *
*  The Chili DirectX Framework is distributed in the hope that it will be useful,        *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of                        *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                         *
*  GNU General Public License for more details.                                          *
*                                                                                        *
*  You should have received a copy of the GNU General Public License                     *
*  along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
******************************************************************************************/
#pragma once

#include "D3DGraphics.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Sound.h"
#include "Timer.h"
#include <vector>

#define MAX_LASERS 100
#define MAX_ENEMIES 8
#define SCREENWIDTH 800
#define SCREENHEIGHT 600

class Game
{
public:
	Game(HWND hWnd, const KeyboardServer& kServer, const MouseServer& mServer);
	~Game();
	void Go();
private:
	enum LASER_DIRECTION
	{
		LEFT,
		RIGHT,
		MIDDLE
	};

	enum MEMORY_STATE
	{
		EMPTY,
		USED
	};

	enum GAME_ITEM
	{
		LASER,
		ENEMY
	};

	enum RGB_COLOR
	{
		E_RED = 0,
		E_GREEN = 1,
		E_BLUE = 2
	};

	struct Global_Laser
	{
		int             level;
		const int       level1_speed = 250;
		const int       level2_speed = 375;
		const int       level3_speed = 600;
		char            multiple;
		unsigned int    speed = 250;
		int             width = 3;
		int             height = 10;
		int             count = 0;
		const float     level1_x_offset[1] = { 15.0f };
		const float     level2_x_offset[2] = { 10.0f,20.0f };
		const float     level3_x_offset[3] = { 5.0f,15.0f,25.0f };
		const float     level4_x_offset[4] = { 0.0f,10.0f,20.0f,30.0f };
		LASER_DIRECTION level1_direction[1] = { MIDDLE };
		LASER_DIRECTION level2_direction[2] = { MIDDLE,MIDDLE };
		LASER_DIRECTION level3_direction[3] = { MIDDLE,MIDDLE,MIDDLE };
		LASER_DIRECTION level4_direction[4] = { LEFT,MIDDLE,MIDDLE,RIGHT };
	};

	struct Global_Enemy
	{
		unsigned char       level1_color[3] = { 158,156,114 };
		unsigned char       level2_color[3] = { 0,255,255 };
		unsigned char       level3_color[3] = { 86,188,84 };
		unsigned char       level4_color[3] = { 79,87,193 };
		unsigned char       color[3];
		const char          level1_hp = 3;
		const char          level2_hp = 10;
		const char          level3_hp = 20;
		const char          level4_hp = 30;
		const char          level5_hp = 40;
		const unsigned char level6_hp = 200;
		int                 level = 1;
		int                 count = 0;
		float               wait_count = 0;
		float               wait_time = 2.0f;
		int                 width = 150;
		int                 height = 10;
		float               speed = 150.0f;
		int                 hp;
		char                multiple = 1; // default
	};

	struct Laser
	{
		float x;
		int   y;
		bool isAlive;
		LASER_DIRECTION direction = MIDDLE; // default
		MEMORY_STATE index = EMPTY;         // default
	};

	struct Enemy
	{
		Enemy()
			:
			x(0), y(0), hp(0), color{}, index(EMPTY), prop(nullptr)
		{}
		Enemy(int Hp, unsigned char Color[3], Global_Enemy *Props)
			:
			x(rand() % (SCREENWIDTH - Props->width * 2)),
			y(0),
			hp(Hp),			
			index(EMPTY),
			prop(Props)
		{
			for (int i = 0; i < 3; ++i)
			{
				color[i] = Color[i];
			}
		}
		void Update(const float DT)
		{
			int frameStep = DT * prop->speed;
			y += frameStep;
		}
		void Draw(D3DGraphics &Gfx)
		{
			Gfx.DrawBox(x, y, prop->width, prop->height, D3DCOLOR_XRGB(color[0], color[1], color[2]));
		}
		
		float         x;
		float         y;
		int           hp;
		unsigned char color[3];
		MEMORY_STATE index = EMPTY; // default
		Global_Enemy *prop;
	};

	struct MGame
	{
		int       score = 0;
		const int score_x = 600;
		const int score_y = 566;
		bool      is_over = false;
		char      level;
		const int level1 = 0;
		const int level2 = 10;
		const int level3 = 20;
		const int level4 = 30;
		const int level5 = 40;
		const int level6 = 50;
		const int digit_width = 20;
		bool      is_paused = false;
	};

	struct Ship
	{
		void Draw(D3DGraphics &Gfx)
		{
			Gfx.DrawBox(x, y, width, height, color);
		}
		int          x = 385;
		const int    y = 569;
		int          width = 30;
		int          height = 30;
		unsigned int speed = 250;
		D3DCOLOR	 color = D3DCOLOR_XRGB(255, 255, 255);
	};

private:
	void ComposeFrame();
	/********************************/
	/*  User Functions              */

	void Draw_Score(int x, int y);
	void Draw_Digit(int digit, int x, int y);
	void Draw_Laser(int x, int y, LASER_DIRECTION direction);

	void Deploy_Enemy(float delta_time);
	void Update_Enemy(float delta_time);

	void Set_New_Lasers(LASER_DIRECTION* direction, const float* offset);
	void Update_Laser(float delta_time);
	
	void UpdateGameObjectStates();
	void CheckLaserOnScreen(Laser &ThisLaser);
	void CheckEnemyCollisions(Enemy &ThisEnemy);
	void CleanVectors();

	void Update_Keyboard_Input(float delta_time);
	void Update_Progression();
	void Restart_Game();


	/********************************/
private:
	D3DGraphics gfx;
	KeyboardClient kbd;
	MouseClient mouse;
	DSound audio;
	/********************************/
	/*  User Variables              */

	Timer timer;
	Global_Laser  global_laser;
	Global_Enemy  global_enemy;
	std::vector<Laser> laser;
	std::vector<Enemy> enemy;
	/*Laser         laser[MAX_LASERS];
	Enemy         enemy[MAX_ENEMIES];*/

	MGame game;

	Ship ship;

	bool      up_is_pressed;
	bool      enter_is_pressed;
	bool      space_is_pressed;
	/********************************/
};
