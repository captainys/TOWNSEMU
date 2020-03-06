#include <fstream>
#include <iostream>

#include "i486symtable.h"
#include "cpputil.h"



i486Symbol::i486Symbol()
{
	CleanUp();
}
void i486Symbol::CleanUp(void)
{
	temporary=false;
	symType=SYM_ANY;
	return_type="";
	label="";
	inLineComment="";
	param="";
	info.clear();
	rawDataBytes=0;
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
	const int STATE_OUTSIDE=0,STATE_INSIDE=1;
	int state=STATE_OUTSIDE;
	i486DX::FarPointer curPtr;
	i486Symbol curSymbol;

	// /begin0
	// T 0/1/2,2  Type (0:Any 1:Procedure 2:Jump Destination 3:Data)
	// * 000C:00004000  SEG:OFFSET
	// # Inline Comment
	// R void  Return-Type
	// L main  Label
	// P int argc,char *argv[]
	// I Supplimental Info
	// % Raw Data Byte Count
	// /end

	while(true!=ifp.eof())
	{
		std::string str;
		std::getline(ifp,str);

		if(STATE_OUTSIDE==state)
		{
			cpputil::Capitalize(str);
			if(str=="/BEGIN0")
			{
				curPtr.SEG=0;
				curPtr.OFFSET=0;
				curSymbol.CleanUp();
				state=STATE_INSIDE;
			}
		}
		else if(STATE_INSIDE==state)
		{
			if('/'==str[0])
			{
				cpputil::Capitalize(str);
				if("/END"==str)
				{
					symTable[curPtr]=curSymbol;
				}
			}
			else if(2<=str.size())
			{
				switch(str[0])
				{
				case 't':
				case 'T':
					curSymbol.symType=cpputil::Atoi(str.c_str()+2);
					break;
				case '*':
					curPtr.MakeFromString(str.c_str()+2);
					break;
				case '#':
					curSymbol.inLineComment=(str.c_str()+2);
					break;
				case 'r':
				case 'R':
					curSymbol.return_type=(str.c_str()+2);
					break;
				case 'l':
				case 'L':
					curSymbol.label=(str.c_str()+2);
					break;
				case 'p':
				case 'P':
					curSymbol.param=(str.c_str()+2);
					break;
				case 'i':
				case 'I':
					curSymbol.info.push_back(str.c_str()+2);
					break;
				case '%':
					curSymbol.rawDataBytes=cpputil::Atoi(str.c_str()+2);
					break;
				}
			}
		}
	}
	return true;
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
	for(auto &ptrAndSym : symTable)
	{
		auto &ptr=ptrAndSym.first;
		auto &sym=ptrAndSym.second;
		if(true!=sym.temporary)
		{
			ofp << "/begin0" << std::endl;
			ofp << "T " << (int)(sym.symType) << std::endl;
			ofp << "* " << ptr.Format() << std::endl;
			ofp << "# " << sym.inLineComment << std::endl;
			ofp << "R " << sym.return_type << std::endl;
			ofp << "L " << sym.label  << std::endl;
			ofp << "P " << sym.param <<  std::endl;
			ofp << "% " << sym.rawDataBytes <<  std::endl;
			for(auto &i : sym.info)
			{
				ofp << "I " << i <<  std::endl;
			}
			ofp << "/end" << std::endl;
		}
	}
	return true;
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
std::pair <i486DX::FarPointer,i486Symbol> i486SymbolTable::FindSymbolFromLabel(const std::string &label) const
{
	for(auto &addrAndSym : symTable)
	{
		if(addrAndSym.second.label==label)
		{
			return addrAndSym;
		}
	}
	std::pair <i486DX::FarPointer,i486Symbol> empty;
	return empty;
}
i486Symbol *i486SymbolTable::Update(i486DX::FarPointer ptr,const std::string &label)
{
	auto &symbol=symTable[ptr];
	symbol.label=label;
	return &symbol;
}
i486Symbol *i486SymbolTable::SetComment(i486DX::FarPointer ptr,const std::string &inLineComment)
{
	auto &symbol=symTable[ptr];
	symbol.inLineComment=inLineComment;
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
bool i486SymbolTable::DeleteComment(i486DX::FarPointer ptr)
{
	auto iter=symTable.find(ptr);
	if(symTable.end()!=iter)
	{
		iter->second.inLineComment="";
		return true;
	}
	return false;
}
const std::map <i486DX::FarPointer,i486Symbol> &i486SymbolTable::GetTable(void) const
{
	return symTable;
}

unsigned int i486SymbolTable::GetRawDataBytes(i486DX::FarPointer ptr) const
{
	auto *sym=Find(ptr);
	if(i486Symbol::SYM_RAW_DATA==sym->symType &&
	   i486Symbol::SYM_TABLE_WORD==sym->symType &&
	   i486Symbol::SYM_TABLE_DWORD==sym->symType &&
	   i486Symbol::SYM_TABLE_FWORD16==sym->symType &&
	   i486Symbol::SYM_TABLE_FWORD32==sym->symType)
	{
		return sym->rawDataBytes;
	}
	return 0;
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
	
		if(0<sym.inLineComment.size())
		{
			text.back()+=" ; ";
			text.back()+=sym.inLineComment;
		}
	}
	return text;
}
