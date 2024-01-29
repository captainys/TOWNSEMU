/* LICENSE>>
Copyright 2020 Soji Yamakawa (CaptainYS, http://www.ysflight.com)

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<< LICENSE */
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
	immIsIOAddr=false;
	immIsSymbol=false;
	immIsASCII=false;
	offsetIsSymbol=false;
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

	if(0<imported.size())
	{
		str=imported;
		str.push_back(' ');
	}

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
	i486DXCommon::FarPointer curPtr;
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
	// M 0/1 immIsIOAddr flag
	// B 0/1 immIsSymbol flag
	// O 0/1 offsetIsSymbol flag
	// X label Imported from .EXP symbol table.
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
				case 'm':
				case 'M':
					curSymbol.immIsIOAddr=(0!=cpputil::Atoi(str.c_str()+2));
					break;
				case 'b':
				case 'B':
					curSymbol.immIsSymbol=(0!=cpputil::Atoi(str.c_str()+2));
					break;
				case 'o':
				case 'O':
					curSymbol.offsetIsSymbol=(0!=cpputil::Atoi(str.c_str()+2));
					break;
				case 'x':
				case 'X':
					curSymbol.imported=(str.c_str()+2);
					break;
				case 'a':
				case 'A':
					curSymbol.immIsASCII=(0!=cpputil::Atoi(str.c_str()+2));
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
			ofp << "M " << (sym.immIsIOAddr ? "1" : "0") << std::endl;
			ofp << "B " << (sym.immIsSymbol ? "1" : "0") << std::endl;
			ofp << "A " << (sym.immIsASCII ? "1" : "0") << std::endl;
			ofp << "O " << (sym.offsetIsSymbol ? "1" : "0") << std::endl;
			ofp << "X " << sym.imported  << std::endl;
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
	i486DXCommon::FarPointer ptr;
	ptr.SEG=SEG;
	ptr.OFFSET=OFFSET;
	return Find(ptr);
}
const i486Symbol *i486SymbolTable::Find(i486DXCommon::FarPointer ptr) const
{
	auto iter=symTable.find(ptr);
	if(symTable.end()!=iter)
	{
		return &iter->second;
	}
	return nullptr;
}
const i486Symbol *i486SymbolTable::FindFromOffset(uint32_t OFFSET) const
{
	for(auto &addrAndSym : symTable)
	{
		if(addrAndSym.first.OFFSET==OFFSET)
		{
			return &addrAndSym.second;
		}
	}
	return nullptr;
}
std::pair <i486DXCommon::FarPointer,i486Symbol> i486SymbolTable::FindSymbolFromLabel(const std::string &label) const
{
	for(auto &addrAndSym : symTable)
	{
		if(addrAndSym.second.label==label || addrAndSym.second.imported==label)
		{
			return addrAndSym;
		}
	}
	std::pair <i486DXCommon::FarPointer,i486Symbol> empty;
	return empty;
}
i486Symbol *i486SymbolTable::Update(i486DXCommon::FarPointer ptr,const std::string &label)
{
	auto &symbol=symTable[ptr];
	symbol.label=label;
	return &symbol;
}
i486Symbol *i486SymbolTable::SetComment(i486DXCommon::FarPointer ptr,const std::string &inLineComment)
{
	auto &symbol=symTable[ptr];
	symbol.inLineComment=inLineComment;
	return &symbol;
}
i486Symbol *i486SymbolTable::SetImmIsIOPort(i486DXCommon::FarPointer ptr)
{
	auto &symbol=symTable[ptr];
	symbol.immIsIOAddr=true;
	return &symbol;
}
i486Symbol *i486SymbolTable::SetImmIsASCII(i486DXCommon::FarPointer ptr)
{
	auto &symbol=symTable[ptr];
	symbol.immIsASCII=true;
	return &symbol;
}
i486Symbol *i486SymbolTable::SetImportedLabel(i486DXCommon::FarPointer ptr,const std::string &label)
{
	bool makeItProcedure=false; // It can be a data, it can be a procedure, but temporarily make it a procedure, if not in the database yet.
	auto found=symTable.find(ptr);
	if(found==symTable.end())
	{
		makeItProcedure=true;
	}
	auto &symbol=symTable[ptr];
	symbol.imported=label;
	if(true==makeItProcedure)
	{
		symbol.symType=i486Symbol::SYM_PROCEDURE;
	}
	return &symbol;
}
i486Symbol *i486SymbolTable::SetImmIsSymbol(i486DXCommon::FarPointer ptr)
{
	auto &symbol=symTable[ptr];
	symbol.immIsSymbol=true;
	return &symbol;
}
i486Symbol *i486SymbolTable::SetOffsetIsSymbol(i486DXCommon::FarPointer ptr)
{
	auto &symbol=symTable[ptr];
	symbol.offsetIsSymbol=true;
	return &symbol;
}
bool i486SymbolTable::Delete(i486DXCommon::FarPointer ptr)
{
	auto iter=symTable.find(ptr);
	if(symTable.end()!=iter)
	{
		symTable.erase(iter);
		return true;
	}
	return false;
}
bool i486SymbolTable::DeleteComment(i486DXCommon::FarPointer ptr)
{
	auto iter=symTable.find(ptr);
	if(symTable.end()!=iter)
	{
		iter->second.inLineComment="";
		return true;
	}
	return false;
}
const std::map <i486DXCommon::FarPointer,i486Symbol> &i486SymbolTable::GetTable(void) const
{
	return symTable;
}

