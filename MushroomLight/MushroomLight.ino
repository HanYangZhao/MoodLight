 #include <ESP8266WiFi.h>
 #include <WiFiClientSecure.h>
 #include <ArduinoJson.h>
 #include <Adafruit_NeoPixel.h>
 #include <math.h>


 #define PIN 4
 //4 Strands of 9 lights connected in parallel
 
 Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, PIN, NEO_RGBW + NEO_KHZ800);

 bool isConnected = false;

 const char* ssid = "";
 const char* password = "";

 //website where we get our data from
 const char* host = "wefeel.csiro.au";
 const int httpsPort = 80;

 //[love,sadness,anger,joy,surprise,fear,all]
 int previousTweetCount;
 int tweetCount[7];
 float currentPercentage[7];
 float previousPercentage[6] = {0.0,0.0,0.0,0.0,0.0,0.0};
 float deltaPercentage[6];

 String emotionColor = "white"; //default to green

 String love = "pink";
 String sadness = "blue";
 String anger = "orange";
 String fear = "red";
 String joy = "green";
 String surprise = "white";

 unsigned long previousMillis = 0;
 unsigned long getEmotionColorMillis = 0;
 const long getEmotionColorInterval = 1800000; 
 //const long getEmotionColorInterval = 3600000; 

 //delay for different lightmodes in ms
 int delay0 = 50;
 int delay1 = 2;
 int delay2 = 20;
 int delay3 = 1;

 int lightModeIndex = 0;
 int lightMode = 0;
 int random_6 = random(6);
 String randomLightColor = "blue";
 
 //Function Prototypes
 String getCurrentEmotionColor();
 void ledPulse(int delay0 , int delay1, int delay2, int delay3 , uint32_t color, int light_mode);
 void ledClear();

 boolean wifiConnect(){
 //wait 1 minute for wifi connection
 WiFi.begin(ssid, password);
 for (int i = 0 ; i < 60 ; i++){
  if (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  } 
  else{
    return true;
  }
 }
 return false;
}

void setup() {
  strip.begin();
    strip.show(); // Initialize all pixels to 'off'
    strip.setBrightness(255);

    Serial.begin(115200);
    Serial.println();
    Serial.print("connecting to ");
    Serial.println(ssid);

   isConnected = wifiConnect();

   if (isConnected)
   emotionColor = getCurrentEmotionColor();
 }

