#include <ESP8266WiFi.h>

const char* ssid = "";
const char* password = "";

const char* mHost     = "api.thingspeak.com";
const char* mHostTime = "your-dns.com";

String mTimeStr="0000";
//uint32_t mNextHttp= 15000;
uint32_t mNextHttp= 30000;

uint32_t mTimerTmpInit=30000;
uint32_t mTimerTmp;
uint32_t mTimerTime;

String mAPI_KEY="your-API-key";
static int mTypeURL_TEMP=1;
static int mTypeURL_TIME=2;

//
void setup() {
  mTimerTmp= mTimerTmpInit;
  
  Serial.begin( 9600 );
  delay(10);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
//  Serial.println("IP address: ");
//  Serial.println(WiFi.localIP());
}


//
void proc_httpTime(){
//Serial.print("connecting to ");
//Serial.println(host);  
      WiFiClient client;
      const int httpPort = 80;
      if (!client.connect(mHostTime, httpPort)) {
        Serial.println("connection failed");
        return;
      }
      String url = "/tst/test7seg/test_7seg4_0109a.php?mc_id=1&tmp=0"; 
      client.print(String("GET ") + url + " HTTP/1.1\r\n" +
        "Host: " + mHostTime + "\r\n" + 
        "Connection: close\r\n\r\n");
      delay(10);      
      int iSt=0;
      while(client.available()){
          String line = client.readStringUntil('\r');
//Serial.print(line);
          int iSt=0;
          iSt = line.indexOf("res=");
          if(iSt >= 0){
              iSt = iSt+ 4;
              String sDat = line.substring(iSt );
Serial.print("sDat[TM]=");
Serial.println(sDat);
              if(sDat.length() >= 4){
                  mTimeStr = sDat.substring(0, 4);
              }
          }
      }  // end_while  
}

//
//void proc_http(String sTemp ){
void proc_http(String sTemp, String sHum){
//Serial.print("connecting to ");
//Serial.println(mHost);  
      WiFiClient client;
      const int httpPort = 80;
      if (!client.connect(mHost, httpPort)) {
        Serial.println("connection failed");
        return;
      }
      String url = "/update?key="+ mAPI_KEY + "&field1="+ sTemp +"&field2=" + sHum;
//      String url = "/update?key="+ mAPI_KEY + "&field1="+ sTemp; 
      client.print(String("GET ") + url + " HTTP/1.1\r\n" +
        "Host: " + mHost + "\r\n" + 
        "Connection: close\r\n\r\n");
      delay(10);      
      int iSt=0;
      while(client.available()){
          String line = client.readStringUntil('\r');
//Serial.print(line);
      }    
}

//
void proc_uart(){
    String sRecv="";
    while( Serial.available()){
      char c = Serial.read();
      sRecv.concat(c);
//Serial.print("sRecv=");
//Serial.println( sRecv );             
//      if(  sRecv.length() >= 6 ){
      if(  sRecv.length() >= 9 ){
          String sHead= sRecv.substring(0,4);
//          if(sHead =="tmp="){
          if(sHead =="dat="){
              Serial.print("res=00");
              Serial.println(mTimeStr);             
              if (millis() > mTimerTime ) {
                mTimerTime = millis()+ mNextHttp;
                proc_httpTime();
                delay(1000);    
              }
              if (millis() > mTimerTmp) {
                mTimerTmp = millis()+ mNextHttp+ mTimerTmpInit;
                String sTmp = sRecv.substring( 4, 6 );
                String sHum = sRecv.substring( 6, 9 );
  Serial.print("sTmp=");
  Serial.println( sTmp );       
  Serial.print("sHum=");
  Serial.println( sHum );       
                proc_http(sTmp, sHum);
                delay(1000);                  
              }
              return;
          }               
      }
  } //end_while   
}
  
//
void loop() {
  delay( 100 );
  proc_uart();
    
}




