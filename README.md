# Smart-Home-System
Smart Home: Home Automation, Utility and Security System

The aim of this project is to build a smart home. Home automation or smart home is building automation for the home. 
The project has various components: Automation, Smart Utility, and Security of the house. This project aims at providing
a low cost and durable Smart Home with Home automation system having a robust Security and Smart Mirror, giving a simple
system that delivers comfort coupled with safety and security.
 

The system is self-sustainable, senses your needs and presence, captures your picture and uploads it and, controls your
home appliances accordingly. The future scope of this project entails to provide added functionality by adding more
sensors and devices such as gas sensor, soil moisture detector, rain detector etc and to interlink the Smart Mirror with
Home automation. 

# Hardware Components
1. Arduino Mega for automation
2. Raspberry Pi3 for smart utility
3. Intel Galileo Gen2 for security.

# Implementation

# RFID based Security system

The RFID based entry system has the following functions:
Each member is provided with a RFID tag for entering or leaving the building. Each time, the card must be scanned for entry
or exit. There is a master card which stays with the owner, and can be used to access the program mode of the device. 
The program mode can be used to give access to any new card or to remove an existing card. Program mode can be used without
any prior working knowledge of the device. A wipe button can be used in case of loss of card.
For a temporary entry or one time access, a One-Time Password (OTP) can be requested by pressing a button. This OTP is a 
secure 4-digit code sent to an already registered mobile number. The OTP generated is sent to the owner through SMS and
owner provides this to requested party to gain access.
The OTP is entered through a keypad provided beside the RFID Scanner. Both RFID as well as OTP system can work simultaneously.

# Home Automation system
The home control system consists of several client modules which aim at providing optimum usage as well as maximum comfort
by automating the tasks according to the needs of the user. The control of all appliances can be managed by any Bluetooth
enabled smart phone.

It consists of the Bluetooth Controller:

An app is provided which can be easily installed in the phone. It will have different controller options to control all
appliances in the house.
   
1. Mood Lighting 
- Automatic
- Manual preset
- Manual Adjust
- Theme Lighting
- Disco mode
       
 2. Fan control: The speed of the fan is regularized in reference to the temperature of the concerned environment. 
  
 3. Presence Detector: PIR sensors are used to sense any presence in the room. If a person enters a room, then the lights 
  gets automatically switched ON. 
  
 4. Automatic Temperature Control

# Home Safety System
Home safety is the most crucial function for an autonomous system. In this system, safety was kept at top priority and the 
following functions were added:

1. Panic Button: It raises an alarm and also all lights in the house are switched ON.
2. Fire Alarm: This alarm detects Infra-Red (IR) instead of smoke, hence is more reliable and full proof than other fire 
detection system. In case of fire, it raises an alarm. It also places a call to the nearest Fire station with a pre-recorded
message containing the address of the house. It switches off all lights and appliances to prevent any further harm and damage.


# Utility : Smart Mirror
A magic mirror is a raspberry pi powered monitor behind a double sided mirror. A mostly black web page allows you to add some
widgets to the mirror’s reflection as if by magic. This version includes widgets for displaying the weather forecast, the 
date/time and an integrated Outfit of the Day Application.

Uses three servers : one local server hosted on Raspberry Pi and two on Heroku cloud
Local server renders the Web Application


1. Weather Forecast : The current weather module is one of the default modules of the Smart Mirror. This module displays the 
current weather, including the windspeed, the sunset or sunrise time, the temperature and an icon to display the current 
conditions.
The weather forecast module is one of the default modules of the Smart Mirror. This module displays the weather forecast 
for the coming week, including an an icon to display the current conditions, the minimum temperature and the maximum 
temperature.

 2. Outfit of the day Application

See what the world is wearing. Fashion forward individuals aren't only found in magazines. They are all over the world 
posting photos and videos daily to social media. Ootd (Outfit of the Day) helps you discover the latest fashion trends and 
the individuals behind them with the same weather condition as the user. 

- Discover fashion from around the world.
- top 10 rated  inspiring photos and videos of available.
- Uses PHP and Node.JS server and SQL database

- Communicates with local server for capturing pictures through a connected webcam which gets uploaded to hosted server
- Browse button requests the PHP server to display top 10 images of the same weather type, ordered by decreasing capture time
- Likes and comments directly pushed onto the smart mirror

