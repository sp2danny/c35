
#include "stdafx.h"

// --------------------------------------------

#include "sdlGuiColl.h"

#include "Font.h"

using namespace std;

list<EditField*> EditField::all = list<EditField*>();
Font* EditField::f =0;

EditField::EditField(int _x,int _y,int _w)
	: x(_x), y(_y), w(_w)
{
	h = f?f->Height():0;
	all.push_back(this);
	active=false;
	text.clear();
}

EditField::~EditField()
{
	list<EditField*>::iterator i;
	i = find(all.begin(),all.end(),this);
	all.erase(i);
}

static bool inside( int x,int y, int x1,int y1, int w,int h)
{
	if( (x<x1) || (x>(x1+w)) ) return false;
	if( (y<y1) || (y>(y1+h)) ) return false;
	return true;
}

bool EditField::ParseClick(int mx,int my)
{
	if(inside( mx,my, x,y, w,h ))
	{
		active=true;
		deactivate_others(this);
		return true;
	}
	return false;
}

bool EditField::ParseKey(int k)
{
	if(active)
	{
		if(k==SDLK_BACKSPACE)
		{
			int n = text.size();
			if(n)
				text = text.substr(0,n-1);
			return true;
		}
		else if(isprint(k))
		{
			text += (char)k;
			return true;
		}
	}
	return false;
}

void EditField::Display()
{
	SDL_Surface* s = SDL_GetVideoSurface();
	Uint32 bc = SDL_MapRGB( s->format, 127,127,127 );
	SDL_Rect r = { x,y,w,h };
	SDL_FillRect(s,&r,bc);

	string ss = text;
	if( active && ((SDL_GetTicks()/120)%2) )
		ss += "|";

	if( f->Width(text.c_str()) > w )
	{
		static bool first = true;
		static TTF_Font* ttf_font;
		static SDL_Color col = { 0,0,0 };
		if(first)
		{
			TTF_Init();
			ttf_font = TTF_OpenFont("mss.ttf", 16);
			first=false;
		}
		SDL_Surface* tt;
		tt = TTF_RenderText_Solid(ttf_font, ss.c_str(), col );
		SDL_Rect r = { x,y,tt->w,tt->h };
		SDL_BlitSurface(tt,0,s,&r);
		SDL_FreeSurface(tt);
	} else {
		f->Print(s,ss.c_str(),x,y);
	}
}

string EditField::GetText() { return text; }

void EditField::SetText(string s) { text=s; }

void EditField::SetFont(Font& _f) { f=&_f; }

bool EditField::ParseClickAll(int mx,int my)
{
	bool found = false;
	for( EditField* ef : all )
		if(ef->ParseClick(mx,my))
			found=true;
	if(!found)
		deactivate_others(0);
	return found;
}

bool EditField::ParseKeyAll(int k)
{
	bool found = false;
	for( EditField* ef : all )
		if(ef->ParseKey(k))
			found=true;
	return found;
}
void EditField::DisplayAll()
{
	for( EditField* ef : all )
		ef->Display();
}

void EditField::deactivate_others( EditField* me )
{
	for( EditField* ef : all )
		if(ef!=me)
			ef->active=false;
}
	
extern int mx,my;
extern bool lmd;
extern bool want_out;
extern Font f_w;
extern Font f_b;

#define screen SDL_GetVideoSurface()

bool inside(SDL_Rect r)
{
	if( mx < r.x ) return false;
	if( my < r.y ) return false;
	if( (mx - r.x) > r.w ) return false;
	if( (my - r.y) > r.h ) return false;
	return true;
}

bool Button(int x,int y,const char* txt)
{
	static Uint32 grey  = SDL_MapRGB( screen->format, 127,127,127 );
	static Uint32 white = SDL_MapRGB( screen->format, 255,255,255 );

	int w = f_b.Width(txt);
	int h = f_b.Height();

	SDL_Rect r = { x-w/2-5, y-5, w+10, h+10 };

	bool ins = inside(r);
	if(ins)
		SDL_FillRect(screen,&r,white);
	else
		SDL_FillRect(screen,&r,grey);

	f_b.Print(screen,txt,x-w/2,y);

	return ins;
}

// ***************
// *** ListBox ***
// ***************

ListBox::ListBox(int _x,int _y,int _w,int _h,Font& _f)
	: x(_x), y(_y), w(_w), h(_h), f(_f)
{
	lines.clear();
	visible = true;
	sel = false;
}

ListBox::~ListBox()
{
}

void ListBox::Display()
{
	if(!visible) return ;

	static Uint32 grey  = SDL_MapRGB( screen->format, 127,127,127 );
	static Uint32 white = SDL_MapRGB( screen->format, 255,255,255 );

	int fh = f.Height();

	SDL_Rect r = { x,y,w,h };

	SDL_FillRect(screen,&r,grey);

	int i, n=lines.size();
	int yy=y;
	for(i=0;i<n;++i)
	{
		if(sel&&(si==i))
		{
			SDL_Rect r2 = { x,yy,w,fh };
			SDL_FillRect(screen,&r2,white);
		}
		f.Print(screen,lines[i].c_str(),x,yy);
		yy+=fh;
	}

}

bool ListBox::ParseClick(int mx,int my)
{
	if( visible && inside(mx,my,x,y,w,h) )
	{
		int fh = f.Height();
		int idx = (my-y) / fh;
		int n = lines.size();
		if( idx < n )
		{
			si=idx;
			sel=true;
			return true;
		}
	}
	return false;
}

void ListBox::SetLines(std::vector<std::string> l)
{
	lines = l;
	sel=false;
}

bool ListBox::Selected(std::string& l)
{
	if(sel) l = lines[si];
	return sel;
}

bool ListBox::Selected(int& i)
{
	if(sel) i=si;
	return sel;
}

