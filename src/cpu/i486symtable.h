#ifndef I486SYMTABLE_IS_INCLUDED
#define I486SYMTABLE_IS_INCLUDED
/* { */

#include <istream>
#include <ostream>
#include <vector>
#include <string>
#include <map>

#include "i486.h"

class i486Symbol
{
public:
	enum
	{
		SYM_ANY,
		SYM_CODE,
		SYM_DATA,
	};
	i486DX::FarPointer ptr;
	bool temporary;  // If true, it will not be saved to file.
	unsigned int symType;
	std::string return_type;
	std::string label;
	std::string param;
	std::vector <std::string> info;

	// Format
	// \begin0
	// 0/1/2  Type (0:Any 1:Code 2:Data)
	// 000C:00004000  SEG:OFFSET
	// void  Return-Type
	// main  Label
	// int argc,char *argv[]
	// Supplimental Info
	// \end

	i486Symbol();
};

class i486SymbolTable
{
private:
	std::map <i486DX::FarPointer,i486Symbol> symTable;
public:
	mutable std::string fName;

	/*! Open the given file name.  
	    It updates data member fName to the given file name if successful.
	*/
	bool Load(const char fName[]);
	bool Load(std::istream &ifp);

	/*! Save to the given file name.
	    It updates data member fName to the given file name if successful.
	*/
	bool Save(const char fName[]) const;
	bool Save(std::ostream &ofp) const;

	const i486Symbol *Find(i486DX::FarPointer ptr) const;
	i486Symbol *Update(i486DX::FarPointer ptr,const std::string &label);
	bool Delete(i486DX::FarPointer ptr);
	const std::map <i486DX::FarPointer,i486Symbol> &GetTable(void) const;

	std::vector <std::string> GetList(bool returnType=false,bool label=true,bool param=true) const;
};


/* } */
#endif
