# SR-14 CANsensors Repository
A.k.a the Sensor2CAN.<br/>
<br/>

## Dependencies
- Arduino IDE
- Arduino Nano Board Select
- MCP_INO 2.1.0 library
- MD_MAX72XX 3.3.0 library
<br/>

## How to Navigate Repo
- main (Code that has been tested on the car for SRE-6)
- test-bench (Code being developed for Perfboarding)
<br/>

## Environment Setup
1. Install [git](https://git-scm.com/downloads)
2. Clone this repo into any reachable folder using, in a command line of your choice (WSL Ubuntu, PowerShell, Terminal, etc), `git clone https://github.com/spartanracingelectric/Sensor2CAN.git`
    1. Change your branch if necessary by using `git checkout <BRANCH_NAME>`
3. Install the [Arduino IDE](https://www.arduino.cc/en/software) **Make sure to install version 1.8.19**
4. Within the Arduino IDE `Tools->Manage Libraries...`, install all needed libraries
5. Configure Arduino IDE board to the Raspberry Pi Pico.
    1. ![](media/boardsetup.png)
6. Upload the sketch!

## SR-14 Sensor2CAN Team
_Shinika Balasundar_, Software Lead, Integration(CANBus/DAQ communication)<br/>
_Aryan Saini_, Project Lead, Hardware (Schematic and PCB design, PCBA) & Software Validation<br/>
_Rudy Saurez Serrano_, Project Designer, Hardware (Schematic and PCB design, PCBA)<br/>
_Novel Alam_, Project Designer, Software (ADC/MUX programming)<br/>
_Andy Neidhart_, Project Designer, Software (CAN programming)<br/>
_Edric Ong Jieh_, Project Designer, Software (MUX programming)<br/>
_Wesley Tam_, Project Designer, Software (MUX programming)<br/>
<br/>

## Links
[Spartan Racing Website](https://www.sjsuformulasae.com/)<br/>
[DDR Slides](https://docs.google.com/presentation/d/1vasINowhk0g1KwvPsnOtJW3btOEstg6LOq-Vg9krGOk/edit#slide=id.g171971f4135_0_310)<br/>
