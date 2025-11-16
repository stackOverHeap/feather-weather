# Project schematics
Theses were made with **KiCad EDA**

## files

- ***station météo externe.zip*** is the main module. It is designed for inside use and display temperature and humidity informations on a LCD screen. It also holds the time and date thanks to the use of a RTC clock.

- ***station météo interne.zip*** is the external module. It is made to be used outside for measuring humidity and temperature data, that it then sends trough radio to the station. A NE555 time is designed to wake the card on 30 secoond interval to reduce power consumption, since it is powered from a single 3.7v ion cell.  