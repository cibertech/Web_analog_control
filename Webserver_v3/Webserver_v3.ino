int responseTime = 100; //communication timeout. Use a safe value

#include <Servo.h> //library for servo motor

Servo Servo1; //Servo Up/Down
Servo Servo2; //Servo Left/Right
int angServo1 = 90;  
int angServo2 = 90;

char SerialinData[50];
char *SerialinParse[20];
int Serialindex = 0;
boolean SerialstringComplete = false;

char Serial1inData[50];
char *Serial1inParse[20];
char *Serial1inParse2[20];
int Serial1index = 0;
boolean Serial1stringComplete = false;

int Analog1_Value = 0;
int Analog2_Value = 0;
int Old_Slider1 = 0;
int Old_Slider2 = 0;
int Slider1_Value = 0;
int Slider2_Value = 0;

int Input1 = A8;
int Input2 = A9;
int Input3 = A10;
int Input4 = A11;
int Slider1 = A0;
int Slider2 = A1; 

int aux1[4] = {1, 1, 1, 1};
int Old_aux1[4] = {1, 1, 1, 1};
int aux2 = 0;

byte LEDpins[] = {2, 3, 4, 5, 6, 7, 8, 9};
int state;

void setup()
{
  Serial.begin(115200);
  Serial1.begin(115200);

  Servo1.attach(10); 
  Servo2.attach(11); 
  Servo1.write(angServo1); 
  Servo2.write(angServo2);
  
  pinMode(Input1,INPUT_PULLUP);
  pinMode(Input2,INPUT_PULLUP);
  pinMode(Input3,INPUT_PULLUP);
  pinMode(Input4,INPUT_PULLUP);
  pinMode(Slider1,INPUT);
  pinMode(Slider2,INPUT);

  for(int i = 0; i < 8; i++) {
    pinMode(LEDpins[i], OUTPUT);
  }

  sendToESP8266("AT+RST\r\n",2000); // reset module
  Serial.println("ESP8266 reset"); 
  delay(1000); // Have a timeout here instead
  sendToESP8266("AT+CWMODE=1\r\n",1000); // 1 -> configure as station (client); 2 -> configure as access point
  Serial.println("Config as access point"); 
  //sendData("AT+CWLAP\r\n",1000); // List all networks
  sendToESP8266("AT+CWJAP=\"Genedra\",\"Gene4181\"\r\n",1000); // connect to the network. Modify username and password accordingly
  Serial.println("Connect to network"); 
  delay(2000); // Have a timeout here instead
  String IP_add = sendToESP8266("AT+CIFSR",responseTime); // get ip address
  Serial.print("Got IP address: "); 
  Serial.print(IP_add); 
  sendToESP8266("AT+CIPMUX=1",responseTime); // configure for multiple connections
  Serial.println("Configure for multiple connections"); 
  sendToESP8266("AT+CIPSERVER=1,80",responseTime); // turn on server on port 80 
  Serial.println("Wifi connection is running!"); 
  //sendToESP8266("Wifi connection is running!",responseTime);
}

void loop()
{
  if (SerialstringComplete) {
    ParseSerialData();
    memset(SerialinData, 0, sizeof(SerialinData));//clear the SerialinData string
    SerialstringComplete = false; 
  }

  if (Serial.available() && SerialstringComplete == false){
    char inChar;
    while(Serial.available()>0){
      inChar = Serial.read();    
      if (inChar == '\n') 
      {
        Serialindex = 0;
        SerialstringComplete = true;
      }
      else
      {
        SerialinData[Serialindex] = inChar; 
        Serialindex++;
      }
    }
  }
  else{
  aux1[0] = digitalRead(Input1);
  aux1[1] = digitalRead(Input2);
  aux1[2] = digitalRead(Input3);
  aux1[3] = digitalRead(Input4);
  if (aux1[0] != Old_aux1[0] or aux1[1] != Old_aux1[1] or aux1[2] != Old_aux1[2] or aux1[3] != Old_aux1[3]){
    sendData("B," + String(aux1[0]) + "," + String(aux1[1]) + "," + String(aux1[2]) + "," + String(aux1[3]) + ",");
    Old_aux1[0] = aux1[0];
    Old_aux1[1] = aux1[1];
    Old_aux1[2] = aux1[2];
    Old_aux1[3] = aux1[3];
  }
  
  Analog1_Value = analogRead(Slider1);
  Analog2_Value = analogRead(Slider2);
  Slider1_Value = Analog1_Value * (100.0 / 1023.0);
  Slider2_Value = Analog2_Value * (100.0 / 1023.0);
  if (Slider1_Value != Old_Slider1 or Slider2_Value != Old_Slider2){
    sendData("S," + String(Slider1_Value) + "," + String(Slider2_Value) + ",");
    Old_Slider1 = Slider1_Value;
    Old_Slider2 = Slider2_Value;
  }
  delay(20);
  }
  
  //if(Serial.available()>0){
  //   String message = readSerialMessage();
  //  if(find(message,"debugEsp8266:")){
  //    String result = sendToWifi(message.substring(13,message.length()),responseTime,DEBUG);
  //    if(find(result,"OK"))
  //      sendData("\nOK");
  //    else
  //      sendData("\nEr");
  //  }
  //}

  if (Serial1stringComplete) {
    ParseSerial1Data();
    memset(Serial1inData, 0, sizeof(Serial1inData));//clear the Serial1inData string
    Serial1stringComplete = false; 
  }

  if (Serial1.available() && Serial1stringComplete == false){
    char inChar;
    while(Serial1.available()>0){
      inChar = Serial1.read();    
      if (inChar == '\n') 
      {
        Serial1index = 0;
        Serial1stringComplete = true;
      }
      else
      {
        Serial1inData[Serial1index] = inChar; 
        Serial1index++;
      }
    }
  }
  /*
  if(Serial1.available()>0){
    
    String message = readWifiSerialMessage();
    
    if(find(message,"esp8266:")){
       String result = sendToESP8266(message.substring(8,message.length()),responseTime);
      if(find(result,"OK"))
        sendData("\n"+result);
      else
        sendData("\nErrRead,");               //At command ERROR CODE for Failed Executing statement
    }else
    if(find(message,"HELLO")){  //receives HELLO from wifi
        sendData("HI!,1,");    //arduino says HI
    }else if(find(message,"LEDON,")){
      digitalWrite(8,HIGH);
    }else if(find(message,"LEDOFF,")){
      digitalWrite(8,LOW);
    }else if(find(message,"SLIDER1,")){
        Analog1_Value = analogRead(Slider1);
        sendData(String(Analog1_Value));
    }else if(find(message,"SLIDER2,")){
        Analog2_Value = analogRead(Slider2);
        sendData(String(Analog2_Value));
    }else{
      //sendData("ErrRead");//Command ERROR CODE for UNABLE TO READ. Creates an endless loop. remove after debugging
    }
  }
  */
  delay(20);//responseTime);
}

