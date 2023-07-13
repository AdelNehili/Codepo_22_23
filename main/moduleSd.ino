/*
Pour la personne qui va lire ce code:
    -Puisque nous travaillons avec Arduino DUE, les pin MOSI/MISO/SCK pin vojnt au centre de l'Arduino et la CS pin va en 53.
    -Toutes les fonctions pratiques pour écrire/lire/envoyer la data à partir de la carte SD sont dans la catégorie USEFULL FUNCTIONS.
    -Dans les cas où l'on veut overwrite un fichier, nous le supprimons avec de le recréer en écrivant dans le même fichier.
    -Avant d'écrire la data dans la carte SD, nous pouvons faire un parsing (déjà présent). Cependant, pour le moment, nous n'avons pas
    encore déterminer le comportement de l'Arduino lorsque ce parsing dit que la date/heure/mesure soit incorrect
    
    ATTENTION: La carte SD a un comportement étrange pour les noms. Elle n'accepte pas plus de 7 caractères pour un nom de fichier
      et étrangement, elle n'accepte plus une arborescence de dossier/fichier. Dès lors, une pseudo-arborescence a été crée dans le nom
      même des fichier:
      Les 3 premiers caractères expriment le système dont on s'intéresse.
        -Battery : bat
        -Panneau photovoltaique : ppv
        -Reseau : res
        -Charges : chr
        -Rapport : report

      La valeur suivante permet de savoir quelle mesures nous intéresse:
        -courant : 0
        -tension  : 1
        -puissance : 2
        -temperature : 4
        -SOC : 5
        -SOH : 6
        

      La valeur suivante permet de savoir si la mesure est entrante/sortante/aucune:
        -aucune : 0
        -entrante : 1
        -sortante : 2

      Enfin, la dernière valeur est différente de 0 ssi on mesure une batterie particulière:
        -pack de batteries OU pas de battery (PV/Reseau) : 0
        -batterie num 1 : 1
        -batterie num 2 : 2
        -batterie num 3 : 3
        -batterie num 4 : 4
*/

//____________________________USEFULL FUNCTIONS_______________________//
void SD_init(int *init_flag_SD){
  //print_variable("CS_PIN",CS_PIN);
  if(SD.begin(CS_PIN)==true){
    Serial.println("La carte SD a bien démarrée");
    *init_flag_SD = 1;
  }
  else{
    Serial.println("La carte SD n'a PAS ete trouve!");
  }
}
void writeData(String filename, String data){
  //Cette fonction sert à écrire, dans un ficher de la carte SD, les valeurs voulues à condition que la carte SD soit détectée (init_flag_SD==1)
  
  if(init_flag_SD==1){
    file = SD.open(filename,FILE_WRITE);
    if(!file){
      Serial.println("CARE I COULDN'T OPEN THE FILE: "+filename+" FOR WRITING '"+data+"'");
    }
    else{
      Serial.println("WRITING '"+data+"' TO THE FILE: "+filename);
      file.println(data);
      file.close();
    }
  }else{
    Serial.println("Un truc chelou!");delay(100);
  }
}
void read_data_and_send(String target,String filename){
  Serial.println("read_data_and_send a été call avec la target : "+target+" et le filename : "+filename);
  String res = "";
  //Cette fonction sert à lire dans la carte SD
  //Pour l'instant, on lit TOUTES les donnés (à voir si c'est logique)
  if(init_flag_SD==1){
    delay(100); //Au cas où l'on enchaine écriture et lecture, on laisse le temps de terminer l'ecriture. On ne sait jamais
    file = SD.open(filename);
    String current_data;
    if(!file){
      Serial.println("CARE I COULDN'T OPEN THE FILE: "+filename+" FOR READING");
    }
    else{
      Serial.println("READING FROM THE FILE: "+filename);
      char buffer[128];
      while (file.available()) {
        memset(buffer, '\0', sizeof(buffer)); // clear the buffer      
        file.readBytesUntil('\n', buffer, sizeof(buffer));
        //Serial.println(buffer);
        
        send_data(target,String(buffer));
        
        //ATTENTION, POINT CRITIQUE POUR LA COMMUNIQUATION AVEC ESP32. Ce point doit BIEN être géré
        answering_mode = false; //Maintenant qu'on a répondu, on se libère du mode answering_mode et on a de nouveau le droit d'envoyer des requêtes à ESP32
        
        
        memset(buffer, '\0', sizeof(buffer)); // clear the buffer
      }
      //Serial2.write(file.read());//Envoie la data à l'ESP par Serial2
      //Serial2.println(current_data);//Envoie la data à l'ESP par Serial2

      file.close();
    }
  }
}



void read_SD_to_print(String filename){
  Serial.println("read_SD_to_print a été call avec le file : "+filename);
  File file = SD.open(filename); //open the file
  if (file) { //if the file opened successfully
    while (file.available()) { //read the file until the end
      Serial.write(file.read()); //print the data to the serial monitor
    }
    file.close(); //close the file
  } else {
    Serial.println("Error opening file!");
  }
}

void print_filenames() {
  File root = SD.open("/"); //open the root directory
  while (true) {
    File file = root.openNextFile(); //open the next file in the directory
    if (!file) { //if there are no more files, break out of the loop
      break;
    }
    Serial.println(file.name()); //print the filename to the serial monitor
    String file_str = String(file);
    file.close(); //close the file
  }
  root.close(); //close the root directory
}


