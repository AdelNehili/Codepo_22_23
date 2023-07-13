/*
Pour la personne qui va lire ce code:
  Le rôle de ces fonctions est de communiquer avec l'Arduino en interprétant le texte reçu ainsi que de lui répondre s'il le faut.
    C'est donc ici que l'Arduino demande l'heure qu'il est tout comme c'est ici que l'ESP32 demande à l'Arduino les Data qu'il doit afficher
    sur le site.
  Ceci se fait grâce à la communication UART possible entre les deux micro-controlleurs. Il est toute fois possible de communiquer par
    Bluetooth

  De plus, nous prenons l'hypothèse que quelque soit le contexte, si une réponse est attendue, alors elle doit avoir la forme suivante :
    16/04/23 12:07
*/

//____________________________USEFULL FUNCTIONS_______________________//
void cleanSerialPort() {
  //Knowing sometimes, the serial keep some noisy bits (during manipulation), this funnction allows to
  // clean the Serial just before using it.
  // This function should be useless once we created the PCB and that nobody can get access to the circuits
  while (Serial.available() > 0) {
    Serial.read();
  }
}

//____________________________SEND CALLED FUNCTIONS

String send_query_Arduino(String my_query,int waiting_time){
  //Pour rappel, le RX/TX de l'ESP32 est connecté à l'Arduino. Pour communiquer via UART, nous utiliseront
  //Serial1.print(my_querry) et Serial1.readStringUntil('\n')
  //La fonction send_query() est appelée au niveau de asynch_server

  //Serial2.println("Here's my query : "+my_query+"! You have "+String(waiting_time)+" seconds to give me the answer!");
  
  Serial2.print("set_mode_to_hearing"+'\n');
  cleanSerialPort();
  Serial2.print(my_query+'\n');
  
  return check_Anwer_Arduino(my_query,waiting_time);
}
String check_Anwer_Arduino(String my_query,int waiting_time) {
  unsigned long timeInit = millis()*0.001;
  char message[150];
  int count = 0;

  String correct_received_msg = "";
  while(true) {
    // If maximum time (5s) to wait for message reception exceeded
    if(millis()*0.001 - timeInit > waiting_time) { 
      //Serial.print(".");
      return "";
    }
    if (Serial2.available()>0){
      while (Serial2.available()>0){
        String received_msg = Serial2.readStringUntil('\n');
        //correct_received_msg+= analyse_answer(my_query,received_msg);
        correct_received_msg+= received_msg;
        delay(100); // Au cas où la lecture de l'ESP32 soit trop rapide p/r à l'envoie de l'Arduino
      }
      return correct_received_msg;
    }
  }
}


//____________________________RECEIVED CALLED FUNCTIONS
void checkReception_Arduino_bis(int waiting_time) {
  unsigned long timeInit = millis()*0.001;
  char message[150];
  int count = 0;
  
  while(true) {
    // If maximum time (20s) to wait for message reception exceeded
    if(millis()*0.001 - timeInit > waiting_time) { 
      //Serial.println("To Long Time");
      return;
     }
    if (Serial2.available()){
        char st = Serial2.read();
        message[count] = st;  
        message[count+1] = '\0';  
        count += 1;
        
        if(message[count-1] == '\n'){
          analyse_query(String(message));
        }
     }
  }
}
void checkReception_Arduino(int waiting_time) {
  //Vérifie quel query l'ESP32 a reçu et la traite
  //Cette fonction est appelée PERIODIQUEMENT contrairement à check_Anwer_PC qui est appelé juste après avoir envoyé
  //une query
  unsigned long timeInit = millis()*0.001;
  
  while(true) {
    // If maximum time (20s) to wait for message reception exceeded
    if(millis()*0.001 - timeInit > waiting_time) {
      already_answering = false; //just to be SURE that at a moment, we reset already_answering to false 
      return;
    }
    if (Serial2.available()>0){
      already_answering = true;
      while (Serial2.available()>0){
        already_answering = true; // At THIS MOMENT, we won't be able to send ANY message from the webserver
        String my_msg = Serial2.readStringUntil('\n');
        analyse_query(my_msg);
      }
      already_answering = false;
    }
  }
}
void analyse_query(String msg){
  //Lis les querys et partage les datas correspondants
  
  Serial.println("Le message recu : "+msg);
  if (msg=="getIP"){
    if (WiFi.status() != WL_CONNECTED) {
      Serial2.print("IP: ");
      Serial2.println(WiFi.localIP());
    }else{
      Serial2.println("");
    }
  }
  else if (msg=="getDate" ||msg=="getTime" ){
    Serial2.println(get_time());
  }
  else{
    /*
    Serial.println("Query pas reconnue, voici les options:");
    Serial.println("  -getIP");
    Serial.println("  -getDate or getTime");
    */
  }
}

