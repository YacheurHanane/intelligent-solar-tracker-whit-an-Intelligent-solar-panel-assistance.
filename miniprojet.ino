//*******************************bibio **************************
#include <math.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
//*******************************identife des valeurs temperature *******************************
#define SUR_TEMP_PIN A2 // Analog input pin connect to temperature sensor SUR pin
#define OBJ_TEMP_PIN A3 //Analog input pin connect to temperature sensor OBJ  pin
float temperature_range=10;     
  float final_temp=0;                             
float offset_vol=0.014; 
float tempValue = 0; 
float objtValue= 0;  
float current_temp=0;
float temp=0;
float temp1=0;
float temp2=0;
unsigned int temp3=0;
const float reference_vol=0.500;
unsigned char clear_num=0;//when use lcd to display
float R=0;
float voltage=0;
//***************************defenir Servos*****************
Servo servohori;
int servoh = 0;
int servohLimitHigh = 20;
int servohLimitLow = 1;
Servo servoverti; 
int servov = 0; 
int servovLimitHigh = 20;
int servovLimitLow = 1;
//*************************Assigner photo DIODEs***********************
int ldrtopl = 7; //guache haut  
int ldrtopr = 8; //droite haut 
int ldrbotl = 9; // gauche bas 
int ldrbotr = 10; // droite bas 
//***********************************definir LCD ************************************
LiquidCrystal_I2C lcd(0x20,20,4);
//***************************definir les valeurs humidite********************************
int pourcentage=0;
int humidite = analogRead(A5);
//********************** tablaux des valeurs pour capteur tempertur ************************************
long res[100]={
                 318300,302903,288329,274533,261471,249100,237381,226276,215750,205768,
                 196300,187316,178788,170691,163002,155700,148766,142183,135936,130012,
                 124400,119038,113928,109059,104420,100000,95788,91775,87950,84305,
                 80830,77517,74357,71342,68466,65720,63098,60595,58202,55916,
                 53730,51645,49652,47746,45924,44180,42511,40912,39380,37910,
                 36500,35155,33866,32631,31446,30311,29222,28177,27175,26213,
                 25290,24403,23554,22738,21955,21202,20479,19783,19115,18472,
                 17260,16688,16138,15608,15098,14608,14135,13680,13242,12819,
                 12412,12020,11642,11278,10926,10587,10260,9945,9641,9347,
                 9063,8789,8525,8270,8023,7785,7555,7333,7118,6911};
                 
float obj [13][12]={
/*0*/             { 0,-0.274,-0.58,-0.922,-1.301,-1.721,-2.183,-2.691,-3.247,-3.854,-4.516,-5.236}, //
/*1*/             { 0.271,0,-0.303,-0.642,-1.018,-1.434,-1.894,-2.398,-2.951,-3.556,-4.215,-4.931},  //→surrounding temperature,from -10,0,10,...100
/*2*/             { 0.567,0.3,0,-0.335,-0.708,-1.121,-1.577,-2.078,-2.628,-3.229,-3.884,-4.597},   //↓object temperature,from -10,0,10,...110
/*3*/             { 0.891,0.628,0.331,0,-0.369,-0.778,-1.23,-1.728,-2.274,-2.871,-3.523,-4.232},
/*4*/             { 1.244,0.985,0.692,0.365,0,-0.405,-0.853,-1.347,-1.889,-2.482,-3.13,-3.835},
/*5*/             { 1.628,1.372,1.084,0.761,0.401,0,-0.444,-0.933,-1.47,-2.059,-2.702,-3.403},
/*6*/             { 2.043,1.792,1.509,1.191,0.835,0.439,0,-0.484,-1.017,-1.601,-2.24,-2.936},
/*7*/             { 2.491,2.246,1.968,1.655,1.304,0.913,0.479,0,-0.528,-1.107,-1.74,-2.431},
/*8*/             { 2.975,2.735,2.462,2.155,1.809,1.424,0.996,0.522,0,-0.573,-1.201,-1.887},
/*9*/             { 3.495,3.261,2.994,2.692,2.353,1.974,1.552,1.084,0.568,0,-0.622,-1.301},
/*10*/            { 4.053,3.825,3.565,3.27,2.937,2.564,2.148,1.687,1.177,0.616,0,-0.673},
/*11*/            { 4.651,4.43,4.177,3.888,3.562,3.196,2.787,2.332,1.829,1.275,0.666,0},
/*12*/            { 5.29,5.076,4.83,4.549,4.231,3.872,3.47,3.023,2.527,1.98,1.379,0.72}
}; 
/************** 3"independant" timed events ************************/
const long eventTime_1_LDR = 5000; 
const long eventTime_2_temp = 5000; 
const long eventTime_3_temp = 100;
 

