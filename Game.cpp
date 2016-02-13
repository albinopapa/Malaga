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

Game::Game( HWND hWnd,const KeyboardServer& kServer,const MouseServer& mServer )
:	gfx( hWnd ),
	audio( hWnd ),
	kbd( kServer ),
	mouse( mServer ),
    enemy_level( 0 ),
    // enemy_hp( enemy_level1_hp ),
    // game_level( game_level1 ),
    game_score( 0 ),
    game_is_over( false ),
    ship_x( 385 ),
    ship_speed( 250 ),
    ship_width( 30 ),
    ship_height( 30 ),
    // laser_speed( laser_level1_speed ),
    laser_count( 0 ),
    laser_width( 3 ),
    laser_height( 10 ),
    enter_is_pressed( false ),
    up_is_pressed( false ),
    enemy_speed( 150 ),
    enemy_count( 0 ),
    enemy_wait_count( 0 ),
    enemy_wait_time( 80 ),
    enemy_width( 150 ),
    enemy_height( 10 )
{
	srand( (unsigned int)time( NULL ) );
    for (int index = 0; index < MAX_ENEMIES; index++)
    {
        enemy[index].x = NULL;
        enemy[index].y = NULL;
        enemy[index].hp = NULL;
        for (int i = 0; i < 3; i++)
        {
            enemy[index].color[i] = NULL;
        }
    }
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
    if( !kbd.SpaceIsPressed() )
    {
        Update_Progression();
        Update_Keyboard_Input( dt );
        Deploy_Enemy();
        Update_Laser( dt );
        Update_Enemy( dt );
    }

	gfx.BeginFrame();
	ComposeFrame();
	gfx.EndFrame();
}

// void Game::Set_PxPy(){
// 	px = max(radius, min(px, SCREENWIDTH - radius - 1));
// 	py = max(radius, min(py, SCREENHEIGHT - radius - 1));
// }

void Game::Draw_Ship( int x,int y ){
    for( int r = 0; r < ship_width; r++ )
    {
        for( int c = 0; c < ship_height; c++ )
        {
            gfx.PutPixel( x + c, y + r,255,255,255 );
        }
    }
}

void Game::Draw_Laser( int x,int y ){
    for( int r = 0; r < 3; r++ )
    {
        for( int c = 0; c < 10; c++ )
        {
            gfx.PutPixel( x + r, y + c,255,50,50 );
        }
    }
}

void Game::Draw_Laser_Diagonal( float x,int y,char* direction){
    float old_x = x;
    if( direction == "Left")
    {
        float old_x = x;
        for( int r = 0; r < 3; r++ )
        {
            for( int c = 0; c < 10; c++ )
            {
                gfx.PutPixel( x + r, y + c,255,50,50 );
                // Draws top to bottom so use +
                x += 1.0f;
            }
            x = old_x + 1;
        }
    }
    else if( direction == "Right" )
    {
        for( int r = 0; r < 3; r++ )
        {
            for( int c = 0; c < 10; c++ )
            {
                gfx.PutPixel( x + r, y + c,255,50,50 );
                // Draws top to bottom so use -
                x -= 1.0;
            }
            x = old_x - 1;
        }
    }
}

void Game::Reverse_String( char* pChar ){
    int temp; int e = 0; int s = 0;
    while( pChar[e] != 0 )
        e++;
    e--;
    for( ; s < e; s++, e-- )
    {
        temp = pChar[s];
        pChar[s] = pChar[e];
        pChar[e] = temp;
    }
}

void Game::Int_To_String( unsigned int num, char* chars,int buff_length ){

    if ( num == 0 )
    {
        chars[ 0 ] = '0';
    }
    else
    {
        int i;
        for( i = 0; i < buff_length - 1 && num > 0; i++ )
        {
            chars[i] = num % 10 + 48;
            num /= 10;
        }
        chars[i] = '\0';
        Reverse_String( chars );
    }
}

