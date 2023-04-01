#include <EEPROM.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(7, 8, 10, 9, 12, 11);
int length = 0;
float vector[30];

void setup() {

  EEPROM.get(0, length);

  if(analogRead(0) > 64)
  {
    Serial.begin(9600);
    Serial.print("Segment: ");
    Serial.println(printMarkedInt(length));
    Serial.println("Print new in mm or 'no' ?");

    while(true)
    {   

      String input = Serial.readString();
      int newLength = input.toInt();
      Serial.print(input);

      if (input.startsWith("no"))
      {       
        break;
      }

      if (newLength != 0)
      {
        EEPROM.put(0, newLength);
        Serial.print(length);
        Serial.print(" -> "); 
        EEPROM.get(0, length);               
        Serial.println(length);

        Serial.print("Segment: ");
        Serial.println(printMarkedInt(length));

        break;
      }
    }
  }

  FillVectorOnLength();  

  pinMode(LED_BUILTIN, OUTPUT);
  
  lcd.begin(20, 4);
  lcd.print("Hello!");

}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
}

String printMarkedInt(int toPrint)
{
  String builded = "";

  int metres = toPrint / 1000;
  int millimetres = toPrint % 1000;

  if (metres == 0)
  {
     builded = "0.";

    if (millimetres/100 == 0)
    {
      builded += "0";
    }

    if (millimetres/10 == 0)
    {
      builded += "0";
    }

     builded += String(millimetres) + " metres";
  } else
  {
     builded = String(metres) + "." + String(millimetres) + " metres";
  }
  
  return(builded);
}

void FillVectorOnLength()
{
  vector[0] = 0;
  float buffer = 0;

  for(int i=1; i<30 ; i++)
  {
    vector[i] = (vector[i-1]*1000 + length)/1000;
  }
  
  /* for testing

  for(int i=0; i<30 ; i++)
  {
    Serial.print(i);  
    Serial.print(":");  
    Serial.println(vector[i],3);
  }
  */
}

void StandartOutput()
{

}
