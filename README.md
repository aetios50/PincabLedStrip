# What is it ?
A fork of https://github.com/aetios50/PincabLedStrip improving significantly performances for Wemos D1 mini pro driven addressable leds (WS2812). DOF effects are smooth and responsive compared to initial firmware. 
Still a WIP.

# Important notes
Currently requires a patched DOF, the below won't work on existing one. Please use the **DirectOutput.DLL** from this repository

Your **cabinet.xml** file needs to be adapted to use a 2Mbs serial connection & to activate a new feature (PerLedstripLength)

    <ComPortBaudRate>2000000</ComPortBaudRate>
	<SendPerLedstripLength>true</SendPerLedstripLength>


