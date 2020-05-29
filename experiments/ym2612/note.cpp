// Does the formula on [2] pp.204 make sense?  It doesn't.
#include <iostream>



int main(void)
{
	for(unsigned int F_NUM=0; F_NUM<2048; F_NUM+=64)
	{
		unsigned int F10=((F_NUM>>10)&1);
		unsigned int F9= ((F_NUM>> 9)&1);
		unsigned int F8= ((F_NUM>> 8)&1);
		unsigned int F7=((F_NUM>>11)&1);
		unsigned int N3=(F10&(F9|F8|F7))|((~F10)&F9&F8&F7);
		unsigned int NOTE=(F10<<1)|N3;
		std::cout << F_NUM << "(" << (F_NUM>>9) << ")"<<  "->" << NOTE << std::endl;
	};
	return 0;
}