void Game::Draw_Digit( int digit, int x,int y ){
    if     ( digit == 0 )
    {
        gfx.PutPixel(1 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 8 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 8 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 8 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 8 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 8 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 8 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 15 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 15 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 15 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 15 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 15 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 15 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 18 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 18 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 18 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 18 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 18 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 18 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 23 + y, 218, 198, 0);
    }
    else if( digit == 1 )
    {
        gfx.PutPixel(5 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 8 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 8 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 8 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 15 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 15 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 15 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 18 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 18 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 18 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 23 + y, 218, 198, 0);
    }
    else if( digit == 2 )
    {
        gfx.PutPixel(2 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 8 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 8 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 8 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 8 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 8 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 8 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 15 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 15 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 15 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 18 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 18 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 18 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 23 + y, 218, 198, 0);
    }
    else if( digit == 3 )
    {
        gfx.PutPixel(2 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 8 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 8 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 8 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 15 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 15 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 15 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 15 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 15 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 15 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 18 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 18 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 18 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 18 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 18 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 18 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 23 + y, 218, 198, 0);
    }
    else if( digit == 4 )
    {
        gfx.PutPixel(9 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 8 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 8 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 8 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 8 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 8 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 15 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 15 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 15 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 15 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 15 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(15 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(15 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 18 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 18 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 18 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 23 + y, 218, 198, 0);
    }
    else if( digit == 5 )
    {
        gfx.PutPixel(1 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 8 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 8 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 8 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 15 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 15 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 15 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 18 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 18 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 18 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 18 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 23 + y, 218, 198, 0);
    }
    else if( digit == 6 )
    {
        gfx.PutPixel(8 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 8 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 8 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 8 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(15 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(15 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 15 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 15 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 15 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 15 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 15 + y, 218, 198, 0);
        gfx.PutPixel(15 + x, 15 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(15 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(15 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 18 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 18 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 18 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 18 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 18 + y, 218, 198, 0);
        gfx.PutPixel(15 + x, 18 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 23 + y, 218, 198, 0);

    }
    else if( digit == 7 )
    {
        gfx.PutPixel(0 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(15 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(15 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(15 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(15 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(15 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 8 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 8 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 8 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 8 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 15 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 15 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 15 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 18 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 18 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 18 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 23 + y, 218, 198, 0);
    }
    else if( digit == 8 )
    {
        gfx.PutPixel(5 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 8 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 8 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 8 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 8 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 8 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 15 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 15 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 15 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 15 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 15 + y, 218, 198, 0);
        gfx.PutPixel(15 + x, 15 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(15 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(15 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 18 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 18 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 18 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 18 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 18 + y, 218, 198, 0);
        gfx.PutPixel(15 + x, 18 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(15 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 23 + y, 218, 198, 0);
    }
    else if( digit == 9 )
    {
        gfx.PutPixel(5 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 0 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 1 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 2 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 3 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 4 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 5 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(15 + x, 6 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(15 + x, 7 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 8 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 8 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 8 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 8 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 8 + y, 218, 198, 0);
        gfx.PutPixel(15 + x, 8 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(15 + x, 9 + y, 218, 198, 0);
        gfx.PutPixel(0 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(15 + x, 10 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(15 + x, 11 + y, 218, 198, 0);
        gfx.PutPixel(1 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(15 + x, 12 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(15 + x, 13 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(15 + x, 14 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 15 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 15 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 16 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(14 + x, 17 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 18 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 18 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 18 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(13 + x, 19 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(12 + x, 20 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(11 + x, 21 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(8 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(9 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(10 + x, 22 + y, 218, 198, 0);
        gfx.PutPixel(2 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(3 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(4 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(5 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(6 + x, 23 + y, 218, 198, 0);
        gfx.PutPixel(7 + x, 23 + y, 218, 198, 0);
    }
}

void Game::Draw_Score( int x,int y ){
    char score[11] = { NULL };
    if(game_score)
        game_score = game_score;
    Int_To_String( game_score,score,sizeof( score ) );
    for( int i = 0; score[ i ] != '\0'; i++ )
    {
        Draw_Digit( score[ i ] - 48,x + digit_width * i,y );
    }
}

void Game::Draw_Enemy( int x,int y,
    unsigned char red,
    unsigned char green,
    unsigned char blue ){
    for( int r = 0; r < enemy_width; r++ )
    {
        for( int c = 0; c < enemy_height; c++ )
        {
            gfx.PutPixel( x + r, y + c,red,green,blue );
        }
    }
}

void Game::Deploy_Enemy(){
    if ( enemy_wait_count >= enemy_wait_time && enemy_count < MAX_ENEMIES )
    {
        // void Write_Index(int* array, int buff_length) {
        for (int index_enemy = 0; index_enemy <= MAX_ENEMIES; index_enemy++)
        {
            if (enemy[ index_enemy ].x == '\0')
            {
                enemy[ index_enemy ].x = rand() % (799 - enemy_width * 2);
                enemy[ index_enemy ].y = 0;
                // array[i] = new_value;
                if( game_level == 1 )
                {
                    for( int index_color = 0; index_color < 3; index_color++ )
                    {
                        enemy[ index_enemy ].color[ index_color ] = enemy_level1_color[ index_color ];
                        enemy[ index_enemy ].hp = enemy_level1_hp;
                    }
                }
                else if( game_level == 2 )
                {
                    for( int index_color = 0; index_color < 3; index_color++ )
                    {
                        enemy[ index_enemy ].color[ index_color ] = enemy_level2_color[ index_color ];
                        enemy[ index_enemy ].hp = enemy_level2_hp;
                    }
                }
                else if( game_level == 3 )
                {
                    for( int index_color = 0; index_color < 3; index_color++ )
                    {
                        enemy[ index_enemy ].color[ index_color ] = enemy_level3_color[ index_color ];
                        enemy[ index_enemy ].hp = enemy_level3_hp;
                    }
                }
                else if( game_level == 4 )
                {
                    for( int index_color = 0; index_color < 3; index_color++ )
                    {
                        enemy[ index_enemy ].color[ index_color ] = enemy_level3_color[ index_color ];
                        enemy[ index_enemy ].hp = enemy_level4_hp;
                    }
                }
                else if( game_level == 5)
                {
                    for( int index_color = 0; index_color < 3; index_color++ )
                    {
                        enemy[ index_enemy ].color[ index_color ] = enemy_level3_color[ index_color ];
                        enemy[ index_enemy ].hp = enemy_level5_hp;
                    }
                }
                else if( game_level == 6 )
                {
                    for( int index_color = 0; index_color < 3; index_color++ )
                    {
                        enemy[ index_enemy ].color[ index_color ] = enemy_level3_color[ index_color ];
                        enemy[ index_enemy ].hp = enemy_level6_hp;
                    }
                }
                break;
            }
        }
        enemy_wait_count = 0;
        enemy_count++;
    }
    enemy_wait_count++;
}

void Game::Null_Mem(int index,GAME_ITEM item){
    if( item == ENEMY )
    {
        enemy[ index ].x = NULL;
        enemy[ index ].y = NULL;
        enemy[ index ].hp= NULL;
        for (int i = 0; i < 3; i++)
        {
            enemy[index].color[ i ] = NULL;
        }
        enemy_count--;
    }
    else if( item == LASER )
    {
        laser_x[ index ] = NULL;
        laser_y[ index ] = NULL;
        laser_count--;
    }
}

// void Game::Shift_Memory( int current_index,int item_count,GAME_ITEM item ){
//     if( item == LASER )
//     {
//         for( int index_laser = current_index; index_laser < item_count - 1; index_laser++ )
//         {
//             laser_x[ index_laser ] = laser_x[ index_laser + 1 ];
//             laser_y[ index_laser ] = laser_y[ index_laser + 1 ];
//         }
//         laser_count--;
//     }
//     else if( item == ENEMY )
//     {
//         for( int index_enemy = current_index; index_enemy < item_count - 1; index_enemy++ )
//         {
//             enemy[ index_enemy ].x = enemy[ index_enemy + 1 ].x;
//             enemy[ index_enemy ].y = enemy[ index_enemy + 1 ].y;
//         }
//         enemy_count--;
//     }
// }

void Game::Update_Keyboard_Input( float delta_time){
    int frameStep = delta_time * ship_speed;

    // Shoot lasers
    if ( kbd.UpIsPressed() )
    {
        if ( !up_is_pressed && laser_count < MAX_LASERS)
        {
            up_is_pressed = true;
            if( game_level == 1 )
            {
                laser_x[ laser_count ] = (float)ship_x + 15.0f;
                laser_y[ laser_count ] = 564; 
                laser_count += 1;
            }
            else if( game_level == 2 )
            {
                laser_x[ laser_count ]    = (float)ship_x + 10.0f;
                laser_x[ laser_count + 1] = (float)ship_x + 20.0f;
                laser_y[ laser_count ]    = 564; 
                laser_y[ laser_count + 1] = 564; 
                laser_count += 2;
            }
            else if( game_level == 3 )
            {
                laser_x[ laser_count ]    = (float)ship_x + 5.0f;
                laser_x[ laser_count + 1] = (float)ship_x + 15.0f;
                laser_x[ laser_count + 2] = (float)ship_x + 25.0f;
                laser_y[ laser_count ]    = 564; 
                laser_y[ laser_count + 1] = 564; 
                laser_y[ laser_count + 2] = 564; 
                laser_count += 3;
            }
            else if( game_level == 4 )
            {
                laser_x[ laser_count ]    = (float)ship_x + 0.0f;
                laser_x[ laser_count + 1] = (float)ship_x + 10.0f;
                laser_x[ laser_count + 2] = (float)ship_x + 20.0f;
                laser_x[ laser_count + 3] = (float)ship_x + 30.0f;
                laser_y[ laser_count ]    = 564; 
                laser_y[ laser_count + 1] = 564; 
                laser_y[ laser_count + 2] = 564; 
                laser_y[ laser_count + 3] = 564; 
                laser_count += 4;
            }
        }
    }
    else
    {
        up_is_pressed = false;
    }
    // Ship movement
    if (kbd.LeftIsPressed())
    {
        if( ship_x - frameStep <= 0)
        {
            ship_x = 0;
        }
        else
        {
        	ship_x += -frameStep;
        }
    }
    else if (kbd.RightIsPressed())
    {
        if( ship_x + frameStep > 799 - 30)
        {
            ship_x = 799 - 30;
        }
        else
        {
        	ship_x += frameStep;
        }
    }

    // Restart game
    if ( kbd.EnterIsPressed() )
    {
        if( !enter_is_pressed )
        {
            if( game_is_over )
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

void Game::Update_Progression(){
    // Check for Level 1
    if( game_score >= 0 && game_score < game_level2 )
    {
        game_level = 1;
        for( int index_color = 0; index_color < 3; index_color++ )
        {
            enemy_color[ index_color ] = enemy_level1_color[ index_color ];
        }
        enemy_hp = enemy_level1_hp;
        laser_speed = laser_level1_speed;
        laser_multiple = 1;
    }
    else if( game_score >= game_level2 && game_score < game_level3 )
    {
        game_level = 2;
        for( int index_color = 0; index_color < 3; index_color++ )
        {
            enemy_color[ index_color ] = enemy_level2_color[ index_color ];
        }
        enemy_hp = enemy_level2_hp;
        laser_speed = laser_level2_speed;
        laser_multiple = 2;
    }
    else if( game_score >= game_level3 && game_score < game_level4 )
    {
        game_level = 3;
        for( int index_color = 0; index_color < 3; index_color++ )
        {
            enemy_color[ index_color ] = enemy_level3_color[ index_color ];
        }
        enemy_hp = enemy_level3_hp;
        laser_speed = laser_level3_speed;
        laser_multiple = 3;
    }
    else if( game_score >= game_level4 )
    {
        game_level = 4;
        for( int index_color = 0; index_color < 3; index_color++ )
        {
            enemy_color[ index_color ] = enemy_level4_color[ index_color ];
        }
        enemy_hp = enemy_level4_hp;
        laser_speed = laser_level3_speed;
        laser_multiple = 4;
    }
}

void Game::Update_Laser( float delta_time ){
	int frameStep = delta_time * laser_speed;
    for( int index_laser = 0; index_laser < laser_count; index_laser++ )
    {
        // Check if laser hits enemy
        for( int index_enemy = 0; index_enemy < MAX_ENEMIES; index_enemy++ )
        {
            if( (int)laser_x[ index_laser ] + laser_width > enemy[ index_enemy ].x &&
                (int)laser_x[ index_laser ] < enemy[ index_enemy ].x + enemy_width &&
                laser_y[ index_laser ] + laser_height > enemy[ index_enemy ].y &&
                laser_y[ index_laser ] < enemy[ index_enemy].y + enemy_height)
            {
                enemy[ index_enemy ].hp--;
                // Check if enemy hp is 0
                if( enemy[ index_enemy ].hp < 1 )
                {
                    // Remove enemy and laser
                    // Shift memory slots down
                    // Shift_Memory( index_enemy,enemy_count,ENEMY );
                    Null_Mem( index_enemy,ENEMY );
                    enemy[ index_enemy ];
                    game_score++;
                }
                // Shift_Memory( index_laser,laser_count,LASER );
                Null_Mem( index_laser,LASER );
            }
        }
        // Check if laser hits top of screen
        if     (laser_y[ index_laser ] - frameStep <= 0)
        {
            Null_Mem( index_laser,LASER );
            // Shift_Memory( index_laser,laser_count,LASER);
            continue;
        }
        // Check if laser hits screen left
        else if(laser_x[ index_laser ] - frameStep <= 0)
        {
            Null_Mem( index_laser,LASER );
            // Shift_Memory( index_laser,laser_count,LASER);
            continue;
        }
        // Check if laser hits screen right
        else if(laser_x[ index_laser ] + frameStep + 3 > SCREENWIDTH - 1)
        {
            Null_Mem( index_laser,LASER );
            // Shift_Memory( index_laser,laser_count,LASER);
            continue;
        }
        laser_y[ index_laser ] -= frameStep;
    }
}

void Game::Update_Enemy( float delta_time ){
    // Update colors
    // Set to < enemy_count - 1 to see green bar before it starts
    int frameStep = delta_time * enemy_speed;
    for( int index_enemy = 0; index_enemy < enemy_count; index_enemy++ )
    {
        // Check if enemy hits player ship
        if( ship_x + ship_width > enemy[ index_enemy ].x &&
            ship_x < enemy[ index_enemy ].x + enemy_width &&
            ship_y + ship_height > enemy[ index_enemy ].y &&
            ship_y < enemy[ index_enemy ].y + enemy_height)
        {
            game_is_over = true;
        }
        // Check if hits bottom of screen
        if ( enemy[ index_enemy ].y + frameStep >= 599 - 10)
        {
            Null_Mem( index_enemy,ENEMY );
            // Shift_Memory( 0,enemy_count,ENEMY );
            continue;
        }
        // Update movement
        enemy[ index_enemy ].y += frameStep;
    }
}

void Game::Restart_Game(){
    game_is_over = false;
    game_score = 0;
    ship_x = 385;
    enemy_count = 0;
    enemy_wait_count = 0;
    laser_count = 0;
}

void Game::ComposeFrame(){
    if( !game_is_over )
    {
        // float temp_x
        Draw_Ship( ship_x,ship_y);
        for( int i = 0; i < laser_count; i++ )
        {
            if( laser_multiple == 4)
            {
                // 4 lasers, 0 diag left, 1 & 2 straight 3 diag right
                //   \ | | /
                if( i % 4 == 0 )
                {
                    laser_x[ i ] -= 3.5f;
                    Draw_Laser_Diagonal( laser_x[ i ],laser_y[ i ],"Left" );
                }
                else if( i % 4 == 3 )
                {
                    laser_x[ i ] += 3.5f;
                    Draw_Laser_Diagonal( laser_x[ i ],laser_y[ i ],"Right" );
                }
                else
                {
                    Draw_Laser( (int)laser_x[ i ],laser_y[ i ] );
                }
            }
            else
            {
                Draw_Laser( (int)laser_x[ i ],laser_y[ i ] );
            }
        }
        for( int index_enemy = 0; index_enemy < enemy_count; index_enemy++ )
        {
            Draw_Enemy( enemy[ index_enemy ].x,enemy[ index_enemy ].y,
                enemy[ index_enemy ].color[ E_RED ],
                enemy[ index_enemy ].color[ E_GREEN ],
                enemy[ index_enemy ].color[ E_BLUE ]);
        }
    }
    // if( game_score )
    Draw_Score( game_score_x,game_score_y );
}
