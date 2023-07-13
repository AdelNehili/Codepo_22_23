/*
Pour la personne qui va lire ce code:
    Ce header file (.h) permet d'avoir un fichier aillant tous les includes/constantes nécessaires au bon déroulement de notre protoype.
    Cela permet surtout une lecture plus aisée du projet.
*/
#include "all_constant2.h"



#include <SD.h>
#include <SPI.h>
#include "TimeLib.h"
#include "time.h"

#include <LiquidCrystal.h>

#include <stdlib.h>



////////////////////////////////////////////////////////////////////
//Initialisation des variables                                    //
////////////////////////////////////////////////////////////////////

//_____________________Kalman Variables___//

//_____________________Multi Threading version Wish___//
unsigned long lastMeasurementTime = 0; // variable to store the time of the last measurement
unsigned long lastHearingTime = 0; // variable to store the time of the last time we heard Arduino
unsigned long lastReportWriting = 0; // variable to store the time of the last time we wrote into the Arduino
unsigned long lastAskingTime = 0; // variable to store the time of the last time we asked IP address to ESP32


//_____________________Thingstream___________________//
unsigned long mytime; //Permet de connaitre le temps d'attente pour recevoir une réponde du thingStream
int init_flag = 0;  //Permet de savoir si toutes les étapes de l'initialisation du Thingstream sont okay
bool already_publish = false; //Variable tests 

//_____________________SD Card______________________//
int writing = 0;  //Variable tests 
int reading = 0;  //Variable tests 

File myFile;  //Objet permettant l'ouverture de fichiers en accès w et/ou r
File myReport;  //Par souçis de cohérence des parties de codes, on garde ces Files
File file;  //Par soucis de cohérence des parties de codes, on garde ces Files
#define CS_PIN 53 //Pin utilisée pour transmettre la data à la carte SD
int init_flag_SD = 0;


//_____________________ESP32______________________//
String esp32_ip = "";
bool answering_mode = false; //It allows to block send any queries to ESP32 if this mode is activated
//_____________________Measures______________________//
// Define the input and output pins for the current sensors
#define INPUT_CURRENT_PIN A0
#define OUTPUT_CURRENT_PIN A1
// Define the sample interval (in milliseconds) and the number of samples to take
#define SAMPLE_INTERVAL 1000
#define NUM_SAMPLES 10
// Define variables to store the input and output currents and the charge flow
float SOC_init = 0.8;

// Define a variable to keep track of the last sample time
unsigned long lastSampleTime = 0;