/*********************** debut de moniteur ? ****************m*/
unsigned long previousTime_1 = 0;
unsigned long previousTime_2 = 0;  
unsigned long previousTime_3 = 0;
void setup() {
  // ************initialize serial communication at 9600 bits per second**********************
  Serial.begin(9600);
  // *****************pour LCD****************
  lcd.init();
  //******************************** insere la valeur  temperature pour arduino mega **************************
   analogReference(INTERNAL1V1);
   // ************************pin et initialisation des moteurs serveur************************************** 
   servohori.attach(10);
  servohori.write(0);
  servoverti.attach(9);
  servoverti.write(0);
   
}

void loop(){
//************************* Mises à jour fréquemment*************
  unsigned long currentTime = millis();
 //***************affiche phrase de bienvenue pendant 5 seconde  ************************
 if (currentTime<5000){
 lcd.backlight();
 lcd.setCursor(0,0);
  lcd.print("****           ****");
   lcd.setCursor(5,1);
  lcd.print("bienvenue ... " );
   lcd.setCursor(0,2);
  lcd.print("panneau assistant");
     lcd.setCursor(0,3);
     lcd.print("****           ****");
}
//*********************************démarrer le suiveur solair***********************
if ( currentTime - previousTime_3 >= eventTime_3_temp) {
  servoh = servohori.read();
  servov = servoverti.read();
  //*****************capter values analog de chaque diode*********************
  int topl = analogRead(ldrtopl);
  int topr = analogRead(ldrtopr);
  int botl = analogRead(ldrbotl);
  int botr = analogRead(ldrbotr);
  // *************************calculer moyenne**********************
  int avgtop = (topl + topr) / 2; //moyenne photodiode Haut
  int avgbot = (botl + botr) / 2; //moyenne de photodiode bas
  int avgleft = (topl + botl) / 2; //moyenne of photodiode gauche
  int avgright = (topr + botr) / 2; //moyenne of  photodiodes droite
//*****************  orionter les servo moteurs*******************************
  if (avgtop < avgbot)
  {
    servoverti.write(servov +1);
    if (servov > servovLimitHigh) 
     { 
      servov = servovLimitHigh;
     }
    delay(10);
  }
  else if (avgbot < avgtop)
  {
    servoverti.write(servov -1);
    if (servov < servovLimitLow)
  {
    servov = servovLimitLow;
  }
    delay(10);
  }
  else 
  {
    servoverti.write(servov);
  }
  
  if (avgleft > avgright)
  {
    servohori.write(servoh +1);
    if (servoh > servohLimitHigh)
    {
    servoh = servohLimitHigh;
    }
    delay(10);
  }
  else if (avgright > avgleft)
  {
    servohori.write(servoh -1);
    if (servoh < servohLimitLow)
     {
     servoh = servohLimitLow;
     }
    delay(10);
  }
  else 
  {
    servohori.write(servoh);
  }
  delay(50);

//****************************Mettre à jour le moniteur *************************

 previousTime_3 = currentTime;
}
// ***************** declencher  capteur humidite  *********************************************************************************************
  if ( currentTime - previousTime_1 >= eventTime_1_LDR || analogRead(A5)!=1023 ){
   
    humidite = analogRead(A5);
Serial.println(humidite);
pourcentage= conversion (humidite) ;
lcd.clear();
  lcd.backlight();
 lcd.setCursor(0,0);
  lcd.print("Hum:");
   lcd.setCursor(4,0);
  lcd.print(pourcentage );
   lcd.setCursor(8,0);
  lcd.print("%");
//****************************Mettre à jour le moniteur ************
    previousTime_1 = currentTime;
  }

    /*  ********************declencher capteur temperature ********************** */
  if (currentTime - previousTime_2 >= eventTime_2_temp) {

     //***************************Tem**************************************************************************************************************************
   measureSurTemp();//measure the Surrounding temperature around the sensor
  measureObjectTemp();
  lcd.setCursor(10,0);
  lcd.print("T:");
  lcd.setCursor(12,0);
  lcd.print(final_temp );
  lcd.setCursor(17,0);
  lcd.print("C");
  if ( final_temp >25) {
        lcd.setCursor(0,1);
  lcd.print("perte Rd= 30%" );
lcd.setCursor(0,2);
  lcd.print("syteme roidissement");
  lcd.setCursor(0,3);
  lcd.print("activee");
  }
//******************************condition d'affichage message de assitens panneaux d'après les valeur de T ***************
if ( final_temp <25) {
      lcd.setCursor(0,1);
  lcd.print("etats ideale" );
lcd.setCursor(0,2);
  lcd.print("rien active");}  
//****************************Mettre à jour le moniteur *************************
    previousTime_2 = currentTime;
  }}


 
