//#include <LiquidCrystal.h>


/******************************
 * ESP8266 edition
 * Forza Dash Buddy
 * By Kenneth "Cmdr_Scotty" Walters
 * www.commanderscotty.com
 * Build 210701
 */

// Libraries
  #include <ESP8266WiFi.h>
  #include <WiFiUdp.h>
  //#include <Wire.h> 
  #include <LiquidCrystal_I2C.h>
  
//

// Vars
#define STASSID "Mygeeto22"
#define STAPSK  "ThereIsNoRogue2"

IPAddress local_IP(192, 168, 2, 99);
IPAddress gateway(192, 168, 2, 1);
IPAddress subnet(255, 255, 255, 0);
  unsigned int localPort = 8125;  
  char packetBuffer[UDP_TX_PACKET_MAX_SIZE + 1];
float hz;
unsigned long previousMillis = 0;
      unsigned long duration;    

uint8_t interval = 150;
boolean bState = LOW;
int old_gear=0;
    float old_lLAP;
    float old_bLAP;
    float old_fuel;  


// Remaining configs
WiFiUDP Udp;
LiquidCrystal_I2C lcd(0x27,20,4);
//

void setup() {
  Serial.begin(115200);
  //pinMode(16,OUTPUT);
  WiFi.disconnect();
    //lcd.begin();
    lcd.init();
  
    lcd.backlight();
    lcd.setCursor(3,1);
    lcd.print("Intializing");
    lcd.setCursor(0,0);
  
      if (!WiFi.config(local_IP, gateway, subnet))
      {
      lcd.clear();
      lcd.print("STA Failed to configure");
      }
    WiFiConnect:   
    Serial.begin(115200);
      WiFi.mode(WIFI_STA);
      WiFi.config(local_IP, gateway, subnet);
      WiFi.begin(STASSID, STAPSK);
     
    int timeout=0;
    int timeout2=0;
    lcd.clear();
    while (WiFi.status() != WL_CONNECTED) 
    {
    lcd.print('.');
    delay(100);
    
    timeout2++;
    if (timeout2 >= 19){
      lcd.clear();
      timeout2 = 0;
      timeout++;
    }
    if (timeout >= 5)
      {
      lcd.clear();
      lcd.print("Failed to connect...");
      delay(500);
      goto WiFiConnect;
      }
    }
        lcd.setCursor(0,1);
        lcd.print("Connected! IP address: ");
        lcd.setCursor(0,2);
        lcd.print(WiFi.localIP());
        lcd.setCursor(0,3);
        lcd.print("UDP port: ");lcd.print(localPort);

        Udp.begin(localPort);
        delay(3000);
    lcd.clear();
}

