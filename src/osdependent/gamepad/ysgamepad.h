#ifndef YSGAMEPAD_IS_INCLUDED
#define YSGAMEPAD_IS_INCLUDED
/* { */

#ifdef __cplusplus
extern "C" {
#endif

#define YSGAMEPAD_MAX_NUM_BUTTONS 16
#define YSGAMEPAD_MAX_NUM_DIRS 8
#define YSGAMEPAD_MAX_NUM_AXES 8

struct YsGamePadDirectionButton
{
	unsigned char upDownLeftRight[4];
};

struct YsGamePadReading
{
	unsigned char buttons[YSGAMEPAD_MAX_NUM_BUTTONS];
	struct YsGamePadDirectionButton dirs[YSGAMEPAD_MAX_NUM_DIRS];
	float axes[YSGAMEPAD_MAX_NUM_AXES];
};

void YsGamePadInitialize(void);
void YsGamePadTerminate(void);
void YsGamePadWaitReady(void);
int YsGamePadGetNumDevices(void);
void YsGamePadRead(struct YsGamePadReading *reading,int gamePadId);
void YsGamePadClear(struct YsGamePadReading *reading);
void YsGamdPadTranslateAnalogToDigital(struct YsGamePadDirectionButton *dir,float x,float y);

#ifdef __cplusplus
}
#endif

/* } */
#endif
