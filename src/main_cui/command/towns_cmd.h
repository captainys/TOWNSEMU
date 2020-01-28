#ifndef TOWNS_CMD_IS_INCLUDED
#define TOWNS_CMD_IS_INCLUDED
/* { */


class TownsCommand
{
public:
	enum
	{
		CMD_NONE,

		CMD_RUN,
		CMD_RUN_TO_NEXTLINE,

		CMD_ENABLE,
		// ENABLE CMDLOG
		// DISABLE CMDLOG

		CMD_PRINT,

		CMD_BREAK_POINT,
	};

	enum
	{
		ENABLE_NONE,
		ENABLE_CMDLOG,
		ENABLE_DISASSEMBLE_EVERY_INST,
	};

	enum
	{
		PRINT_NONE,
		PRINT_CALLSTACK,
	};


	class Command
	{
	public:
		std::string cmdline;
		std::vector <std::string> argv;
		int cmd;
	};
};


/* } */
#endif