String getCurrentEmotionColor(){
  if (WiFi.status() != WL_CONNECTED)
    isConnected = false;
  else {

    WiFiClient client;
    Serial.print("connecting to ");
    Serial.println(host);
    if (!client.connect(host, httpsPort)) {
      Serial.println("connection failed");
      isConnected = false;
      return emotionColor;
    }

    String url = "/api/emotions/primary/totals";
    Serial.print("requesting URL: ");
    Serial.println(url);

    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
      "Host: " + host + "\r\n" +
      "User-Agent: BuildFailureDetectorESP8266\r\n" +
      "Connection: close\r\n\r\n");

    Serial.println("request sent");
    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        Serial.println("headers received");
      break;
      }
    }
    String line = client.readStringUntil('\n');

    Serial.println("reply was:");
    Serial.println("==========");
    Serial.println(line);
    Serial.println("==========");
    Serial.println("closing connection");

    char * response = new char[line.length() + 1];

    strcpy(response,line.c_str());

    StaticJsonBuffer<200> jsonBuffer;

    JsonObject& root = jsonBuffer.parseObject(response);

    tweetCount[0] = root["love"];
    tweetCount[1] = root["sadness"];
    tweetCount[2] = root["anger"];
    tweetCount[3] = root["joy"];
    tweetCount[4] = root["surprise"];
    tweetCount[5] = root["fear"];
    tweetCount[6] = root["*"]; //total tweets

    Serial.println("TweetCount");
    Serial.println(tweetCount[0]);
    Serial.println(tweetCount[1]);
    Serial.println(tweetCount[2]);
    Serial.println(tweetCount[3]);
    Serial.println(tweetCount[4]);
    Serial.println(tweetCount[5]);
    Serial.println(tweetCount[6]);

    //data hasn't been updated
    if (previousTweetCount == tweetCount[6]){
      Serial.println("no update");
        return emotionColor;
    }

    previousTweetCount = tweetCount[6];
        
    currentPercentage[0] = (float) tweetCount[0] / tweetCount[6];
    currentPercentage[1] = (float) tweetCount[1] / tweetCount[6];
    currentPercentage[2] = (float) tweetCount[2] / tweetCount[6];
    currentPercentage[3] = (float) tweetCount[3] / tweetCount[6];
    currentPercentage[4] = (float) tweetCount[4] / tweetCount[6];
    currentPercentage[5] = (float) tweetCount[5] / tweetCount[6];

    char buffer1[50];
        
    Serial.println("CurrentPercentage");
    strcpy(buffer1, "love percentage is ");
    dtostrf(currentPercentage[0] , 6, 4, &buffer1[strlen(buffer1)]);
    Serial.println(buffer1);

    strcpy(buffer1, "sadness percentage is ");
    dtostrf(currentPercentage[1] , 6, 4, &buffer1[strlen(buffer1)]);
    Serial.println(buffer1);

    strcpy(buffer1, "anger percentage is ");
    dtostrf(currentPercentage[2] , 6, 4, &buffer1[strlen(buffer1)]);
    Serial.println(buffer1);

    strcpy(buffer1, "joy percentage is ");
    dtostrf(currentPercentage[3] , 6, 4, &buffer1[strlen(buffer1)]);
    Serial.println(buffer1);

    strcpy(buffer1, "surprise percentage is ");
    dtostrf(currentPercentage[4] , 6, 4, &buffer1[strlen(buffer1)]);
    Serial.println(buffer1);

    strcpy(buffer1, "fear percentage is ");
    dtostrf(currentPercentage[5] , 6, 4, &buffer1[strlen(buffer1)]);
    Serial.println(buffer1);

    //find the trending emotion 
    deltaPercentage[0] = currentPercentage[0] - previousPercentage[0];
    deltaPercentage[1] = currentPercentage[1] - previousPercentage[1];
    deltaPercentage[2] = currentPercentage[2] - previousPercentage[2];
    deltaPercentage[3] = currentPercentage[3] - previousPercentage[3];
    deltaPercentage[4] = currentPercentage[4] - previousPercentage[4];
    deltaPercentage[5] = currentPercentage[5] - previousPercentage[5];

    Serial.println("");
    Serial.println("Delta Percentage");
    
    strcpy(buffer1, "delta love percentage is ");
    dtostrf(deltaPercentage[0] , 7, 5, &buffer1[strlen(buffer1)]);
    Serial.println(buffer1);

    strcpy(buffer1, "delta sadness percentage is ");
    dtostrf(deltaPercentage[1] , 7, 5, &buffer1[strlen(buffer1)]);
    Serial.println(buffer1);

    strcpy(buffer1, "delta anger percentage is ");
    dtostrf(deltaPercentage[2] , 7, 5, &buffer1[strlen(buffer1)]);
    Serial.println(buffer1);

    strcpy(buffer1, "delta joy percentage is ");
    dtostrf(deltaPercentage[3] , 7, 5, &buffer1[strlen(buffer1)]);
    Serial.println(buffer1);

    strcpy(buffer1, " delta surprise percentage is ");
    dtostrf(deltaPercentage[4] , 7, 5, &buffer1[strlen(buffer1)]);
    Serial.println(buffer1);

    strcpy(buffer1, "delta fear percentage is ");
    dtostrf(deltaPercentage[5] , 6, 4, &buffer1[strlen(buffer1)]);
    Serial.println(buffer1);
    
    previousPercentage[0] = currentPercentage[0];
    previousPercentage[1] = currentPercentage[1];
    previousPercentage[2] = currentPercentage[2];
    previousPercentage[3] = currentPercentage[3];
    previousPercentage[4] = currentPercentage[4];
    previousPercentage[5] = currentPercentage[5];

    int index = 0;
    float max_p = deltaPercentage[0];
    for (int i = 0 ; i < 6 ; i++){
      if(deltaPercentage[i] > max_p){
        max_p = deltaPercentage[i];
        index = i;
      }
    }
    //[love,sadness,anger,joy,surprise,fear,all]
    switch(index){
      //love
      case 0 :
      Serial.println("love");
      emotionColor = love;
      break;

      //sadness
      case 1 :
      Serial.println("sadness");
      emotionColor = sadness;
      break;

      //anger
      case 2 : 
      Serial.println("anger");
      emotionColor = anger;
      break;
      
      //joy
      case 3 :
      Serial.println("joy");
      emotionColor = joy;
      break;
      
      //surprise
      case 4 : 
      Serial.println("surprise");
      emotionColor = surprise; 
      break;

      case 5:
      Serial.println("fear");
      emotionColor = fear;
      break;
    }
    return emotionColor;
  }
}

