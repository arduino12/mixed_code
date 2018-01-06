/*
	Super Stoper V0.0.1.1 Software!
	==========================
	Copyright (C) 2013 Arad Eizen (A.E Tec) LTD INC.
	This Program Count And Calck Running Time For Sports Leassons!
	Main Board: Arduino Pro Mini.
	Language: cxx.
	License For Makif Tet Only!
*/

#include <MY_VS1838B.h>				  // For IR1838B IR Sensor
#include <MY_TM1638.h>				  // For LKM1638 Display Module
#include <MY_DHT11.h>				  // For DHT11 Humidity And Temperature Sensor
#include <EEPROM.h>					  // For Saving Settings
#include "MyTypes.h"				  // For timeB Stract

const int TMP_PIN        = 2;		  // DHT11 Output (Data)
const int CLK_PIN        = 3;		  // LKM1638 Clock
const int DATA_PIN       = 4;		  // LKM1638 Data
const int STB_PIN        = 5;		  // LKM1638 Load (CS)
const int IRCV_PIN       = 6;		  // IR1838B Output (Data)
const int IR_PIN         = 7;		  // IR Output (Activ LOW)
// Key-Matrix = 8 - 11 , A0 - A3	  // 4*4 Key Matrix
const int BIP_PIN        = 12;		  // Bipper (Activ LOW)
const int BUZ_PIN        = 13;		  // Buzzer (Activ HIGH)

const int TMP_BIP        = 3;		  // Short Measurement Bip Timout
const int OFF_BIP        = 5;		  // Short Off Alarm Bip Timout
const int KEY_BIP        = 10;		  // Key press Bip Timout
const int IR_BIP         = 20;		  // IR Pass Bip Timout
const int LAP_BIP        = 40;		  // Lap Completed Bip Timout
const int STP_BIP        = 70;		  // Stoper Ends Bip Timout
const int TIMER_BIP      = 80;		  // Timer Ends Bip Timout
const int TIMER_LED      = 200;		  // Timer Ends LED Timout

const int IR_TIMEOUT     = 300;		  // Ignor IR Sensor Errors
const int KEY_TIMEOUT    = 16000;	  // Timout To Enter Numbers
const int OFF_TIMEOUT    = 6000;	  // Auto Off Alarm Timout (ms)

const byte KEY_DOWN      = 10;		  // Key Down Index
const byte KEY_UP        = 11;		  // Key Up Index
const byte KEY_ENTER     = 12;		  // Key Enter Index
const byte KEY_MODE      = 13;		  // Key Mode Index
const byte KEY_TEST      = 14;		  // Key Test Index
const byte KEY_RESET     = 15;		  // Key Reset Index
const byte KEY_NONE      = 255;		  // No Key Is Pressed Index

const byte SET_BIP       = 1;		  // EEPROM Bip Setting Adress
const byte SET_BRIGHT    = 2;		  // EEPROM Bright Setting Adress
const byte SET_RF        = 3;		  // EEPROM Wireless Setting Adress
const byte SET_AUTO      = 4;		  // EEPROM Auto Power Off Setting Adress

const byte MODE_COUNTER  = 0;		  // Mode Counter Index
const byte MODE_TIMER    = 1;		  // Mode Timer Index
const byte MODE_STOPER   = 2;		  // Mode Stoper Index
const byte MODE_SENSOR   = 3;		  // Mode Sensor Index
const byte MODE_SETTINGS = 4;		  // Mode Settings Index

const byte MAX_LAPS      = 8;		  // Max Number Of Laps In Run
const long MAX_COUNT     = 99999999;  // Display Limit (8 Digit)
const long MAX_TIME      = 599999;	  // Display Limit (6 Digit)

void dht11_wrapper();				  // MY_DHT11 CallBack Function
decode_results results;				  // MY_VS1838B Result Structure
timeB timer, stoper, running;		  // MyTypes TimeB Structure
timeB laps[MAX_LAPS];

MY_TM1638  myDis     (DATA_PIN, CLK_PIN, STB_PIN, false);
MY_DHT11   dhtSensor (TMP_PIN, 0, dht11_wrapper);
MY_VS1838B irRecv    (IRCV_PIN);

boolean key_press   = false;
boolean count_up	= true;
boolean timer_up	= true;
boolean timer_on	= false;
boolean stoper_on   = false;
boolean stoper_lock = false;
boolean powered_off = false;
boolean bip_on, rf_on, auto_on;

byte key_index = KEY_NONE;
byte key_scan  = 0;
byte mode      = 0;
byte setting   = 0;
byte sensor    = 0;
byte cur_lap   = 0;
byte old_lap   = 0;
byte bright;