//*****************************fonction clcul de porcetage d himudite **********************************
 int conversion(int valeur){
    int k=0;
k = map(valeur,1750,0, 0, 100) ;
return k;
}
//*************************les fonctions  calcule de temperateur objet, environs , voltage  ***************************************
float binSearch(long x)// this function used for measure the surrounding temperature
{
  int low,mid,high;
  low=0;
  //mid=0;
  high=100;
  while (low<=high)
  {
    mid=(low+high)/2;
    if(x<res[mid])
      low= mid+1;
    else//(x>res[mid])
      high=mid-1;
  }
  return mid;
}

float arraysearch(float x,float y)//x is the surrounding temperature,y is the object temperature
{
  int i=0;
  float tem_coefficient=100;//Magnification of 100 times  
  i=(x/10)+1;//Ambient temperature      
  voltage=(float)y/tem_coefficient;//the original voltage   
  //Serial.print("sensor voltage:\t");    
  //Serial.print(voltage,5);  
  //Serial.print("V");      
  for(temp3=0;temp3<13;temp3++)   
  {     
    if((voltage>obj[temp3][i])&&(voltage<obj[temp3+1][i]))        
    {     
      return temp3;         
    }     
  }
}
float measureSurTemp()
{  
  unsigned char i=0;
  float current_temp=0;    
  int signal=0;   
  tempValue=0;

  for(i=0;i<10;i++)       //    
  {     
    tempValue+= analogRead(SUR_TEMP_PIN);       
    delay(10);    
  }   
  tempValue=tempValue/10;   
  temp = tempValue*1.1/1023;    
  R=2000000*temp/(2.50-temp);   
  signal=binSearch(R);    
//  current_temp = signal-1+temp_calibration+(res[signal-1]-R)/(res[signal-1]-res[signal]);
  Serial.print("Surrounding temperature:");
  Serial.print(current_temp);
  return current_temp;
}

float measureObjectTemp(){
  unsigned char i=0;  
  unsigned char j=0;  
float sur_temp=0;  
  unsigned int array_temp=0;  
  float temp1,temp2; 
  
  objtValue=0;  
  for(i=0;i<10;i++)
  {
    objtValue+= analogRead(OBJ_TEMP_PIN); 
    delay(10); 
    }       
  objtValue=objtValue/10;//Averaging processing     
  temp1=objtValue*1.1/1023;//+objt_calibration; 
 sur_temp=temp1-(reference_vol+offset_vol);             
  //Serial.print("\t Sensor voltage:");   
  //Serial.print(sur_temp,3); 
  //Serial.print("V");  
array_temp=arraysearch(current_temp,sur_temp*1000);        
  temp2=current_temp;        
  temp1=(temperature_range*voltage)/(obj[array_temp+1][(int)(temp2/10)+1]-obj[array_temp][(int)(temp2/10)+1]);        
  final_temp=temp2+temp1; 
 //****************************** nature d objet  selon T ***********************************     
  if((final_temp>100)||(final_temp<=-10))
    {
    Serial.println ("\t out of range!");
    }
  else
    {
      Serial.print("\t object temperature:");   
      Serial.print(final_temp,2); 
      if ( final_temp >35) {
        Serial.println(" - OBJECT IS MELTING? :)");
      }
      if ( final_temp >25 && final_temp <35) {
        Serial.println(" - HOT object temperature");
      }
      if ( final_temp >20 && final_temp <25) {
        Serial.println(" - WARM object temperature");
      }
      if ( final_temp >10 && final_temp <20) {
        Serial.println(" - NORMAL object temperature");
      }
      if ( final_temp >0 && final_temp <10) {
        Serial.println(" - COLD object temperature");
      }
      }
}
