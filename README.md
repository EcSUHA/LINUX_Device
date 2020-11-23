# Linux Device (template)
### is an platform template (Linux) used to start creating custom IoT Devices running the

# SCDE
## Smart Connected Device Engine - Generation 2

### The new tool for makers to create IoT Devices

Makers can quickly build a firmware for their IoT-Devices
based on the Smart Connected Device Engine core extended by 
  * included commands and modules
  * own commands and modules
  * third party commands and modules
  
Programmers can provide
  * commands and modules for use in Makers IoT-Devices
  
IoT Devices setup is stored in the maker.cfg, IoT Devices state is stored in the state.cfg

Embedded Commands (SCDE-Core)
  * Attr
  * Define
  * Delete
  * Deleteattr
  * Get
  * Help
  * Include
  * IOWrite
  * List
  * Rename
  * Rereadcfg
  * Save
  * Set
  * Setstate 
  * Shutdown

Embedded Modules (SCDE-Core)
  * Global

Embedded Commands (linked in build process, Linux-Platform)
  * Reloadcommand

Embedded Modules (linked in build process, Linux-Platform)
  * -/-
  
Additional Commands (loadable module, Linux-Platform)
  * Reloadmodule

Additional Modules (loadable module, Linux-Platform)
  * Telnet (maintained as Module-Template)


---


Created by Maik Schulze, Sandfuhren 4, 38448 Wolfsburg, Germany, for EcSUHA.de

MSchulze780@GMAIL.COM

Copyright by EcSUHA

This is part of

- ESP 8266EX & ESP32 SoC Activities ...

- HoME CoNTROL & Smart Connected Device Engine Activities ...
 
EcSUHA - ECONOMIC SURVEILLANCE AND HOME AUTOMATION - WWW.EcSUHA.DE

---


Recommended installation and use:

1. create folder home/LINUX

2. clone LINUX_Device into this folder, can use custom name - recurse-submodules

$ git clone --recurse-submodules https://github.com/EcSUHA/LINUX_Device.git My_LINUX_Device

4. ! fix SPIFFs path in source code, commands/Include_Command.c !
				,"/home/maikschulze/LINUX/LINUX_Device/spiffs/%.*s"

5. rebuild (in the folder)
./rebuild.sh

6. make executable
chmod a+x ~/LINUX/SCDE/dist/release/bin/SCDE

7. run
./SCDE

8. Connect via telnet (Putty) on port 9999 (according to maker.cfg file)


---

NOTES:

build:
./build.sh

aufräumen:
./clean.sh

aufräumen+build:
./rebuild.sh

install preparations:
./prepare.sh





Bauen und starten:
./rebuild.sh
chmod a+x ~/LINUX/SCDE/dist/release/bin/SCDE
cd ..
cd bin
./SCDE

SPIFFS im Home Dir

