/*
Pour la personne qui va lire ce code:
    Ici, nous déposons toutes les fonctions permettant de d'écrire un rapport et de l'envoyer à l'equipe de la CAMESKIN.
    On y traite les données sauvegardées afin de les transmettre sous le bon format.
*/

//____________________________USEFULL FUNCTIONS_______________________//
void writing_report(){
  Serial.println("Report Writing... Please Wait, it can take long time.");
  int  numFiles = 5; //ATTENTION, la taille d'une liste est toujours un peu compliqué à gérer. Ici, on choisit manuellement la taille
  String fileNames[] = {"ppv020.txt","bat100.txt","bat020.txt","bat401.txt","bat500.txt"};
  float mean_values[] = {0,0,0,0,0};
  
  int count = 0;
  float sum = 0.0;
  float arr[10];

  delete_file("report.txt"); //Permet de nettoyer le rapport précédent

  for (int i=0; i < numFiles; i++){
    
    String current_file_name = fileNames[i];
    myFile = SD.open(current_file_name);
    Serial.println("Lecture du fichier "+current_file_name+" afin d'en faire un rapport.");

    if(myFile){
      char buffer[40];
      count = 0;
      float sum = 0.0;

      while(myFile.available()){

        myFile.readBytesUntil('\n', buffer, sizeof(buffer));
        buffer[sizeof(buffer)-1] = '\0';

        //Recherche du "#"
        int pos = search_hashtag(buffer, sizeof(buffer));

        if (pos != -1) {
          sum += find_value(buffer,pos);
          count++;

        } else {
          Serial.println("N'a pas trouvé la valeur (après le #");
        }

      }

    float mean_value = calculateMean(sum, count);
    mean_values[i] = mean_value;

    Serial.println("La valeur moyenne de : "+current_file_name+" est de : "+String(mean_value));    
    myFile.close();

    }else {
      Serial.println("Erreur dans la lecture de : "+current_file_name);
    }
    
  }    
  writingReport(arr,"report.txt");
}

float calculateMean(float sum, int count){
  return sum / count;
}

int search_hashtag(char buffer[],int buff_size){
  int pos = -1;
  for (int i = 0; i < buff_size; i++) {
    if (buffer[i] == '#') {
      //On a trouvé la position du séparateur #
      pos = i;
      break;
    }
  }
  return pos;
}

float find_value(char buffer[], int pos){
  
  //Serial.print("Le Buffer : ");Serial.println(buffer);

  char sub_string[strlen(buffer) - pos];
  for (int i = pos + 1; i < strlen(buffer); i++) {
    sub_string[i - pos - 1] = buffer[i];
  }

  sub_string[strlen(buffer) - pos - 1] = '\0';

  //Serial.print("Voici la valeur lu : ");Serial.println(sub_string);

  //Transformation de la mesure (valeur après le "#" en float)
  float value = atof(sub_string);
  //Serial.print("value : ");Serial.println(value);

  return value;
}
void writingReport(float arr[], String filename){
  myReport = SD.open(filename, FILE_WRITE);
  float PV_mean_current = arr[0];
  float pack_bat_mean_voltage = arr[1]; //
  float pack_bat_mean_current = arr[2];
  float bat_mean_temperature = arr[3];
  float pack_bat_SOC = arr[4];
  
  
  int getArrayLength = sizeof(arr) / sizeof(float);

  if (myReport) {
    myReport.println("Le courant moyen des Panneau Photovoltaique vaut: "+String(PV_mean_current)+" Amper \n");
    delay(100);

    myReport.println("La tension moyenne du pack de batterie vaut : "+String(pack_bat_mean_voltage)+" Volt \n");
    delay(100);

    myReport.println("Le courant moyen en sortie du pack de batterie vaut : "+String(pack_bat_mean_current)+" Amper \n");
    delay(100);
    
    myReport.println("La temperature moyenne des batteries vaut : "+String(bat_mean_temperature)+" Celsius \n");
    delay(100);

    myReport.println("L'etat de charge moyen du pack de batteries vaut : "+String(pack_bat_SOC)+" % \n");
    delay(100);
    
    
    Serial.println("Rapport ecrit.");
    myReport.close();
  } else {
    Serial.println("Erreur lors de l'ouverture du rapport.");
  }
}



