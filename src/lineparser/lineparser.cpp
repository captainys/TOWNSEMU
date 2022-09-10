#include <stdlib.h>
#include <iostream>

#include "lineparser.h"

const char *const LineParser::allOp[]=
{
	"+",
	"-",
	"*",
	"/",
	"%",
	"|",
	"&",
	"^",
	"~",
	"(",
	")",
	"[",
	"]",
	"{",
	"}",
	nullptr
};

const char *const LineParser::highPriorityOp[]=
{
	"*",
	"/",
	"%",
	nullptr
};

const char *const LineParser::lowPriorityOp[]=
{
	"+",
	"-",
	nullptr
};

const char *const LineParser::lowestPriorityOp[]=
{
	"|",
	"&",
	"^",
	nullptr
};

const char *const LineParser::unaryOp[]=
{
	"+",
	"-",
	"~",
	nullptr
};

LineParser::LineParser()
{
	tree=nullptr;
}
LineParser::~LineParser()
{
	CleanUp();
}
void LineParser::CleanUp(void)
{
	Delete(tree);
	tree=nullptr;
}
void LineParser::Delete(Term *tree)
{
	if(nullptr!=tree)
	{
		Delete(tree->next);
		Delete(tree->child);
	}
	delete tree;
}
bool LineParser::Analyze(const std::string &str)
{
	return Analyze(str.c_str());
}
bool LineParser::Analyze(const char *str)
{
	// Step 1 Make Linear Connection.
	MakeLinear(str);

	{
		auto ptr=tree;
		if(true!=ClampParenthesis(ptr,0))
		{
			return false;
		}
		CleanParenthesis(tree);
	}

	GroupByUnaryOperator(tree,unaryOp);
	GroupByOperator(tree,highPriorityOp);
	GroupByOperator(tree,lowPriorityOp);
	GroupByOperator(tree,lowestPriorityOp);

	return true;
}

void LineParser::MakeLinear(const char *str)
{
	enum
	{
		STATE_VOID,
		STATE_WORD,
		STATE_OPERATOR,
	};
	int state=STATE_VOID;
	std::string current;

	CleanUp();
	auto tail=tree;
	while(0!=*str)
	{
		bool custom=false; // Tentative
		std::string matchOperator=MatchCustomKeyword(str);
		if(0==matchOperator.size())
		{
			matchOperator=MakeLinear_GetOperator(str);
		}
		else
		{
			custom=true;
		}
		if(0<matchOperator.size())
		{
			if(0<current.size())
			{
				auto newTerm=new Term;
				newTerm->label=current;
				MakeLinear_AddToTail(tail,newTerm);
				current="";
			}
			auto newTerm=new Term;
			newTerm->custom=custom;
			std::swap(newTerm->label,matchOperator);
			MakeLinear_AddToTail(tail,newTerm);
			state=STATE_VOID;
			str+=newTerm->label.size();
		}
		else
		{
			if(STATE_VOID==state)
			{
				if(true!=IsBlank(*str))
				{
					current.push_back(*str);
					state=STATE_WORD;
				}
			}
			else if(STATE_WORD==state)
			{
				if(true==IsBlank(*str))
				{
					auto newTerm=new Term;
					newTerm->label=current;
					MakeLinear_AddToTail(tail,newTerm);
					current="";
					state=STATE_VOID;
				}
				else
				{
					current.push_back(*str);
				}
			}
			++str;
		}
	}

	if(0<current.size())
	{
		auto newTerm=new Term;
		newTerm->label=current;
		MakeLinear_AddToTail(tail,newTerm);
		current="";
	}
}

void LineParser::MakeLinear_AddToTail(Term * & tail, Term *newTerm)
{
	if(nullptr==tail)
	{
		tree=newTerm;
		tail=newTerm;
	}
	else
	{
		tail->next=newTerm;
		newTerm->prev=tail;
		tail=newTerm;
	}
}

