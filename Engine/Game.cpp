/******************************************************************************************
*    Chili DirectX Framework Version 16.07.20                                              *
*    Game.cpp                                                                              *
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
#include "MainWindow.h"
#include "Game.h"

Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd )
{
	start = std::chrono::steady_clock::now();

	for( int i = 0; i < max_bullets; ++i )
	{
		bullet_x[ i ] = 0.f;
		bullet_y[ i ] = 0.f;
		bullet_vx[ i ] = 0.f;
		bullet_vy[ i ] = 0.f;
	}
}

void Game::Go()
{
	end = std::chrono::steady_clock::now();

	frame_time = std::chrono::duration<float>( end - start ).count();
	UpdateModel();
	gfx.BeginFrame();
	ComposeFrame();
	gfx.EndFrame();

	start = std::chrono::steady_clock::now();

}

void Game::UpdateModel()
{
	// A change to commit
	// Update fire rate tracker or bullet spawn timer
	fire_rate_tracker += frame_time;

	// Helpful vars for checking if ship is on/near edges
	const float screen_width = ( float )Graphics::ScreenWidth;
	const float screen_height = ( float )Graphics::ScreenHeight;
	const float right_bounds = screen_width - 1.f - ship_size;
	const float lower_bounds = screen_height - 1.f - ship_size;

	// Clamp the ship to the edges
	ship_x = std::max( 0.f, std::min( right_bounds, ship_x ) );
	ship_y = std::max( 0.f, std::min( lower_bounds, ship_y ) );

	for( unsigned int i = 0; i < bullet_count; ++i )
	{
		// Update bullet positions
		bullet_x[ i ] += ( bullet_vx[ i ] * bullet_speed );
		bullet_y[ i ] += ( bullet_vy[ i ] * bullet_speed );

		// Check if bullet off screen
		if( bullet_x[ i ] < 0 || bullet_x[ i ] >= screen_width ||
			bullet_y[ i ] < 0 || bullet_y[ i ] >= screen_height )
		{
			for( unsigned int j = i + 1; j < bullet_count; ++j )
			{
				const int k = j - 1;
				std::swap( bullet_x[ j ], bullet_x[ k ] );
				std::swap( bullet_y[ j ], bullet_y[ k ] );
				std::swap( bullet_vx[ j ], bullet_vx[ k ] );
				std::swap( bullet_vy[ j ], bullet_vy[ k ] );
			}
			--bullet_count;
		}
	}

	// Move clockwise
	if( wnd.kbd.KeyIsPressed( VK_LEFT ) || wnd.kbd.KeyIsPressed( 'A' ) )
	{
		if( ship_y <= 0.f && ship_x < right_bounds )
		{
			ship_x += ship_speed;
		}
		else if( ship_y >= lower_bounds && ship_x > 0.f )
		{
			ship_x -= ship_speed;
		}
		else
		{
			if( ship_x <= 0.f && ship_y > 0.f )
			{
				ship_y -= ship_speed;
			}
			else if( ship_x >= right_bounds && ship_y < lower_bounds )
			{
				ship_y += ship_speed;
			}
		}
	}

	// Move counter clockwise
	if( wnd.kbd.KeyIsPressed( VK_RIGHT ) || wnd.kbd.KeyIsPressed( 'D' ) )
	{
		if( ship_y <= 0.f && ship_x > 0.f )
		{
			ship_x -= ship_speed;
		}
		else if( ship_y >= lower_bounds && ship_x < right_bounds )
		{
			ship_x += ship_speed;
		}
		else
		{
			if( ship_x <= 0.f && ship_y < lower_bounds )
			{
				ship_y += ship_speed;
			}
			else if( ship_x >= right_bounds && ship_y > 0.f )
			{
				ship_y -= ship_speed;
			}
		}
	}

	// Fire bullet
	if( wnd.kbd.KeyIsPressed( VK_SPACE ) )
	{
		if( fire_rate_tracker >= fire_rate )
		{
			if( bullet_count < max_bullets )
			{
				// Reset fire rate tracker
				fire_rate_tracker = 0.f;

				// Helpful var to determine ships center for bullet spawning
				const float ship_half_size = ship_size * .5f;

				// Set bullet to ship center
				const float ship_center_x = ship_x + ship_half_size;
				const float ship_center_y = ship_y + ship_half_size;
				bullet_x[ bullet_count ] = ship_center_x;
				bullet_y[ bullet_count ] = ship_center_y;

				// Determine travel direction of bullet
				const float dx = ( screen_width * .5f ) - ship_center_x;
				const float dy = ( screen_height * .5f ) - ship_center_y;
				const float rcp_dist = 1.f / sqrt( dx*dx + dy*dy );
				bullet_vx[ bullet_count ] = dx * rcp_dist;
				bullet_vy[ bullet_count ] = dy * rcp_dist;

				// Increase bullet count
				++bullet_count;
			}
		}
	}
}

void Game::ComposeFrame()
{
	{ // Draw ship
		const int x = ( int )ship_x;
		const int y = ( int )ship_y;
		const int w = ( int )ship_size;
		gfx.DrawRect( x, y, w, w, Colors::White );
	}

	{ // Draw bullets
		for( int i = 0; i < bullet_count; ++i )
		{
			const int x = ( int )bullet_x[ i ];
			const int y = ( int )bullet_y[ i ];
			const int w = ( int )bullet_length;
			gfx.DrawRect( x, y, w, w, Colors::Cyan );
		}
	}
}
