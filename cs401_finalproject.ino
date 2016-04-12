/*  Nicole Lintott || 200252351
 *  University of Regina
 *  CS401 - Constrained Computing
 *  for adafruit's feather adalogger using phidgets sound sensor 1133
 *  logs data as a txt file
 */

#include <SPI.h>
#include <SD.h>
#define cardSelect 4

File dataFile;
int value = 0;  //current value of sensor
int oldValue = 0;  //old value of sensor.
int maxValue = 0; //min value of current noise.
int minValue = 0; //max value of current noise.
int threshold = 25; //how different the current and old values have to be in order to be considered a "new noise"
double currTime = 0;  //artifcial timer. Not very accurate, but should be good enough for the kind of granularity i want. measured in seconds.
double startTime = 0; //start time of noise
double endTime = 0; //end time of noise

void createAndWrite (int minV, int maxV, double startT, double endT); //creates a new file and writes to it.

//----------------------- SETUP -------------------//
void setup() 
{
  Serial.begin(9600);
  pinMode(13, OUTPUT);  //redLED
  pinMode(8, OUTPUT);   //greenLED
  while (!Serial) 
  {
    ; // wait for serial port to connect.
  }
  // see if the card is present and can be initialized:
  if (!SD.begin(cardSelect)) 
  {
    Serial.println("Card init. failed!");
  }  
  Serial.println("Setting up complete.");
}

//----------------------- LOOP -------------------//
void loop() 
{
  value = analogRead(0);
  int temp = value - oldValue;
  abs(temp); //not allowed to have functions inside abs(). done like this just to be safe.
  if (temp > threshold) //this is a new noise
  {
    //first we need to write the previous noise
    endTime = currTime;
    createAndWrite(minValue, maxValue, startTime, endTime);
    //then set up new noise
    startTime = currTime;
    minValue = value;
    maxValue = value;
  }
  else //this is the "same" noise
  {
    //min max values here
    if (minValue > value)
    {
      minValue = value;
    }
    if (maxValue < value)
    {
      maxValue = value;
    }
  }
  oldValue = value;
  delay(200); //short delay to keep from overloading system. also gives approximent time.
  currTime = currTime + 0.2;
}

//----------------------- CREATEANDWRITE -------------------//
void createAndWrite (int minV, int maxV, double startT, double endT)
{
  //setup
  //the code for implimenting a counting file is from the adafruit example for the adalogger
  //see page 37-39 of:
  //https://learn.adafruit.com/downloads/pdf/adafruit-feather-32u4-adalogger.pdf
  char filename[15];
  strcpy(filename, "output00.txt"); //.json doesnt work?
  for (uint8_t i = 0; i < 100; i++) 
  {
    filename[6] = '0' + i/10;
    filename[7] = '0' + i%10;
    // create if does not exist, do not open existing, write, sync after write
    if (! SD.exists(filename)) 
    {
      break;
    }
  }
  dataFile = SD.open(filename, FILE_WRITE);
  if( ! dataFile ) 
  {
    Serial.print("Couldnt create... ");
    Serial.println(filename);
  }
  else
  {
    //turn greenLED on when writing.
    digitalWrite(8, HIGH);
    Serial.print("Writing to... ");
    Serial.println(filename);
    
    //writing to file.
    dataFile.println("{");
    dataFile.print(" \"minValue\": "); dataFile.print(minV); dataFile.println(",");
    dataFile.print(" \"maxValue\": "); dataFile.print(maxV); dataFile.println(",");
    dataFile.print(" \"range\": "); dataFile.print(maxV - minV); dataFile.println(",");
    dataFile.print(" \"startTime\": "); dataFile.print(startT); dataFile.println(",");
    dataFile.print(" \"endTime\": "); dataFile.print(endT); dataFile.println(",");
    dataFile.print(" \"duration\": "); dataFile.println((endT - startT));
    dataFile.println("}");
    digitalWrite(8, LOW);
  }
  dataFile.close();
}

