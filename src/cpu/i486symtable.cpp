#include <fstream>

#include "i486symtable.h"



i486Symbol::i486Symbol()
{
	ptr.SEG=0;
	ptr.OFFSET=0;
	symType=SYM_ANY;
}


////////////////////////////////////////////////////////////


bool i486SymbolTable::Load(const char fName[])
{
	std::ifstream ifp(fName);
	return ifp.is_open() && Load(ifp);
}
bool i486SymbolTable::Load(std::istream &ifp)
{
	return false;
}

bool i486SymbolTable::Save(const char fName[]) const
{
	std::ofstream ofp(fName);
	return ofp.is_open() && Save(ofp);
}
bool i486SymbolTable::Save(std::ostream &ofp) const
{
	return false;
}

const i486Symbol *i486SymbolTable::Find(i486DX::FarPointer ptr)
{
	return nullptr;
}
i486Symbol *i486SymbolTable::Update(i486DX::FarPointer ptr,const std::string &label)
{
	auto &symbol=symTable[ptr];
	symbol.label=label;
	return &symbol;
}
