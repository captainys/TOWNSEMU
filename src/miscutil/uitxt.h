#ifndef TSUGARU_UITXT_H_IS_INCLUDED
#define TSUGARU_UITXT_H_IS_INCLUDED

#include <string>
#include <map>

/*
UITXT Format

UITXT  First line.  File ID.
# Comment line
+tag Message. Tag and message must be separated by space or tab.
*/

class UiText
{
private:
	std::map <std::string,std::string> textTable;
public:
	void clear(void);
	bool Load(std::string fileName);
	std::string operator()(std::string tag,std::string dflt) const;
};

#endif
