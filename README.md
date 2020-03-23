# Instagram Follower Counter with esp8266

## Letzte Änderungen / Changelog

### 1.8 

  * Linksbündige Positionierung des Logos, damit man das Element austauschen kann (von @drkuebel)
    
### 1.7
 
 * Webinterface für die Konfiguration der Helligkeit, Start des Updates und Reset
 * Besseres Startverhalten


## Einrichtung IDE 

1. Download und Arduino IDE https://www.arduino.cc/en/main/software
   macOS erfordert zusätzlich die Installation von Python 3, das via Homebrew installiert werden kann¹
2. Hinzufügen des ESP-Boards über die Voreinstellung Zusätzliche Boardverwalter-URLS 
  http://arduino.esp8266.com/stable/package_esp8266com_index.json
3. Installieren der benötigten Bibliotheken über Werkzeuge -> Bibliotheken verwalten
   * InstagramStats
   * Json Streaming Parser 
   * ArduinoJSON
   * WifiManager
4. Auswahl des "Board WeMos D1 R1" unter Werkzeuge -> Board 
5. Download der letzten Version unter Release 
6. FollowerCounter.ino in Arduino öffnen
6. Sketch -> Hochladen, lädt die neue Firmware

¹ https://brew.sh/index_de und ``brew install python3``

## Debugger / Fehlersuche

Unter Werkzeuge -> Serieller Monitor öffen. Die Baudrate auf 115200 stellen und einmal den kleinen Reset-Taster, direkt am Controller drücken. 

## Configure / Einrichtung

1. Den ESP8266 mit dem Sketch flashen
2. Es startet automatisch der Wifi-Manager mit dem die Einrichtung der eigenen WLAN-Daten als auch des Instagramm-Namens erfolgen kann

Einstellungen im Wifi-Manager

* WLAN-Name (SSID)
* WLAN Kennwort 
* Instagram-Name 
* Helligkeit von 0 bis 15 (15 das hellste)
* Anzahl der Module (Standard:4 - Mehr möglich) 

## Tasterfunktionen (seit Version 1.3)

* 1x Drücken => Follower anzeigen
* 2x Drücken => WLAN Status anzeigen
* 3x Drücken => Eigene IP anzeigen
* 4x Drücken => Version anzeigen
* 5x Drücken => Helligkeitsdemo
* 6x Drücken => Neustarten (Reset)
* 7x Drücken => Firmware-Update via Internet
* 10x Drücken =>  Zurücksetzen aller Einstellungen - Neukonfiguration nötig

## Parts / Bauteile

Links to amazon.de

* [D1 Mini NodeMCU Clone](https://amzn.to/31fsvza)
* [Mini Breadboard](https://amzn.to/2OiJpYl)
* [Dot Matrix Display](https://amzn.to/2tlg5co)
* [Leitungen / Wires](https://amzn.to/2OhZrSb)
* [Taster-Set](https://amzn.to/2V1nUPH)

Alternative

* [Set mit Breadboard, Leitungen und Tastern](https://amzn.to/2P5Xitb)

## Schaltplan // Schematic

![Schaltplan](https://raw.githubusercontent.com/jegade/esp8266-insta-counter/master/Schaltung/schematic.png)
![Wiring](https://raw.githubusercontent.com/jegade/esp8266-insta-counter/master/Schaltung/wiring.png)
