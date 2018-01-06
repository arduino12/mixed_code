//{ Includes

	#include <MyMP3.h>
	#include <DS3232RTC.h>
	#include <Time.h>
	#include <Wire.h>

//}

//{ Consts And Defines

	const int RST_PIN = 5;
	const int CLK_PIN = 4;
	const int DAT_PIN = 6;
	const int BUS_PIN = 7;
	
	const int CHARGE_PIN = 8;

	const int ANSWER_PIN = 10;

//}

//{ Veraibles

	MyMP3 myMP3(RST_PIN, CLK_PIN, DAT_PIN, BUS_PIN);

	volatile bool dtmfReady;

	uint8_t curMode, oldMode;
	int curAsteriskNum;
	uint8_t mathGameResult, mathGameCount;
	
	bool oldCharge;
	
	void (*asteriskNum)();
	void getAsteriskNum(void (*asteriskEvent)());
	
//}

//{ Main and Loop

void setup()
{
	srand(analogRead(7));		// Init Randomizer
	setSyncProvider(RTC.get);	//setTime(13,58,0,28, 7, 2013);

	dtmfReady = false;
	DDRC &= 0xF0;
	
	attachInterrupt(0, readDTMF, RISING);
	pinMode(ANSWER_PIN, OUTPUT);
	digitalWrite(ANSWER_PIN, LOW);
	
	pinMode(CHARGE_PIN, OUTPUT);
	digitalWrite(CHARGE_PIN, LOW);
	
	//digitalWrite(CHARGE_PIN, HIGH);
	
	myMP3.reset();
	delay(1000);
	myMP3.volume(5);
	
	//Serial.begin(115200);
	
	curMode = 0;
	oldCharge = false;
}

void loop()
{

	if (oldCharge ^ (((weekday() % 2) == 0) && (hour() == 2) && (minute() < 40))) // ימי שני, רביעי, שישי משעה 2:00 עד 2:40
	{
		oldCharge = !oldCharge;
		digitalWrite(CHARGE_PIN, oldCharge);
		//Serial.print("a ");
	}
	
	if (!dtmfReady || oldCharge) return;
	dtmfReady = false;
	delay(10);
	uint8_t dtmf = PINC & 0x0F;
	
	if (dtmf == 0) // מישו מתקשר
	{
		curMode = 1;
		//delay(1000);
		clickAnswer();
		playFile(8000);
		playFile(8001);
		playFile(8002);
	}
	else if (dtmf == 15) // ארד מתקשר
	{
/*
		curMode = 1;
		clickAnswer();
		getAsteriskNum(&loginPass);
		playFile(8003);
*/
		curMode = 2;
		clickAnswer();
		playFile(8012);
		playMenu();
	}
	else if (dtmf < 11)
	{
		dtmf %= 10;
		if (curMode == 1)
		{
			if (dtmf == 1)
			{
				getAsteriskNum(&loginPass);
				playFile(8003);
			}
			else
			{
				playFile(8001);
				playFile(8002);
			}
		}
		else if (curMode == 2)
		{
			if (dtmf == 1)
			{
				playTime();
				playMenu();
			}
			else if (dtmf == 2)
			{
				mathGameStart(5);
			}
			else if (dtmf == 3)
			{
				playNum(random(101));
				playMenu();
			}
			else if (dtmf == 4)
			{
				curMode = 0;
				playFile(8018);
				playFile(8013 + random(3));
				clickAnswer();
			}
			else if (dtmf == 5)
			{
				//playAll(????); // הטמפ היא
				playNum(RTC.temperature() / 4);
				//playAll(????); // מעלות צלסיוס
				playMenu();
			}
			else
			{
				playMenu();
			}
		}
		else if (curMode == 255)
		{
			curAsteriskNum = curAsteriskNum * 10 + dtmf;
		}
	}
	else if (dtmf == 11) // *
	{
		if (curMode == 255)
		{
			curMode = oldMode;			
			asteriskNum();
		}
	}
	else if (dtmf == 12) // #
	{
		playMenu();
	}
}

//}

//{ Oters

void loginPass()
{
	if (curAsteriskNum == 1234) playAll(8012);
	playMenu();
}

void mathGameStart(uint8_t count)
{
	mathGameCount = count;
	curAsteriskNum = 255;
	mathGameNext();
}

void mathGameNext()
{
	if (curAsteriskNum != 255)
	{
		if (curAsteriskNum == mathGameResult) playAll(8007);
		else playAll(8008);
	}
	
	if (mathGameCount == 0)
	{
		playMenu();
		return;
	}

	uint8_t a = random(101), b = random(101), c = random(2);
	
	if (c) if (b > a) {c = a; a = b; b = c; c = 1;}
	
	playNum(a);
	playAll(8005 + c); // + -
	playNum(b);
	
	mathGameCount--;
	mathGameResult = a + (c ? -b : b);
	getAsteriskNum(&mathGameNext);
}

void playTime()
{
	uint8_t h = hour(), m = minute();
	
	playDay(0); // היום
	playDay(weekday());
	
	//playAll(????); // ה
	//playNum(day());
	//playAll(????); // ל
	//playNum(month());
	//playAll(????); // אלפיים ו
	//playNum(year());
	
	playAll(1120); // השעה היא
	
	if (m == 45)
	{
		playAll(1127); // רבע ל
		h++;
	}
	uint8_t hh = h % 12;
	if (hh == 0) hh = 12;
	playNum(hh);

	if (m == 15)
		playAll(1125); // ורבע
	else if (m == 30)
		playAll(1126); // וחצי
	else if (m == 45)
		m = m;
	else if (m == 1)
		playAll(1128); // ודקה
	else if (m)
	{
		playAll(1130); // ו
		playNum(m);
		playAll(1129); // דקות
	}
	
	if (h < 3 || h > 21)
		playAll(1124); // בלילה
	else if (h < 12)
		playAll(1121); // בבוקר
	else if (h < 18)
		playAll(1122); // בצהריים
	else
		playAll(1123); // בערב

}

void playMenu()
{
	curMode = 2;
	playFile(8004);
	playFile(8009);
	playFile(8010);
	playFile(8011);
}

void getAsteriskNum(void (*asteriskEvent)())
{
	asteriskNum = asteriskEvent;
	curAsteriskNum = 0;
	oldMode = curMode;
	curMode = 255;
}

void playNum(int num)
{
	playFile(102 + num);
}

void playDay(int num)
{
	playFile(1400 + num);
}

void playAll(int num)
{
	myMP3.playAll(num);
	delay(50);
}

void playFile(int num)
{
	if (dtmfReady) return;
	myMP3.play(num);
	while(digitalRead(BUS_PIN) && !dtmfReady);
	if (dtmfReady) myMP3.stop();
	delay(50);
}

void readDTMF()
{
	dtmfReady = true;
}

void clickAnswer()
{
	delay(1000);//delay(200);
	digitalWrite(ANSWER_PIN, HIGH);
	delay(200);//delay(200);
	digitalWrite(ANSWER_PIN, LOW);
	delay(200);
}

//}


