# What is it ?
An alternative to drive WS2812 ledstrips with DirectOutput Framework (Virtual Pinball Cabinet) from a Wemos D1 mini pro (much cheaper) rather than a Teensy.

# Important notes
Currently requires a patched DOF, the below won't work on existing one. Please use the **DirectOutput.DLL** from this repository

Latest version of the DirectOutput.DLL (the one that will be released to mjr R3++ version) is now using a separate controller for the Wemos D1 Mini Pro, called WemosD1MPStripController
In you **cabinet.xml**, you'll have to change the <TeensyStripController></TeesyStripController> headers by <WemosD1MPStripController></WemosD1MPStripController> to make it work.

Note that the <Name> field in the controller can be anything you want, doesn't need to match the controller type name.
Just be aware that this name has to match to any <OutputControllerName> field in the <LedStrip> descriptors.

Your **cabinet.xml** file needs also to be adapted to use a 2Mbs serial connection & to activate a new feature (PerLedstripLength)

    <ComPortBaudRate>2000000</ComPortBaudRate>
	<SendPerLedstripLength>true</SendPerLedstripLength>
	
You can also set the brightness values per ledstrips, so you don't have to reflash to change brightness any more (except boot sequence)
use PerLedstripBrightness xml tag and set values separated by spaces

	<PerLedstripBrightness>160 255 255 0</PerLedstripBrightness> 

Here first declared ledstrip will be lowered at 160, the two next ones will stay at full brightness, the last one will be disabled
By default Wemos firmware create ledstrip at 255 brightness.
	
	