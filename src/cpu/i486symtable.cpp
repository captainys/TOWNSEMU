#include <fstream>
#include <iostream>

#include "i486symtable.h"
#include "cpputil.h"



i486Symbol::i486Symbol()
{
	symType=SYM_ANY;
	temporary=false;
}
std::string i486Symbol::Format(bool returnType,bool label,bool param) const
{
	std::string str;
	if(true==returnType)
	{
		str+=this->return_type;
		if(true==label || true==param)
		{
			str+=" ";
		}
	}
	if(true==label)
	{
		str+=this->label;
		if(SYM_JUMP_DESTINATION==symType)
		{
			str+=':';
		}
	}
	if((SYM_PROCEDURE==symType || SYM_ANY==symType) && true==param)
	{
		str+="(";
		str+=this->param;
		str+=")";
	}
	return str;
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

bool i486SymbolTable::AutoSave(void) const
{
	return Save(fName.c_str());
}

const i486Symbol *i486SymbolTable::Find(unsigned int SEG,unsigned int OFFSET) const
{
	i486DX::FarPointer ptr;
	ptr.SEG=SEG;
	ptr.OFFSET=OFFSET;
	return Find(ptr);
}
const i486Symbol *i486SymbolTable::Find(i486DX::FarPointer ptr) const
{
	auto iter=symTable.find(ptr);
	if(symTable.end()!=iter)
	{
		return &iter->second;
	}
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

void i486SymbolTable::PrintIfAny(unsigned int SEG,unsigned int OFFSET,bool returnType,bool label,bool param) const
{
	i486DX::FarPointer ptr;
	ptr.SEG=SEG;
	ptr.OFFSET=OFFSET;
	auto *sym=Find(ptr);
	if(nullptr!=sym)
	{
		std::cout << sym->Format(returnType,label,param) << std::endl;
	}
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

		text.back()+=" ";
		text.back()+=sym.Format(returnType,label,param);
	}
	return text;
}
