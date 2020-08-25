#ifndef CUSTOMSTICK_IS_INCLUDED
#define CUSTOMSTICK_IS_INCLUDED
/* { */



/*! Abstraction for a fully-custom joystick.
*/
class CustomStick
{
public:
	class Assignable
	{
	public:
		bool assigned=false;
		unsigned int physicalId=0; // Physical device ID
		unsigned int number;
	};
	class Axis : public Assignable
	{
	public:
		float position=0.0;
	};
	class Button : public Assignable
	{
	public:
		bool pressed=false;
		bool prevPressed=false;
	};
	class VirtualKey : public Button
	{
	public:
		unsigned int keyCode=0;
	};

	class Assignable2D : public Assignable
	{
	public:
		unsigned int number2;
	};
	class DirectionButton : public Assignable2D
	{
	public:
		/*! If useAnalogAxes==true, X-axis is taken from Assignable::number, and Y-axis is taken from Assignable2D::number2.
		    Otherwise, direction button of Assignable::physicalId will be used as a source.
		*/
		bool useAnalogAxes=false;
		unsigned char upDownLeftRight[4]={0,0,0,0};
	};


	enum
	{
		MAX_NUM_AXES=4,
		MAX_NUM_BUTTONS=16,
		MAX_NUM_VIRTUALKEYS=16
	};
	bool enabled=false;
	Button enableSwitch;              // Like Analog/Digital switch of Cyberstick.
	Axis axes[MAX_NUM_AXES];
	Button buttons[MAX_NUM_BUTTONS];
	DirectionButton dir;
	DirectionButton pov;

	unsigned int numVirtualKeys=0;
	VirtualKey virtualKey[MAX_NUM_VIRTUALKEYS];
};


/* } */
#endif
