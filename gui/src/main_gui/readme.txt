
This template demonstrates a standard-way of using FS GUI library.

Some APIs takes over main loop and never gives control to the application.  I don't think it is right.  The programmer should be able to choose whether the application takes control of the main loop.

However, at least iOS doesn't give control of the main loop, and if you look at the OpenGL ES template, everything needs to be driven from event-handling functions.  Probably the easiest way is to drive the application from the interval function.

fsgui3dmain.cpp mimics the framework of the iOS OpenGL ES application template.  The application needs to be driven from FsGui3DAppIntervalCallBack function in fsgui3dapp.cpp.  This call-back function shouldn't block and give back control to the application framework, unfortunately.

But, by following this style, the program should be easily ported to iOS.



As of 03/04/2013, fsgui3d library is yet to be ready for OpenGL ES 2.0.  So, this framework is yet to be ready for iOS.  But, it will be done soon.
