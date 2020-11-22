# What is it ?
An alternative to drive WS2812 ledstrips from a Wemos D1 mini pro (much cheaper) rather than a Teensy

# Important notes
Currently requires a patched DOF, the below won't work on existing one. Please use the **DirectOutput.DLL** from this repository

Your **cabinet.xml** file needs to be adapted to use a 2Mbs serial connection & to activate a new feature (PerLedstripLength)

    <ComPortBaudRate>2000000</ComPortBaudRate>
	<SendPerLedstripLength>true</SendPerLedstripLength>


