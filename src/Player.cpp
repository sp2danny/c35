
#include "stdafx.h"

// --------------------------------------------

#include "Player.h"

#include "City.h"

int C35::Player::nextId = 0 ;
std::vector<C35::Player*> C35::Player::players ;

int C35::Player::NewID()
{
	return ++nextId;
}

void C35::Player::Register(int id,C35::Player* p)
{
	if( players.size() <= id )
	{
		players.resize( id+1, 0 );
	}
	assert( players[id] == 0 );
	players[id]=p;
}

C35::Player* C35::Player::Lookup(int id)
{
	if(id<0) return 0;
	if(id>=players.size()) return 0;
	return players[id];
}

C35::Player* C35::Player::MakeNew()
{
	Player* p = new Player();
	p->capital = 0;
	//p->controller = 0;
	p->game = 0;
	p->gold = 0;
	p->id = NewID();
	Register( p->id, p );
	return p;
}

void C35::Player::AllToStream( std::ostream& o )
{
	int j,n=players.size();
	WriteBinary(o,n-1);
	for(j=1;j<n;++j)
	{
		Player* p = players[j];
		//if(p->capital)
		//	WriteBinary(o,p->capital->
		WriteBinary(o,p->color);
		WriteBinary(o,p->gold);
		WriteBinary(o,p->isbarb);
	}
}

void C35::Player::AllFromStream( std::istream& i )
{
	int j,n;
	ReadBinary(i,n);
	players.clear();
	nextId = 0;
	for(j=1;j<=n;++j)
	{
		Player* p = Player::MakeNew();
		ReadBinary(i,p->color);
		ReadBinary(i,p->gold);
		ReadBinary(i,p->isbarb);
	}
}