int off_duration  = OFF_TIMEOUT;
int bip_duration  = 0;
int led_duration  = 0;
int ir_block_time = 0;
int key_timeout   = 0;

long counter, old_millis = 0;
/*****************************************************************/

void setup() // Main
{
	bright  = EEPROM.read(SET_BRIGHT);	// Brightness Value
	bip_on  = EEPROM.read(SET_BIP);		// Bip Value
	rf_on   = EEPROM.read(SET_RF);		// Wireless Value
	auto_on = EEPROM.read(SET_AUTO);	// Auto Power Off Value
	
	myDis.setupDisplay(true, bright);	// Start The Display
	irRecv.enableIRIn(); 				// Start The IR Receiver
	
	pinMode(BIP_PIN, OUTPUT);			// OUTPUT
	pinMode(BUZ_PIN, OUTPUT);			// OUTPUT
	pinMode(IR_PIN,  INPUT_PULLUP);		// INPUT
	
	digitalWrite(BIP_PIN, HIGH);		// Set Bip Off
	digitalWrite(BUZ_PIN, LOW);			// Set Buz Off
	
	DDRB  &= 0xF0;  					// (8 - 11)  INPUT
	PORTB |= 0x0F; 						// (8 - 11)  PULLUP
	DDRC  |= 0x0F;  					// (A0 - A3) OUTPUT

	ZeroTime(&running);					// Init Time

	myDis.setDisplayToString("SUPEr   ");
	NextLed(); NextLed(); NextLed();
	myDis.setDisplayToString("StOPPEr");
	NextLed(); NextLed(); NextLed();
	myDis.setDisplayToString("bY- ArAd", 0x11);
	NextLed(); NextLed(); NextLed(); NextLed();
	myDis.setDisplayToString("--2013--", 0x44);
	NextLed(); NextLed();
	myDis.setDisplayToString("AE TEC  ", 0xC7);
	NextLed(); NextLed(); NextLed(); NextLed();

	RefCount();							// The First Screen
}

