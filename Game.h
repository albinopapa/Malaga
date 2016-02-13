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

#define MAX_LASERS 100
#define MAX_ENEMIES 1
#define SCREENWIDTH 800
#define SCREENHEIGHT 600

class Game
{
public:
    Game( HWND hWnd,const KeyboardServer& kServer,const MouseServer& mServer );
    ~Game();
    void Go();
private:
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
    struct Enemy
    {
        int           x;
        int           y;
        int           hp;
        // unsigned int  speed;
        unsigned char color[ 3 ];
    };
private:
    void ComposeFrame();
    /********************************/
    /*  User Functions              */

    void Draw_Score( int x,int y );
    void Draw_Digit( int digit, int x,int y );
    void Draw_Ship( int x,int y );
    void Draw_Laser( int x,int y );
    void Draw_Laser_Diagonal( float x,int y,char* direction );
    void Draw_Enemy(int x, int y,
        unsigned char red,
        unsigned char green,
        unsigned char blue);
    void Deploy_Enemy();

    void Update_Laser( float delta_time );
    void Update_Enemy( float delta_time );
    void Update_Keyboard_Input( float delta_time );
    void Update_Progression();

    // void Shift_Memory( int current_index,int item_count,GAME_ITEM item );
    void Null_Mem(int index, GAME_ITEM item);
    void Restart_Game();

    void Int_To_String( unsigned int num, char* chars, int buff_length );
    void Reverse_String( char* pChar );
    // void Set_PxPy();

    /********************************/
private:
    D3DGraphics gfx;
    KeyboardClient kbd;
    MouseClient mouse;
    DSound audio;
    /********************************/
    /*  User Variables              */
        
    Timer timer;

    // int px, py, radius;
    // const unsigned int speed = 250;
    int             game_score;
    const int       game_score_x = 600;
    const int       game_score_y = 566;
    const int       digit_width = 20;
    //const int       score_number_widths[ 9 ] = {15,7,15,15,16,15,16,16,16,16}
    // 0_width = 15;
    // 0_height = 24;
    // 1_width = 7;
    // 1_height = 24;
    // 2_width = 15;
    // 2_height = 24;
    // 3_width = 15;
    // 3_height = 24;
    // 4_width = 16;
    // 4_height = 24;
    // 5_width = 15;
    // 5_height = 24;
    // 6_width = 16;
    // 6_height = 24;
    // 7_width = 16;
    // 7_height = 24;
    // 8_width = 16;
    // 8_height = 24;
    // 9_width = 16;
    // 9_height = 24;

    bool      game_is_over;
    
    int       ship_x;
    const int ship_y = 569;
    int       ship_width;
    int       ship_height;
    unsigned int       ship_speed;

    float        laser_x[ MAX_LASERS ];
    int          laser_y[ MAX_LASERS ];
    int          laser_level;
    const int    laser_level1_speed = 250;
    const int    laser_level2_speed = 375;
    const int    laser_level3_speed = 600;
    char         laser_multiple;
    unsigned int laser_speed;
    int          laser_width;
    int          laser_height;
    int          laser_count;

    Enemy         enemy[ MAX_ENEMIES ];
    unsigned char enemy_level1_color[ 3 ] = { 158,156,114 };
    unsigned char enemy_level2_color[ 3 ] = { 0,255,255 };
    unsigned char enemy_level3_color[ 3 ] = { 86,188,84 };
    unsigned char enemy_level4_color[ 3 ] = { 79,87,193 };
    unsigned char enemy_color[ 3 ];
    const char    enemy_level1_hp = 3;
    const char    enemy_level2_hp = 10;
    const char    enemy_level3_hp = 20;
    const char    enemy_level4_hp = 50;
    const char    enemy_level5_hp = 100;
    const char    enemy_level6_hp = 200;
    int           enemy_hp;
    int           enemy_level;
    int           enemy_count;
    int           enemy_wait_count;
    int           enemy_wait_time;
    int           enemy_width;
    int           enemy_height;
    int           enemy_speed;

    char      game_level;
    const int game_level1 = 0;
    const int game_level2 = 10;
    const int game_level3 = 20;
    const int game_level4 = 30;
    const int game_level5 = 40;
    const int game_level6 = 50;

    bool      up_is_pressed;
    bool      enter_is_pressed;
    /********************************/
};