void loop() {



  int packetSize = Udp.parsePacket();
  if(packetSize)
  {
    Udp.read(packetBuffer,UDP_TX_PACKET_MAX_SIZE + 1); //grab the incoming packet data and stuff it into the packetBuffer char, this will assign one byte (think 0xFF) into an array address, starting at 0)
    
    if (packetBuffer[0])
    {
      
    
  //IPAddress remote = Udp.remoteIP();
        
    //speed, need 4 bytes starting at address 244
    union Speed_V {
      float Speed;
      char Buffer[4]; // temp buffer to store 4 bytes
    }sf;
    //map the data from packet buffer to the temporary array for the union
    sf.Buffer[0] = packetBuffer[244]; //first byte
    sf.Buffer[1] = packetBuffer[245]; //second byte
    sf.Buffer[2] = packetBuffer[246]; //third byte
    sf.Buffer[3] = packetBuffer[247]; //fourth byte
    int Speed_I = (int) (sf.Speed * 2.236936); //convert m/s to mph

    //current rpm, 4 bytes from address 16
    union RPM_V {
      float RPM; 
      char Buffer[4]; // temp buffer to store 4 bytes
    }rf;
    //map the data from packet buffer to the temporary array for the union
    rf.Buffer[0] = packetBuffer[16]; //first byte
    rf.Buffer[1] = packetBuffer[17]; //second byte
    rf.Buffer[2] = packetBuffer[18]; //third byte
    rf.Buffer[3] = packetBuffer[19]; //fourth byte
    int RPM_I = (int) round(rf.RPM); //put it in a INT variable cause we don't care about anything beyond the decimal point

    //Max RPM, 4 bytes from address 8
    union mRPM_V {
      float mRPM; 
      char Buffer[4]; // temp buffer to store 4 bytes
    }mrf;
    //map the data from packet buffer to the temporary array for the union
    mrf.Buffer[0] = packetBuffer[8]; //first byte
    mrf.Buffer[1] = packetBuffer[9]; //second byte
    mrf.Buffer[2] = packetBuffer[10]; //third byte
    mrf.Buffer[3] = packetBuffer[11]; //fourth byte
    int mRPM_I = (int) round(mrf.mRPM); //put it in a INT variable cause we don't care about anything beyond the decimal point


    //HorsePower, 4 bytes from address 248
    union Power_V {
      float Power;
      char Buffer[4]; // temp buffer to store 4 bytes
    }pf;
    //map the data from packet buffer to the temporary array for the union
    pf.Buffer[0] = packetBuffer[248]; //first byte
    pf.Buffer[1] = packetBuffer[249]; //second byte
    pf.Buffer[2] = packetBuffer[250]; //third byte
    pf.Buffer[3] = packetBuffer[251]; //fourth byte
    int Power_I = (int) (pf.Power / 746); //do math to change watts to horse power (cause 'merica)

    //Torque, 4 bytes from address 252
    union Torque_V {
      float Torque;
      char Buffer[4]; // temp buffer to store 4 bytes
    }tf;
    //map the data from packet buffer to the temporary array for the union
    tf.Buffer[0] = packetBuffer[252]; //first byte
    tf.Buffer[1] = packetBuffer[253]; //second byte
    tf.Buffer[2] = packetBuffer[254]; //third byte
    tf.Buffer[3] = packetBuffer[255]; //fourth byte
    int Torque_I = (int) (tf.Torque / 1.356); //do math to change newton-meters to ft-lbs (cause you know why)

    //Boost, 4 bytes from address 272
    union Boost_V {
      float Boost;
      char Buffer[4]; // temp buffer to store 4 bytes
    }bf;
    //map the data from packet buffer to the temporary array for the union 
    bf.Buffer[0] = packetBuffer[272]; //first byte
    bf.Buffer[1] = packetBuffer[273]; //second byte
    bf.Buffer[2] = packetBuffer[274]; //third byte
    bf.Buffer[3] = packetBuffer[275]; //fourth byte
    
    union Fuel_V {
      float Fuel;
      char Buffer[4]; // temp buffer to store 4 bytes
    }ff;
    //map the data from packet buffer to the temporary array for the union
    ff.Buffer[0] = packetBuffer[276]; //first byte
    ff.Buffer[1] = packetBuffer[277]; //second byte
    ff.Buffer[2] = packetBuffer[278]; //third byte
    ff.Buffer[3] = packetBuffer[279]; //fourth byte
    int Fuel_I = (int) round((ff.Fuel * 100));

    uint16_t LAP_count = (packetBuffer[301] << 8) | packetBuffer[300];

    if ((ff.Fuel*100) != old_fuel){
      lcd.setCursor(0,0);
      lcd.print("Fuel: ");
      lcd.print((ff.Fuel*100));
      lcd.print("% ");
      old_fuel = (ff.Fuel*100);
    }
    
    //last lap
    union lLAP_V {
      float lLAP;
      char Buffer[4];
    }llf;
    llf.Buffer[0] = packetBuffer[288];
    llf.Buffer[1] = packetBuffer[289];
    llf.Buffer[2] = packetBuffer[290];
    llf.Buffer[3] = packetBuffer[291];

    //gears
        int gear = (int)packetBuffer[307];

    if (gear != old_gear){
        lcd.setCursor(13,0);
        lcd.print("Gear: ");
        if (gear == 11) {
        lcd.print("N");  
        }
        if (gear < 1){
          lcd.print("R");
        }
        if (gear <= 8 && gear > 0) {
          lcd.print(gear);
        }
        Serial.print(gear);
        Serial.print(" ");
        Serial.println(old_gear);
        old_gear = gear;
    }

    
    int ltime_mins = (int) (llf.lLAP / 60);
    float ltime_secs =  (60.00 * ((llf.lLAP/60) - ltime_mins));

    if (llf.lLAP < 0.001) {
      llf.lLAP = 0.001;
    }
    
    float bLAP;
    //bLAP = llf.lLAP;
    if (bLAP > llf.lLAP){
      bLAP = llf.lLAP;
    } 
    if(bLAP < 0.01) {
      bLAP = llf.lLAP;
    }


    
/*
    //best lap
    union bLAP_V {
      float bLAP;
      char Buffer[4]; // temp buffer to story 4 bytes
    }blf;
    //map the data from packet buffer to the temporary array for the union
    blf.Buffer[0] = packetBuffer[284]; //first byte
    blf.Buffer[1] = packetBuffer[285]; //second byte
    blf.Buffer[2] = packetBuffer[286]; //third byte
    blf.Buffer[3] = packetBuffer[287]; //fourth byte
*/
    //fancy math to convert the float time (in seconds) into minutes:seconds.fractionofseconds
    
    int btime_mins = (int) (bLAP / 60);
    float btime_secs =  (60.00 * ((bLAP/60) - btime_mins));
    

    
    lcd.setCursor(0,0);
    lcd.print("RPM:");
    lcd.setCursor(1,4);
    lcd.print(RPM_I);
    lcd.print(" ");
/*
    lcd.setCursor(0,1);
    lcd.print("Boost:");
    lcd.print(bf.Boost);
    lcd.print(" ");
    //lcd.L:::::::::::::::::::::::::::::::;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;print("MAX:");
    //lcd.print(mRPM_I);
    //lcd.print(" ");

    lcd.setCursor(0,2);
    lcd.print("Fuel:");
    lcd.print(Fuel_I);
    lcd.print("% ");

    lcd.setCursor(0,3);
    lcd.print("BLap:");
    lcd.print(btime_mins);
    lcd.print(":");
    lcd.print(btime_secs);
    lcd.print(" ");
    */

    //Serial.print("RPM: ");Serial.print(RPM_I);
    //Serial.println(" ");
    //Serial.print("llf.lLAP ");Serial.print(llf.lLAP);Serial.print(" bLAP ");Serial.print(bLAP);
    //Serial.println(" ");

    if (llf.lLAP != old_lLAP){
        lcd.setCursor(0,2);
        lcd.print("LLap:");
        lcd.print(ltime_mins);
        lcd.print(":");
        lcd.print(ltime_secs);
        lcd.print(" ");
        old_lLAP = llf.lLAP;
    }



    if (bLAP != old_bLAP){
        lcd.setCursor(0,3);
        lcd.print("BLap:");
        lcd.print(btime_mins);
        lcd.print(":");
        lcd.print(btime_secs);
        lcd.print(" ");
        old_bLAP = bLAP;
    }
    

    
    //hz = (RPM_I)/60.00;
    //duration = (1000000/(hz*2));
    //hz = (RPM_I*2)/60;
   // duration = (1/hz)*100000;


/*
    
    currentMicros = micros();
    //Serial.print("duration ");Serial.println(duration);
    if (currentMicros - previousMicros >= duration) {
      Serial.print(currentMicros-previousMicros);Serial.print(" ");Serial.println(duration);
      previousMicros = currentMicros;
        if (cState ==LOW){
          digitalWrite(3,HIGH);
          cState=HIGH;
        }else{
          digitalWrite(3,LOW);
          cState=LOW;
        }
    }
  */  
    //tone(16,(int)hz);
    //Serial.print("HZ ");Serial.println(hz);
    
    //this handles flashing the backlight on the lcd to act as a shift light. however there is noticable lag with this, unknown currently if this is lag due to network, CPU, or just the protocol for the i2c LCD module
    //unsigned long currentMillis = millis();
    if (mRPM_I - RPM_I <=1750){
        if (millis() - previousMillis+interval >= interval) {
        Serial.print(millis() - previousMillis+interval);Serial.println(" ");
        previousMillis = millis();

    
        if (bState == LOW) {
          bState = HIGH;
          lcd.backlight();
          //digitalWrite(2,HIGH);
        } else {
          bState = LOW;
          lcd.noBacklight();
          //digitalWrite(2,LOW);
          //Serial.println("shift!");
        }
      }
    } else {
      bState = HIGH;
      lcd.backlight();
    }

  
    } else  {
        lcd.setCursor(0,0);
        lcd.print("In Menu    ");
     //   noTone(2);
          
  }
 }
}
