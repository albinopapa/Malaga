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
#include "Draw_Digit.cpp"

Game::Game( HWND hWnd,const KeyboardServer& kServer,const MouseServer& mServer )
:	gfx( hWnd ),
	audio( hWnd ),
	kbd( kServer ),
	mouse( mServer ),
    enter_is_pressed( false ),
    up_is_pressed( false ),
    space_is_pressed( false )
{
    srand( (unsigned int)time(NULL) );
    game.is_over            = false;
    game.score              = 0;
    ship.x                  = 385;
    global_enemy.count      = 0;
    global_laser.count      = 0;
    // Nullify enemy array
    for( int index_enemy = 0; index_enemy < MAX_ENEMIES; index_enemy++ )
    {
        Null_Mem( index_enemy,ENEMY );
    }
    // Nullify laser array
    for( int index_laser = 0; index_laser < MAX_LASERS; index_laser++ )
    {
        Null_Mem( index_laser,LASER );
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
    if( kbd.SpaceIsPressed() )
    {
        if( !space_is_pressed )
        {
            space_is_pressed = true;
            game.is_paused = !game.is_paused;
        }
    }
    else
    {
        space_is_pressed = false;
        if( !game.is_paused )
        {
            Update_Progression();
            Update_Keyboard_Input( dt );
            Deploy_Enemy( dt );
            Update_Laser( dt );
            Update_Enemy( dt );
        }
    }
	gfx.BeginFrame();
	ComposeFrame();
	gfx.EndFrame();
}

void Game::Draw_Ship( int x,int y ){
    for( int r = 0; r < ship.width; r++ )
    {
        for( int c = 0; c < ship.height; c++ )
        {
            gfx.PutPixel( x + c, y + r,255,255,255 );
        }
    }
}

void Game::Draw_Laser( int x,int y,LASER_DIRECTION direction ){
    float old_x = x;
    if( direction == MIDDLE )
    {
        for( int r = 0; r < 3; r++ )
        {
            for( int c = 0; c < 10; c++ )
            {
                gfx.PutPixel( x + r, y + c,255,50,50 );
            }
        }
    }
    else if( direction == LEFT)
    {
        for( int r = 0; r < 3; r++ )
        {
            for( int c = 0; c < 10; c++ )
            {
                gfx.PutPixel( x + r, y + c,255,50,50 );
                // Draws top to bottom so use +=
                x += 1.0f;
            }
            x = old_x;
        }
    }
    else if( direction == RIGHT )
    {
        for( int r = 0; r < 3; r++ )
        {
            for( int c = 0; c < 10; c++ )
            {
                gfx.PutPixel( x + r, y + c,255,50,50 );
                // Draws top to bottom so use -=
                x -= 1.0f;
            }
            x = old_x - 3;
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
    if( num == 0 )
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

void Game::Draw_Score( int x,int y ){
    char score[ 11 ] = { NULL };
    Int_To_String( game.score,score,sizeof( score ) );
    for( int i = 0; score[ i ] != '\0'; i++ )
    {
        Draw_Digit( score[ i ] - 48,x + game.digit_width * i,y );
    }
}

void Game::Draw_Enemy( int x,int y,
        unsigned char red,
        unsigned char green,
        unsigned char blue ){
    for( int r = 0; r < global_enemy.width; r++ )
    {
        for( int c = 0; c < global_enemy.height; c++ )
        {
            gfx.PutPixel( x + r, y + c,red,green,blue );
        }
    }
}

void Game::Set_New_Enemy( unsigned char* color,int hp ){
    for( int index_enemy = 0; index_enemy < MAX_ENEMIES; index_enemy++ )
    {
        if( enemy[ index_enemy ].index == EMPTY  )
        {
            for( int enemy_count = 0; enemy_count < global_enemy.multiple; enemy_count++)
            {
                // shift a memory slot each loop while
                // using index_laser as the base starting point
                // since it was delcared outside of this loop
                enemy[ index_enemy + enemy_count].x     = rand() % ( 799 - global_enemy.width * 2 );
                enemy[ index_enemy + enemy_count].y     = 0;
                enemy[ index_enemy + enemy_count].hp    = hp;
                enemy[ index_enemy + enemy_count].index = USED;
                for( int index_color = 0; index_color < 3; index_color++ )
                {
                    enemy[ index_enemy ].color[ index_color ] = color[ index_color ];
                }
                global_enemy.count++;
            }
            break;
        }
    }
}

void Game::Set_New_Lasers( LASER_DIRECTION* direction,const float* offset ){
    for( int index_laser = 0; index_laser < MAX_LASERS; index_laser++ )
    {
        if(laser[ index_laser ].index == EMPTY &&
           index_laser + global_laser.multiple <= MAX_LASERS )
        {
            for( int laser_count = 0; laser_count < global_laser.multiple; laser_count++)
            {
                // shift a memory slot each loop while
                // using index_laser as the base starting point
                // since it was delcared outside of this loop
                laser[ index_laser + laser_count ].x         = (float)ship.x + offset[ laser_count ];
                laser[ index_laser + laser_count ].direction = direction[ laser_count ];
                laser[ index_laser + laser_count ].y         = 564;
                laser[ index_laser + laser_count ].index     = USED;
                global_laser.count++;
            }
            break;
        }
    }
}

void Game::Deploy_Enemy( float delta_time ){
    if( global_enemy.wait_count >= global_enemy.wait_time && global_enemy.count < MAX_ENEMIES )
    {
        switch( game.level )
        {
            case 1:
                Set_New_Enemy( global_enemy.level1_color,global_enemy.level1_hp );
                break;
            case 2:
                Set_New_Enemy( global_enemy.level2_color,global_enemy.level2_hp );
                break;
            case 3:
                Set_New_Enemy( global_enemy.level3_color,global_enemy.level3_hp );
                break;
            case 4:
                Set_New_Enemy( global_enemy.level4_color,global_enemy.level4_hp );
                break;
            default:
            break;
        }
        global_enemy.wait_count = 0.0f;
    }
    global_enemy.wait_count += delta_time;
}

void Game::Null_Mem( int index,GAME_ITEM item ){
    if( item == ENEMY )
    {
        enemy[ index ].x     = NULL;
        enemy[ index ].y     = NULL;
        enemy[ index ].hp    = NULL;
        enemy[ index ].index = EMPTY;
        for( int i = 0; i < 3; i++ )
        {
            enemy[index].color[ i ] = NULL;
        }
    }
    else if( item == LASER )
    {
        laser[ index ].x         = NULL;
        laser[ index ].y         = NULL;
        laser[ index ].direction = MIDDLE;
        laser[ index ].index     = EMPTY;
    }
}

void Game::Update_Keyboard_Input( float delta_time){
    int frameStep = delta_time * ship.speed;
    // Shoot lasers
    if( kbd.UpIsPressed() )
    {
        if( !up_is_pressed && global_laser.count < MAX_LASERS)
        {
            if( global_laser.count > 4 )
                global_laser.count = global_laser.count;
            up_is_pressed = true;
            switch( game.level )
            {
                case 1:
                    Set_New_Lasers( global_laser.level1_direction,
                        global_laser.level1_x_offset );
                    break;
                case 2:
                    Set_New_Lasers( global_laser.level2_direction,
                        global_laser.level2_x_offset );
                    break;
                case 3:
                    Set_New_Lasers( global_laser.level3_direction,
                        global_laser.level3_x_offset );
                    break;
                case 4:
                    Set_New_Lasers( global_laser.level4_direction,
                        global_laser.level4_x_offset );
                    break;
                default:
                break;
            }
        }
    }
    else
    {
        up_is_pressed = false;
    }
    // Ship movement
    if( kbd.LeftIsPressed() )
    {
        if( ship.x - frameStep <= 0)
        {
            ship.x = 0;
        }
        else
        {
        	ship.x += -frameStep;
        }
    }
    else if( kbd.RightIsPressed() )
    {
        if( ship.x + frameStep > 799 - 30)
        {
            ship.x = 799 - 30;
        }
        else
        {
        	ship.x += frameStep;
        }
    }
    // Restart game
    if( kbd.EnterIsPressed() )
    {
        if( !enter_is_pressed )
        {
            if( game.is_over )
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

void Game::Shift_Memory( int current_index,int item_count,GAME_ITEM item ){
    if( item == LASER )
    {
        // We don't need to loop if the index is the same as MAX
        if( current_index == MAX_LASERS - 1 )
        {
            Null_Mem( current_index,LASER);
        }
        else
        {
            // Declare index outside loop to retain value
            // This loop takes the memory states of the adjacent state
            // and copies it to the index. It will do this based on the
            // index that is given to the function. Therefore the array
            // will always be tidy since all values will be stored in the
            // lowest available index of the memory array.
            int index_laser;
            for( index_laser = current_index; 
                     laser[ index_laser ].index != EMPTY &&
                     index_laser + 1 < MAX_LASERS; index_laser++ )
            {
                laser[ index_laser ].direction = laser[ index_laser + 1 ].direction;
                laser[ index_laser ].x         = laser[ index_laser + 1 ].x;
                laser[ index_laser ].y         = laser[ index_laser + 1 ].y;
                laser[ index_laser ].index     = laser[ index_laser + 1 ].index;
            }
            // This Null_Mem will clear the last memory slot that gets
            // skipped after the last loop.
            Null_Mem( index_laser,LASER);
        }
        global_laser.count--;
    }
    else if( item == ENEMY )
    {
        if( current_index == MAX_ENEMIES - 1 )
        {
            Null_Mem( current_index,ENEMY);
        }
        else
        {
            int index_enemy;
            for( index_enemy = current_index; 
                     enemy[ index_enemy ].index != EMPTY &&
                     index_enemy + 1 < MAX_ENEMIES; index_enemy++ )
            {
                enemy[ index_enemy ].x          = enemy[ index_enemy + 1 ].x;
                enemy[ index_enemy ].y          = enemy[ index_enemy + 1 ].y;
                enemy[ index_enemy ].hp         = enemy[ index_enemy + 1 ].hp ;
                enemy[ index_enemy ].index      = enemy[ index_enemy + 1 ].index;
                for( int index_color = 0; index_color < 3; index_color++ )
                {
                    enemy[ index_enemy ].color[ index_color ] = enemy[ index_enemy + 1].color[ index_color ];
                }
            }
            Null_Mem( index_enemy,ENEMY);
        }
        global_enemy.count--;
    }
}

void Game::Update_Progression(){
    // Check for Level 1
    if( game.score >= 0 && game.score < game.level2 )
    {
        game.level = 1;
        for( int index_color = 0; index_color < 3; index_color++ )
        {
            global_enemy.color[ index_color ] = global_enemy.level1_color[ index_color ];
        }
        global_enemy.hp = global_enemy.level1_hp;
        global_laser.speed = global_laser.level1_speed;
        global_laser.multiple = 1;
    }
    else if( game.score >= game.level2 && game.score < game.level3 )
    {
        game.level = 2;
        for( int index_color = 0; index_color < 3; index_color++ )
        {
            global_enemy.color[ index_color ] = global_enemy.level2_color[ index_color ];
        }
        global_enemy.hp = global_enemy.level2_hp;
        global_laser.speed = global_laser.level2_speed;
        global_laser.multiple = 2;
    }
    else if( game.score >= game.level3 && game.score < game.level4 )
    {
        game.level = 3;
        for( int index_color = 0; index_color < 3; index_color++ )
        {
            global_enemy.color[ index_color ] = global_enemy.level3_color[ index_color ];
        }
        global_enemy.hp = global_enemy.level3_hp;
        global_laser.speed = global_laser.level3_speed;
        global_laser.multiple = 3;
    }
    else if( game.score >= game.level4 )
    {
        game.level = 4;
        for( int index_color = 0; index_color < 3; index_color++ )
        {
            global_enemy.color[ index_color ] = global_enemy.level4_color[ index_color ];
        }
        global_enemy.hp = global_enemy.level4_hp;
        global_laser.speed = global_laser.level3_speed;
        global_laser.multiple = 4;
    }
}

void Game::Update_Laser( float delta_time ){
    int frameStep = delta_time * global_laser.speed;
    // Check if lasers hits enemy
    for( int index_laser = 0; index_laser < global_laser.count; index_laser++ )
    {
        for( int index_enemy = 0; index_enemy < global_enemy.count; index_enemy++ )
        {
            if( (int)laser[ index_laser ].x + global_laser.width > enemy[ index_enemy ].x &&
                (int)laser[ index_laser ].x < enemy[ index_enemy ].x + global_enemy.width &&
                laser[ index_laser ].y + global_laser.height > enemy[ index_enemy ].y &&
                laser[ index_laser ].y < enemy[ index_enemy].y + global_enemy.height)
            {
                enemy[ index_enemy ].hp--;
                // Check if enemy hp is 0
                if( enemy[ index_enemy ].hp < 1 )
                {
                    // Remove enemy and laser
                    // Shift memory slots down
                    // Shift_Memory( index_enemy,global_enemy.count,ENEMY );
                    Shift_Memory( index_enemy,global_enemy.count,ENEMY );
                    index_enemy--;
                    game.score++;
                }
                // Remove laser from memory
                Shift_Memory( index_laser,1,LASER );
                index_laser--;
            }
        }
    }
    // Check if lasers hits screen edges
    for( int index_laser = 0; index_laser < global_laser.count; index_laser++ )
    {
        // Check if laser hits top of screen
        if( laser[ index_laser ].y - frameStep <= 0 )
        {
            Shift_Memory( index_laser,1,LASER );
            // =====================================
            // =====================================
            //   THIS IS HOLY GOD IMPORTANT
            //   step the index back if you removed
            //   something from memory
            //   so that you don't skip updating it
            //   in the next loop. Think about it.
            index_laser--;
            continue;
        }
        // Check if laser hits screen left
        else if(laser[ index_laser ].x - frameStep <= 0)
        {
            Shift_Memory( index_laser,1,LASER );
            index_laser--;
            continue;
        }
        // Check if laser hits screen right
        else if( laser[ index_laser ].x + frameStep + 3 > SCREENWIDTH - 1 )
        {
            Shift_Memory( index_laser,1,LASER );
            index_laser--;
            continue;
        }
    }
    // Move remaining lasers that haven't hit anything... (facepalm myself)
    for( int index_laser = 0; index_laser < global_laser.count; index_laser++ )
    {
        if( laser[ index_laser ].direction == LEFT )
        {
            laser[ index_laser ].x -= frameStep;
        }
        else if( laser[ index_laser ].direction == RIGHT )
        {
            laser[ index_laser ].x += frameStep;
        }
        laser[ index_laser ].y -= frameStep;
    }
}

void Game::Update_Enemy( float delta_time ){
    // Update colors
    // Set to < global_enemy.count - 1 to see green bar before it starts
    int frameStep = delta_time * global_enemy.speed;
    for( int index_enemy = 0; index_enemy < global_enemy.count; index_enemy++ )
    {
        // Check if enemy hits player ship
        if( ship.x + ship.width > enemy[ index_enemy ].x &&
            ship.x < enemy[ index_enemy ].x + global_enemy.width &&
            ship.y + ship.height > enemy[ index_enemy ].y &&
            ship.y < enemy[ index_enemy ].y + global_enemy.height)
        {
            game.is_over = true;
        }
        // Check if hits bottom of screen
        if( enemy[ index_enemy ].y + frameStep >= 599 - 10)
        {
            Shift_Memory( index_enemy,1,ENEMY );
            index_enemy--;
            continue;
        }
    }
    // Update movement
    for( int index_enemy = 0; index_enemy < global_enemy.count; index_enemy++ )
    {
        enemy[ index_enemy ].y += frameStep;
    }
}

void Game::Restart_Game(){
    for( int index_enemy = 0; index_enemy < MAX_ENEMIES; index_enemy++ )
    {
        Null_Mem( index_enemy,ENEMY );
    }
    // Nullify laser array
    for( int index_laser = 0; index_laser < MAX_LASERS; index_laser++ )
    {
        Null_Mem( index_laser,LASER );
    }
    game.is_over = false;
    game.score = 0;
    ship.x = 385;
    global_enemy.count = 0;
    global_enemy.wait_count = 0;
    global_laser.count = 0;
}

void Game::ComposeFrame(){
    if( !game.is_over )
    {
        // Draw ship stuff
        Draw_Ship( ship.x,ship.y);
        // Draw laser stuff
        for( int index_laser = 0; index_laser < global_laser.count; index_laser++ )
        {
            Draw_Laser((int)laser[ index_laser ].x,
                            laser[ index_laser ].y,
                            laser[ index_laser ].direction);
        }
        // Draw enemy stuff
        for( int index_enemy = 0; index_enemy < global_enemy.count; index_enemy++ )
        {
            Draw_Enemy( enemy[ index_enemy ].x,enemy[ index_enemy ].y,
                enemy[ index_enemy ].color[ E_RED ],
                enemy[ index_enemy ].color[ E_GREEN ],
                enemy[ index_enemy ].color[ E_BLUE ]);
        }
    }
    Draw_Score( game.score_x,game.score_y );
}
