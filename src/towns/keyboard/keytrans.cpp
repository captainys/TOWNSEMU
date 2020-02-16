#include "keyboard.h"
#include "townsdef.h"

/* static */ TownsKeyboard::TownsKeyCombination TownsKeyboard::translation_data[256]=
{
	{false,false,TOWNS_JISKEY_NULL},                            // 0x00
	{false,false,TOWNS_JISKEY_NULL},                            // 0x01
	{false,false,TOWNS_JISKEY_NULL},                            // 0x02
	{false,false,TOWNS_JISKEY_NULL},                            // 0x03
	{false,false,TOWNS_JISKEY_NULL},                            // 0x04
	{false,false,TOWNS_JISKEY_NULL},                            // 0x05
	{false,false,TOWNS_JISKEY_NULL},                            // 0x06
	{false,false,TOWNS_JISKEY_NULL},                            // 0x07
	{false,false,TOWNS_JISKEY_BACKSPACE},                       // 0x08
	{false,false,TOWNS_JISKEY_TAB},                             // 0x09
	{false,false,TOWNS_JISKEY_NULL},                            // 0x0A
	{false,false,TOWNS_JISKEY_NULL},                            // 0x0B
	{false,false,TOWNS_JISKEY_NULL},                            // 0x0C
	{false,false,TOWNS_JISKEY_RETURN},                          // 0x0D
	{false,false,TOWNS_JISKEY_NULL},                            // 0x0E
	{false,false,TOWNS_JISKEY_NULL},                            // 0x0F

	{false,false,TOWNS_JISKEY_NULL},                            // 0x10
	{false,false,TOWNS_JISKEY_NULL},                            // 0x11
	{false,false,TOWNS_JISKEY_NULL},                            // 0x12
	{false,false,TOWNS_JISKEY_NULL},                            // 0x13
	{false,false,TOWNS_JISKEY_NULL},                            // 0x14
	{false,false,TOWNS_JISKEY_NULL},                            // 0x15
	{false,false,TOWNS_JISKEY_NULL},                            // 0x16
	{false,false,TOWNS_JISKEY_NULL},                            // 0x17
	{false,false,TOWNS_JISKEY_NULL},                            // 0x18
	{false,false,TOWNS_JISKEY_NULL},                            // 0x19
	{false,false,TOWNS_JISKEY_NULL},                            // 0x1A
	{false,false,TOWNS_JISKEY_NULL},                            // 0x1B
	{false,false,TOWNS_JISKEY_NULL},                            // 0x1C
	{false,false,TOWNS_JISKEY_NULL},                            // 0x1D
	{false,false,TOWNS_JISKEY_NULL},                            // 0x1E
	{false,false,TOWNS_JISKEY_NULL},                            // 0x1F

	{false,false,TOWNS_JISKEY_SPACE},                            // 0x20
	{true ,false,TOWNS_JISKEY_1},                            // 0x21 !
	{true ,false,TOWNS_JISKEY_2},                          // 0x22 "
	{true ,false,TOWNS_JISKEY_3},                            // 0x23 #
	{true ,false,TOWNS_JISKEY_4},                            // 0x24 $
	{true ,false,TOWNS_JISKEY_5},                            // 0x25 %
	{true ,false,TOWNS_JISKEY_6},                            // 0x26 &
	{true ,false,TOWNS_JISKEY_7},                            // 0x27 ' -> a?
	{true ,false,TOWNS_JISKEY_8},                            // 0x28 (
	{true ,false,TOWNS_JISKEY_9},                            // 0x29 )
	{true ,false,TOWNS_JISKEY_COLON},                           // 0x2A *
	{true ,false,TOWNS_JISKEY_SEMICOLON},                       // 0x2B +
	{false,false,TOWNS_JISKEY_COMMA},                            // 0x2C ,
	{false,false,TOWNS_JISKEY_MINUS},                            // 0x2D -
	{false,false,TOWNS_JISKEY_DOT},                            // 0x2E .
	{false,false,TOWNS_JISKEY_SLASH},                            // 0x2F /

	{false,false,TOWNS_JISKEY_0},                            // 0x30
	{false,false,TOWNS_JISKEY_1},                            // 0x31
	{false,false,TOWNS_JISKEY_2},                            // 0x32
	{false,false,TOWNS_JISKEY_3},                            // 0x33
	{false,false,TOWNS_JISKEY_4},                            // 0x34
	{false,false,TOWNS_JISKEY_5},                            // 0x35
	{false,false,TOWNS_JISKEY_6},                            // 0x36
	{false,false,TOWNS_JISKEY_7},                            // 0x37
	{false,false,TOWNS_JISKEY_8},                            // 0x38
	{false,false,TOWNS_JISKEY_9},                            // 0x39
	{false,false,TOWNS_JISKEY_COLON},                            // 0x3A :
	{false,false,TOWNS_JISKEY_SEMICOLON},                            // 0x3B ;
	{true ,false,TOWNS_JISKEY_COMMA},                            // 0x3C <
	{true ,false,TOWNS_JISKEY_MINUS},                            // 0x3D =
	{true ,false,TOWNS_JISKEY_DOT},                            // 0x3E >
	{true ,false,TOWNS_JISKEY_SLASH},                            // 0x3F ?

	{false,false,TOWNS_JISKEY_AT},                            // 0x40 @
	{true ,false,TOWNS_JISKEY_A},                            // 0x41 'A'
	{true ,false,TOWNS_JISKEY_B},                            // 0x42
	{true ,false,TOWNS_JISKEY_C},                            // 0x43
	{true ,false,TOWNS_JISKEY_D},                            // 0x44
	{true ,false,TOWNS_JISKEY_E},                            // 0x45
	{true ,false,TOWNS_JISKEY_F},                            // 0x46
	{true ,false,TOWNS_JISKEY_G},                            // 0x47
	{true ,false,TOWNS_JISKEY_H},                            // 0x48
	{true ,false,TOWNS_JISKEY_I},                            // 0x49
	{true ,false,TOWNS_JISKEY_J},                            // 0x4A
	{true ,false,TOWNS_JISKEY_K},                            // 0x4B
	{true ,false,TOWNS_JISKEY_L},                            // 0x4C
	{true ,false,TOWNS_JISKEY_M},                            // 0x4D
	{true ,false,TOWNS_JISKEY_N},                            // 0x4E
	{true ,false,TOWNS_JISKEY_O},                            // 0x4F

	{true ,false,TOWNS_JISKEY_P},                            // 0x50
	{true ,false,TOWNS_JISKEY_Q},                            // 0x51
	{true ,false,TOWNS_JISKEY_R},                            // 0x52
	{true ,false,TOWNS_JISKEY_S},                            // 0x53
	{true ,false,TOWNS_JISKEY_T},                            // 0x54
	{true ,false,TOWNS_JISKEY_U},                            // 0x55
	{true ,false,TOWNS_JISKEY_V},                            // 0x56
	{true ,false,TOWNS_JISKEY_W},                            // 0x57
	{true ,false,TOWNS_JISKEY_X},                            // 0x58
	{true ,false,TOWNS_JISKEY_Y},                            // 0x59
	{false,false,TOWNS_JISKEY_Z},                            // 0x5A
	{false,false,TOWNS_JISKEY_LEFT_SQ_BRACKET},                            // 0x5B [
	{false,false,TOWNS_JISKEY_BACKSLASH},                            // 0x5C (Back Slash)
	{false,false,TOWNS_JISKEY_RIGHT_SQ_BRACKET},                            // 0x5D ]
	{false,false,TOWNS_JISKEY_HAT},                            // 0x5E ^
	{true ,false,TOWNS_JISKEY_DOUBLEQUOTE},                            // 0x5F _

	{true ,false,TOWNS_JISKEY_AT},                            // 0x60 `
	{false,false,TOWNS_JISKEY_A},                            // 0x61
	{false,false,TOWNS_JISKEY_B},                            // 0x62
	{false,false,TOWNS_JISKEY_C},                            // 0x63
	{false,false,TOWNS_JISKEY_D},                            // 0x64
	{false,false,TOWNS_JISKEY_E},                            // 0x65
	{false,false,TOWNS_JISKEY_F},                            // 0x66
	{false,false,TOWNS_JISKEY_G},                            // 0x67
	{false,false,TOWNS_JISKEY_H},                            // 0x68
	{false,false,TOWNS_JISKEY_I},                            // 0x69
	{false,false,TOWNS_JISKEY_J},                            // 0x6A
	{false,false,TOWNS_JISKEY_K},                            // 0x6B
	{false,false,TOWNS_JISKEY_L},                            // 0x6C
	{false,false,TOWNS_JISKEY_M},                            // 0x6D
	{false,false,TOWNS_JISKEY_N},                            // 0x6E
	{false,false,TOWNS_JISKEY_O},                            // 0x6F
                              
	{false,false,TOWNS_JISKEY_P},                            // 0x70
	{false,false,TOWNS_JISKEY_Q},                            // 0x71
	{false,false,TOWNS_JISKEY_R},                            // 0x72
	{false,false,TOWNS_JISKEY_S},                            // 0x73
	{false,false,TOWNS_JISKEY_T},                            // 0x74
	{false,false,TOWNS_JISKEY_U},                            // 0x75
	{false,false,TOWNS_JISKEY_V},                            // 0x76
	{false,false,TOWNS_JISKEY_W},                            // 0x77
	{false,false,TOWNS_JISKEY_X},                            // 0x78
	{false,false,TOWNS_JISKEY_Y},                            // 0x79
	{false,false,TOWNS_JISKEY_Z},                            // 0x7A
	{true ,false,TOWNS_JISKEY_LEFT_SQ_BRACKET},                            // 0x7B {
	{true ,false,TOWNS_JISKEY_BACKSLASH},                            // 0x7C |
	{true ,false,TOWNS_JISKEY_RIGHT_SQ_BRACKET},                            // 0x7D }
	{true ,false,TOWNS_JISKEY_HAT},                            // 0x7E `
	{false,false,TOWNS_JISKEY_NULL},                            // 0x7F

	{false,false,TOWNS_JISKEY_NULL},                            // 0x80
	{false,false,TOWNS_JISKEY_NULL},                            // 0x81
	{false,false,TOWNS_JISKEY_NULL},                            // 0x82
	{false,false,TOWNS_JISKEY_NULL},                            // 0x83
	{false,false,TOWNS_JISKEY_NULL},                            // 0x84
	{false,false,TOWNS_JISKEY_NULL},                            // 0x85
	{false,false,TOWNS_JISKEY_NULL},                            // 0x86
	{false,false,TOWNS_JISKEY_NULL},                            // 0x87
	{false,false,TOWNS_JISKEY_NULL},                            // 0x88
	{false,false,TOWNS_JISKEY_NULL},                            // 0x89
	{false,false,TOWNS_JISKEY_NULL},                            // 0x8A
	{false,false,TOWNS_JISKEY_NULL},                            // 0x8B
	{false,false,TOWNS_JISKEY_NULL},                            // 0x8C
	{false,false,TOWNS_JISKEY_NULL},                            // 0x8D
	{false,false,TOWNS_JISKEY_NULL},                            // 0x8E
	{false,false,TOWNS_JISKEY_NULL},                            // 0x8F

	{false,false,TOWNS_JISKEY_NULL},                            // 0x90
	{false,false,TOWNS_JISKEY_NULL},                            // 0x91
	{false,false,TOWNS_JISKEY_NULL},                            // 0x92
	{false,false,TOWNS_JISKEY_NULL},                            // 0x93
	{false,false,TOWNS_JISKEY_NULL},                            // 0x94
	{false,false,TOWNS_JISKEY_NULL},                            // 0x95
	{false,false,TOWNS_JISKEY_NULL},                            // 0x96
	{false,false,TOWNS_JISKEY_NULL},                            // 0x97
	{false,false,TOWNS_JISKEY_NULL},                            // 0x98
	{false,false,TOWNS_JISKEY_NULL},                            // 0x99
	{false,false,TOWNS_JISKEY_NULL},                            // 0x9A
	{false,false,TOWNS_JISKEY_NULL},                            // 0x9B
	{false,false,TOWNS_JISKEY_NULL},                            // 0x9C
	{false,false,TOWNS_JISKEY_NULL},                            // 0x9D
	{false,false,TOWNS_JISKEY_NULL},                            // 0x9E
	{false,false,TOWNS_JISKEY_NULL},                            // 0x9F

	{false,false,TOWNS_JISKEY_NULL},                            // 0xA0
	{false,false,TOWNS_JISKEY_NULL},                            // 0xA1
	{false,false,TOWNS_JISKEY_NULL},                            // 0xA2
	{false,false,TOWNS_JISKEY_NULL},                            // 0xA3
	{false,false,TOWNS_JISKEY_NULL},                            // 0xA4
	{false,false,TOWNS_JISKEY_NULL},                            // 0xA5
	{false,false,TOWNS_JISKEY_NULL},                            // 0xA6
	{false,false,TOWNS_JISKEY_NULL},                            // 0xA7
	{false,false,TOWNS_JISKEY_NULL},                            // 0xA8
	{false,false,TOWNS_JISKEY_NULL},                            // 0xA9
	{false,false,TOWNS_JISKEY_NULL},                            // 0xAA
	{false,false,TOWNS_JISKEY_NULL},                            // 0xAB
	{false,false,TOWNS_JISKEY_NULL},                            // 0xAC
	{false,false,TOWNS_JISKEY_NULL},                            // 0xAD
	{false,false,TOWNS_JISKEY_NULL},                            // 0xAE
	{false,false,TOWNS_JISKEY_NULL},                            // 0xAF

	{false,false,TOWNS_JISKEY_NULL},                            // 0xB0
	{false,false,TOWNS_JISKEY_NULL},                            // 0xB1
	{false,false,TOWNS_JISKEY_NULL},                            // 0xB2
	{false,false,TOWNS_JISKEY_NULL},                            // 0xB3
	{false,false,TOWNS_JISKEY_NULL},                            // 0xB4
	{false,false,TOWNS_JISKEY_NULL},                            // 0xB5
	{false,false,TOWNS_JISKEY_NULL},                            // 0xB6
	{false,false,TOWNS_JISKEY_NULL},                            // 0xB7
	{false,false,TOWNS_JISKEY_NULL},                            // 0xB8
	{false,false,TOWNS_JISKEY_NULL},                            // 0xB9
	{false,false,TOWNS_JISKEY_NULL},                            // 0xBA
	{false,false,TOWNS_JISKEY_NULL},                            // 0xBB
	{false,false,TOWNS_JISKEY_NULL},                            // 0xBC
	{false,false,TOWNS_JISKEY_NULL},                            // 0xBD
	{false,false,TOWNS_JISKEY_NULL},                            // 0xBE
	{false,false,TOWNS_JISKEY_NULL},                            // 0xBF

	{false,false,TOWNS_JISKEY_NULL},                            // 0xC0
	{false,false,TOWNS_JISKEY_NULL},                            // 0xC1
	{false,false,TOWNS_JISKEY_NULL},                            // 0xC2
	{false,false,TOWNS_JISKEY_NULL},                            // 0xC3
	{false,false,TOWNS_JISKEY_NULL},                            // 0xC4
	{false,false,TOWNS_JISKEY_NULL},                            // 0xC5
	{false,false,TOWNS_JISKEY_NULL},                            // 0xC6
	{false,false,TOWNS_JISKEY_NULL},                            // 0xC7
	{false,false,TOWNS_JISKEY_NULL},                            // 0xC8
	{false,false,TOWNS_JISKEY_NULL},                            // 0xC9
	{false,false,TOWNS_JISKEY_NULL},                            // 0xCA
	{false,false,TOWNS_JISKEY_NULL},                            // 0xCB
	{false,false,TOWNS_JISKEY_NULL},                            // 0xCC
	{false,false,TOWNS_JISKEY_NULL},                            // 0xCD
	{false,false,TOWNS_JISKEY_NULL},                            // 0xCE
	{false,false,TOWNS_JISKEY_NULL},                            // 0xCF

	{false,false,TOWNS_JISKEY_NULL},                            // 0xD0
	{false,false,TOWNS_JISKEY_NULL},                            // 0xD1
	{false,false,TOWNS_JISKEY_NULL},                            // 0xD2
	{false,false,TOWNS_JISKEY_NULL},                            // 0xD3
	{false,false,TOWNS_JISKEY_NULL},                            // 0xD4
	{false,false,TOWNS_JISKEY_NULL},                            // 0xD5
	{false,false,TOWNS_JISKEY_NULL},                            // 0xD6
	{false,false,TOWNS_JISKEY_NULL},                            // 0xD7
	{false,false,TOWNS_JISKEY_NULL},                            // 0xD8
	{false,false,TOWNS_JISKEY_NULL},                            // 0xD9
	{false,false,TOWNS_JISKEY_NULL},                            // 0xDA
	{false,false,TOWNS_JISKEY_NULL},                            // 0xDB
	{false,false,TOWNS_JISKEY_NULL},                            // 0xDC
	{false,false,TOWNS_JISKEY_NULL},                            // 0xDD
	{false,false,TOWNS_JISKEY_NULL},                            // 0xDE
	{false,false,TOWNS_JISKEY_NULL},                            // 0xDF

	{false,false,TOWNS_JISKEY_NULL},                            // 0xE0
	{false,false,TOWNS_JISKEY_NULL},                            // 0xE1
	{false,false,TOWNS_JISKEY_NULL},                            // 0xE2
	{false,false,TOWNS_JISKEY_NULL},                            // 0xE3
	{false,false,TOWNS_JISKEY_NULL},                            // 0xE4
	{false,false,TOWNS_JISKEY_NULL},                            // 0xE5
	{false,false,TOWNS_JISKEY_NULL},                            // 0xE6
	{false,false,TOWNS_JISKEY_NULL},                            // 0xE7
	{false,false,TOWNS_JISKEY_NULL},                            // 0xE8
	{false,false,TOWNS_JISKEY_NULL},                            // 0xE9
	{false,false,TOWNS_JISKEY_NULL},                            // 0xEA
	{false,false,TOWNS_JISKEY_NULL},                            // 0xEB
	{false,false,TOWNS_JISKEY_NULL},                            // 0xEC
	{false,false,TOWNS_JISKEY_NULL},                            // 0xED
	{false,false,TOWNS_JISKEY_NULL},                            // 0xEE
	{false,false,TOWNS_JISKEY_NULL},                            // 0xEF

	{false,false,TOWNS_JISKEY_NULL},                            // 0xF0
	{false,false,TOWNS_JISKEY_NULL},                            // 0xF1
	{false,false,TOWNS_JISKEY_NULL},                            // 0xF2
	{false,false,TOWNS_JISKEY_NULL},                            // 0xF3
	{false,false,TOWNS_JISKEY_NULL},                            // 0xF4
	{false,false,TOWNS_JISKEY_NULL},                            // 0xF5
	{false,false,TOWNS_JISKEY_NULL},                            // 0xF6
	{false,false,TOWNS_JISKEY_NULL},                            // 0xF7
	{false,false,TOWNS_JISKEY_NULL},                            // 0xF8
	{false,false,TOWNS_JISKEY_NULL},                            // 0xF9
	{false,false,TOWNS_JISKEY_NULL},                            // 0xFA
	{false,false,TOWNS_JISKEY_NULL},                            // 0xFB
	{false,false,TOWNS_JISKEY_NULL},                            // 0xFC
	{false,false,TOWNS_JISKEY_NULL},                            // 0xFD
	{false,false,TOWNS_JISKEY_NULL},                            // 0xFE
	{false,false,TOWNS_JISKEY_NULL},                            // 0xFF
};


/* static */ unsigned int TownsKeyboard::TranslateChar(unsigned char keyCodeBuf[2],unsigned char c)
{
	keyCodeBuf[1]=translation_data[c].keyCode;
	if(TOWNS_JISKEY_NULL!=keyCodeBuf[1])
	{
		keyCodeBuf[0]=TOWNS_KEYFLAG_THUMBSHIFT_TYPE_JIS|TOWNS_KEYFLAG_THUMBSHIFT_TYPE_FIRSTBYTE;
		keyCodeBuf[0]|=(true==translation_data[c].shift ? TOWNS_KEYFLAG_THUMBSHIFT_SHIFT : 0);
		keyCodeBuf[0]|=(true==translation_data[c].ctrl ? TOWNS_KEYFLAG_THUMBSHIFT_CTRL : 0);
		return 2;
	}
	return 0;
}