void ParseSerialData(){
    char *p = SerialinData;
    char *str;   
    int count = 0;
    
    while ((str = strtok_r(p, ",", &p)) )
    {   
      SerialinParse[count] = str;
      count++;      
    }
    if(count >= 3)//
    {
      char *func = SerialinParse[0];
      char *prop1 = SerialinParse[1];
      char *prop2 = SerialinParse[2];   
      switch(*func)
      {
        case 'S': Update_LEDs(prop1, prop2); break;//Serial.println("OK"); break;//
      }
      //Serial.println("OK");
    }
  }

void ParseSerial1Data(){
    char *str;   
    int count = 0;
    char *p = Serial1inData;   

    while ((str = strtok_r(p, ":", &p)) != NULL)//Remove the "IPD" part first
    {   
      Serial1inParse[count] = str;
      count++;      
    }
    
    count = 0;
    char *p2 = Serial1inParse[1];   
    
    while ((str = strtok_r(p2, ",", &p2)) )
    {   
      Serial1inParse2[count] = str;
      count++;      
    }
    
    if(count >= 3)//
    {
      char *func = Serial1inParse2[0];
      char *prop1 = Serial1inParse2[1];
      char *prop2 = Serial1inParse2[2]; 
       
      switch(*func)
      {
        case 'S': Update_LEDs(prop1, prop2); break;//Serial.println("OK"); break;//
      }
      //Serial.println("OK");
    }
    
  }
/*
boolean find(String string, String value){
  return string.indexOf(value)>=0;
}
*/
void sendData(String str){
  String len="";
  len+=str.length();
  sendToESP8266("AT+CIPSEND=0,"+len,responseTime);
  delay(10);
  sendToESP8266(str,responseTime);
}

String sendToESP8266(String command, const int timeout){
  String response = "";
  Serial1.println(command);
  long int time = millis();
  while( (time+timeout) > millis())
  {
    while(Serial1.available())
    {
    char c = Serial1.read(); // read the next character.
    response+=c;
    }  
  }
  return response;
}
/*
boolean find(String string, String value){
  return string.indexOf(value)>=0;
}
*/
/*
String  readSerialMessage(){
  char value[100]; 
  int index_count =0;
  while(Serial.available()>0){
    value[index_count]=Serial.read();
    index_count++;
    value[index_count] = '\0'; // Null terminate the string
  }
  String str(value);
  str.trim();
  return str;
}

String  readWifiSerialMessage(){
  char value[100]; 
  int index_count =0;
  while(Serial1.available()>0){
    value[index_count]=Serial1.read();
    index_count++;
    value[index_count] = '\0'; // Null terminate the string
  }
  String str(value);
  str.trim();
  return str;
}
*/
/*
String sendToUno(String command, const int timeout, boolean debug){
  String response = "";
  Serial.println(command); // send the read character to the esp8266
  long int time = millis();
  while( (time+timeout) > millis())
  {
    while(Serial.available())
    {
      char c = Serial.read(); // read the next character.
      response+=c;
    }  
  }
  if(debug)
  {
    Serial.println(response);
  }
  return response;
}
*/

void Update_LEDs(char *prop1, char *prop2){    
  int value1 = atoi(prop1); 
  int value2 = atoi(prop2);
  //int Servo1_val = map(value, 0, 100, 0, 180);
  Servo1.write(value1 * 1.8);//Servo1_val); 
  Servo2.write(value2 * 1.8);//Servo2_val);
  for (int i = 1; i < 9; i++){
    if (value1 > i * 11)
      digitalWrite(LEDpins[i-1], HIGH);
    else
      digitalWrite(LEDpins[i-1], LOW);
  }
}
