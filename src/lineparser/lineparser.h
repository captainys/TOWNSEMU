#ifndef LINEPARSER_IS_INCLUDED
#define LINEPARSER_IS_INCLUDED
/* { */

#include <string>

class LineParser
{
public:
	static const char *const allOp[];
	static const char *const highPriorityOp[];
	static const char *const lowPriorityOp[];
	static const char *const lowestPriorityOp[];
	static const char *const unaryOp[];

	class Term
	{
	public:
		std::string label;
		bool custom=false;
		Term *next=nullptr,*prev=nullptr,*child=nullptr;
	};

	mutable bool error=false;
	mutable std::string errorMessage;

	Term *tree=nullptr;

	LineParser();
	~LineParser();
	void CleanUp(void);
private:
	void Delete(Term *tree);
public:

	bool Analyze(const std::string &str);
	bool Analyze(const char *str);

protected:
	void MakeLinear(const char *str);
	bool IsBlank(char c) const;
	void MakeLinear_AddToTail(Term * & tail, Term *newTerm);
	std::string MakeLinear_GetOperator(const char *str) const;
	virtual std::string MatchCustomKeyword(std::string str) const; // Used in MakeLinear
	virtual bool IsCustomUnaryOperator(std::string str) const;

public:
	long long int Evaluate(void) const;
protected:
	virtual long long int Evaluate(const Term *t) const;
	virtual long long int EvaluateTerm(const Term *t) const;
	virtual long long int EvaluateRawNumber(const std::string &str) const;

	virtual long long int EvaluateCustomTerm(const Term *t) const;
	virtual long long int EvaluateCustomUnaryOperator(const Term *t,long long int operand) const;
	virtual long long int EvaluateCustomBinaryOperator(const Term *t,long long int left,long long int right) const;

public:
	void Print(void) const;
private:
	void Print(const Term *t,int indent) const;

protected:
	bool ClampParenthesis(Term * &current,char left);
	void CleanParenthesis(Term *current);

protected:
	void GroupByOperator(Term *current,const char *const op[]);
	bool IsOneOf(const std::string &str,const char *const op[]) const;

	void GroupByUnaryOperator(Term *current,const char *const op[]);
};


/* } */
#endif
