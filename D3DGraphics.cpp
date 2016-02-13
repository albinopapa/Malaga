/****************************************************************************************** 
 *	Chili DirectX Framework Version 12.04.24											  *	
 *	D3DGraphics.cpp																		  *
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
#include "D3DGraphics.h"
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "Bitmap.h"
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

void LoadSprite( Sprite* sprite,const char* filename,
	unsigned int width,unsigned int height,D3DCOLOR key )
{
	sprite->surface = (D3DCOLOR*)malloc( sizeof( D3DCOLOR ) * width * height );
	LoadBmp( filename,sprite->surface );
	sprite->height = height;
	sprite->width = width;
	sprite->key = key;
}

void LoadSpriteAlpha(Sprite* sprite)
{
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

	Gdiplus::Bitmap bitmap(L"alphahalf.png");
	Gdiplus::Color pixel;

	sprite->height = bitmap.GetHeight();
	sprite->width = bitmap.GetWidth();
	sprite->key = 0x010301F3;
	sprite->surface = (D3DCOLOR*)malloc(sizeof(D3DCOLOR) * sprite->height * sprite->width);

	for (int y = 0; y < sprite->height; y++)
	{
		for (int x = 0; x < sprite->width; x++)
		{
			bitmap.GetPixel(x, y, &pixel);
			sprite->surface[x + y*sprite->width] = D3DCOLOR_ARGB(pixel.GetA(), pixel.GetR(),
				pixel.GetG(), pixel.GetB());
		}
	}

}



void FreeSprite( Sprite* sprite )
{
	free( sprite->surface );
}

void LoadFont( Font* font,D3DCOLOR* surface,const char* filename,
	int charWidth,int charHeight,int nCharsPerRow )
{
	LoadBmp( filename,surface );
	font->charHeight = charHeight;
	font->charWidth = charWidth;
	font->nCharsPerRow = nCharsPerRow;
	font->surface = surface;
}

D3DGraphics::D3DGraphics(HWND hWnd)

	:
	pDirect3D(nullptr),
	pDevice(nullptr),
	pBackBuffer(nullptr),
	pSysBuffer(nullptr)

{
	HRESULT result;

	backRect.pBits = NULL;
	
	pDirect3D = Direct3DCreate9( D3D_SDK_VERSION );
	assert( pDirect3D != NULL );

    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp,sizeof( d3dpp ) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

    result = pDirect3D->CreateDevice( D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE,&d3dpp,&pDevice );
	assert( !FAILED( result ) );

	result = pDevice->GetBackBuffer( 0,0,D3DBACKBUFFER_TYPE_MONO,&pBackBuffer );
	assert( !FAILED( result ) );

	pSysBuffer = new D3DCOLOR[SCREENWIDTH * SCREENHEIGHT];
}

D3DGraphics::~D3DGraphics()
{
	if( pDevice )
	{
		pDevice->Release();
		pDevice = nullptr;
	}
	if( pDirect3D )
	{
		pDirect3D->Release();
		pDirect3D = nullptr;
	}
	if( pBackBuffer )
	{
		pBackBuffer->Release();
		pBackBuffer = nullptr;
	}

	if (pSysBuffer)
	{
		delete pSysBuffer;
		pSysBuffer = nullptr;

	}
}

void D3DGraphics::PutPixel( int x,int y,int r,int g,int b )
{	
	assert( x >= 0 );
	assert( y >= 0 );
	assert( x < SCREENWIDTH );
	assert( y < SCREENHEIGHT );
	pSysBuffer[ x + SCREENWIDTH * y ] = D3DCOLOR_XRGB( r,g,b );
}

void D3DGraphics::PutPixel( int x,int y,D3DCOLOR c )
{	
	assert( x >= 0 );
	assert( y >= 0 );
	assert( x < SCREENWIDTH );
	assert( y < SCREENHEIGHT );
	pSysBuffer[ x + SCREENWIDTH * y ] = c;
}

D3DCOLOR D3DGraphics::GetPixel(int x, int y)
{
	assert(x >= 0);
	assert(y >= 0);
	assert(x < SCREENWIDTH);
	assert(y < SCREENHEIGHT);
	return pSysBuffer[x + SCREENWIDTH * y];
}

void D3DGraphics::BeginFrame()
{
	ZeroMemory(pSysBuffer, SCREENWIDTH * SCREENHEIGHT * sizeof(D3DCOLOR));
}

void D3DGraphics::EndFrame()
{
	HRESULT result;

	result = pBackBuffer->LockRect(&backRect, NULL, NULL);
	assert(!FAILED(result));


	for (int y = 0; y < 600; y++)
	{
		memcpy(&((BYTE*)backRect.pBits)[backRect.Pitch * y], &pSysBuffer[800 * y], sizeof(D3DCOLOR) * 800);
	}	

	result = pBackBuffer->UnlockRect();
	assert( !FAILED( result ) );

	result = pDevice->Present( NULL,NULL,NULL,NULL );
	assert( !FAILED( result ) );
}

void D3DGraphics::DrawDisc( int cx,int cy,int r,int rd,int g,int b )
{
	for( int x = cx - r; x < cx + r; x++ )
	{
		for( int y = cy - r; y < cy + r; y++ )
		{
			if( sqrt( (float)( (x - cx)*(x - cx) + (y - cy)*(y - cy) ) ) < r )
			{
				PutPixel( x,y,rd,g,b );
			}
		}
	}
}

void D3DGraphics::DrawLine( int x1,int y1,int x2,int y2,int r,int g,int blu )
{
	int dx = x2 - x1;
	int dy = y2 - y1;

	if( dy == 0 && dx == 0 )
	{
		PutPixel( x1,y1,r,g,blu );
	}
	else if( abs( dy ) > abs( dx ) )
	{
		if( dy < 0 )
		{
			int temp = x1;
			x1 = x2;
			x2 = temp;
			temp = y1;
			y1 = y2;
			y2 = temp;
		}
		float m = (float)dx / (float)dy;
		float b = x1 - m*y1;
		for( int y = y1; y <= y2; y = y + 1 )
		{
			int x = (int)(m*y + b + 0.5f);
			PutPixel( x,y,r,g,blu );
		}
	}
	else
	{
		if( dx < 0 )
		{
			int temp = x1;
			x1 = x2;
			x2 = temp;
			temp = y1;
			y1 = y2;
			y2 = temp;
		}
		float m = (float)dy / (float)dx;
		float b = y1 - m*x1;
		for( int x = x1; x <= x2; x = x + 1 )
		{
			int y = (int)(m*x + b + 0.5f);
			PutPixel( x,y,r,g,blu );
		}
	}
}

void D3DGraphics::DrawCircle( int centerX,int centerY,int radius,int r,int g,int b )
{
	int rSquared = radius*radius;
	int xPivot = (int)(radius * 0.707107f + 0.5f);
	for( int x = 0; x <= xPivot; x++ )
	{
		int y = (int)(sqrt( (float)( rSquared - x*x ) ) + 0.5f);
		PutPixel( centerX + x,centerY + y,r,g,b );
		PutPixel( centerX - x,centerY + y,r,g,b );
		PutPixel( centerX + x,centerY - y,r,g,b );
		PutPixel( centerX - x,centerY - y,r,g,b );
		PutPixel( centerX + y,centerY + x,r,g,b );
		PutPixel( centerX - y,centerY + x,r,g,b );
		PutPixel( centerX + y,centerY - x,r,g,b );
		PutPixel( centerX - y,centerY - x,r,g,b );
	}
}

void D3DGraphics::DrawSprite( int xoff,int yoff,Sprite* sprite )
{
	for( int y = 0; y < sprite->height; y++ )
	{
		for( int x = 0; x < sprite->width; x++ )
		{
			D3DCOLOR c = sprite->surface[ x + y * sprite->width ];
			if( c != sprite->key )
			{
				PutPixel( x + xoff,y + yoff,c );
			}
		}
	}
}

void D3DGraphics::DrawSpriteAlpha(int xoff, int yoff, Sprite* sprite)
{
	for (int y = 0; y < sprite->height; y++)
	{
		for (int x = 0; x < sprite->width; x++)
		{
			// load source and destination pixels
			const D3DCOLOR src = sprite->surface[x+ y * sprite->width];
			const D3DCOLOR dest = GetPixel(x+ xoff, y + yoff);
			// extract channels
			const unsigned char srcAlpha =	(src & 0xFF000000) >> 24;
			const unsigned char srcRed =	(src & 0x00FF0000) >> 16;
			const unsigned char srcGreen =	(src & 0x0000FF00) >> 8;
			const unsigned char srcBlue =	(src & 0x000000FF);
			
			const unsigned char destRed =	(dest & 0x00FF0000) >> 16;
			const unsigned char destGreen = (dest & 0x0000FF00) >> 8;
			const unsigned char destBlue =	(dest & 0x000000FF);

			const unsigned char rltRed = (srcRed * srcAlpha + destRed * (255 - srcAlpha)) / 255;
			const unsigned char rltGreen = (srcGreen * srcAlpha + destGreen * (255 - srcAlpha)) / 255;
			const unsigned char rltBlue = (srcBlue * srcAlpha + destBlue * (255 - srcAlpha)) / 255;

			// pack channels pack into pixel
			PutPixel(x + xoff, y + yoff, D3DCOLOR_XRGB(rltRed, rltGreen, rltBlue));
			//PutPixel(x + xoff, y = yoff, rltRed, rltGreen, rltBlue);

			}
		}
}

void D3DGraphics::DrawChar( char c,int xoff,int yoff,Font* font,D3DCOLOR color )
{
	if( c < ' ' || c > '~' )
		return;

	const int sheetIndex = c - ' ';
	const int sheetCol = sheetIndex % font->nCharsPerRow;
	const int sheetRow = sheetIndex / font->nCharsPerRow;
	const int xStart = sheetCol * font->charWidth;
	const int yStart = sheetRow * font->charHeight;
	const int xEnd = xStart + font->charWidth;
	const int yEnd = yStart + font->charHeight;
	const int surfWidth = font->charWidth * font->nCharsPerRow;

	for( int y = yStart; y < yEnd; y++ )
	{
		for( int x = xStart; x < xEnd; x++ )
		{
			if( font->surface[ x + y * surfWidth ] == D3DCOLOR_XRGB( 0,0,0 ) )
			{
				PutPixel( x + xoff - xStart,y + yoff - yStart,color );
			}
		}
	}
}

void D3DGraphics::DrawString( const char* string,int xoff,int yoff,Font* font,D3DCOLOR color )
{
	for( int index = 0; string[ index ] != '\0'; index++ )
	{
		DrawChar( string[ index ],xoff + index * font->charWidth,yoff,font,color );
	}
}