void loop() // Infinity Loop
{
	long tmp_millis = millis() / 10;
	byte delta_millis = abs(tmp_millis - old_millis);
	boolean ir_interapt = false;
	boolean led_timeout = false;	
	
	if (delta_millis)
	{
		old_millis = tmp_millis;
		if (bip_duration) if (--bip_duration == 0) digitalWrite(BIP_PIN, HIGH);
		if (led_duration) if (--led_duration == 0) led_timeout = true;
		
		if (auto_on)
		{
			if (off_duration) 
			{
				if (powered_off)
				{
					powered_off = false;
					digitalWrite(BIP_PIN, HIGH);
					myDis.setupDisplay(true, bright);
				}
				off_duration--;
			}
			else
			{
				if ((mode == MODE_TIMER && timer_on) || (mode == MODE_STOPER && stoper_on))
				{
					off_duration = OFF_TIMEOUT;
				}
				else if (!powered_off)
				{
					powered_off = true;
					digitalWrite(BIP_PIN, LOW);				
					myDis.setupDisplay(false, bright);
					//bip_duration = OFF_BIP;
				}
			}
		}
	}
	
	if (ir_block_time) ir_block_time--;
	if (digitalRead(IR_PIN)) 
	{
		if (ir_block_time == 0)
		{
			ir_interapt = true;
			off_duration = OFF_TIMEOUT;
		}
		ir_block_time = IR_TIMEOUT;
	}
	
	ScanKey();
	ScanIR();
	
	switch (mode)
	{
		case MODE_COUNTER:
			switch (key_index)
			{
				case KEY_MODE:
					mode = MODE_TIMER;
					CopyTime(&running, &timer);
					InitTime(&running);
				break;			
				case KEY_RESET:
					key_timeout = 0;
					counter = 0;
					count_up = true;
					RefCount();
				break;
				default:
					if (key_timeout)
					{
						if (key_index < KEY_DOWN)
						{
							key_timeout = KEY_TIMEOUT;
							counter = (counter % 10000000) * 10 + key_index;
							RefCount();
						}
						else if (key_index < KEY_NONE) key_timeout = 0;
						else key_timeout--;
					}
					else
					{
						if (key_index < KEY_DOWN)
						{
							key_timeout = KEY_TIMEOUT;
							counter = key_index;
							count_up = (key_index == 0);
							RefCount();
						}
						else if (key_index < KEY_ENTER)
						{
							count_up = (key_index == KEY_UP);
							StpCount();
						}
						else if (ir_interapt)
						{
							StpCount();
							Bip(IR_BIP);
						}
					}
				break;
			}
		break;
		case MODE_TIMER:
			switch (key_index)
			{
				case KEY_MODE:
					mode = MODE_STOPER;
					RefStoper();
				break;		
				case KEY_ENTER:
					if (timer_on)
					{
						timer_on = false;
						CopyTime(&timer, &running);
					}
					else
					{
						CopyTime(&running, &timer);
						timer_on = true;
					}
					if (bip_duration) Bip(0);
					else Bip(TIMER_BIP);
				break;		
				case KEY_RESET:
					timer_on = false;
					timer_up = true;
					ZeroTime(&timer);
					ZeroTime(&running);
					RefTime(&timer);
				break;
				default:
					if (timer_on)
					{
						if (delta_millis)
						{
							if (timer_up) AddTime(&running, delta_millis);
							else SubTime(&running, delta_millis);
						}
					}
					else
					{
						if (key_timeout)
						{
							if (key_index < KEY_DOWN)
							{
								key_timeout = KEY_TIMEOUT;
								timer.bMin = (timer.bMin % 10) * 10 + timer.bSec / 10;
								timer.bSec = (timer.bSec % 10) * 10 + timer.bMil / 10;							
								timer.bMil = (timer.bMil % 10) * 10 + key_index;
								RefTime(&timer);
							}
							else if (key_index < KEY_NONE) key_timeout = 0;
							else key_timeout--;
						}
						else
						{
							if (key_index < KEY_DOWN)
							{
								key_timeout = KEY_TIMEOUT;
								ZeroTime(&timer);
								timer.bMil = key_index;
								timer_up = (key_index == 0);
								RefTime(&timer);
							}
							else if (key_index < KEY_ENTER)
							{
								timer_up = false;
								if (key_index == KEY_UP) AddTime(&timer, 3000);
								else SubTime(&timer, 3000);
							}
							if (led_timeout) RefTime(&timer);
						}
					}
				break;
			}
		break;
		case MODE_STOPER:
			switch (key_index)
			{
				case KEY_MODE:
					mode = MODE_SENSOR;
					myDis.setLEDs(0);
					RefSensor();
				break;		
				case KEY_ENTER:
					if (stoper_lock) InitStoper();
					else if (stoper_on) NextLap();
					else StartStoper();
				break;		
				case KEY_RESET:
					stoper_on = false;
					old_lap = 0;
					InitStoper();
				break;
				default:
					if (stoper_on)
					{	
						if (ir_interapt) NextLap();
						if (delta_millis) AddTime(&stoper, delta_millis);
					}
					else
					{				
						if (key_index < KEY_DOWN)
						{
							if (key_index && key_index < 9 && key_index != cur_lap + 1) UpdateLap(key_index - 1);
						}
						else if (key_index < KEY_ENTER)
						{
							if (key_index == KEY_UP) UpdateLap(cur_lap + 1);
							else UpdateLap(cur_lap - 1);
						}
						else if (ir_interapt && !stoper_lock)
						{
							StartStoper();
							Bip(IR_BIP);
						}
					}
				break;
			}
		break;
		case MODE_SENSOR:
			switch (key_index)
			{
				case KEY_MODE:
					mode = MODE_SETTINGS;
					RefSetting();
				break;		
				case KEY_ENTER:
					sensor = (sensor ? 0 : 1);
					RefSensor();
				break;		
				case KEY_RESET:
					sensor = 0;
					RefSensor();
				break;
				default:
					if ((key_index == KEY_DOWN) || (key_index == KEY_UP))
					{
						sensor = (sensor ? 0 : 1);
						RefSensor();
					}
					else if (old_millis % 300 == 0)
					{
						UpdateSensor();
						Bip(TMP_BIP);
					}
				break;
			}
		break;
		case MODE_SETTINGS:
			switch (key_index)
			{
				case KEY_MODE:
					mode = MODE_COUNTER;
					RefCount();
				break;		
				case KEY_ENTER:
					switch (setting)
					{
						case 0: UpdateSetting(!bip_on);			 break;
						case 1: UpdateSetting((bright + 1) % 8); break;
						case 2: UpdateSetting(!rf_on); 			 break;
						case 3: UpdateSetting(!auto_on); 		 break;
					}
				break;		
				case KEY_RESET:
					setting = 3;
					UpdateSetting(true);
					setting = 2;
					UpdateSetting(false);					
					setting = 1;
					UpdateSetting(0);	
					setting = 0;
					UpdateSetting(true);
					RefSetting();
				break;
				default:
					if (key_index < KEY_DOWN)
					{
						UpdateSetting(key_index);
					}
					else if (key_index < KEY_ENTER)
					{
						if (key_index == KEY_UP) setting = (setting > 2 ? 0 : setting + 1);
						else setting = (setting ? setting - 1 : 3);
						RefSetting();
					}
				break;
			}
		break;
	}
	
	if (led_timeout) myDis.setLEDs(0);
	
	if (key_index < KEY_NONE)
	{
		key_index = KEY_NONE;
		off_duration = OFF_TIMEOUT;
		Bip(KEY_BIP);
	}
}

