/*
Pour la personne qui va lire ce code:
    -Ici, la méthode de Coulomb Couting a été choisie. Il est toute fois possible de changer cet algo par un autre plus fin comme
      le filtre de Kalman.
    -Comme précédement dit, en nous basant sur les travaux réalisés l'année passée, nous allons essayer d'appliquer le filtre de Kalman.
      Pour ce faire, nous nous baseront sur les valeurs des test HPPC fait à Kinshasa afin de déterminer les coef. des matrices gain
      de Kalman/d'erreur
*/

//____________________________USEFULL FUNCTIONS_______________________//
float determine_SOC(double input_current,double output_current){
  float SOC_variation = 0;
  float chargeFlow = 0;
  //float SOC_init = 0.8;
  // Paramètres initiaux
  float battery_capacity = 200*20; //unité : [Ah]


  // Calculate the charge flow as the integral of the difference between the input and output currents
  chargeFlow += ((input_current - output_current) * SAMPLE_INTERVAL) / 1000.0;
  SOC_variation = chargeFlow/battery_capacity;
  SOC_init = verify_SOC(SOC_init,SOC_variation);

  // Print the input and output currents and the charge flow for debugging
  //print_things(inputCurrent,outputCurrent,chargeFlow,SOC_variation);
  
  return SOC_init;
  
}

float verify_SOC(float SOC_init,float SOC_variation){
  float SOC=0;
  if(SOC_init+SOC_variation>=1){
    Serial.println("Attention, le SOC calculé vaut 1");
    SOC = 1;
  }else{
    SOC = SOC_init + SOC_variation;
  }
  return SOC;
}

//________________________________TEST_______________________________//
void print_things(float inputCurrent,float outputCurrent,float chargeFlow,float SOC_variation){
  Serial.print("Input current: ");
  Serial.print(inputCurrent);
  Serial.print(" mA\nOutput current: ");
  Serial.print(outputCurrent);
  Serial.print(" mA\nCharge flow: ");
  Serial.print(chargeFlow);
  Serial.print("mAh\nSOC variation: ");
  Serial.println(SOC_variation);
}