bool LineParser::IsBlank(char c) const
{
	if(' '==c || '\t'==c)
	{
		return true;
	}
	return false;
}
std::string LineParser::MakeLinear_GetOperator(const char *str) const
{
	for(int i=0; nullptr!=allOp[i]; ++i)
	{
		bool match=true;
		for(int j=0; allOp[i][j]!=0; ++j)
		{
			if(str[j]!=allOp[i][j])
			{
				match=false;
				break;
			}
		}
		if(true==match)
		{
			return allOp[i];
		}
	}
	return "";
}
/* virtual */ std::string LineParser::MatchCustomKeyword(std::string str) const
{
	return "";
}

/* virtual */ bool LineParser::IsCustomUnaryOperator(std::string str) const
{
	return false;
}

long long int LineParser::Evaluate(void) const
{
	error=false;
	errorMessage="";
	return Evaluate(tree);
}
long long int LineParser::Evaluate(const Term *t) const
{
	if(nullptr==t)
	{
		return 0;
	}
	else if(nullptr==t->next)
	{
		if(true==t->custom)
		{
			return EvaluateCustomTerm(t);
		}
		return EvaluateTerm(t);
	}
	else if(nullptr==t->next->next) // Unary operator
	{
		if(true==t->custom)
		{
			return EvaluateCustomUnaryOperator(t,EvaluateTerm(t->next));
		}
		else if("-"==t->label)
		{
			return -EvaluateTerm(t->next);
		}
		else if("+"==t->label)
		{
			return EvaluateTerm(t->next);
		}
		else if("~"==t->label)
		{
			return ~EvaluateTerm(t->next);
		}
		else
		{
			error=true;
			errorMessage="Unsupported unary operator: "+t->next->label;
			std::cout << errorMessage << std::endl;
			return 0;
		}
	}
	else if(nullptr==t->next->next->next) // Binary operator
	{
		auto left=EvaluateTerm(t);
		auto right=EvaluateTerm(t->next->next);
		if(true==t->custom)
		{
			return EvaluateCustomBinaryOperator(t,left,right);
		}
		else if("*"==t->next->label)
		{
			return left*right;
		}
		else if("+"==t->next->label)
		{
			return left+right;
		}
		else if("-"==t->next->label)
		{
			return left-right;
		}
		else if("|"==t->next->label)
		{
			return left|right;
		}
		else if("&"==t->next->label)
		{
			return left&right;
		}
		else if("^"==t->next->label)
		{
			return left^right;
		}
		else if("/"==t->next->label)
		{
			if(0==right)
			{
				error=true;
				errorMessage="Division by zero.";
				std::cout << "Division by zero." << std::endl;
				return 0;
			}
			else
			{
				return left/right;
			}
		}
		else if("%"==t->next->label)
		{
			if(0==right)
			{
				error=true;
				errorMessage="Division by zero.";
				std::cout << "Division by zero." << std::endl;
				return 0;
			}
			else
			{
				return left%right;
			}
		}
		else
		{
			error=true;
			errorMessage="Unsupported binary operator: "+t->next->label;
			std::cout << errorMessage << std::endl;
			return 0;
		}
	}
	else
	{
		error=true;
		errorMessage="Decomposition failed.";
		std::cout << "Decomposition failed." << std::endl;
		return 0;
	}
}
long long int LineParser::EvaluateTerm(const Term *t) const
{
	if(nullptr!=t->child)
	{
		return Evaluate(t->child);
	}
	else
	{
		return EvaluateRawNumber(t->label);
	}
}
/* virtual */ long long int LineParser::EvaluateRawNumber(const std::string &str) const
{
	return atoll(str.c_str());
}
/* virtual */ long long int LineParser::EvaluateCustomTerm(const Term *t) const
{
	return 0;
}
/* virtual */ long long int LineParser::EvaluateCustomUnaryOperator(const Term *t,long long int operand) const
{
	return 0;
}
/* virtual */ long long int LineParser::EvaluateCustomBinaryOperator(const Term *t,long long int left,long long int right) const
{
	return 0;
}

void LineParser::Print(void) const
{
	Print(tree,0);
}
void LineParser::Print(const Term *t,int indent) const
{
	while(nullptr!=t)
	{
		for(int i=0; i<indent; ++i)
		{
			std::cout << " ";
		}
		std::cout << t->label;
		if(true==t->custom)
		{
			std::cout << "(custom)";
		}
		std::cout << std::endl;
		Print(t->child,indent+2);
		t=t->next;
	}
}



