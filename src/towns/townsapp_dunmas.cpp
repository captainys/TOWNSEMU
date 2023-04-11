// Dungeon Master for FM TOWNS
// Keyboard Shortcuts
#include "towns.h"



// F1 F2 F3 F4  Translated to 1,2,3,4 (Open char window)
// 1 2 3 4      Select who to cast a spell
// Q W E R T Y  Select magic power
// L            Light Spell (OH IR RA)
// O            Torch Spell
// I            Make Life Potion
// P            Make Detox Potion
// M            Make Stamina Potion
// B            Lightning Bolt
// F            Fireball
// V            See Through
// D            Party Defense Spell
// S            Party Fire Defense Spell

// SPACE        Front member low-level attack
// Shift+SPACE  Front member low-level attack continuous
// TAB          All member low-leverl attack
// Shift+TAB    All member low-leverl attack continuous

// Enter        4 Fireballs

// ESC          Abort action



static void Dunmas_PushBack_MouseClick(TownsEventLog &eventLog,int x,int y)
{
	TownsEventLog::Event e;
	e.eventType=TownsEventLog::EVT_LBUTTONDOWN;
	e.t=std::chrono::milliseconds(75);
	e.mos.Set(x,y);
	eventLog.AddEvent(e);

	e.eventType=TownsEventLog::EVT_LBUTTONUP;
	e.t=std::chrono::milliseconds(75);
	e.mos.Set(x,y);
	eventLog.AddEvent(e);
}

static void Dunmas_Click_Spell_Symbol(TownsEventLog &eventLog,int level,int Yoffset)
{
	// Spell Level 1
	// MOS 241 56
	// Spell Level 2
	// MOS 255 56
	// Spell Level 3
	// MOS 269 56
	// Spell Level 4
	// MOS 283 56
	// Spell Level 5
	// MOS 297 56
	// Spell Level 6
	// MOS 311 56

	int x=241+14*level;
	Dunmas_PushBack_MouseClick(eventLog,x,56+Yoffset);
}

static void Dunmas_Click_Cast_Spell(TownsEventLog &eventLog,int Yoffset)
{
	Dunmas_PushBack_MouseClick(eventLog,280,67+Yoffset);
}

void FMTownsCommon::Dunmas_Spell_Char(int charNum,int Yoffset)
{
	// Spell Char 1
	// MOS 239 46
	// Spell Char 2
	// MOS 239 46
	// MOS 285 46
	// Spell Char 3
	// MOS 313 45
	// MOS 266 46
	// Spell Char 4
	// MOS 312 46
	switch(charNum)
	{
	case 0:
		Dunmas_PushBack_MouseClick(eventLog,239,46+Yoffset);
		break;
	case 1:
		Dunmas_PushBack_MouseClick(eventLog,239,46+Yoffset);
		Dunmas_PushBack_MouseClick(eventLog,285,46+Yoffset);
		break;
	case 2:
		Dunmas_PushBack_MouseClick(eventLog,313,46+Yoffset);
		Dunmas_PushBack_MouseClick(eventLog,266,46+Yoffset);
		break;
	case 3:
		Dunmas_PushBack_MouseClick(eventLog,313,46+Yoffset);
		break;
	}
}

void FMTownsCommon::Dunmas_Spell_Level(int level)
{
	if(level<0)
	{
		level=0;
	}
	else if(5<level)
	{
		level=5;
	}
	state.appSpecific_Dunmas_SpellPower=level;
}

