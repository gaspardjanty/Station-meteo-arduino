//LIBRAIRIES
#include <EEPROM.h>                    //Bibliothèque nécessaire à l'utilisation de l'EEPROM

#include <stdlib.h>

#include <Wire.h>

#include <ChainableLED.h>

#include "DS1307.h"

#include <SPI.h>

#include <SD.h>

#include <Adafruit_BME280.h>
 //=================================================================================================================
//CONSTANTES/VARIABLES GLOBALES
const int boutonR = 2; //Assignation du pin connecté au bouton rouge
const int boutonV = 3; //Assignation du pin connecté au bouton vert
short int v_mode;
short int v_mode_precedent;
short int v_mode_precedent_MTN;
short int v_mode_save;
int i;
unsigned long start = 0;
unsigned long temp = 0;
ChainableLED leds(7, 8, 1); //Assignation du pin connecté à la DEL
File myFile;
DS1307 clock; //Initialisation de l'horloge DS1307
Adafruit_BME280 bme; //Initialisation du capteur BME280
float temperature, humidity, pressure, altitude; //Paramètrage des variables en nombres flottants 
//=================================================================================================================
//SETUP
void setup() {
  Serial.begin(9600);
  Serial.println("ON");
  clock.begin(); //Début de l'horloge
  clock.fillByYMD(2013, 1, 19); //Paramètrage de la date sous format année/mois/jours
  clock.fillByHMS(15, 28, 30); //Paramètrage des heures/minutes/secondes
  clock.fillDayOfWeek(SAT); //Paramètrage du jour
  clock.setTime(); //Installation de l'horloge
  bme.begin(0x76); //Paramètrage de l'adresse I2C
  while (!Serial) {} // Wait

  Wire.begin();

  int LOG_INTERVAL = 10; //LOG_INTERVAL est le délai entre chaque acquisition de données (en minutes)
  EEPROM.write(1, LOG_INTERVAL); //On stocke LOG_INTERVAL dans l'EEPROM, à l'adresse 1

  int FILE_MAX_SIZE = 4; //EN Ko
  EEPROM.write(2, FILE_MAX_SIZE);

  int TIMEOUT = 30;
  EEPROM.write(3, TIMEOUT);

  bool LUMIN = 1;
  EEPROM.write(4, LUMIN);

  int LUMIN_LOW = 25; //x10
  EEPROM.write(5, LUMIN_LOW);

  int LUMIN_HIGH = 77; //x10
  EEPROM.write(6, LUMIN_HIGH);

  bool TEMP_AIR = 1;
  EEPROM.write(7, TEMP_AIR);

  int MIN_TEMP_AIR = -10;
  EEPROM.write(8, MIN_TEMP_AIR);

  int MAX_TEMP_AIR = 60;
  EEPROM.write(9, MAX_TEMP_AIR);

  bool HYGR = 1;
  EEPROM.write(10, HYGR);

  int HYGR_MINT = 0;
  EEPROM.write(11, HYGR_MINT);

  int HYGR_MAXT = 50;
  EEPROM.write(12, HYGR_MAXT);

  bool PRESSURE = 1;
  EEPROM.write(13, PRESSURE);

  int PRESSURE_MIN = 85; //x10
  EEPROM.write(14, PRESSURE_MIN);

  int PRESSURE_MAX = 108; //x10
  EEPROM.write(15, PRESSURE_MAX);

  Serial.setTimeout(10);
  start = millis();
  while ((millis() - start) < 10000) {
    if (digitalRead(boutonR) == LOW) {
      v_mode = 4;
      break;
    } else {
      v_mode = 1;
    }
  }
  delay(500);
  initialisation_interruption();
  Serial.println("OK");

}
//=================================================================================================================
//INTERRUPTION
void initialisation_interruption() {
  attachInterrupt(digitalPinToInterrupt(boutonR), change_mode1, FALLING);
  attachInterrupt(digitalPinToInterrupt(boutonV), change_mode2, FALLING);
}
//=================================================================================================================
//ISR de la presse des boutons
void change_mode1() {
    delay(200);
    v_mode_precedent = v_mode;
    v_mode = 5;
}

