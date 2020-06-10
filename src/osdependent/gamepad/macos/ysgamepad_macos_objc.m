#include <stdio.h>
#include <Cocoa/Cocoa.h>
#include <GameController/GameController.h>
#include "../ysgamepad.h"
#include "ysgamepad_macos_objc.h"



static bool newGameController=false;

void YsJoyReader_MacOS_InitializeController(void)
{
    // Ref https://stackoverflow.com/questions/55226373/how-do-i-use-apples-gamecontroller-framework-from-a-macos-command-line-tool
    NSNotificationCenter * center = [NSNotificationCenter defaultCenter];
    [center 
        addObserverForName:GCControllerDidConnectNotification 
        object:nil
        queue:nil
        usingBlock:^(NSNotification * note){newGameController=true;}
     ];
}

int YsJoyReader_MacOS_NumControllers(void)
{
    NSArray *ary=[GCController controllers];
    return (int)[ary count];
}

void YsJoyReader_MacOS_ReadController(struct YsGamePadReading *reading,int controllerID)
{
    YsGamePadClear(reading);
    NSArray *ary=[GCController controllers];
    if(controllerID<[ary count])
    {
        GCController *controller=ary[controllerID];
        GCExtendedGamepad *profile=[controller extendedGamepad];
        if(nil!=profile)
        {
                reading->axes[0]=profile.leftThumbstick.xAxis.value;
                reading->axes[1]=profile.leftThumbstick.yAxis.value;
                reading->axes[2]=profile.rightThumbstick.xAxis.value;
                reading->axes[3]=profile.rightThumbstick.yAxis.value;

                reading->buttons[0]=profile.buttonA.pressed;
                reading->buttons[1]=profile.buttonB.pressed;
                reading->buttons[2]=profile.buttonX.pressed;
                reading->buttons[3]=profile.buttonY.pressed;
                reading->buttons[4]=profile.leftShoulder.pressed;
                reading->buttons[5]=profile.rightShoulder.pressed;
                reading->buttons[6]=profile.leftTrigger.pressed;
                reading->buttons[7]=profile.rightTrigger.pressed;

                reading->dirs[0].upDownLeftRight[0]=profile.dpad.up.pressed;
                reading->dirs[0].upDownLeftRight[1]=profile.dpad.down.pressed;
                reading->dirs[0].upDownLeftRight[2]=profile.dpad.left.pressed;
                reading->dirs[0].upDownLeftRight[3]=profile.dpad.right.pressed;
        }
        else
        {
            GCMicroGamepad *profile=[controller microGamepad];
            if(nil!=profile)
            {
                reading->axes[0]=0;
                reading->axes[1]=0;
                reading->axes[2]=0;
                reading->axes[3]=0;

                reading->buttons[0]=profile.buttonA.pressed;
                reading->buttons[1]=profile.buttonX.pressed;
                reading->buttons[2]=false;
                reading->buttons[3]=false;
                reading->buttons[4]=false;
                reading->buttons[5]=false;
                reading->buttons[6]=false;
                reading->buttons[7]=false;

                reading->dirs[0].upDownLeftRight[0]=profile.dpad.up.pressed;
                reading->dirs[0].upDownLeftRight[1]=profile.dpad.down.pressed;
                reading->dirs[0].upDownLeftRight[2]=profile.dpad.left.pressed;
                reading->dirs[0].upDownLeftRight[3]=profile.dpad.right.pressed;
            }
        }
    }
}