void send_data(String target,String data){
  /*
  if(!verify_parsed_text(data)){
    return;
  }
  Serial.println("Le texte lu est correctement écrit :"+data);
  */
  if(target=="PC"){
    Serial.println(data);
  }
  else if(target=="ThingStream"){
    Serial1.println(data);
  }
  else if(target=="ESP32"){
    Serial2.print(data);
  }

  else{
    Serial.println("Je fais semblant d'envoyer la data : "+data);
  }
}


//_____________TEST_____________//

void delete_all_files(){
  //Allow to delete ALL FILES easily

  File root = SD.open("/"); //open the root directory
  while (true) {
    File file = root.openNextFile(); //open the next file in the directory
    if (!file) { //if there are no more files, break out of the loop
      break;
    }
    delete_file(file.name());
    String file_str = String(file);
    file.close(); //close the file
  }
  root.close(); //close the root directory
}
void delete_file(String filename){
  //Si le file existe, on le supprime (utile pour les tests)  
  if(verify_file_existing(filename)){
    SD.remove(filename);
    Serial.println("Le file "+filename+" a été delete");
  }
}
bool verify_file_existing(String filename){
  //Si le file existe, on le supprime (utile pour les tests)
  if(SD.exists(filename)){
    Serial.println("Le file "+filename+" existe!");
    return true;
  }else{
    Serial.println("Le file "+filename+" n'existe pas, tout n'était qu'une imagination collective hehe!");
    return false;
  }
}
bool verify_parsed_text(String str) {
    // Cette fonction a pour unique but de vérifier que le texte lu ait la bonne forme
    // Check the length of the string

    Serial.println("Here's the read string :"+str);
    print_variable_int("str.length()",str.length());
    if (str.length() <= 17 || 22 <= str.length()) {
        Serial.println("Invalid string length.");
        return 0;
    }

    // Check the format of the string
    if (str[2] != '/' || str[5] != '/' || str[8] != ' ' || str[11] != ':'
        || str[14] != ' '|| str[15] != '#' || str[16] != ' '|| str[17] < '0' || str[17] > '9'|| str[18] < '0' || str[18] > '9') {
        Serial.println("Invalid string format.");
        return 0;
    }

    // Extract the day, month, year, hour, minute, and number
    int day = str.substring(0, 2).toInt();
    int month = str.substring(3, 5).toInt();
    int year = str.substring(6, 8).toInt();
    int hour = str.substring(9, 11).toInt();
    int minute = str.substring(12, 14).toInt();
    int value = str.substring(17, 19).toInt();

    // Check the validity of the day, month, year, hour, and minute
    if (day < 1 || day > 31 || month < 1 || month > 12 || year < 0
        || hour < 0 || hour > 23 || minute < 0 || minute > 59) {
        Serial.println("Invalid date or time.");
        return 0;
    }

    // Print the parsed values
    print_variable_int("day",day);
    print_variable_int("month",month);
    print_variable_int("year",year);
    print_variable_int("hour",hour);
    print_variable_int("minute",minute);
    print_variable_int("value",value);

    return 1;
}


/*_____________________________________PAS UTIL POUR LE MOMENT_____________________________________*/
int initializeSD(){
  if(is_SD_ready() != 1){
    return 0;
  }
  if(createFileRead("test.txt") != 1){
    return 0;
  }
  writeToFile("Hello World! from moduleSD");

  char mon_text[8000];
  readFile(mon_text);
  closeFile();
  return 1;
}
int is_SD_ready(){
  Serial.println("Initializing SD card...");
  

  if (SD.begin(4)) {
    Serial.println("SD card is ready to use.");
    return 1;
  } else {
    Serial.println("SD card initialization failed");
    return 0;
  }
}
int createFileWrite(const char *filename){
  file = SD.open(filename, FILE_WRITE);
  if(file){
    Serial.println("File created successfully.");
    return 1;
  }else{
    Serial.println("Error while creating file.");
    return 0;
  }
}
int createFileRead(const char *filename){
  file = SD.open(filename);

  if(file)
  {
    Serial.println("File created successfully.");
    return 1;
  } else
  {
    Serial.println("Error while creating file.");
    return 0;
  }
}
int writeToFile(char *text){
  if (file) {
    file.println(text);
    Serial.println("Writing to file: ");
    Serial.println(text);
    return 1;
  } else {
    Serial.println("Couldn't write to file");
    return 0;
  }
}
int readFile(char *text) {
  if (file) {
    const char *res;
    while (file.available()) {
      res = file.readString().c_str();
      Serial.print("Reading file: ");
      Serial.println(res);
    }
    sprintf(text, "\0");
    strcat(text, res); //Concatener
    
    return 1;
  } else {
    Serial.println("Couldn't read file");
    return 0;
  }
}
void readDataFromFile(char *text){
}
void closeFile(){
  if(file)
  {
    file.close();
    Serial.println("File closed");
  }
}
void saveSD(const char *filename, char* text) {
  char input[800];
  sprintf(input, text);
  SD.remove(filename);
  if(createFileWrite(filename)) {
      writeToFile(input);
      closeFile();
  }
}
void ReadSD(char *input, const char *filename) {
  if(createFileRead(filename)) {
    readFile(input);
    closeFile();
  }
}


