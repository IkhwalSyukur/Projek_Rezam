  
  void kirim(){
  
  if (Firebase.ready() && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0))
  {
    sendDataPrevMillis = millis();
    if (updateData==1){
      hasil();
      Serial.printf("Set int... %s\n", Firebase.setInt(fbdo, F("/result/Tinggi"), tinggi) ? "ok" : fbdo.errorReason().c_str());
      Serial.printf("Set int... %s\n", Firebase.setInt(fbdo, F("/result/Berat"), berat) ? "ok" : fbdo.errorReason().c_str());
      fideal();
      Serial.printf("Set int... %s\n", Firebase.setInt(fbdo, F("/result/Ideal"), isIdeal) ? "ok" : fbdo.errorReason().c_str());
      Serial.printf("Set int... %s\n", Firebase.setInt(fbdo, F("/result/Send"), 1) ? "ok" : fbdo.errorReason().c_str());
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Tinggi= ");
      lcd.setCursor(9,0);
      lcd.print(tinggi);
      lcd.setCursor(0,1);
      lcd.print("Berat= ");
      lcd.setCursor(8,1);
      lcd.print(berat);
      updateData=0;
      Status=0;
      Serial.printf("Set int... %s\n", Firebase.setInt(fbdo, F("/result/Status"), 0) ? "ok" : fbdo.errorReason().c_str());
      Serial.printf("Set int... %s\n", Firebase.setInt(fbdo, F("/result/Send"), 0) ? "ok" : fbdo.errorReason().c_str());
    }
    Serial.printf("Get string... %s\n", Firebase.getString(fbdo, F("/biodata/gender")) ? fbdo.to<const char *>() : fbdo.errorReason().c_str());
    gender=Firebase.getString(fbdo, F("/biodata/gender")) ? fbdo.to<const char *>() : fbdo.errorReason().c_str();
    Serial.printf("Get int... %s\n", Firebase.getInt(fbdo, F("/result/Status")) ? String(fbdo.to<int>()).c_str() : fbdo.errorReason().c_str());
    kondisi = Firebase.getInt(fbdo, F("/result/Status")) ? String(fbdo.to<int>()).c_str() : fbdo.errorReason().c_str();
    Serial.println(kondisi);
    if (kondisi=="1"){
      Status=1;
    }
  }
  if (Status==0){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Silahkan isi biodata");
  }

  if (Status==1){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Mengukur...");
    hasil();
    updateData=1;
  }
 }

 void hasil() {
    ///////////////////////////////////////////////////////
    float adcpin=analogRead(pin);
    int cm = 10650.08 * pow(adcpin,-0.6999) - 14;
    tinggi = 210-cm;
    /////////////////////////////////////////////////////
    static boolean newDataReady = 0;
    const int serialPrintInterval = 500;
    if (LoadCell.update()) newDataReady = true;
    if (newDataReady) {
      if (millis() > t + serialPrintInterval) {
        float i = LoadCell.getData();
        berat = i/1000;
        Serial.print("Load_cell output val: ");
        Serial.println(i);
        Serial.print("Tinggi dalam cm = ");
        Serial.println(210-cm);
        if (i<=-300){
          ESP.restart();
        }
        newDataReady = 0;
        t = millis();
        }
    }
      // receive command from serial terminal, send 't' to initiate tare operation:
  if (Serial.available() > 0) {
    char inByte = Serial.read();
    if (inByte == 't') LoadCell.tareNoDelay();
  }

  // check if last tare operation is complete:
  if (LoadCell.getTareStatus() == true) {
    Serial.println("Tare complete");
  }
}

void fideal(){
  if(gender=="L"){
      bbIdeal=(tinggi-100)-((tinggi-100)*10/100);
      if (berat==bbIdeal){
      isIdeal=1;
       }
      else{
      isIdeal=0;
      }
    }
  if(gender=="P"){
      bbIdeal=(tinggi-100)-((tinggi-100)*15/100);
      if (berat==bbIdeal){
      isIdeal=1;
       }
      else{
      isIdeal=0;
      }    
  }
}
