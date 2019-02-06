
#pragma once

#include <string>
#include <list>
#include <vector>

#include "Font.h"

class EditField
{
public:
	EditField(int,int,int);
	~EditField();

	bool ParseClick(int,int);
	bool ParseKey(int);
	void Display();

	std::string GetText();
	void SetText(std::string);

	static void SetFont(Font&);
	static bool ParseClickAll(int,int);
	static bool ParseKeyAll(int);
	static void DisplayAll();
private:

	std::string text;
	bool active;
	int x,y;
	int w,h;
	
	static Font* f;
	static void deactivate_others( EditField* );

	static std::list<EditField*> all;
};

extern bool inside(SDL_Rect r);

extern bool Button(int x,int y,char* txt);

class ListBox
{
public:
	ListBox(int,int,int,int,Font&);
	~ListBox();

	void Display();

	bool ParseClick(int,int);
	void SetLines(std::vector<std::string>);

	bool Selected(std::string&);
	bool Selected(int&);

	void Visible(bool v) { visible=v; }
private:
	std::vector<std::string> lines;
	bool sel;
	int si;
	Font& f;
	int x,y,w,h;
	bool visible;
};