unsigned int i486SymbolTable::GetRawDataBytes(i486DXCommon::FarPointer ptr) const
{
	auto *sym=Find(ptr);
	if(nullptr!=sym &&
	   (i486Symbol::SYM_RAW_DATA==sym->symType ||
	    i486Symbol::SYM_TABLE_WORD==sym->symType ||
	    i486Symbol::SYM_TABLE_DWORD==sym->symType ||
	    i486Symbol::SYM_TABLE_FWORD16==sym->symType ||
	    i486Symbol::SYM_TABLE_FWORD32==sym->symType))
	{
		return sym->rawDataBytes;
	}
	return 0;
}

void i486SymbolTable::PrintIfAny(unsigned int SEG,unsigned int OFFSET,bool returnType,bool label,bool param) const
{
	auto str=GetFormattedSymbol(SEG,OFFSET,returnType,label,param);
	if(""!=str)
	{
		std::cout << str << std::endl;
	}
}

std::string i486SymbolTable::GetFormattedSymbol(unsigned int SEG,unsigned int OFFSET,bool returnType,bool label,bool param) const
{
	i486DXCommon::FarPointer ptr;
	ptr.SEG=SEG;
	ptr.OFFSET=OFFSET;
	auto *sym=Find(ptr);
	if(nullptr!=sym && (0<sym->label.size() || 0<sym->imported.size()))
	{
		return sym->Format(returnType,label,param);
	}
	return "";
}

