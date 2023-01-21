#define CAMERA_MODEL_AI_THINKER
#include "esp_camera.h"
#include "camera_pins.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "Botao.h"  

#define wifiLed 2
#define flash 4
#define buttonPin 14
#define buzzer 15
#define redLed 13
#define greenLed 12

const char* ssid = "wifi ssid";
const char* password = "wifipassword";
const unsigned long timeout = 30000; // 30 seconds

void setup() {
  Serial.begin(115200);
  setupPins();
  camSetup();
  wifiConnect();  
  delay(1000);
}

void loop() {
  sendingImage();
  delay(15000);  
}

void setupPins(){
  pinMode(wifiLed, OUTPUT);
  digitalWrite(wifiLed, HIGH);  
  pinMode(flash, OUTPUT);
  digitalWrite(flash, HIGH);
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, HIGH);
  pinMode(greenLed, OUTPUT);
  digitalWrite(greenLed, HIGH);
  pinMode(redLed, OUTPUT);
  digitalWrite(redLed, HIGH);
  delay(500);
  digitalWrite(wifiLed, LOW);  
  digitalWrite(flash, LOW);
  digitalWrite(buzzer, LOW);
  digitalWrite(greenLed, LOW);
  digitalWrite(redLed, LOW);  
}

void wifiConnect(){
  unsigned long startingTime = millis();
  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
    if((millis() - startingTime) > timeout){
      Serial.println("Check you wifi credentials");
    }
  }
  Serial.println("");
  IPAddress ip = WiFi.localIP();
  digitalWrite(wifiLed, HIGH);
  Serial.println("--------------WIFI--------------");
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println("--------------------------------");
  checkIsServerOn();
}

void camSetup(){
  Serial.println("-----------INIT CAMERA----------");
  Serial.println("--------------------------------");
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 10000000;
  config.pixel_format = PIXFORMAT_JPEG;
  //init with high specs to pre-allocate larger buffers
  if(psramFound()){
//    config.frame_size = FRAMESIZE_QVGA; // 320x240
    config.frame_size = FRAMESIZE_VGA; // 640x480
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }


  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
}

camera_fb_t* capture(){
  camera_fb_t *fb = NULL;
  esp_err_t res = ESP_OK;
  fb = esp_camera_fb_get();
  return fb;
}

void parsingResult(String response){
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, response);
  JsonArray array = doc.as<JsonArray>();
  
  Serial.println("---------------API-RESPONSE-----------");
  
  for(JsonVariant v : array){
    JsonObject object = v.as<JsonObject>();
    const char* description = object["description"];
    float score = object["score"];
    String label = "";
    label += description;
    label += ":";
    label += (score * 100);
    Serial.print("=> ");
    Serial.print(label);
    Serial.println("% <<<");
  }
    Serial.println("--------------------------------");
}

void checkIsServerOn() {
  HTTPClient client;
  client.begin("http://192.168.1.107:8080");
  int httpResponseCode = client.GET();
  if(httpResponseCode == 200){
    Serial.println("---------------API--------------");
    Serial.println("->Local API is running");
    Serial.println("--------------------------------");
  } else {
    Serial.println("---------------API--------------");
    Serial.println("-> Local API is OFF");
    Serial.println("--------------------------------");
  }
  client.end();
}

void postingImage(camera_fb_t *fb){
  digitalWrite(buzzer, HIGH);
  delay(500);
  digitalWrite(buzzer, LOW);
  HTTPClient client;
  client.begin("http://192.168.1.107:8080/imageUpdate");
  client.addHeader("Content-Type", "image/jpeg");
  int httpResponseCode = client.POST(fb->buf, fb->len);
  Serial.print("Server response: ");
  Serial.println(httpResponseCode);
  if(httpResponseCode == 200){
    String response = client.getString();
    parsingResult(response);
    digitalWrite(greenLed, HIGH);
    beep(300);
    beep(300);
    delay(5000);
    digitalWrite(greenLed, LOW);        
  }else{
    //Error
    digitalWrite(redLed, HIGH);
    beep(800);
    digitalWrite(redLed, LOW); 
    Serial.println("Check Your Server!!!");
  }

  client.end();
}
void beep(int beepDelay){
  digitalWrite(buzzer, HIGH);
  delay(beepDelay);
  digitalWrite(buzzer, LOW);
}

void sendingImage(){
  digitalWrite(flash, HIGH);
  delay(500);
  camera_fb_t *fb = capture();
  if(!fb || fb->format != PIXFORMAT_JPEG){
    Serial.println("Camera capture failed");
    esp_camera_fb_return(fb);
    return;
  } else {
    digitalWrite(flash, LOW);
    postingImage(fb);
  }
  esp_camera_fb_return(fb);  
}