void led_clear(){
  for (int i = 0; i < strip.numPixels(); i++){
    strip.setPixelColor(i,strip.Color(0, 0, 0));
  }
  strip.show();
}

void ledPulse(int delay0, int delay1, int delay2, int delay3 , String color, int light_mode){
  float colorArray[3];
  float maxBrightness;

  if (color == "red"){
    colorArray[1] = (float) (255 ) / 100 ;
    colorArray[0] = 0;
    colorArray[2] = 0;        
  }
  else if (color == "blue"){
    colorArray[1] = (float) (5) / 100;
    colorArray[0] = 0;
    colorArray[2] = (float) (200 ) / 100;
  }
  else if (color == "green"){
    colorArray[1] = 0;
    colorArray[0] = (float) (255 ) / 100;
    colorArray[2] = 0;
  }
  else if (color == "orange"){
    colorArray[1] = (float) (255 ) / 100;
    colorArray[0] = (float) (75 ) / 100;
    colorArray[2] = 0;
  }
  else if (color == "pink"){
    colorArray[1] = (float) (255) / 100;
    colorArray[0] = (float) (23 ) / 100;
    colorArray[2] = (float) (147 ) / 100;
  }
  else if (color == "white"){
    colorArray[0] = (float) (200) / 100;
    colorArray[1] = (float) (255) / 100;
    colorArray[2] = (float) (255) / 100;
  }


  if (light_mode == 0){
    for (int j = 0 ; j < 100 ; j++){
      for (int i = 0 ; i < strip.numPixels() ; i++){
        strip.setPixelColor(i, strip.Color( (j * colorArray[0]) , (j * colorArray[1]) , (j * colorArray[2]) ));
      }
      strip.show();
      delay(delay0);
    }
    for (int j = 100 ; j > -1 ; j--){
      for (int i = 0 ; i < strip.numPixels() ; i++){
        strip.setPixelColor(i, strip.Color( (j * colorArray[0]) , (j * colorArray[1]) , (j * colorArray[2]) ) );
      }
      strip.show();
      delay(delay0);
    }
    led_clear();
  }
}

void loop() {

  unsigned long currentMillis = millis();

  if(lightModeIndex == 4){
    random_6 = random(5);

    switch (random_6) {
        case 0 :
          randomLightColor = "red";
          break;
        case 1:
          randomLightColor = "green";
          break;
        case 2:
          randomLightColor = "pink";
        case 3:
          randomLightColor = "blue";
        case 4:
            randomLightColor = "orange";
        case 5:
          randomLightColor = "white";    
    }

    lightMode = 0;
    lightModeIndex = 0;
  }
  lightModeIndex++; 
  if(isConnected){
    if(currentMillis - getEmotionColorMillis > getEmotionColorInterval){
      emotionColor = getCurrentEmotionColor();
      getEmotionColorMillis = currentMillis;
    }
    ledPulse(delay0,delay1,delay2,delay3,emotionColor,lightMode);
 }
 else{
      //show random color
     //retry wifi connection every 5 mins 
     if(currentMillis - getEmotionColorMillis > 300000){
       isConnected = wifiConnect;
       getEmotionColorMillis = currentMillis;
     }
     ledPulse(delay0,delay1,delay2,delay3,randomLightColor, lightMode);
   }
 }
