#include <LiquidCrystal.h>

#include "FPS_GT511C3.h"
#include "SoftwareSerial.h"

#include <Servo.h> 

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);  //RS,EN,D4,D5,D6,D7

FPS_GT511C3 fps(6, 7);

Servo riceservo;
Servo sugarservo;
Servo pulseservo;

const int EnrollPin = A3;
const int EnterPin = A4;

const int RicePin = A0;
const int SugarPin = A1;
const int PulsePin = A2;

const int g_led = 0;
const int w_led = 0;

int state = 0;

void Enroll(void);
int ID_check(void);

int verified = 0;

void setup() {
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("  AUTOMATIC");
  lcd.setCursor(0,1);
  lcd.print("RASHAN SUPPLY");
  delay(2000);
  lcd.clear();
  fps.Open();
  fps.SetLED(true);  
  
//  Serial.begin(9600);
  

  
  pinMode(RicePin, INPUT);
  pinMode(SugarPin, INPUT);
  pinMode(PulsePin, INPUT);
  pinMode(EnrollPin, INPUT);
  pinMode(EnterPin, INPUT);
  
  pinMode(g_led, OUTPUT);
  pinMode(w_led, OUTPUT);
  digitalWrite(g_led,LOW);
  digitalWrite(w_led,LOW);  
}

void loop() {
  
  switch(state)
  {
    case 0:
      lcd.setCursor(0, 0);
      lcd.print("ENTER CHOICE    "); 
      lcd.setCursor(0, 1);
      lcd.print("1:REG  2:DISP   ");       
      if(!digitalRead(EnrollPin))
      {
        state = 1;
        lcd.clear();
        lcd.print("ENROLL MODE");
        delay(2000);
      }
      else if(!digitalRead(EnterPin))
      {
        state = 2;
        lcd.clear();
        lcd.print("RASHAN MODE");        
        delay(2000);
      }      
    break;
    case 1:  //Enrollment
      Enroll();
      state = 0;
    break;
    case 2:  //ID Verification
      lcd.clear();
      lcd.print("Press Finger");
      if(ID_check())
      {
        riceservo.attach(4);
        sugarservo.attach(3);
        pulseservo.attach(2);
        state = 3;
        lcd.clear();
        lcd.print("ENTER CHOICE    ");
        lcd.setCursor(0,1);
        lcd.print("1:   2:   3:    ");
      }
      if(!digitalRead(EnterPin))
      {
//        riceservo.detach(4);
//        sugarservo.detach(3);
//        pulseservo.detach(2);        
        state = 0;
        delay(1000);
      }
    break;
    case 3:  //Rashan Supply
      if(!digitalRead(RicePin))
      {
        riceservo.write(0);
        delay(3000);
        riceservo.write(90);
        delay(1000);
      }
      else if(!digitalRead(SugarPin))
      {
        sugarservo.write(0);
        delay(3000);
        sugarservo.write(90);
        delay(1000);        
      }
      else if(!digitalRead(PulsePin))
      {
        pulseservo.write(0);
        delay(3000);
        pulseservo.write(90);
        delay(1000);        
      }      
    break;
    default:
    break;
  }
}

void Enroll()
{
	// Enroll test

	// find open enroll id
	int enrollid = 0;
	bool usedid = true;
	while (usedid == true)
	{
		usedid = fps.CheckEnrolled(enrollid);
		if (usedid==true) enrollid++;
	}
	fps.EnrollStart(enrollid);

	// enroll
        lcd.clear();
	lcd.print("Press finger ");
	lcd.print(enrollid);
	while(fps.IsPressFinger() == false) delay(100);
	bool bret = fps.CaptureFinger(true);
	int iret = 0;
	if (bret != false)
	{
                lcd.clear();
		lcd.print("Remove finger");
		fps.Enroll1(); 
		while(fps.IsPressFinger() == true) delay(100);
		lcd.clear();
                lcd.print("Press same");
                lcd.setCursor(0,1);
                lcd.print("finger again");
		while(fps.IsPressFinger() == false) delay(100);
		bret = fps.CaptureFinger(true);
		if (bret != false)
		{
                        lcd.clear();
			lcd.print("Remove finger");
			fps.Enroll2();
			while(fps.IsPressFinger() == true) delay(100);
		        lcd.clear();
                        lcd.print("Press same");
                        lcd.setCursor(0,1);
                        lcd.print("finger again");
			while(fps.IsPressFinger() == false) delay(100);
			bret = fps.CaptureFinger(true);
			if (bret != false)
			{
                                lcd.clear();
				lcd.print("Remove finger");
				iret = fps.Enroll3();
				if (iret == 0)
				{
                                        lcd.clear();
					lcd.print("Successfull");
				}
				else
				{
                                        lcd.clear();
					lcd.print("Unsuccessful");
					lcd.print(iret);
				}
			}
			else 
                        {
		          lcd.clear();
                          lcd.print("FAILED TO");
                          lcd.setCursor(0,1);
                          lcd.print("CAPTURE 3");                          
                          //Serial.println("Failed to capture third finger");
                        }
		}
		else 
                {
		  lcd.clear();
                  lcd.print("FAILED TO");
                  lcd.setCursor(0,1);
                  lcd.print("CAPTURE 2");                  
                  //Serial.println("Failed to capture second finger");
                }
	}
	else 
        {
          lcd.clear();
          lcd.print("FAILED TO");
          lcd.setCursor(0,1);
          lcd.print("CAPTURE 1");          
          //Serial.println("Failed to capture first finger");
        }
}

int ID_check()
{
  	// Identify fingerprint test
	if (fps.IsPressFinger())
	{
		fps.CaptureFinger(false);
		int id = fps.Identify1_N();
		if (id <200)
		{
                        lcd.clear();
			lcd.print("Verified ID:");
			lcd.print(id);
                        delay(1000);
                        return 1;
		}
		else
		{
                        lcd.clear();
			lcd.print("Finger not found");
                        delay(1000);
                        return 0;
		}
	}
	else
	{
          return 0;
          lcd.clear();
          lcd.print("Finger not psnt");
		//Serial.println("Please press finger");
	}
	delay(100);
}
