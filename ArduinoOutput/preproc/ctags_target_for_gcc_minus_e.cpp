# 1 "c:\\Github\\Esp32-Practice-EncoderRfid\\Esp32-Practice-EncoderRfid.ino"
 /*

 * @file Esp32-Practice-EncoderRfid.ino

 * @author 홍진우 KH.jinu (kevinlike@naver.com)

 * @brief

 * @version 1.0

 * @date 2023-04-25

 *

 * @copyright Copyright (c) 2023

 *

 */
# 12 "c:\\Github\\Esp32-Practice-EncoderRfid\\Esp32-Practice-EncoderRfid.ino"
# 13 "c:\\Github\\Esp32-Practice-EncoderRfid\\Esp32-Practice-EncoderRfid.ino" 2

void setup(void) {
  Serial.begin(115200);
  RfidInit();
  Encoder_Setup();
}

void loop(void) {
  GameSystem();
}
# 1 "c:\\Github\\Esp32-Practice-EncoderRfid\\encoder.ino"
long readEncoderValue(void){
    return encoderValue/4;
}

boolean isButtonPushDown(void){
  if(!digitalRead(4)){
    delay(5);
    if(!digitalRead(4))
      return true;
  }
  return false;
}

void Starter_Guage(int AnswerRev){
  int SingleRevVal = 4000;
  int AnswerVal = 4000;

  AnswerVal=SingleRevVal*AnswerRev;

  if(readEncoderValue()>=(AnswerVal+50*AnswerRev)){
    Serial.print("====== Starter Guage Full!!!");
    StarterProgress = 1;
  }
  else{
    Serial.print("====== Keep going");
    StarterProgress = 0;
  }
}

void Encoder_Setup(){
  pinMode(34, 0x01);
  pinMode(35, 0x01);
  pinMode(4, 0x01);

  digitalWrite(34, 0x1); //turn pullup resistor on
  digitalWrite(35, 0x1); //turn pullup resistor on

  //call updateEncoder() when any high/low changed seen
  //on interrupt 0 (pin 2), or interrupt 1 (pin 3)
  attachInterrupt(34, updateEncoder, 0x03);
  attachInterrupt(35, updateEncoder, 0x03);
}

void Encoder_Progress_Loop(){ // "encoder값, 버튼눌림" 을 표시
  if(isButtonPushDown()){
    Serial.print(readEncoderValue());
    Serial.print(", ");
    Serial.print("1");
  }else{
    Serial.print(readEncoderValue());
    Serial.print(", ");
    Serial.print("0");
  }
  Serial.print(", ");
  Starter_Guage(CorrectRev);
  Serial.println("");
  delay(50);
}

void updateEncoder(){
  int MSB = digitalRead(34); //MSB = most significant bit
  int LSB = digitalRead(35); //LSB = least significant bit

  int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
  int sum = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++;
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue ;

  lastEncoded = encoded; //store this value for next time
}
# 1 "c:\\Github\\Esp32-Practice-EncoderRfid\\game.ino"
void GameSetup(){

}

void GameSystem(){
  if(RfidPASS == 0){
    RfidLoop();
    Serial.println("SEARCHING FOR PLAYER CHIP.....");
  }
  else{
    if(StarterProgress == 0){
      Encoder_Progress_Loop();
    }
    else{
      RfidCheckLoop();
    }
  }
}
# 1 "c:\\Github\\Esp32-Practice-EncoderRfid\\rfid.ino"
void RfidInit(){
  Serial.println("------------Rfid Initialized------------");
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();

  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }

  Serial.println("Found PN532"); // Got ok data, print it out!
  nfc.SAMConfig(); // configure board to read RFID tags
  Serial.println("Waiting for Card ...");
}

void Rfid_Identify(uint8_t data[32]){
  String RfidID = "";

  for(int i=0; i<4; i++){
    RfidID += (char)data[i];
  }

  if(RfidID == "G1P9")
  {
    Serial.print("G1P9");
    CorrectRev = 10;
    RfidPASS = 1;
  }
  else if(RfidID == "G1P7")
  {
    Serial.print("G1P7");
    CorrectRev = 5;
    RfidPASS = 1;
  }
  else{
    Serial.print("Unidentified Chip");
    RfidPASS = 0;
  }
  Serial.println("");
}

void RfidLoop(){
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 }; // Buffer to store the returned UID
  uint8_t uidLength; // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  success = nfc.readPassiveTargetID((0x00), uid, &uidLength);

  if (success) {
    if (uidLength == 7)
    {
      uint8_t data[32];

      Serial.print("NTAG2xx tag Detected : ");

      success = nfc.ntag2xx_ReadPage(7, data);

      if (success)
      {
        Rfid_Identify(data);
      }
    }
    else
    {
      Serial.println("This doesn't seem to be an NTAG203 tag");
    }
    Serial.flush();
  }
  delay (50);
}

void RfidCheckLoop(){
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 }; // Buffer to store the returned UID
  uint8_t uidLength; // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  success = nfc.readPassiveTargetID((0x00), uid, &uidLength);

  if (success) {
    if (uidLength == 7)
    {
      uint8_t data[32];
      success = nfc.ntag2xx_ReadPage(7, data);
      if (success)
      {
        Serial.print("NTAG2xx tag Detected");
        RfidPASS = 0;
        StarterProgress = 0;
        encoderValue = 0;
      }
    }
    else
    {
      Serial.println("This doesn't seem to be an NTAG203 tag");
    }
    Serial.flush();
  }
  delay (50);
}
