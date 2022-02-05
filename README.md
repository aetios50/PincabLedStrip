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

* new with firmware 2.0, EEprom customizable firmware :

	Up to 8 firmware settings are now configurable through EEProm data patching.
	You have to set READ_EEPROM_SETTINGS to 1 to enable this feature
	There are 8 available settings, 3 used for now.

	Current used settings are : 
		-	Test On Reset : will make an RGB sequence each time the Wemos is powered or when the reset button is pressed.
		- 	Test Switch : Arnoz' new Pinmos has a button wired on D0, enable this setting to use it, disable it if D0 is not wired or you'll have issues.
		-	Activity Led : activity led is using D4, if your setup use this output, disable this setting to avoid interferences.

	All other settings are free for future firmware customization.

	To patch your settings in the EEprom, you'll have to flash an 8 bytes file containing your settings at the 0x3FB000 address.
	esptool command line example (Windows version) : 
		esptool.exe -cp <COMPORT> -cd nodemcu -cb 921600 -cf <FirmwareFile> -ca 0x3FB000 -cf <SettingsFile>


	




	
	
