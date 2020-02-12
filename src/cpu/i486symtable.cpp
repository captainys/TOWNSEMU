#include <fstream>

#include "i486symtable.h"
#include "cpputil.h"



i486Symbol::i486Symbol()
{
	ptr.SEG=0;
	ptr.OFFSET=0;
	symType=SYM_ANY;
	temporary=false;
}


////////////////////////////////////////////////////////////


bool i486SymbolTable::Load(const char fName[])
{
	std::ifstream ifp(fName);
	if(ifp.is_open() && Load(ifp))
	{
		this->fName=fName;
		return true;
	}
	return false;
}
bool i486SymbolTable::Load(std::istream &ifp)
{
	return false;
}

bool i486SymbolTable::Save(const char fName[]) const
{
	std::ofstream ofp(fName);
	if(ofp.is_open() && Save(ofp))
	{
		this->fName=fName;
		return true;
	}
	return false;
}
bool i486SymbolTable::Save(std::ostream &ofp) const
{
	return false;
}

const i486Symbol *i486SymbolTable::Find(i486DX::FarPointer ptr) const
{
	return nullptr;
}
i486Symbol *i486SymbolTable::Update(i486DX::FarPointer ptr,const std::string &label)
{
	auto &symbol=symTable[ptr];
	symbol.label=label;
	return &symbol;
}
bool i486SymbolTable::Delete(i486DX::FarPointer ptr)
{
	auto iter=symTable.find(ptr);
	if(symTable.end()!=iter)
	{
		symTable.erase(iter);
		return true;
	}
	return false;
}
const std::map <i486DX::FarPointer,i486Symbol> &i486SymbolTable::GetTable(void) const
{
	return symTable;
}

std::vector <std::string> i486SymbolTable::GetList(bool returnType,bool label,bool param) const
{
	std::vector <std::string> text;
	for(const auto &addrAndSym : symTable)
	{
		std::string empty;

		auto &addr=addrAndSym.first;
		auto &sym=addrAndSym.second;

		text.push_back(empty);
		text.back()=cpputil::Ustox(addr.SEG);
		text.back()+=":";
		text.back()+=cpputil::Uitox(addr.OFFSET);

		if(true==returnType)
		{
			text.back()+=" ";
			text.back()+=sym.return_type;
		}
		if(true==label)
		{
			text.back()+=" ";
			text.back()+=sym.label;
		}
		if(true==param)
		{
			text.back()+=" (";
			text.back()+=sym.param;
			text.back()+=")";
		}
	}
	return text;
}
