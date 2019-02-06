
#include "stdafx.h"

// --------------------------------------------

#include "MakeFrame.h"

SDL_Surface* MakeFrame(int w,int h)
{
	SDL_Surface* vidsurf = SDL_GetVideoSurface();

	int bpp = vidsurf->format->BitsPerPixel;
	int rm  = vidsurf->format->Rmask;
	int gm  = vidsurf->format->Gmask;
	int bm  = vidsurf->format->Bmask;
	int am  = vidsurf->format->Amask;

	SDL_Surface* surf = SDL_CreateRGBSurface( SDL_HWSURFACE, w, h, bpp, rm,gm,bm,am );

	Uint32 ck = SDL_MapRGB(surf->format,255,0,255);

	SDL_FillRect( surf, 0, ck );
	SDL_SetColorKey( surf, SDL_SRCCOLORKEY, ck );

	static SDL_Surface* UL = 0;
	static SDL_Surface* U  = 0;
	static SDL_Surface* UR = 0;
	static SDL_Surface* L  = 0;
	static SDL_Surface* R  = 0;
	static SDL_Surface* DL = 0;
	static SDL_Surface* D  = 0;
	static SDL_Surface* DR = 0;

	if( ! UL ) UL = SDL_LoadBMP( "gfx/MiniMap/frame_ul.bmp" );
	if( ! U  ) U  = SDL_LoadBMP( "gfx/MiniMap/frame_u.bmp"  );
	if( ! UR ) UR = SDL_LoadBMP( "gfx/MiniMap/frame_ur.bmp" );
	if( ! L  ) L  = SDL_LoadBMP( "gfx/MiniMap/frame_l.bmp"  );
	if( ! R  ) R  = SDL_LoadBMP( "gfx/MiniMap/frame_r.bmp"  );
	if( ! DL ) DL = SDL_LoadBMP( "gfx/MiniMap/frame_dl.bmp" );
	if( ! D  ) D  = SDL_LoadBMP( "gfx/MiniMap/frame_d.bmp"  );
	if( ! DR ) DR = SDL_LoadBMP( "gfx/MiniMap/frame_dr.bmp" );

	// up-left
	{
		SDL_Rect r = { 0,0, UL->w, UL->h };
		SDL_BlitSurface( UL, 0, surf, &r );
	}

	// up
	int x = UL->w;
	while( x < (w-UR->w) )
	{
		SDL_Rect r = { x,0, U->w, U->h };
		SDL_BlitSurface( U, 0, surf, &r );
		x+=U->w;
	}

	// up-right
	{
		SDL_Rect r = { w-UR->w,0, UR->w, UR->h };
		SDL_BlitSurface( UR, 0, surf, &r );
	}

	// left
	int y = UL->h;
	while( y < (h-DL->h) )
	{
		SDL_Rect r = { 0,y, L->w, L->h };
		SDL_BlitSurface( L, 0, surf, &r );
		y+=L->h;
	}

	// right
	y = UR->h;
	while( y < (h-DR->h) )
	{
		SDL_Rect r = { w-R->w,y, R->w, R->h };
		SDL_BlitSurface( R, 0, surf, &r );
		y+=R->w;
	}

	// down-left
	{
		SDL_Rect r = { 0,h-DL->h, DL->w, DL->h };
		SDL_BlitSurface( DL, 0, surf, &r );
	}

	// down
	x = DL->w;
	while( x < (w-DR->w) )
	{
		SDL_Rect r = { x,h-D->h, D->w, D->h };
		SDL_BlitSurface( D, 0, surf, &r );
		x+=D->w;
	}

	// down-right
	{
		SDL_Rect r = { w-DR->w,h-DR->h, DR->w, DR->h };
		SDL_BlitSurface( DR, 0, surf, &r );
	}

	// SDL_SaveBMP( surf, "gfx/latest_frame.bmp" );

	return surf;
}
