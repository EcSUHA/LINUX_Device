# Linux Device (template)
### is an platform template (Linux) used to start creating custom IoT Devices running the

# SCDE
## Smart Connected Device Engine - Generation 2

### The new tool for makers to create IoT Devices

Makers can quickly build a firmware for their IoT-Devices
based on the Smart Connected Device Engine powered by 
  * included modules (core)
  * own modules
  * third party modules
  
Programmers can provide
  * modules for use in Makers IoT-Devices
  
IoT Devices setup is stored in the maker.cfg
IoT Devices state is stored in the state.cfg

Embedded Commands
  * Add
  * Attr
  * Define
  * Delete
  * Deleteattr
  * Help
  * Include
  * List
  * Set
  * Shutdown
  * Sub

Embedded Modules
  * Global

Additional Modules
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