//____________________________TEST FUNCTIONS__________________________//
String send_query_PC(String my_query,int waiting_time){
  //Pour rappel, le RX/TX de l'ESP32 est connecté à l'Arduino. Pour communiquer via UART, nous utiliseront
  //Serial1.print(my_querry) et Serial1.readStringUntil('\n')
  //La fonction send_query() est appelée au niveau de asynch_server

  Serial.println("Here's my query : "+my_query+"! You have "+String(waiting_time)+" seconds to give me the answer!");
  return check_Anwer_PC(my_query,waiting_time);
}
String check_Anwer_PC(String my_query,int waiting_time) {
  unsigned long timeInit = millis()*0.001;
  char message[150];
  int count = 0;

  String correct_received_msg = "";
  while(true) {
    // If maximum time (5s) to wait for message reception exceeded
    if(millis()*0.001 - timeInit > waiting_time) { 
      //Serial.print(".");
      return "";
    }
    if (Serial.available()>0){
      while (Serial.available()>0){
        String received_msg = Serial.readStringUntil('\n');
        //correct_received_msg+= analyse_answer(my_query,received_msg);
        correct_received_msg+= received_msg;
        delay(100); // Au cas où la lecture de l'ESP32 soit trop rapide p/r à l'envoie de l'Arduino
      }
      return correct_received_msg;
    }
  }
}
int checkReception_PC(int waiting_time) {
  //Vérifie quel query l'ESP32 a reçu et la traite
  //Cette fonction est appelée PERIODIQUEMENT contrairement à check_Anwer_PC qui est appelé juste après avoir envoyé
  //une query
  unsigned long timeInit = millis()*0.001;
  char message[150];
  int count = 0;
  
  while(true) {
    // If maximum time (20s) to wait for message reception exceeded
    if(millis()*0.001 - timeInit > waiting_time) { 
      //Serial.print(".");
      return 0;
    }
    while (Serial.available()>0){
      String my_msg = Serial.readStringUntil('\n');
      analyse_query_pc(my_msg);
    }
  }
}
void analyse_query_pc(String msg){
  //Lis les querys et partage les datas correspondants
  
  Serial.println("Le message recu : "+msg);
  if (msg=="getIP"){
    Serial.print("local ip: ");
    Serial.println(WiFi.localIP());
  }
  else if (msg=="getDate" ||msg=="getTime" ){
    Serial.println(get_time());
  }
  else{
    /*
    Serial.println("Query pas reconnue, voici les options:");
    Serial.println("  -getIP");
    Serial.println("  -getDate or getTime");
    */
  }
}

String analyse_answer(String my_query,String msg){
  //Lis le réponse reçu et la display sur le site
  

  Serial.println("Le message envoyé : "+my_query);
  if (msg=="get_pv_voltage"){
    if(well_parsed(msg)){
      return msg;      
    }
    else{
      return "";
    }
  }
  /*
  else if (msg=="get_pv_current"){
  }
  else if (msg=="get_battery_voltage"){
  }
  else if (msg=="get_battery_current"){
  }
  else if (msg=="clean_screen"){
  }
  else if (msg=="get_periodic_data"){
  }
  else if (msg=="get_charge_current"){
  }
  else if (msg=="get_periodic_data"){
  }
  else if (msg=="get_charge_current"){
  }
  */
  else{
    Serial.println("Query pas reconnue, voici les options:");
    Serial.println("  -get_pv_voltage");
    Serial.println("  -get_pv_current");
    Serial.println("  -get_battery_voltage");
    Serial.println("  -get_battery_current");
    Serial.println("  -clean_screen");
    Serial.println("  -get_periodic_data");
    Serial.println("  -get_charge_current");
    return "";
  }
}
bool well_parsed(String str) {
    // Cette fonction a pour unique but de vérifier que le texte lu ait la bonne forme
    // Check the length of the string

    Serial.println("Here's the read string :"+str);
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

    // Check the validity of the day, month, year, hour, minute and value
    if (day < 1 || day > 31 || month < 1 || month > 12 || year < 0
        || hour < 0 || hour > 23 || minute < 0 || minute > 59 || value < 0) {
        Serial.println("Invalid date/time or value");
        return 0;
    }
    return 1;
}


int checkReception_easy(){
  //Read the whole received message and put it in a String
  while(!Serial1.available()){
    Serial.print(".");
    delay(1000);
  }
  String message = Serial1.readString();
  Serial.println("Voici ce que j'ai recu = "+message);
  
  return 0;
}
int analyse_ESP32(String st) {
  // Arret de la publication si le mot STOP est reçu par le Click
  //Serial.println("Analyse has been called");
  int res = 1;  
  if(st.indexOf("TIME") > 0 || st.indexOf("SUCCESS") > 0) {
    Serial.println("Yes il y a TIME dans le message!");
    res = 1;
  } else {
    res = 0;
  }
  return res;
}

