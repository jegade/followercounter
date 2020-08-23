# Instagram Follower Counter with esp8266

## Letzte Änderungen / Changelog

### 1.9rc7

  * Im Schaltplan hat sich ein Fehler eingeschlichen: 5V und 3,3V waren vertauscht. Jetzt stimmt das wieder.

### 1.9rc6
    
  * Bessere IP Anzeige


### 1.9rc5
    
  * Update-Status besser ausrichten

### 1.9rc4
    
  * Neues Webinterface
  * Permanente Speicherung der Helligkeit und des Modus bei Änderungen im Webinterface

### 1.9rc3 

  * Bei mehr als 9999 Follower wieder rechtsbündig ausrichten 

### 1.9rc2
  
  * Eigene Schriftart für die Zahlen, max 4x7 Punkte 

### 1.9rc1
  
  * Anzeige der Zeit (Internetzeit)
  * Drei verschiedene Modi: 1. Nur Counter, 2. Nur Uhr, 3. Im Wechsel 
  * Erweiterung der Weboberfläche um Moduswahl
  * Erweiterung der Tastfunktion um Moduswahl
  * Umstellung auf u8g2 für die Anzeige
  * Wegfall des Scroll-In-Effekts

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
3. Installieren des ESP8266-Board über die Boardverwaltung  
4. Installieren der benötigten Bibliotheken über Werkzeuge -> Bibliotheken verwalten
   * InstagramStats
   * Json Streaming Parser 
   * ArduinoJSON
   * WifiManager
   * u8g2
   * NTPClient
   * ESPStringTemplate
5. Auswahl des "Board WeMos D1 R1" unter Werkzeuge -> Board 
6. Download der letzten Version unter Release 
7. FollowerCounter.ino in Arduino öffnen
8. Sketch -> Hochladen, lädt die neue Firmware

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

* 1x Drücken => Followermodus 
* 2x Drücken => Uhrmodus
* 3x Drücken => wechselmodus Uhr/Follower
* 4x Drücken => InfoWLAN
* 5x Drücken => Info IP
* 6x Drücken => Version anzeigen
* 7x Drücken => Firmware-Update via Internet
* 8x Drücken => Neustarten (Reset)
* 10x Drücken =>  Zurücksetzen aller Einstellungen - Neukonfiguration nötig

## Parts / Bauteile

Links to amazon.de

* [D1 Mini NodeMCU Clone](https://amzn.to/31fsvza)
* [Mini Breadboard](https://amzn.to/2OirTmG)
* [Dot Matrix Display](https://amzn.to/2tlg5co)
* [Leitungen / Wires](https://amzn.to/2OhV8pC)
* [Taster-Set](https://amzn.to/2V1nUPH)


## Schaltplan // Schematic

<img align="left" src="https://raw.githubusercontent.com/jegade/esp8266-insta-counter/master/Schaltung/counter-01.jpg" width="300">
<img align="left" src="https://raw.githubusercontent.com/jegade/esp8266-insta-counter/master/Schaltung/counter-02.jpg" width="300">
<img align="left" src="https://raw.githubusercontent.com/jegade/esp8266-insta-counter/master/Schaltung/counter-03.jpg" width="300">
<img align="left" src="https://raw.githubusercontent.com/jegade/esp8266-insta-counter/master/Schaltung/counter-04.jpg" width="300">


 

![Schaltplan](https://raw.githubusercontent.com/jegade/esp8266-insta-counter/master/Schaltung/schematic.png)
![Wiring](https://raw.githubusercontent.com/jegade/esp8266-insta-counter/master/Schaltung/wiring.png)
