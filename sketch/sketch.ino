/*
* Notes:
* Connect jumper to 3.3/5V and A0 port to start configuration mode.
* Connect jumper to GND and A0 port to make sure board starts in normal mode.
* In all modes connecting 5V to D2 and then to D3, incriments the inner counter.
* Reverse connecting decrements the inner counter.
* Inner counter value updating every 0.4 sec with a displaying function depending on mode.
*
* Made by:
* fars40k@yandex.ru
*/

#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <TimerMs.h>

TimerMs tmr(400,1,1);
LiquidCrystal lcd(8, 7, 9, 10, 11, 12);

bool isNormal = false;
bool isForward = false;
bool isBackward = false;

int length = 0;
int shiftSize = 0;
long counter = 0;

void (*Output_method) (void);

void setup() {

  EEPROM.get(0, length);

  if(analogRead(0) > 64)
  {
    // Configuration mode

    isNormal = false;

    Serial.begin(9600);
    Serial.print("Stored segment: ");
    Serial.println(PrintMeasurements(length));
    Serial.println("Print new value(e.g. '1234' = 0.1234m or 123.4mm ) from 1 to 20000");
    Serial.println("or print 'no':");

    while(true)
    {   

      String input = Serial.readString();
      int newLength = input.toInt();
      Serial.print(input);

      if (input.startsWith("no"))
      {       
        break;
      }

      if ((newLength != 0)&&(newLength > 0)&&(newLength <= 20000))
      {

        EEPROM.put(0, newLength);
        Serial.print(length);
        Serial.print(" -> "); 
        EEPROM.get(0, length);               
        Serial.println(length);

        Serial.print("Segment: ");
        Serial.println(PrintMeasurements(length));

        break;

      }
    }
  } else
  {
    // Normal mode

    isNormal = true;

    lcd.begin(16, 2);
    lcd.print("S: ");
    lcd.print(length);

    delay(3000);    
  }
   
  pinMode(2 , INPUT);
  pinMode(13 , INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  attachInterrupt(0, INT0_Encoder, RISING);
  attachInterrupt(1, INT1_Encoder, RISING);  

  if (isNormal == false)
  {

    Serial.println("Timer engaged.");
    Output_method = TMR_Output_config_mode;

  } else
  {

    Output_method = TMR_Output_normal_mode;

  }

}

void loop() 
{
  if (tmr.tick()) Output_method();
}

void INT0_Encoder()
{
  if (isBackward == true)
  { 

    counter -= length;
    isBackward = false;

  } else
  {
    isForward = true;
  }  
}

void INT1_Encoder()
{
   if (isForward == true)
  { 

    counter += length;
    isForward = false;

  } else
  {
    isBackward = true;
  }

}

void TMR_Output_config_mode()
{
  Serial.println(PrintMeasurements(counter));
  tmr.start();
}

void TMR_Output_normal_mode()
{
  lcd.clear();
  lcd.print(PrintMeasurements(counter)); 
  tmr.start(); 
}

String PrintMeasurements(long toPrint)
{
  String composite = "m: ";

  int metres = toPrint / 10000;
  int millimetres = toPrint % 10000;

  if (toPrint < 0)
  {

     composite += "-";
     metres *= -1;
     millimetres *= -1;

  }

  if (metres == 0)
  {

     composite += "0.";

  } else
  {

     composite += String(metres) + ".";

  }

  if (millimetres/1000 == 0)
    {
      composite += "0";
    }

    if (millimetres/100 == 0)
    {
      composite += "0";
    }

    if (millimetres/10 == 0)
    {
      composite += "0";
    }

  composite += String(millimetres);
  
  return(composite);
}
