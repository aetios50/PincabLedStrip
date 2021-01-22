# What is it ?
An alternative to drive WS2812 ledstrips with DirectOutput Framework (Virtual Pinball Cabinet) from a Wemos D1 mini pro (much cheaper) rather than a Teensy.

# Important notes
Currently requires a patched DOF, the below won't work on existing one. Please use the **DirectOutput.DLL** from this repository

Latest version of the DirectOutput.DLL (the one that will be released to mjr R3++ version) is now using a separate controller for the Wemos D1 Mini Pro, called WemosD1MPStripController
In you **cabinet.xml**, you'll have to change the <TeensyStripController></TeesyStripController> headers by <WemosD1MPStripController></WemosD1MPStripController> to make it work.

Note that the <Name> field in the controller can be anything you want, doesn't need to match the controller type name.
Just be aware that this name has to match to any <OutputControllerName> field in the <LedStrip> descriptors.

* Upgrade animation fluidity

Your **cabinet.xml** file needs also to be adapted to use a 2Mbs serial connection & to activate a new feature (PerLedstripLength)

    <ComPortBaudRate>2000000</ComPortBaudRate>
	<SendPerLedstripLength>true</SendPerLedstripLength>

There is also now te possibility to use compressed ledstrip data sent to the Wemos, highly reducing the band-with consumption.
Just add this setup to the WemosD1MPStripController to activate it.

	<UseCompression>true</UseCompression>
	
With the latest version of the firmware, you don't have to change neither MaxLedsPerStrip nor NUMBER_LEDSTRIP.
After the boot sequence updating all 8 hardcoded ledstrips, everything is reset to zero size ledstrips until DirectOutput send its setup for each ledstrip.
So your Wemos will update exactly what needed for your setup, leading to way more fluidity if you have a regular ledstrips setup.

* Ledstrip brightness :
	
You can also set the brightness values per ledstrips with the new DirectOutput.dll provided.
Just add this setup to any of your <LedStrip> descriptors in cabinet .xml
You don't need to change any brightness anymore in the firmware.

	<Brightness>value</Brightness> where value is the brightness in percent, so 0 to 100 values are accepted, default is 100
	
* Test Command : 

The usual RGB test at boot can now be triggered in other ways
- enabling the TEST_ON_RESET define in PincabLedstrip.ino to have it trigerred on each Wemos reset
- using a switch between D0 and GND
- using DirectOutput via a 'T' command, set <TestOnConnect>true</TestOnConnect> in the WemosD1MPStripController descriptor in cabinet.xml



	
You MUST need to change the default speed of wemos in Arduino console from 80mhz to 160mhz, otherwise you will experiment random crashes/wemos freeze while using it:

![Wemos Speed Settings](https://github.com/vakaobr/PincabLedStrip/blob/master/wemos%20arduino%20settings.jpeg?raw=true)