void change_mode2() {
    delay(200);
    v_mode_precedent = v_mode;
    v_mode = 6;
}
//==================================================================================================================
void BouttonRouge() {
  Serial.println("");
  if (v_mode_precedent == 1) {
    for (i = 0; i < 10; i++) {
      if (digitalRead(boutonR) == LOW) {
        delay(500);
        Serial.print(i);
      } else if (digitalRead(boutonR) == HIGH && i < 9) {
        v_mode = v_mode_precedent;
        delay(300);
        return;
      }
    }
    Serial.println(" -> MTN");
    v_mode_precedent_MTN = 1;
    v_mode = 3;
    v_mode_save = v_mode;
    delay(3000);
    return;
  } else if (v_mode_precedent == 2) {
    for (i = 0; i < 10; i++) {
      if (digitalRead(boutonR) == LOW) {
        delay(500);
        Serial.print(i);
      } else if (digitalRead(boutonR) == HIGH && i < 9) {
        v_mode = v_mode_precedent;
        delay(300);
        return;
      }
    }
    Serial.println(" -> MTN");
    v_mode_precedent_MTN = 2;
    v_mode = 3;
    v_mode_save = v_mode;
    delay(3000);
    return;
  } else if (v_mode_precedent == 3) {
    for (i = 0; i < 10; i++) {
      if (digitalRead(boutonR) == LOW) {
        delay(500);
        Serial.print(i);
      } else if (digitalRead(boutonR) == HIGH && i < 9) {
        v_mode = v_mode_precedent;
        delay(300);
        return;
      }
    }
    Serial.println(" -> STD/ECO");
    v_mode = v_mode_precedent_MTN;
    v_mode_save = v_mode;
    delay(3000);
    return;
  }
  else{
    delay(500);
    v_mode = v_mode_save;
    return;}
}
//=================================================================================================================
void BouttonVert() {
  Serial.println("");
  if (v_mode_precedent == 1) {
    for (i = 0; i < 10; i++) {
      if (digitalRead(boutonV) == LOW) {
        delay(500);
        Serial.print(i);
      } else if (digitalRead(boutonV) == HIGH && i < 9) {
        v_mode = v_mode_precedent;
        delay(300);
        return;
      }
    }
    Serial.println(" -> ECO");
    v_mode = 2;
    v_mode_save = v_mode;
    delay(3000);
    return;
  } else if (v_mode_precedent == 2) {
    for (i = 0; i < 10; i++) {
      if (digitalRead(boutonV) == LOW) {
        delay(500);
        Serial.print(i);
      } else if (digitalRead(boutonV) == HIGH && i < 9) {
        v_mode = v_mode_precedent;
        delay(300);
        return;
      }
    }
    Serial.println(" -> STD");
    v_mode = 1;
    v_mode_save = v_mode;
    delay(3000);
    return;
  }
  else{
    delay(500);
    v_mode = v_mode_save;
    return;}
}
//=================================================================================================================
//MAIN
void loop() {
  choix_mode();
}
//=================================================================================================================
//CHOIX DU MODE
// 1 = MODE STANDARD, 2 = MODE ECONOMIQUE, 3 = MODE MAINTENANCE, 4 = MODE CONFIGURATION
void choix_mode() {
  if (v_mode == 1) {
    Serial.println("Mode STD");
    delay(500);
    leds.setColorRGB(0, 59, 255, 0);
    mode_standard();
  } else if (v_mode == 2) {
    Serial.println("Mode ECO");
    delay(500);
    leds.setColorRGB(0, 5, 0, 255);
    mode_economique();
  } else if (v_mode == 3) {
    Serial.println("Mode MTN");
    delay(500);
    leds.setColorRGB(0, 255, 77, 0);
    mode_maintenance();
  } else if (v_mode == 4) {
    delay(500);
    leds.setColorRGB(0, 252, 255, 0);
    mode_configuration();
  }
    else if(v_mode == 5)
    {
    BouttonRouge();
  }
    else if(v_mode == 6)
    {
     BouttonVert();
  }
}
//=================================================================================================================
//PROCEDURE STANDARD
void mode_standard() {
  {
    start = millis();
    long int del = EEPROM.read(1);
    bool del_stop = 0;
    Serial.println("TIMER STD");

    del = del * 10 * 60; //On passe del en en millisecondes
    while (del_stop == 0) {

      if ((millis() - start) > del) {
        //Serial.println("Acquisition des donnees en cours...");

        acquisition_d(); //On appelle la fonction acquisition pour prélever les données
        del_stop = 1;
      } else if (v_mode != 1) {
        return;
      }
    }
  }

}
//=================================================================================================================
//PROCEDURE CONFIGURATION
void mode_configuration() {
  long unsigned int start = millis(); //On lance le timer
  bool verif = 0; //On crée la variable vérif qui permet de relançer la config après l'input

  Serial.println("Commande :");

  while (verif == 0) {
    if (Serial.available() > 0 || (millis() - start) > 300000) //On attend l'input de l'utilisateur tout en vérifiant que ce dernier ne prend pas trop de temps
    {
      String a = Serial.readString();
      int tmp;
      tmp = (int) a.toInt();
      Serial.println(tmp);
      delay(500);

      if (tmp == 1) {
        Serial.println("Valeur LOG_INTERVAL :");
        while (Serial.available() == 0) {} //Autre façon d'attendre la saisie de l'utilisateur
        String b = Serial.readString(); //L'utilisateur entre la nouvelle valeur de LOG_INTERVAL
        int c;
        c = (int) b.toInt(); //Ici on passe le string en int en faisant : string -> toInt -> cast int
        Serial.print(c);
        EEPROM.update(1, c);
        delay(5000);
        verif = 1;
      } else if (tmp == 2) {
        Serial.println("Valeur FILE_MAX_SIZE :");
        while (Serial.available() == 0) {}
        String b = Serial.readString();
        int c;
        c = (int) b.toInt();
        Serial.print(c);
        EEPROM.update(2, c);
        delay(5000);
        verif = 1;
      } else if (tmp == 3) {
        Serial.println("Reinitialisation");
        setup(); //On relance setup où toutes les variables sont définies initalement (=> on reset les variables)
        delay(5000);
        verif = 1;
      } else if (tmp == 4) {
        //Placeholder version 
        delay(5000);
        verif = 1;
      } else if (tmp == 5) {
        Serial.println("Valeur TIMEOUT :");
        while (Serial.available() == 0) {}
        String b = Serial.readString();
        int c;
        c = (int) b.toInt();
        Serial.print(c);
        EEPROM.update(3, c);
        delay(5000);
        verif = 1;
      } else if (tmp == 6) {
        Serial.println("Valeur LUMIN :");
        while (Serial.available() == 0) {}
        String b = Serial.readString();
        int c;
        c = (int) b.toInt();
        Serial.print(c);
        EEPROM.update(4, c);
        delay(5000);
        verif = 1;
      } else if (tmp == 7) {
        Serial.println("Valeur LUMIN_LOW :");
        while (Serial.available() == 0) {}
        String b = Serial.readString();
        int c;
        c = (int) b.toInt();
        Serial.print(c);
        EEPROM.update(5, c);
        delay(5000);
        verif = 1;
      } else if (tmp == 8) {
        Serial.println("Valeur LUMIN_HIGH :");
        while (Serial.available() == 0) {}
        String b = Serial.readString();
        int c;
        c = (int) b.toInt();
        Serial.print(c);
        EEPROM.update(6, c);
        delay(5000);
        verif = 1;
      } else if (tmp == 9) {
        Serial.println("Valeur TEMP_AIR :");
        while (Serial.available() == 0) {}
        String b = Serial.readString();
        int c;
        c = (int) b.toInt();
        Serial.print(c);
        EEPROM.update(7, c);
        delay(5000);
        verif = 1;
      } else if (tmp == 10) {
        Serial.println("Valeur MIN_TEMP_AIR :");
        while (Serial.available() == 0) {}
        String b = Serial.readString();
        int c;
        c = (int) b.toInt();
        Serial.print(c);
        EEPROM.update(8, c);
        delay(5000);
        verif = 1;
      } else if (tmp == 11) {
        Serial.println("Valeur MAX_TEMP_AIR :");
        while (Serial.available() == 0) {}
        String b = Serial.readString();
        int c;
        c = (int) b.toInt();
        Serial.print(c);
        EEPROM.update(9, c);
        delay(5000);
        verif = 1;
      } else if (tmp == 12) {
        Serial.println("Valeur HYGR :");
        while (Serial.available() == 0) {}
        String b = Serial.readString();
        int c;
        c = (int) b.toInt();
        Serial.print(c);
        EEPROM.update(10, c);
        delay(5000);
        verif = 1;
      } else if (tmp == 13) {
        Serial.println("Valeur HYGR_MINT :");
        while (Serial.available() == 0) {}
        String b = Serial.readString();
        int c;
        c = (int) b.toInt();
        Serial.print(c);
        EEPROM.update(11, c);
        delay(5000);
        verif = 1;
      } else if (tmp == 14) {
        Serial.println("Valeur HYGR_MAXT :");
        while (Serial.available() == 0) {}
        String b = Serial.readString();
        int c;
        c = (int) b.toInt();
        Serial.print(c);
        EEPROM.update(12, c);
        delay(5000);
        verif = 1;
      } else if (tmp == 15) {
        Serial.println("Valeur PRESSURE :");
        while (Serial.available() == 0) {}
        String b = Serial.readString();
        int c;
        c = (int) b.toInt();
        Serial.print(c);
        EEPROM.update(13, c);
        delay(5000);
        verif = 1;
      } else if (tmp == 16) {
        Serial.println("Valeur PRESSURE_MINT :");
        while (Serial.available() == 0) {}
        String b = Serial.readString();
        int c;
        c = (int) b.toInt();
        Serial.print(c);
        EEPROM.update(14, c);
        delay(5000);
        verif = 1;
      } else if (tmp == 17) {
        Serial.println("Valeur PRESSURE_MAXT :");
        while (Serial.available() == 0) {}
        String b = Serial.readString();
        int c;
        c = (int) b.toInt();
        Serial.print(c);
        EEPROM.update(15, c);
        delay(5000);
        verif = 1;
      } else if (tmp == 18) /////////////////////////////////
      {} else if (tmp == 19) {} else if (tmp == 20) {} else if ((millis() - start) > 300000) {
        Serial.println("Inactivite");
        start = 0; //On break le while et on bascule en mode standard
        break;
      } else {
        Serial.println("Commande error");
        delay(500);
        verif = 1;
      }
    }
  }
  verif = 0;
}
//=================================================================================================================
//PROCEDURE MAINTENANCE
void mode_maintenance() {
  {
    {
      start = millis();
      long int del = EEPROM.read(1);
      bool del_stop = 0;
      Serial.println("TIMER MTN");
      //AJOUTER STOP SD

      del = del * 100 * 60; //On passe del en en millisecondes
      while (del_stop == 0) {

        if ((millis() - start) > del) {
          //Serial.println("Acquisition des donnees en cours...");

          acquisition_d(); //On appelle la fonction acquisition pour prélever les données
          del_stop = 1;
        } else if (v_mode != 3) {
          return;
        }
      }
    }

  }
}
//=================================================================================================================
//PROCEDURE ECONOMIQUE
void mode_economique() {
  {
    long int del = EEPROM.read(1);
    start = millis();

    Serial.println("TIMER ECO");

    del = del * 60 * 100; //*2;   //On passe le delai en millisecondes et on le double 
    bool del_stop = 0;

    while (del_stop == 0) {

      if ((millis() - start) > del) {

        acquisition_d(); //On appelle la fonction acquisition pour prélever les données
        del_stop = 1;
      } else if (v_mode != 2) {
        return;
      }
    }
  }
}
//=================================================================================================================
//ACQUISITION DES DONNEES
int acquisition_d() {
  int luminosite;
  int hygrometrie;
  //int GPS;
  int RealTC;

  if (v_mode == 1 || v_mode == 2) {
    c_luminosite();
    c_hygrometrie();
    c_RealTC();
    delay(500);
  } else if (v_mode == 3) {
    while (v_mode == 3); {
      //IMPRIMER LES DONNER SUR LE SERIAL EN DIRECT
    }
  } else {
    Serial.println("ERROR");
  }

}
//=================================================================================================================
//CAPTEUR LUMNIOSITE
int c_luminosite() {
  Serial.println(analogRead(A0)); //Aquisition et affichage des données du capteur de luminostié
}
//==================================================================================================================
//CARTE SD
void sd_write() {
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.print("Initializing SD card...");
  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("test.txt", FILE_WRITE);
  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.println("This is a test file :)");
    myFile.println("testing 1, 2, 3.");
    for (int i = 0; i < 20; i++) {
      myFile.println(i);
    }
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
}
//=================================================================================================================
//CAPTEUR Hygromètrique
int c_hygrometrie() {

  temperature = bme.readTemperature(); //Aquisition de la valeur de température du capteur BME280
  humidity = bme.readHumidity(); //Aquisition de la valeur d'humidité du capteur BME280
  pressure = bme.readPressure() / 100.0F; //Aquisition de la valeur de pression du capteur BME280
  Serial.print("Temp : ");
  Serial.print(temperature); //Affichage de la température
  Serial.print("°C");
  Serial.print("\t\tPres: "); //Affichage de la pression
  Serial.print(pressure, 2);
  Serial.print(" hPa");
  Serial.print("\t\tHum : ");
  Serial.print(humidity, 2); //Affichage de l'humidité
  Serial.println("%");
}
//=================================================================================================================
//Horloge temps reel
int c_RealTC() {
  clock.getTime(); //Obtention de la valeur du temps de l'horloge
  Serial.print(clock.hour, DEC); 
  Serial.print(":");
  Serial.print(clock.minute, DEC);
  Serial.print(":");
  Serial.print(clock.second, DEC);
  Serial.print("  ");
  Serial.print(clock.dayOfMonth, DEC);
  Serial.print("/");
  Serial.print(clock.month, DEC);
  Serial.print("/");
  Serial.print(clock.year + 2000, DEC);
  Serial.print(" ");
  Serial.print(clock.dayOfMonth);
  Serial.print("*");
  switch (clock.dayOfWeek) //Vérification et affichage du jour
  {
  case MON:
    Serial.print("LUN");
    break;
  case TUE:
    Serial.print("MAR");
    break;
  case WED:
    Serial.print("MER");
    break;
  case THU:
    Serial.print("JEU");
    break;
  case FRI:
    Serial.print("VEN");
    break;
  case SAT:
    Serial.print("SAM");
    break;
  case SUN:
    Serial.print("DIM");
    break;
  }
  Serial.println(" ");
}
