#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;
const char* host = "localhost"; //api
char server[] = "192.168.1.10";
int led = 5;     // LED pin
int button = 16; // push button is connected 
int temp = 0 ;
char t[2] = "", a[2] = "";
char first[] = "http://192.168.1.10/apiconn.php?sensor=";
char second[] = "&button=";
int x ;
boolean freefallDetected = false;

char ssid[] = "TE-Data77";
char wifiPassword[] = "football77";
const int MPU_addr=0x68;  // I2C address of the MPU-6050
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
long timer = 0;

void setup() 
{
   pinMode(D3 , OUTPUT);   
  pinMode(D0, INPUT);
  digitalWrite(D3 , LOW);
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  
  Serial.begin(115200);

  Serial.println("Connection to");
  Serial.println(ssid);
  WiFi.begin(ssid,wifiPassword);
  while (WiFi.status() !=WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Wifi connected");
  Serial.println("IP address:");
  Serial.println(WiFi.localIP());
  Serial.print("Netmask:");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway");
  Serial.println(WiFi.gatewayIP());



Serial.println("Initialize MPU6050");
  
  while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_16G))
  {
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    delay(500);
  }

  mpu.setAccelPowerOnDelay(MPU6050_DELAY_3MS);
  
  mpu.setIntFreeFallEnabled(true);
  mpu.setIntZeroMotionEnabled(false);
  mpu.setIntMotionEnabled(false);
  
  mpu.setDHPFMode(MPU6050_DHPF_5HZ);

  mpu.setFreeFallDetectionThreshold(17);
  mpu.setFreeFallDetectionDuration(2);  
  
  checkSettings();
  
  attachInterrupt(12, doInt, RISING);
}

void doInt()
{
  freefallDetected = true;  
}

void checkSettings()
{
  Serial.println();
  
  Serial.print(" * Sleep Mode:                ");
  Serial.println(mpu.getSleepEnabled() ? "Enabled" : "Disabled");

  Serial.print(" * Motion Interrupt:     ");
  Serial.println(mpu.getIntMotionEnabled() ? "Enabled" : "Disabled");

  Serial.print(" * Zero Motion Interrupt:     ");
  Serial.println(mpu.getIntZeroMotionEnabled() ? "Enabled" : "Disabled");

  Serial.print(" * Free Fall Interrupt:       ");
  Serial.println(mpu.getIntFreeFallEnabled() ? "Enabled" : "Disabled");

  Serial.print(" * Free Fal Threshold:          ");
  Serial.println(mpu.getFreeFallDetectionThreshold());

  Serial.print(" * Free FallDuration:           ");
  Serial.println(mpu.getFreeFallDetectionDuration());
  
  Serial.print(" * Clock Source:              ");
  switch(mpu.getClockSource())
  {
    case MPU6050_CLOCK_KEEP_RESET:     Serial.println("Stops the clock and keeps the timing generator in reset"); break;
    case MPU6050_CLOCK_EXTERNAL_19MHZ: Serial.println("PLL with external 19.2MHz reference"); break;
    case MPU6050_CLOCK_EXTERNAL_32KHZ: Serial.println("PLL with external 32.768kHz reference"); break;
    case MPU6050_CLOCK_PLL_ZGYRO:      Serial.println("PLL with Z axis gyroscope reference"); break;
    case MPU6050_CLOCK_PLL_YGYRO:      Serial.println("PLL with Y axis gyroscope reference"); break;
    case MPU6050_CLOCK_PLL_XGYRO:      Serial.println("PLL with X axis gyroscope reference"); break;
    case MPU6050_CLOCK_INTERNAL_8MHZ:  Serial.println("Internal 8MHz oscillator"); break;
  }
  
  Serial.print(" * Accelerometer:             ");
  switch(mpu.getRange())
  {
    case MPU6050_RANGE_16G:            Serial.println("+/- 16 g"); break;
    case MPU6050_RANGE_8G:             Serial.println("+/- 8 g"); break;
    case MPU6050_RANGE_4G:             Serial.println("+/- 4 g"); break;
    case MPU6050_RANGE_2G:             Serial.println("+/- 2 g"); break;
  }  

  Serial.print(" * Accelerometer offsets:     ");
  Serial.print(mpu.getAccelOffsetX());
  Serial.print(" / ");
  Serial.print(mpu.getAccelOffsetY());
  Serial.print(" / ");
  Serial.println(mpu.getAccelOffsetZ());

  Serial.print(" * Accelerometer power delay: ");
  switch(mpu.getAccelPowerOnDelay())
  {
    case MPU6050_DELAY_3MS:            Serial.println("3ms"); break;
    case MPU6050_DELAY_2MS:            Serial.println("2ms"); break;
    case MPU6050_DELAY_1MS:            Serial.println("1ms"); break;
    case MPU6050_NO_DELAY:             Serial.println("0ms"); break;
  }  
  
  Serial.println();
}

void loop()
{
 Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
   Serial.print("AcX = "); Serial.print(AcX);
  Serial.print(" | AcY = "); Serial.print(AcY);
  Serial.print(" | AcZ = "); Serial.print(AcZ);
   delay(333);

   checkSettings();
  //digitalWrite(D3, LOW);
   temp = digitalRead(D0);
       Vector rawAccel = mpu.readRawAccel();
  Activites act = mpu.readActivites();
     if (temp == HIGH || act.isFreeFall ) {
        digitalWrite(D3, HIGH);
        Serial.println("LED Turned ON");
        delay(1000);
       }
     else {
        digitalWrite(D3, LOW);
        Serial.println("LED Turned OFF");
        delay(1000);
       }
     
  a[0] = act.isFreeFall?'1':'0';
  t[0] = temp?'1':'0';
  Serial.print("Fall");
  Serial.println((a));
  Serial.print("Button");
  Serial.println((t));
  if(act.isFreeFall || temp){
     if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
     
      HTTPClient http;  //Declare an object of class HTTPClient

      if (act.isFreeFall){
        http.begin("http://192.168.1.10/apiconn.php?sensor=1&button=0");
      }else if (temp){
        http.begin("http://192.168.1.10/apiconn.php?sensor=0&button=1");
      }
      else{
        http.begin(strcat(strcat(strcat(first , a) , second) , t));  //Specify request destination
      }
      int httpCode = http.GET();                                                                  //Send the request
       
      if (httpCode > 0) { //Check the returning code
         
        String payload = http.getString();   //Get the request response payload
        Serial.println(payload);                     //Print the response payload
         
      }else{
        Serial.println("no response from server");
      }

    http.end();   //Close connection
   
  }
     delay(10000);
   }


}
