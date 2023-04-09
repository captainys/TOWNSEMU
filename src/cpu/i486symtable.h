/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
#ifndef I486SYMTABLE_IS_INCLUDED
#define I486SYMTABLE_IS_INCLUDED
/* { */

#include <istream>
#include <ostream>
#include <vector>
#include <string>
#include <map>
#include <stdint.h>

#include "i486.h"

class i486Symbol
{
public:
	enum
	{
		SYM_ANY,
		SYM_PROCEDURE,
		SYM_JUMP_DESTINATION,
		SYM_DATA_LABEL,
		SYM_RAW_DATA, // If disassembler hits this address, it just reads rawDataCount bytes and don't disassemble.
		SYM_TABLE_WORD,
		SYM_TABLE_DWORD,
		SYM_TABLE_FWORD16,
		SYM_TABLE_FWORD32,
	};

	bool temporary=false;  // If true, it will not be saved to file.
	bool immIsIOAddr=false; // If true, disassembler should take Imm operand as an IO-port address.
	bool immIsSymbol=false; // Take imm as a symbol.
	bool immIsASCII=false; // Take imm as ASCII code.
	bool offsetIsSymbol=false; // Take offset as a symbol.
	unsigned int symType;
	std::string return_type;
	std::string label;
	std::string inLineComment;
	std::string param;
	std::string imported; // Imported from .EXP symbol table.
	std::vector <std::string> info;
	unsigned int rawDataBytes;

	// Format
	// \begin0
	// 0/1/2,2  Type (0:Any 1:Procedure 2:Jump Destination 3:Data)
	// 000C:00004000  SEG:OFFSET
	// void  Return-Type
	// main  Label
	// int argc,char *argv[]
	// Supplimental Info
	// \end

	i486Symbol();
	void CleanUp(void);
	std::string Format(bool returnType=false,bool label=true,bool param=true) const;
};


class i486INT
{
public:
	std::string label;
};


class i486INTFunc
{
public:
	std::string label;
};


class i486SymbolTable
{
private:
	std::map <i486DXCommon::FarPointer,i486Symbol> symTable;
	std::map <unsigned int,i486INT> INTLabel;
	std::map <unsigned int,i486INTFunc> INTFunc[256];
public:
	mutable std::string fName;

	/*! Open the given file name.  
	    It won't erase the current contents of the symbol table.
	    It adds or overwrites the contents from the symbol-table file.
	    It updates data member fName to the given file name if successful.
	*/
	bool Load(const char fName[]);
	bool Load(std::istream &ifp);

	/*! Save to the given file name.
	    It updates data member fName to the given file name if successful.
	*/
	bool Save(const char fName[]) const;
	bool Save(std::ostream &ofp) const;

	/*! Import symbol table from .EXP.
	    .EXP executable may or may not have a symbol table.
	    MP format .EXP executable does not have a symbol table.
	*/
	bool ImportEXPSymbolTable(std::string fName);

	/*! Save to the stored file name.
	*/
	bool AutoSave(void) const;

	const i486Symbol *Find(unsigned int SEG,unsigned int OFFSET) const;
	const i486Symbol *Find(i486DXCommon::FarPointer ptr) const;
	const i486Symbol *FindFromOffset(uint32_t OFFSET) const;
	i486Symbol *Update(i486DXCommon::FarPointer ptr,const std::string &label);
	i486Symbol *SetComment(i486DXCommon::FarPointer ptr,const std::string &inLineComment);
	i486Symbol *SetImportedLabel(i486DXCommon::FarPointer ptr,const std::string &label);
	i486Symbol *SetImmIsSymbol(i486DXCommon::FarPointer ptr);
	i486Symbol *SetImmIsASCII(i486DXCommon::FarPointer ptr);
	i486Symbol *SetOffsetIsSymbol(i486DXCommon::FarPointer ptr);


	/*! Disassembler should take IMM operand at the given address as an IO-port address.
	*/
	i486Symbol *SetImmIsIOPort(i486DXCommon::FarPointer ptr);

	bool Delete(i486DXCommon::FarPointer ptr);
	bool DeleteComment(i486DXCommon::FarPointer ptr);
	const std::map <i486DXCommon::FarPointer,i486Symbol> &GetTable(void) const;

	/*! Returns the number of bytes if the address is marked as byte data, otherwize returns zero.
	*/
	unsigned int GetRawDataBytes(i486DXCommon::FarPointer ptr) const;

	/*! Print if a symbol is defined for the SEG:OFFSET
	*/
	void PrintIfAny(unsigned int SEG,unsigned int OFFSET,bool returnType=false,bool label=true,bool param=true) const;

	/*! This function does exhaustive search.
	*/
	std::pair <i486DXCommon::FarPointer,i486Symbol> FindSymbolFromLabel(const std::string &label) const;

	std::vector <std::string> GetList(bool returnType=false,bool label=true,bool param=true) const;

	/*!
	*/
	void AddINTLabel(unsigned int INTNum,const std::string label);

	/*!
	*/
	void AddINTFuncLabel(unsigned int INTNum,unsigned int AHorAX,const std::string label);


	/*! It also register INT 2FH as MSCDEX.EXE
	*/
	void MakeDOSIntFuncLabel(void);


	/*!
	*/
	std::string GetINTLabel(unsigned INTNum) const;

	/*!
	*/
	std::string GetINTFuncLabel(unsigned INTNum,unsigned int AHorAX) const;


	/*!
	*/
	std::string FormatImmLabel(uint32_t cs,uint32_t eip,uint32_t imm) const;

	/*!
	*/
	std::string FormatOffsetLabel(uint32_t cs,uint32_t eip,uint32_t imm) const;
};


/* } */
#endif
