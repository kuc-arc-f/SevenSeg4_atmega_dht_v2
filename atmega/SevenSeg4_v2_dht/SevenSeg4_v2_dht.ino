#include <DHT.h>

#define DHTPIN 14     // what digital pin we're connected to
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);

//define
const int mVoutPin = 0;
const static int mAnode[4] = {9, 10, 11, 12 };
const static int mCathod[7] ={2,3,4,5,6,8,7};

int   mCathod_pin[7]   ={6,4,2,1,0,5,3 };

const int mMaxAnode=4;
int mSTAT =0;
int mSTAT_CONECT=1;
int mSTAT_DISP=2;
int iMode=0;
int mMode_TMP =0;
int mMode_HUM =1;
int mMode_TIME=2;

int mTempNum=0;
int mHumNum=0;

//int mCount= 0;
static uint32_t mTimerTmp;
String mReceive="";

bool Num_Array[10][8]={
// a b c d e f g dp 
{ 1,1,1,1,1,1,0,0}, //0 :0
{ 0,1,1,0,0,0,0,0}, //1 :1
{ 1,1,0,1,1,0,1,0}, //2 :2
{ 1,1,1,1,0,0,1,0}, //3 :3
{ 0,1,1,0,0,1,1,0}, //4 :4
{ 1,0,1,1,0,1,1,0}, //5 :5
{ 1,0,1,1,1,1,1,0}, //6 :6
{ 1,1,1,0,0,0,0,0}, //7 :7
{ 1,1,1,1,1,1,1,0}, //8 :8
{ 1,1,1,1,0,1,1,0}, //9 :9   
};

//
void setup() {
  Serial.begin(9600);
  dht.begin();
  Serial.println("# Start-setup");
  mSTAT =mSTAT_CONECT;
  //cathod
  for(int i= 0 ;i< 7;i++){
    pinMode(mCathod[i], OUTPUT);
  }
  for(int j=0 ;j<  mMaxAnode ; j++){
    pinMode(mAnode[j], OUTPUT);
  }
}

//
void NumPrint(int Number){
//Serial.println(Num_Array[iDeg_1][k]);
  for (int i=0; i< 7; i++){
    int iTmp= mCathod_pin[i];
    if(Num_Array[Number][i]==1){
       digitalWrite(mCathod[iTmp ], LOW);
//Serial.print(mCathod[iTmp ]);
    }
    //else{
    //   digitalWrite(mCathod[iTmp ], HIGH );
    //}
    delayMicroseconds( 500 );
    if(Num_Array[Number][i]==1){
       digitalWrite(mCathod[iTmp ], HIGH);
//Serial.print("0");
    }
//Serial.println("");    
  }
}

void set_anode(int pinAnode){
  for(int i=0;i <mMaxAnode ; i++){
    if(mAnode[i]==pinAnode){
       digitalWrite(mAnode[ i  ], HIGH);
    }
    else{
       digitalWrite(mAnode[ i  ], LOW);
    }
  }
}
//
void proc_dispTime(String sTime){
  if(sTime.length() < 4){ return ; }
  //String sOne="";
  String sOne1000= sTime.substring(0,1);
  String sOne100 = sTime.substring(1,2);
  String sOne10  = sTime.substring(2,3);
  String sOne1   = sTime.substring(3,4);
  
  int iDeg_1000= sOne1000.toInt();  
  int iDeg_100= sOne100.toInt();  
  int iDeg_10 = sOne10.toInt();
  int iDeg_1  = sOne1.toInt();
   for(int i=0;  i< 100; i++ ){
       //iDeg_1000
       set_anode(mAnode[3]);
       NumPrint(iDeg_1000 );
       //iDeg_100
       set_anode(mAnode[2]);
       NumPrint(iDeg_100);
       // dig-10
       set_anode(mAnode[1]);
       NumPrint(iDeg_10);
       set_anode(mAnode[0]);
       NumPrint(iDeg_1);       
   }
}

//
//void proc_display(int iTemp){
void proc_display(int iTemp, int typMode){
//  int iDeg_1000= iTemp / 1000;
//  int iDeg_100= iTemp % 100;
  int iDeg_1000= 0;
  int iDeg_100= 0;
  
  int iDeg_10 = iTemp / 10;
  int iDeg_1  = iTemp % 10;
//Serial.print("iDeg_10=");
//Serial.println(iDeg_10);
//Serial.print("iDeg_1=");
//Serial.println(iDeg_1 );
//for(int k=0;  k< 7; k++ ){
//  Serial.println(Num_Array[iDeg_1][k]);
//}
//Serial.println("end.d1===");

//d2
/*
for(int k=0;  k< 7; k++ ){
  int iTmp= mCathod_pin[k];
Serial.println(mCathod[iTmp ]);
}
Serial.println("");

*/
   if(typMode==mMode_TMP){
	   for(int i=0;  i< 200; i++ ){
	       // dig-10
	       set_anode(mAnode[1]);
	       NumPrint(iDeg_10);
	       set_anode(mAnode[0]);
	       NumPrint(iDeg_1);       
	   }
   }else{
       for(int j=0; j<3; j++){
	   for(int i=0;  i< 50; i++ ){
	       // dig-10
	       set_anode(mAnode[1]);
	       NumPrint(iDeg_10);
	       set_anode(mAnode[0]);
	       NumPrint(iDeg_1);       
	   }
	   delay(100);
	} 
   }
}
//
long convert_Map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//
void proc_uart(){
    String sRecv="";
    while( Serial.available()){
      char c = Serial.read();
      sRecv.concat(c);
//Serial.print("sRecv=");
//Serial.println( sRecv );             
      if(  sRecv.length() >= 10 ){
        String sHead= sRecv.substring(0,4);
        //if(sHead =="dat="){
        if(sHead =="res="){
          mReceive=sRecv.substring(4 );
          mSTAT =mSTAT_DISP;
        }
        sRecv="";
        return;
      }
    }      
}
//
void loop() {
  delay( 100 );
  int iNum=0;
  //int itemp=0;
  if(mSTAT == mSTAT_CONECT){
    if (millis() > mTimerTmp) {
      mTimerTmp = millis()+ 3000;
      
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    if (isnan(h) || isnan(t) ) {
    Serial.println("Failed to read from DHT sensor!");
        return;
    }
    mTempNum =(int)t;   
    mHumNum = (int)h;   
    char sTmp[2+1];
    char sHum[3+1];
      
     sprintf(sTmp, "%2d", mTempNum);
     sprintf(sHum, "%03d", mHumNum);
Serial.print("dat="); 
Serial.print(sTmp);   
Serial.println( sHum );      
    }else{
      proc_uart();
    }
  }else if(mSTAT == mSTAT_DISP){
    if((iMode== mMode_TMP) || (iMode== mMode_HUM)){
//Serial.print("iMode="); 
//Serial.println(iMode); 
      if(iMode== mMode_TMP){
        proc_display( mTempNum , mMode_TMP);
      }else{
        proc_display( mHumNum , mMode_HUM);
      }
      iMode=iMode+1;      
      //if(iMode >=mMode_HUM){
//	iMode=mMode_TIME;
      //}
    }else{
      String sBuff="";
      if(mReceive.length() >=6){
        sBuff= mReceive.substring(2);
Serial.print("sBuff=");
Serial.println(sBuff);
        proc_dispTime(sBuff);
      }
//      iMode=mMode_TMP;
      iMode=mMode_TMP;
    }
    mSTAT =mSTAT_CONECT;
  }
}