std::vector <std::string> i486SymbolTable::GetList(bool returnType,bool label,bool param) const
{
	std::vector <std::string> text;
	for(const auto &addrAndSym : symTable)
	{
		auto &addr=addrAndSym.first;
		auto &sym=addrAndSym.second;

		text.push_back("");
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

void i486SymbolTable::AddINTLabel(unsigned int INTNum,const std::string label)
{
	INTLabel[INTNum].label=label;
}
void i486SymbolTable::AddINTFuncLabel(unsigned int INTNum,unsigned int AHorAX,const std::string label)
{
	if(INTNum<256)
	{
		INTFunc[INTNum][AHorAX].label=label;
	}
}
void i486SymbolTable::AddWindowsAPIEntry(unsigned int vmId,unsigned int SEG,unsigned int OFFSET)
{
	i486DXCommon::FarPointer cseip;
	cseip.SEG=SEG;
	cseip.OFFSET=OFFSET;
	windowsAPIEntry[vmId]=cseip;
}
void i486SymbolTable::MakeDOSIntFuncLabel(void)
{
	AddINTLabel(0x2F,"MSCDEX");
	AddINTFuncLabel(0x2F,0x15,"(Prob)Get File Sector Pos");
	// Used in CONTROL.EXE for loading DRIVE_R.IMG
	// 5844:00000800 BF0000                    MOV     DI,0000H
	// 5844:00000803 BB0604                    MOV     BX,0406H  "\DRIVE_R.IMG"
	// 5844:00000806 B91000                    MOV     CX,0010H
	// 5844:00000809 B80F15                    MOV     AX,150FH
	// 5844:0000080C CD2F                      INT     2FH (MSCDEX)
	// 5844:0000080E 721A                      JB      0000082A
	// Return:
	//   [DI+4][DI+5][DI+6]  HSG Sector Address

	AddINTLabel(0x21,"DOS");
	AddINTFuncLabel(0x21,0x00,"Exit Program(OLD)");
	AddINTFuncLabel(0x21,0x01,"GETC with Echo -> AL");
	AddINTFuncLabel(0x21,0x02,"");
	AddINTFuncLabel(0x21,0x03,"");
	AddINTFuncLabel(0x21,0x04,"");
	AddINTFuncLabel(0x21,0x05,"");
	AddINTFuncLabel(0x21,0x06,"");
	AddINTFuncLabel(0x21,0x07,"");
	AddINTFuncLabel(0x21,0x08,"");
	AddINTFuncLabel(0x21,0x09,"");
	AddINTFuncLabel(0x21,0x0A,"");
	AddINTFuncLabel(0x21,0x0B,"");
	AddINTFuncLabel(0x21,0x0C,"");
	AddINTFuncLabel(0x21,0x0D,"");
	AddINTFuncLabel(0x21,0x0E,"Set Current Drive DL=Drive (0=A:)");
	AddINTFuncLabel(0x21,0x0F,"Open File with FCB DS:DX=FCB -> AL=Status(0=ok ff=err)");

	AddINTFuncLabel(0x21,0x10,"Close File with FCB DS:DX=FCB -> AL=Status(0=ok ff=err)");
	AddINTFuncLabel(0x21,0x11,"Find First with FCB DS:DX=FCB -> AL=Status(0=ok ff=err), [DTA]");
	AddINTFuncLabel(0x21,0x12,"Find Next with FCB DS:DX=FCB -> AL=Status(0=ok ff=err), [DTA]");
	AddINTFuncLabel(0x21,0x13,"Delete File with FCB FCB DS:DX=FCB -> AL=Status(0=ok ff=err)");
	AddINTFuncLabel(0x21,0x14,"Sequential Read from FCB DS:DX=FCB AL=status [DTA]");
	AddINTFuncLabel(0x21,0x15,"Sequential Write to FCB DS:DX=FCB [DTA]");
	AddINTFuncLabel(0x21,0x16,"Create File with FCB DS:DX=FCB");
	AddINTFuncLabel(0x21,0x17,"Rename File with FCB DS:DX=FCB");
	AddINTFuncLabel(0x21,0x18,"NOP");
	AddINTFuncLabel(0x21,0x19,"Get Current Drive -> AL=Drive (0=A:)");
	AddINTFuncLabel(0x21,0x1A,"Set DTA Address");
	AddINTFuncLabel(0x21,0x1B,"Get Alloc Info CurDrv-> AL=Sect/Clust,CX=BytesPerSect,DX=#Clust,DX:BX=mediaDesc");
	AddINTFuncLabel(0x21,0x1C,"Get Alloc Info DL=drive(0=Def 1=A:) -> AL=Sect/Clust,CX=BytesPerSect,DX=#Clust,DX:BX=mediaDesc");
	AddINTFuncLabel(0x21,0x1D,"NOP");
	AddINTFuncLabel(0x21,0x1E,"NOP");
	AddINTFuncLabel(0x21,0x1F,"Get DPB CurDrv -> DS:BX=DPB");

	AddINTFuncLabel(0x21,0x20,"NOP");
	AddINTFuncLabel(0x21,0x21,"Get Random Record from FCB");
	AddINTFuncLabel(0x21,0x22,"Write Random Record to FCB");
	AddINTFuncLabel(0x21,0x23,"Get File Size for FCB");
	AddINTFuncLabel(0x21,0x24,"Set Random Record Number for FCB");
	AddINTFuncLabel(0x21,0x25,"Set INT Vector AL=INTNum DS:DX=INTHandler");
	AddINTFuncLabel(0x21,0x26,"Create New PSP DX=PSPSeg");
	AddINTFuncLabel(0x21,0x27,"Random Block Read from FCB");
	AddINTFuncLabel(0x21,0x28,"Random Block Write to FCB");
	AddINTFuncLabel(0x21,0x29,"Parse Filename to FCB DS:SI=Filename ES:DI FCB");
	AddINTFuncLabel(0x21,0x2A,"Get Date");
	AddINTFuncLabel(0x21,0x2B,"Set Date");
	AddINTFuncLabel(0x21,0x2C,"Get Time");
	AddINTFuncLabel(0x21,0x2D,"Set Time");
	AddINTFuncLabel(0x21,0x2E,"Set Verify Flag");
	AddINTFuncLabel(0x21,0x2F,"Get DTA Address");

	AddINTFuncLabel(0x21,0x30,"Get DOS Version / DOS Extender Install Check");
	AddINTFuncLabel(0x21,0x31,"Terminate and Stay Resident");
	AddINTFuncLabel(0x21,0x32,"Get DPB for Drive DL=(0:CurDrv 1=A:) -> DS:BX=DPB");
	AddINTFuncLabel(0x21,0x33,"Extended Break Check");
	AddINTFuncLabel(0x21,0x34,"Get INDOS Flag Pointer -> ES:BX=Ptr to INDOS");
	AddINTFuncLabel(0x21,0x35,"Get INT Vector");
	AddINTFuncLabel(0x21,0x36,"Get Free Disk Space");
	AddINTFuncLabel(0x21,0x37,"Specify \\DEV\\ Prefix Requirement");
	AddINTFuncLabel(0x21,0x38,"Get Country/Region Info");
	AddINTFuncLabel(0x21,0x39,"MKDIR DS:DX=Dirname");
	AddINTFuncLabel(0x21,0x3A,"RMDIR DS:DX=Dirname");
	AddINTFuncLabel(0x21,0x3B,"CHDIR DS:DX=Dirname");
	AddINTFuncLabel(0x21,0x3C,"Create File DS:DX=Filename, CX=Attrib");
	AddINTFuncLabel(0x21,0x3D,"fopen AL=mode(0:r 1:w 2:rw) DS:DX=Filename CL=attrMask");
	AddINTFuncLabel(0x21,0x3E,"fclose BX=FileHandle(JFN)");
	AddINTFuncLabel(0x21,0x3F,"fread BX=FileHandle(JFN),CX=#Bytes,DS:DX=DataBuffer");

	AddINTFuncLabel(0x21,0x40,"fwrite BX=FileHandle(JFN),CX=#Bytes,DS:DX=DataBuffer");
	AddINTFuncLabel(0x21,0x41,"Remove File AL=mode(0:r 1:w 2:rw) DS:DX=Filename CL=attrMask");
	AddINTFuncLabel(0x21,0x42,"fseek AL=(0:SET 1:CUR 2:END),BX=JFN,CX:DX=32bitOffset");
	AddINTFuncLabel(0x21,0x43,"Get File Attrib(4300h), CHMOD(4301h)");
	AddINTFuncLabel(0x21,0x44,"IOCTL");
	AddINTFuncLabel(0x21,0x45,"Duplicate File Handle");
	AddINTFuncLabel(0x21,0x46,"Force Duplicate File Handle");
	AddINTFuncLabel(0x21,0x47,"Getcwd");
	AddINTFuncLabel(0x21,0x48,"Malloc");
	AddINTFuncLabel(0x21,0x49,"Free Memory");
	AddINTFuncLabel(0x21,0x4A,"Resize Memory Block");
	AddINTFuncLabel(0x21,0x4B,"Load or Exec");
	AddINTFuncLabel(0x21,0x4C,"Exit Program AL=Returncode");
	AddINTFuncLabel(0x21,0x4D,"Get Returncode");
	AddINTFuncLabel(0x21,0x4E,"Findfirst CX=AttrMask DS:DX=Filename -> DTA FindFirst DataBlock");
	AddINTFuncLabel(0x21,0x4F,"Findnext DTA is FindFirst DataBlock");

	AddINTFuncLabel(0x21,0x50,"");
	AddINTFuncLabel(0x21,0x51,"");
	AddINTFuncLabel(0x21,0x52,"");
	AddINTFuncLabel(0x21,0x53,"");
	AddINTFuncLabel(0x21,0x54,"");
	AddINTFuncLabel(0x21,0x55,"");
	AddINTFuncLabel(0x21,0x56,"Rename File");
	AddINTFuncLabel(0x21,0x57,"Get/Set File Timestamp");
	AddINTFuncLabel(0x21,0x58,"Get/Set File Allocation Strategy");
	AddINTFuncLabel(0x21,0x59,"Get Extended Error Info");
	AddINTFuncLabel(0x21,0x5A,"Create Temporary File");
	AddINTFuncLabel(0x21,0x5B,"Create New File");
	AddINTFuncLabel(0x21,0x5C,"Record Locking");
	AddINTFuncLabel(0x21,0x5D,"Internal SERVER FUNCTION CALL");
	AddINTFuncLabel(0x21,0x5E,"");
	AddINTFuncLabel(0x21,0x5F,"");

	AddINTFuncLabel(0x21,0x60,"Truename (Get Qualified Filename)");
	AddINTFuncLabel(0x21,0x61,"");
	AddINTFuncLabel(0x21,0x62,"GetPSPAddr");
	AddINTFuncLabel(0x21,0x63,"");
	AddINTFuncLabel(0x21,0x64,"");
	AddINTFuncLabel(0x21,0x65,"");
	AddINTFuncLabel(0x21,0x66,"");
	AddINTFuncLabel(0x21,0x67,"");
	AddINTFuncLabel(0x21,0x68,"");
	AddINTFuncLabel(0x21,0x69,"");
	AddINTFuncLabel(0x21,0x6A,"");
	AddINTFuncLabel(0x21,0x6B,"");
	AddINTFuncLabel(0x21,0x6C,"");
	AddINTFuncLabel(0x21,0x6D,"");
	AddINTFuncLabel(0x21,0x6E,"");
	AddINTFuncLabel(0x21,0x6F,"");

	AddINTFuncLabel(0x21,0x70,"");
	AddINTFuncLabel(0x21,0x71,"");
	AddINTFuncLabel(0x21,0x72,"");
	AddINTFuncLabel(0x21,0x73,"");
	AddINTFuncLabel(0x21,0x74,"");
	AddINTFuncLabel(0x21,0x75,"");
	AddINTFuncLabel(0x21,0x76,"");
	AddINTFuncLabel(0x21,0x77,"");
	AddINTFuncLabel(0x21,0x78,"");
	AddINTFuncLabel(0x21,0x79,"");
	AddINTFuncLabel(0x21,0x7A,"");
	AddINTFuncLabel(0x21,0x7B,"");
	AddINTFuncLabel(0x21,0x7C,"");
	AddINTFuncLabel(0x21,0x7D,"");
	AddINTFuncLabel(0x21,0x7E,"");
	AddINTFuncLabel(0x21,0x7F,"");

	AddINTFuncLabel(0x21,0x80,"");
	AddINTFuncLabel(0x21,0x81,"");
	AddINTFuncLabel(0x21,0x82,"");
	AddINTFuncLabel(0x21,0x83,"");
	AddINTFuncLabel(0x21,0x84,"");
	AddINTFuncLabel(0x21,0x85,"");
	AddINTFuncLabel(0x21,0x86,"");
	AddINTFuncLabel(0x21,0x87,"");
	AddINTFuncLabel(0x21,0x88,"");
	AddINTFuncLabel(0x21,0x89,"");
	AddINTFuncLabel(0x21,0x8A,"");
	AddINTFuncLabel(0x21,0x8B,"");
	AddINTFuncLabel(0x21,0x8C,"");
	AddINTFuncLabel(0x21,0x8D,"");
	AddINTFuncLabel(0x21,0x8E,"");
	AddINTFuncLabel(0x21,0x8F,"");

	AddINTFuncLabel(0x21,0x90,"");
	AddINTFuncLabel(0x21,0x91,"");
	AddINTFuncLabel(0x21,0x92,"");
	AddINTFuncLabel(0x21,0x93,"");
	AddINTFuncLabel(0x21,0x94,"");
	AddINTFuncLabel(0x21,0x95,"");
	AddINTFuncLabel(0x21,0x96,"");
	AddINTFuncLabel(0x21,0x97,"");
	AddINTFuncLabel(0x21,0x98,"");
	AddINTFuncLabel(0x21,0x99,"");
	AddINTFuncLabel(0x21,0x9A,"");
	AddINTFuncLabel(0x21,0x9B,"");
	AddINTFuncLabel(0x21,0x9C,"");
	AddINTFuncLabel(0x21,0x9D,"");
	AddINTFuncLabel(0x21,0x9E,"");
	AddINTFuncLabel(0x21,0x9F,"");

	AddINTFuncLabel(0x21,0xA0,"");
	AddINTFuncLabel(0x21,0xA1,"");
	AddINTFuncLabel(0x21,0xA2,"");
	AddINTFuncLabel(0x21,0xA3,"");
	AddINTFuncLabel(0x21,0xA4,"");
	AddINTFuncLabel(0x21,0xA5,"");
	AddINTFuncLabel(0x21,0xA6,"");
	AddINTFuncLabel(0x21,0xA7,"");
	AddINTFuncLabel(0x21,0xA8,"");
	AddINTFuncLabel(0x21,0xA9,"");
	AddINTFuncLabel(0x21,0xAA,"");
	AddINTFuncLabel(0x21,0xAB,"");
	AddINTFuncLabel(0x21,0xAC,"");
	AddINTFuncLabel(0x21,0xAD,"");
	AddINTFuncLabel(0x21,0xAE,"");
	AddINTFuncLabel(0x21,0xAF,"");

	AddINTFuncLabel(0x21,0xB0,"");
	AddINTFuncLabel(0x21,0xB1,"");
	AddINTFuncLabel(0x21,0xB2,"");
	AddINTFuncLabel(0x21,0xB3,"");
	AddINTFuncLabel(0x21,0xB4,"");
	AddINTFuncLabel(0x21,0xB5,"");
	AddINTFuncLabel(0x21,0xB6,"");
	AddINTFuncLabel(0x21,0xB7,"");
	AddINTFuncLabel(0x21,0xB8,"");
	AddINTFuncLabel(0x21,0xB9,"");
	AddINTFuncLabel(0x21,0xBA,"");
	AddINTFuncLabel(0x21,0xBB,"");
	AddINTFuncLabel(0x21,0xBC,"");
	AddINTFuncLabel(0x21,0xBD,"");
	AddINTFuncLabel(0x21,0xBE,"");
	AddINTFuncLabel(0x21,0xBF,"");

	AddINTFuncLabel(0x21,0xC0,"");
	AddINTFuncLabel(0x21,0xC1,"");
	AddINTFuncLabel(0x21,0xC2,"");
	AddINTFuncLabel(0x21,0xC3,"");
	AddINTFuncLabel(0x21,0xC4,"");
	AddINTFuncLabel(0x21,0xC5,"");
	AddINTFuncLabel(0x21,0xC6,"");
	AddINTFuncLabel(0x21,0xC7,"");
	AddINTFuncLabel(0x21,0xC8,"");
	AddINTFuncLabel(0x21,0xC9,"");
	AddINTFuncLabel(0x21,0xCA,"");
	AddINTFuncLabel(0x21,0xCB,"");
	AddINTFuncLabel(0x21,0xCC,"");
	AddINTFuncLabel(0x21,0xCD,"");
	AddINTFuncLabel(0x21,0xCE,"");
	AddINTFuncLabel(0x21,0xCF,"");

	AddINTFuncLabel(0x21,0xD0,"");
	AddINTFuncLabel(0x21,0xD1,"");
	AddINTFuncLabel(0x21,0xD2,"");
	AddINTFuncLabel(0x21,0xD3,"");
	AddINTFuncLabel(0x21,0xD4,"");
	AddINTFuncLabel(0x21,0xD5,"");
	AddINTFuncLabel(0x21,0xD6,"");
	AddINTFuncLabel(0x21,0xD7,"");
	AddINTFuncLabel(0x21,0xD8,"");
	AddINTFuncLabel(0x21,0xD9,"");
	AddINTFuncLabel(0x21,0xDA,"");
	AddINTFuncLabel(0x21,0xDB,"");
	AddINTFuncLabel(0x21,0xDC,"");
	AddINTFuncLabel(0x21,0xDD,"");
	AddINTFuncLabel(0x21,0xDE,"");
	AddINTFuncLabel(0x21,0xDF,"");

	AddINTFuncLabel(0x21,0xE0,"");
	AddINTFuncLabel(0x21,0xE1,"");
	AddINTFuncLabel(0x21,0xE2,"");
	AddINTFuncLabel(0x21,0xE3,"");
	AddINTFuncLabel(0x21,0xE4,"");
	AddINTFuncLabel(0x21,0xE5,"");
	AddINTFuncLabel(0x21,0xE6,"");
	AddINTFuncLabel(0x21,0xE7,"");
	AddINTFuncLabel(0x21,0xE8,"");
	AddINTFuncLabel(0x21,0xE9,"");
	AddINTFuncLabel(0x21,0xEA,"");
	AddINTFuncLabel(0x21,0xEB,"");
	AddINTFuncLabel(0x21,0xEC,"");
	AddINTFuncLabel(0x21,0xED,"");
	AddINTFuncLabel(0x21,0xEE,"");
	AddINTFuncLabel(0x21,0xEF,"");

	AddINTFuncLabel(0x21,0xF0,"");
	AddINTFuncLabel(0x21,0xF1,"");
	AddINTFuncLabel(0x21,0xF2,"");
	AddINTFuncLabel(0x21,0xF3,"");
	AddINTFuncLabel(0x21,0xF4,"");
	AddINTFuncLabel(0x21,0xF5,"");
	AddINTFuncLabel(0x21,0xF6,"");
	AddINTFuncLabel(0x21,0xF7,"");
	AddINTFuncLabel(0x21,0xF8,"");
	AddINTFuncLabel(0x21,0xF9,"");
	AddINTFuncLabel(0x21,0xFA,"");
	AddINTFuncLabel(0x21,0xFB,"");
	AddINTFuncLabel(0x21,0xFC,"");
	AddINTFuncLabel(0x21,0xFD,"");
	AddINTFuncLabel(0x21,0xFE,"");
	AddINTFuncLabel(0x21,0xFF,"");

	// Information based on 
	AddINTFuncLabel(0x21,0x2501,"DOSEXT Reset");
	AddINTFuncLabel(0x21,0x2502,"DOSEXT Get INT Vec");
	AddINTFuncLabel(0x21,0x2503,"DOSEXT Get RINT Vec");
	AddINTFuncLabel(0x21,0x2504,"DOSEXT Set INT Vec");
	AddINTFuncLabel(0x21,0x2505,"DOSEXT Set RINT Vec");
	AddINTFuncLabel(0x21,0x2506,"DOSEXT Always use 32bit INT");
	AddINTFuncLabel(0x21,0x2507,"DOSEXT");
	AddINTFuncLabel(0x21,0x2508,"DOSEXT Get SEG Linear Base");
	AddINTFuncLabel(0x21,0x2509,"DOSEXT Linear to Phys");
	AddINTFuncLabel(0x21,0x250A,"DOSEXT Map Phys Memory");
	AddINTFuncLabel(0x21,0x250C,"DOSEXT Get Hardware INT Vec");
	AddINTFuncLabel(0x21,0x250D,"DOSEXT Get Call Buffer Info");
	AddINTFuncLabel(0x21,0x250E,"DOSEXT Call RealMode Proc");
	AddINTFuncLabel(0x21,0x250F,"DOSEXT 32bitAddr to RealAddr");
	AddINTFuncLabel(0x21,0x2510,"DOSEXT");
	AddINTFuncLabel(0x21,0x2511,"DOSEXT Isseu RealMode INT");
	AddINTFuncLabel(0x21,0x2512,"DOSEXT");
	AddINTFuncLabel(0x21,0x2513,"DOSEXT");
	AddINTFuncLabel(0x21,0x2514,"DOSEXT");
	AddINTFuncLabel(0x21,0x2515,"DOSEXT");
	AddINTFuncLabel(0x21,0x2516,"DOSEXT");
	AddINTFuncLabel(0x21,0x2517,"DOSEXT Get Call Buffer Info");
	AddINTFuncLabel(0x21,0x2518,"DOSEXT");
	AddINTFuncLabel(0x21,0x2519,"DOSEXT");
	AddINTFuncLabel(0x21,0x251A,"DOSEXT");
	AddINTFuncLabel(0x21,0x251B,"DOSEXT");
	AddINTFuncLabel(0x21,0x251C,"DOSEXT");
	AddINTFuncLabel(0x21,0x251D,"DOSEXT");
	AddINTFuncLabel(0x21,0x251E,"DOSEXT");
	AddINTFuncLabel(0x21,0x251F,"DOSEXT");
	AddINTFuncLabel(0x21,0x2520,"DOSEXT");
	AddINTFuncLabel(0x21,0x2521,"DOSEXT");
	AddINTFuncLabel(0x21,0x2522,"DOSEXT");
	AddINTFuncLabel(0x21,0x2523,"DOSEXT");
	AddINTFuncLabel(0x21,0x2524,"DOSEXT");
	AddINTFuncLabel(0x21,0x2525,"DOSEXT");
	AddINTFuncLabel(0x21,0x2526,"DOSEXT");
	AddINTFuncLabel(0x21,0x2527,"DOSEXT");
	AddINTFuncLabel(0x21,0x2528,"DOSEXT");
	AddINTFuncLabel(0x21,0x2529,"DOSEXT");
	AddINTFuncLabel(0x21,0x252A,"DOSEXT");
	AddINTFuncLabel(0x21,0x252B,"DOSEXT");
	AddINTFuncLabel(0x21,0x252C,"DOSEXT");
	AddINTFuncLabel(0x21,0x252D,"DOSEXT");
	AddINTFuncLabel(0x21,0x252F,"DOSEXT");
	AddINTFuncLabel(0x21,0x252F,"DOSEXT");
	AddINTFuncLabel(0x21,0x2530,"DOSEXT");
	AddINTFuncLabel(0x21,0x2531,"DOSEXT");
	AddINTFuncLabel(0x21,0x2532,"DOSEXT");
	AddINTFuncLabel(0x21,0x2533,"DOSEXT");
	AddINTFuncLabel(0x21,0x2534,"DOSEXT");
	AddINTFuncLabel(0x21,0x2535,"DOSEXT");
	AddINTFuncLabel(0x21,0x2536,"DOSEXT");
	AddINTFuncLabel(0x21,0x2537,"DOSEXT");
	AddINTFuncLabel(0x21,0x2538,"DOSEXT");
	AddINTFuncLabel(0x21,0x2539,"DOSEXT");
	AddINTFuncLabel(0x21,0x253A,"DOSEXT");
	AddINTFuncLabel(0x21,0x253B,"DOSEXT");
	AddINTFuncLabel(0x21,0x253C,"DOSEXT");
	AddINTFuncLabel(0x21,0x253D,"DOSEXT");
	AddINTFuncLabel(0x21,0x253E,"DOSEXT");
	AddINTFuncLabel(0x21,0x253F,"DOSEXT");
	AddINTFuncLabel(0x21,0x2540,"DOSEXT");
	AddINTFuncLabel(0x21,0x2544,"DOSEXT");
	AddINTFuncLabel(0x21,0x2545,"DOSEXT");
	AddINTFuncLabel(0x21,0x2546,"DOSEXT");
	AddINTFuncLabel(0x21,0x25C0,"DOSEXT");
	AddINTFuncLabel(0x21,0x25C1,"DOSEXT");
	AddINTFuncLabel(0x21,0x25C2,"DOSEXT");
	AddINTFuncLabel(0x21,0x25C3,"DOSEXT");
}

std::string i486SymbolTable::GetINTLabel(unsigned INTNum) const
{
	auto found=INTLabel.find(INTNum);
	if(INTLabel.end()!=found)
	{
		return found->second.label;
	}
	return "";
}
std::string i486SymbolTable::GetINTFuncLabel(unsigned INTNum,unsigned int AHorAX) const
{
	if(INTNum<256)
	{
		auto found=INTFunc[INTNum].find(AHorAX);
		if(INTFunc[INTNum].end()!=found)
		{
			return found->second.label;
		}
	}
	return "";
}
std::string i486SymbolTable::FormatImmLabel(uint32_t cs,uint32_t eip,uint32_t imm) const
{
	std::string str;
	auto found=Find(cs,eip);
	if(nullptr!=found && true==found->immIsSymbol)
	{
		auto foundLabel=FindFromOffset(imm);
		if(nullptr!=foundLabel)
		{
			if(""!=foundLabel->label)
			{
				str="("+foundLabel->label+")";
			}
			else if(""!=foundLabel->imported)
			{
				str="("+foundLabel->imported+")";
			}
		}
	}
	return str;
}

std::string i486SymbolTable::FormatOffsetLabel(uint32_t cs,uint32_t eip,uint32_t offset) const
{
	std::string str;
	auto found=Find(cs,eip);
	if(nullptr!=found && true==found->offsetIsSymbol)
	{
		auto foundLabel=FindFromOffset(offset);
		if(nullptr!=foundLabel)
		{
			if(""!=foundLabel->label)
			{
				str="("+foundLabel->label+")";
			}
			else if(""!=foundLabel->imported)
			{
				str="("+foundLabel->imported+")";
			}
		}
	}
	return str;
}