bool LineParser::ClampParenthesis(Term * &current,char left)
{
	while(nullptr!=current)
	{
		if("("==current->label ||
		   "["==current->label ||
		   "{"==current->label)
		{
			auto subGroup=current->next;
			if(true!=ClampParenthesis(subGroup,current->label[0]))
			{
				return false;
			}
			// if no error, subGroup->label must be the closing.
			if(subGroup->prev!=current)
			{
				subGroup->prev->next=nullptr; // Make it the last in the group.
				current->child=current->next;
				current->next->prev=nullptr;
				current->next=subGroup;
			}
			current=subGroup;
		}
		else if(")"==current->label ||
		        "]"==current->label ||
		        "}"==current->label)
		{
			if((left=='(' && current->label[0]!=')') ||
			   (left=='[' && current->label[0]!=']') ||
			   (left=='{' && current->label[0]!='}'))
			{
				std::cout << "Opening and closing mismatch!" << std::endl;
				return false;
			}
			return true;
		}
		current=current->next;
	}
	if(0!=left)
	{
		std::string op;
		op.push_back(left);
		std::cout << "Unclosed: " << op << std::endl;
		return false;
	}
	return true;
}

void LineParser::CleanParenthesis(Term *current)
{
	while(nullptr!=current)
	{
		CleanParenthesis(current->child);

		if("("==current->label ||
		   "["==current->label ||
		   "{"==current->label)
		{
			if(nullptr!=current->next)
			{
				current->label+=current->next->label;

				auto toDel=current->next;
				current->next=toDel->next;
				if(nullptr!=toDel->next)
				{
					toDel->next->prev=current;
				}

				toDel->prev=nullptr;
				toDel->next=nullptr;
				Delete(toDel);
			}
		}
		current=current->next;
	}
}

void LineParser::GroupByOperator(Term *current,const char *const op[])
{
	for(auto term=current; nullptr!=term; term=term->next)
	{
		GroupByOperator(term->child,op);
	}

	// From:
	//   word1 op word2 ....
	//   child
	// To:
	//   () ....
	//   word1 op word2
	//   child
	while(nullptr!=current && nullptr!=current->next && nullptr!=current->next->next) // Need three trems minimum.
	{
		auto opPtr=current->next;
		auto word2=current->next->next;
		auto afterWord2=current->next->next->next;

		if(true==IsOneOf(current->next->label,op))
		{
			auto newTerm=new Term;
			newTerm->label=current->label;
			newTerm->child=current->child;

			current->label="()";
			current->child=newTerm;

			current->next=afterWord2;
			if(nullptr!=afterWord2)
			{
				afterWord2->prev=current;
			}
			word2->next=nullptr;

			current->child->next=opPtr;
			opPtr->prev=current->child;
		}
		else
		{
			current=current->next;
		}
	}
}

bool LineParser::IsOneOf(const std::string &str,const char *const op[]) const
{
	for(int i=0; nullptr!=op[i]; ++i)
	{
		if(str==op[i])
		{
			return true;
		}
	}
	return false;
}

void LineParser::GroupByUnaryOperator(Term *current,const char *const op[])
{
	for(auto term=current; nullptr!=term; term=term->next)
	{
		GroupByUnaryOperator(term->child,op);
	}

	Term *prev=nullptr;
	for(auto term=current; nullptr!=term; term=term->next)
	{
		if((nullptr==prev || true==IsOneOf(prev->label,allOp)) &&
		   (true==IsOneOf(term->label,op) || true==IsCustomUnaryOperator(term->label)) &&
		   nullptr!=term->next)
		{
			// From:
			//   op word ....
			//   child
			// To:
			//   () ....
			//   op word
			//   child

			auto opPtr=term;
			auto wordPtr=term->next;
			auto afterWord=wordPtr->next;

			auto newTerm=new Term;
			newTerm->child=term->child;
			std::swap(newTerm->label,term->label);
			std::swap(newTerm->custom,term->custom);
			term->child=newTerm;

			term->next=afterWord;
			if(nullptr!=afterWord)
			{
				afterWord->prev=term;
			}

			newTerm->next=wordPtr;
			wordPtr->prev=newTerm;

			wordPtr->next=nullptr;

			term->label="()";
		}
		prev=term;
	}
}