void CopyTime(timeB* t1, timeB* t2)
{
	t1->bMil = t2->bMil;
	t1->bSec = t2->bSec;
	t1->bMin = t2->bMin;
}

void ZeroTime(timeB* t)
{
	t->bMil = t->bSec = t->bMin = 0;
}

void FixTime(timeB* t)
{
	t->bMil %= 100;
	t->bSec %=  60;
	t->bMin %= 100;
}

void InitTime(timeB* t)
{
	byte tmp[] = { 0, 0, 64, 0, 0, 64, 0, 0 };
	myDis.setDisplay(tmp);
	RefTime(t);
}

long ToMili(timeB* t)
{
	FixTime (t);
	return (t->bMil + t->bSec * 100 + (long)t->bMin * 6000);
}

void ToTime(timeB* t, long Mili)
{
	t->bMil = Mili % 100;
	Mili /= 100;
	t->bSec = Mili % 60;
	Mili /= 60;
	t->bMin = Mili % 100;
}

void AddTime(timeB* t, int Mili)
{
	ToTime(t, min(MAX_TIME, ToMili(t) + Mili));
	RefTime(t);
}

void SubTime(timeB* t, int Mili)
{
	long tmp = ToMili(t) - Mili;
	if (tmp < 0) 
	{
		tmp = 0;
		ZeroTime(t);
	
		if (timer_on)
		{
			timer_on = false;
			Bip(TIMER_BIP);
			Leds(0xFFFF, TIMER_LED);
		}
	}
	ToTime(t, tmp);
	RefTime(t);
}

void RefTime(timeB* t)
{
	Set2Digit(6, t->bMil);
	Set2Digit(3, t->bSec);
	Set2Digit(0, t->bMin);
}

void Set2Digit(byte start, byte num)
{
	myDis.setDisplayDigit((byte)((int)num % 10), start + 1);
	myDis.setDisplayDigit((byte)((int)num / 10), start);
}

void InitStoper()
{
	stoper_lock = false;
	UpdateLap(old_lap);
	ZeroTime(&stoper);
	RefTime(&stoper);
	for (int i = 0; i < MAX_LAPS; i++)
		ZeroTime(&laps[i]);
}

void StartStoper()
{
	stoper_on = true;
	old_lap = cur_lap;
}

void StopStoper()
{
	stoper_on = false;
	stoper_lock = true;
	Bip(STP_BIP);
}

void NextLap()
{
	CopyTime(&laps[cur_lap], &stoper);
	if (cur_lap)
	{
		UpdateLap(cur_lap - 1);
		Bip(LAP_BIP);
	}
	else StopStoper();
}

void UpdateLap(int id)
{
	if (id < 0) id = MAX_LAPS - 1;
	else if (id >= MAX_LAPS) id = 0;
	myDis.setLED(0, cur_lap);
	cur_lap = id;
	myDis.setLED(3, cur_lap);	
	RefTime(&laps[cur_lap]);
}

void RefStoper()
{
	InitTime(&laps[cur_lap]);
	myDis.setLED(3, cur_lap);
}

void UpdateSensor()
{
	dhtSensor.update();
	switch (sensor)
	{
		case 0: Set2Digit(3, dhtSensor.getCelsius());  break;
		case 1: Set2Digit(3, dhtSensor.getHumidity()); break;
	}
}

void RefSensor()
{
	switch (sensor)
	{
		case 0: myDis.setDisplay((byte[]){0x78, 0x40, 0, 0, 0, 0, 0x63, 0x39}); break; // t-    *C
		case 1: myDis.setDisplay((byte[]){0x74, 0x40, 0, 0, 0, 0, 0, 0x73});    break; // h-     P
		default: return;
	}
	UpdateSensor();
}

void UpdateSetting(byte value)
{
	switch (setting)
	{
		case 0:
			bip_on = (value > 0); 
			EEPROM.write(SET_BIP, bip_on);
			RefSetVal(bip_on);
		break;
		case 1:
			bright = min(value, 7);
			EEPROM.write(SET_BRIGHT, bright);
			myDis.setupDisplay(true, bright);
			RefSetVal(bright);
		break;
		case 2:
			rf_on = (value > 0);
			EEPROM.write(SET_RF, rf_on);
			RefSetVal(rf_on);
		break;
		case 3:
			auto_on = (value > 0);
			EEPROM.write(SET_AUTO, auto_on);
			RefSetVal(auto_on);
		break;		
	}
}

