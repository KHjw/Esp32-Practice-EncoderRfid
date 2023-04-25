void GameSetup(){
  CorrectRev = 10;
}

void RfidCheck(uint8_t data[32]){
  String RfidID = "";

  for(int i=0; i<4; i++){
    RfidID += (char)data[i];
  }

  if(RfidID == "G1P9")
  {
    Serial.print("G1P9");
    CorrectRev = 10;
  }
  else if(RfidID == "G1P7")
  {
    Serial.print("G1P7");
    CorrectRev = 5;
  }
  else
    Serial.print("Unidentified Chip");

  Serial.println("");
}