void FMTownsCommon::Dunmas_Spell_Light(int Yoffset)
{
	// POW,2,3,4
	Dunmas_Click_Spell_Symbol(eventLog,state.appSpecific_Dunmas_SpellPower,Yoffset);
	Dunmas_Click_Spell_Symbol(eventLog,2,Yoffset);
	Dunmas_Click_Spell_Symbol(eventLog,3,Yoffset);
	Dunmas_Click_Spell_Symbol(eventLog,4,Yoffset);
	Dunmas_Click_Cast_Spell(eventLog,Yoffset);
}
void FMTownsCommon::Dunmas_Spell_Torch(int Yoffset)
{
	// POW,3
	Dunmas_Click_Spell_Symbol(eventLog,state.appSpecific_Dunmas_SpellPower,Yoffset);
	Dunmas_Click_Spell_Symbol(eventLog,3,Yoffset);
	Dunmas_Click_Cast_Spell(eventLog,Yoffset);
}
void FMTownsCommon::Dunmas_Spell_SeeThrough(int Yoffset)
{
	// POW,2,1,4
	Dunmas_Click_Spell_Symbol(eventLog,state.appSpecific_Dunmas_SpellPower,Yoffset);
	Dunmas_Click_Spell_Symbol(eventLog,2,Yoffset);
	Dunmas_Click_Spell_Symbol(eventLog,1,Yoffset);
	Dunmas_Click_Spell_Symbol(eventLog,4,Yoffset);
	Dunmas_Click_Cast_Spell(eventLog,Yoffset);
}
void FMTownsCommon::Dunmas_Spell_MakeLifePotion(int Yoffset)
{
	// POW,1
	Dunmas_Click_Spell_Symbol(eventLog,state.appSpecific_Dunmas_SpellPower,Yoffset);
	Dunmas_Click_Spell_Symbol(eventLog,1,Yoffset);
	Dunmas_Click_Cast_Spell(eventLog,Yoffset);
}
void FMTownsCommon::Dunmas_Spell_MakeDetoxPotion(int Yoffset)
{
	// 0,1,5
	Dunmas_Click_Spell_Symbol(eventLog,state.appSpecific_Dunmas_SpellPower,Yoffset);
	Dunmas_Click_Spell_Symbol(eventLog,1,Yoffset);
	Dunmas_Click_Spell_Symbol(eventLog,4,Yoffset);
	Dunmas_Click_Cast_Spell(eventLog,Yoffset);
}
void FMTownsCommon::Dunmas_Spell_MakeStaminaPotion(int Yoffset)
{
	// POW,0
	Dunmas_Click_Spell_Symbol(eventLog,state.appSpecific_Dunmas_SpellPower,Yoffset);
	Dunmas_Click_Spell_Symbol(eventLog,0,Yoffset);
	Dunmas_Click_Cast_Spell(eventLog,Yoffset);
}
void FMTownsCommon::Dunmas_Spell_Fireball(int Yoffset)
{
	// POW,3,3
	Dunmas_Click_Spell_Symbol(eventLog,state.appSpecific_Dunmas_SpellPower,Yoffset);
	Dunmas_Click_Spell_Symbol(eventLog,3,Yoffset);
	Dunmas_Click_Spell_Symbol(eventLog,3,Yoffset);
	Dunmas_Click_Cast_Spell(eventLog,Yoffset);
}
void FMTownsCommon::Dunmas_Spell_FourFireballs(int Yoffset)
{
	Dunmas_PushBack_MouseClick(eventLog,239,46+Yoffset);
	Dunmas_Spell_Fireball(Yoffset);
	Dunmas_PushBack_MouseClick(eventLog,285,46+Yoffset);
	Dunmas_Spell_Fireball(Yoffset);
	Dunmas_PushBack_MouseClick(eventLog,313,46+Yoffset);
	Dunmas_Spell_Fireball(Yoffset);
	Dunmas_PushBack_MouseClick(eventLog,266,46+Yoffset);
	Dunmas_Spell_Fireball(Yoffset);
}
void FMTownsCommon::Dunmas_Spell_LightningBolt(int Yoffset)
{
	// POW,2,2,5
	Dunmas_Click_Spell_Symbol(eventLog,state.appSpecific_Dunmas_SpellPower,Yoffset);
	Dunmas_Click_Spell_Symbol(eventLog,2,Yoffset);
	Dunmas_Click_Spell_Symbol(eventLog,2,Yoffset);
	Dunmas_Click_Spell_Symbol(eventLog,4,Yoffset);
	Dunmas_Click_Cast_Spell(eventLog,Yoffset);
}
void FMTownsCommon::Dunmas_Spell_Defense(int Yoffset)
{
	// POW,0,3
	Dunmas_Click_Spell_Symbol(eventLog,state.appSpecific_Dunmas_SpellPower,Yoffset);
	Dunmas_Click_Spell_Symbol(eventLog,0,Yoffset);
	Dunmas_Click_Spell_Symbol(eventLog,3,Yoffset);
	Dunmas_Click_Cast_Spell(eventLog,Yoffset);
}
void FMTownsCommon::Dunmas_Spell_FireDefense(int Yoffset)
{
	// POW,3,4,3
	Dunmas_Click_Spell_Symbol(eventLog,state.appSpecific_Dunmas_SpellPower,Yoffset);
	Dunmas_Click_Spell_Symbol(eventLog,3,Yoffset);
	Dunmas_Click_Spell_Symbol(eventLog,4,Yoffset);
	Dunmas_Click_Spell_Symbol(eventLog,3,Yoffset);
	Dunmas_Click_Cast_Spell(eventLog,Yoffset);
}
void FMTownsCommon::Dunmas_FrontRow_Attack(int level,bool continuous,int Yoffset)
{
	Dunmas_PushBack_MouseClick(eventLog,246,89+Yoffset);
	Dunmas_PushBack_MouseClick(eventLog,246,89+Yoffset);
	Dunmas_PushBack_MouseClick(eventLog,260,89+Yoffset);
	Dunmas_PushBack_MouseClick(eventLog,260,88+Yoffset);
	if(true==continuous)
	{
		TownsEventLog::Event e;
		e.eventType=TownsEventLog::EVT_REPEAT;
		eventLog.AddEvent(e);
	}
}
void FMTownsCommon::Dunmas_All_Attack(int level,bool continuous,int Yoffset)
{
	Dunmas_PushBack_MouseClick(eventLog,246,89+Yoffset);
	Dunmas_PushBack_MouseClick(eventLog,246,89+Yoffset);
	Dunmas_PushBack_MouseClick(eventLog,260,89+Yoffset);
	Dunmas_PushBack_MouseClick(eventLog,260,88+Yoffset);
	Dunmas_PushBack_MouseClick(eventLog,290,89+Yoffset);
	Dunmas_PushBack_MouseClick(eventLog,290,88+Yoffset);
	Dunmas_PushBack_MouseClick(eventLog,302,89+Yoffset);
	Dunmas_PushBack_MouseClick(eventLog,302,88+Yoffset);
	if(true==continuous)
	{
		TownsEventLog::Event e;
		e.eventType=TownsEventLog::EVT_REPEAT;
		eventLog.AddEvent(e);
	}
}