void RefSetting()
{
	switch (setting)
	{
		case 0:
			myDis.setDisplayToString("Sound- ");
			RefSetVal(bip_on);
		break;
		case 1:
			myDis.setDisplayToString("Light- ");
			RefSetVal(bright);
		break;
		case 2:
			myDis.setDisplayToString("rAdio- ");
			RefSetVal(rf_on);
		break;
		case 3:
			myDis.setDisplayToString("AutoF- ");
			RefSetVal(auto_on);
		break;		
	}
}

void RefSetVal(byte value)
{
	myDis.setDisplayDigit(value, 7);
}

void ScanKey()
{
	byte tmp = PINB & 0x0F;

	if (tmp != 0x0F) 
	{
		if (key_press) return;
		key_press = true;
		switch(tmp)
		{
			case 0x0E: tmp = 3; break;
			case 0x0D: tmp = 2; break;
			case 0x0B: tmp = 1; break;
			case 0x07: tmp = 0; break;
		}
		
		switch(tmp + key_scan * 4)
		{
			case 0:  key_index = 1;	 break; // 1
			case 1:  key_index = 2;  break; // 2
			case 2:  key_index = 3;	 break; // 3
			case 3:  key_index = 12; break; // A
			case 4:  key_index = 4;	 break; // 4
			case 5:  key_index = 5;	 break; // 5
			case 6:  key_index = 6;	 break; // 6
			case 7:  key_index = 13; break; // B
			case 8:  key_index = 7;	 break; // 7
			case 9:  key_index = 8;	 break; // 8
			case 10: key_index = 9;	 break; // 9
			case 11: key_index = 14; break; // C
			case 12: key_index = 10; break; // *
			case 13: key_index = 0;	 break; // 0
			case 14: key_index = 11; break; // #
			case 15: key_index = 15; break; // D
		}		
		return;
	}
	
	if (key_press)
	{
		key_press = false;
		key_index = KEY_NONE;
	}
	
	if (key_scan < 3) key_scan++;
	else key_scan = 0;

	PORTC = (PORTC & 0xF0) | (~(1 << key_scan) & 0x0F);
}

void ScanIR()
{
	if (!(irRecv.decode(&results))) return;
	long tmp = results.value;
	irRecv.resume();
	if (tmp == 0xFFFFFFFF) return;
	key_index = IRindex(tmp);
}

void Bip(int duration)
{
	if (!bip_on) return;
	if (duration)
	{
		if (bip_duration)
		{
			if(duration > bip_duration) bip_duration = duration;
		}
		else
		{
			bip_duration = duration;
			digitalWrite(BIP_PIN, LOW);
		}
	}
	else
	{
		bip_duration = 0;
		digitalWrite(BIP_PIN, HIGH);
	}
}

void Leds(word data, int duration)
{
	led_duration = duration;
	myDis.setLEDs(data);
}

void StpCount()
{
	if (count_up)
	{
		if (counter < MAX_COUNT) counter++;
		else counter = 0;
	}
	else
	{
		if (counter) counter--;
		else counter = MAX_COUNT;
	}
	RefCount();
}

void RefCount()
{
	myDis.setDisplayToDecNumber(counter, 0, 0, false);
}

void NextLed()
{
	delay(1200);
	myDis.setLED((old_millis > 7 ? 0 : 3), old_millis++ % 8);
}

byte IRindex(long value)
{
	switch(value & 0xFFFF)
	{
		case 0xA25D: return 13;	// CH-
		case 0x629D: return 12;	// CH
		case 0xE21D: return 14;	// CH+
		case 0x22DD: return 10;	// |<<
		case 0x02FD: return 11;	// >>|
		case 0xC23D: return 12;	//  >|
		case 0xE01F: return 10;	// -
		case 0xA857: return 11;	// +
		case 0x906F: return 15;	// EQ
		case 0x6897: return 0;	// 0
		case 0x30CF: return 1;	// 1
		case 0x18E7: return 2;	// 2
		case 0x7A85: return 3;	// 3
		case 0x10EF: return 4;	// 4
		case 0x38C7: return 5;	// 5
		case 0x5AA5: return 6;	// 6
		case 0x42BD: return 7;	// 7
		case 0x4AB5: return 8;	// 8
		case 0x52AD: return 9;	// 9
		default:	 return KEY_NONE;
	}
}

void dht11_wrapper()
{
	dhtSensor.isrCallback();
}
