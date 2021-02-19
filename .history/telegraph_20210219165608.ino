/*!
 * Lthiry
 * 2021.02.19
 *
 */
#include "MorseCodeDefs.h"
#include <Wire.h>
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET 4
Adafruit_SSD1306 oled(OLED_RESET);

static const uint8_t PROGMEM Welcome_16x16[] ={
0x00,0x80,0x00,0x80,0xFC,0x80,0x04,0xFC,0x05,0x04,0x49,0x08,0x2A,0x40,0x14,0x40,
0x10,0x40,0x28,0xA0,0x24,0xA0,0x45,0x10,0x81,0x10,0x02,0x08,0x04,0x04,0x08,0x02,//欢0
0x00,0x00,0x20,0x80,0x13,0x3C,0x12,0x24,0x02,0x24,0x02,0x24,0xF2,0x24,0x12,0x24,
0x12,0x24,0x12,0xB4,0x13,0x28,0x12,0x20,0x10,0x20,0x28,0x20,0x47,0xFE,0x00,0x00,//迎1
0x01,0x00,0x01,0x00,0x01,0x00,0x7F,0xFC,0x01,0x00,0x11,0x10,0x09,0x10,0x09,0x20
}; 

// 动态变量
volatile float     n_workMode, n_buttonPress, n_buttonRelease, n_buttonStatus, n_dotFinished, n_indexer, n_AnJianShiJian, n_workModeFirstTime, n_buttonStartTime, n_buttonHoldTime, n_alreadyDot, n_alreadyDash, n_alreadyChar;
bool error = false;
bool dictionaryMode = false;

// 函数声明
void DF_beep(float mind_n_float);
void DF_GongZuoMoShiSheZhi();
void DF_ChuShiHua();
void DF_FaSongMoShi();
void DF_JieShouMoShi();
/* void DF_oeldXianShi(); */

char charStack[5];
int indexer = 0;

// 主程序开始
void setup() {
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
	oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x64)
	DF_ChuShiHua();
}
void loop() {
	DF_GongZuoMoShiSheZhi();
	if ((n_workMode==0)) {
		DF_JieShouMoShi();
	}
	if ((n_workMode==1)) {
		DF_FaSongMoShi();
	}
/* 	DF_oeldXianShi(); */
}


// 自定义函数
void DF_beep(float mind_n_float) {
	for (int index = 0; index < mind_n_float; index++) {
		digitalWrite(11, LOW);
		delay(100);
		digitalWrite(11, LOW);
		delay(100);
	}
}
void DF_GongZuoMoShiSheZhi() {
	if (((digitalRead(9))==0)) {
		n_workMode = 0;
	}
	else {
		n_workMode = 1;
	}
}
void DF_ChuShiHua() {
	pinMode(4, INPUT);
	pinMode(5, OUTPUT);
	pinMode(6, OUTPUT);
	pinMode(7, OUTPUT);
	pinMode(8, OUTPUT);
	pinMode(9, INPUT_PULLUP);
	pinMode(11, OUTPUT);
	pinMode(A0, OUTPUT);
	pinMode(A1, INPUT);
	digitalWrite(5, LOW);
	digitalWrite(6, LOW);
	digitalWrite(7, LOW);
	digitalWrite(8, LOW);
	DF_beep(1);
	n_buttonPress = 0;
	n_buttonRelease = 0;
	n_buttonStatus = 0;
	n_dotFinished = 0;
	n_workMode = 0;
	n_indexer = 0;
	n_buttonStartTime = 0;
	n_buttonHoldTime = 0;
	n_alreadyDot = 0;
	n_alreadyDash = 0;
	n_alreadyChar = 0;
	
	oled.setRotation(2); //显示翻转180度。0:不变，1：转90度，2：转180度。
	//15.显示多个文字（楼主在库里面加入一个ShowCN_16方法 就是为了同时显示多个中文 16X16 ,不然的话 就得一个一个drawBitmap）
	oled.clearDisplay(); // clears the screen and buffer
	oled.ShowCN_16(0,0, Welcome_16x16,sizeof(Welcome_16x16)/32,WHITE);
	oled.display();
	delay(1000);
	oled.clearDisplay(); // clears the screen and buffer
	
	// text display tests
	oled.clearDisplay(); // clears the screen and buffer
	oled.setTextSize(2);
	oled.setTextColor(WHITE);
	oled.setCursor(0,0);
	oled.print("OLD");
	oled.display();
	delay(2000); 
	oled.clearDisplay(); // clears the screen and buffer
}

void DF_FaSongMoShi() {
	oled.setTextSize(1);
	oled.setCursor(0,48);
	oled.println("Send");
	oled.display();
	//
	if (((digitalRead(4)==0) && (n_buttonStatus==0))) {	
		n_buttonPress = millis();
		analogWrite(6, 5);
		digitalWrite(11, LOW);
		digitalWrite(A0, HIGH);
		n_buttonStatus = 1;
	}
	if (((digitalRead(4)==0) && (n_buttonStatus==1))) {
		analogWrite(6, 0);
		digitalWrite(11, LOW);
		digitalWrite(A0, HIGH);
		n_AnJianShiJian = (millis() - n_buttonPress);
		if ((n_AnJianShiJian<200)) {
			if(n_alreadyDot == 0){
				n_alreadyDot = 1;
				oled.setCursor((n_indexer * 8), 0);
				oled.print(".");
				oled.display();
				charStack[indexer] = '.';
			}
		}
		else if ((n_AnJianShiJian<400)) {
			if(n_alreadyDash == 0){
				n_alreadyDash = 1;
				oled.setCursor((n_indexer * 8), 0);
				oled.print("-");
				oled.display();
				charStack[indexer] = '-';
			}
		}
		if ((n_AnJianShiJian>800)){
			if(n_alreadyChar == 0){
				n_alreadyChar = 1;
				checkString(charStack);
				indexer = 0;
				delay(3000);
				memset(&charStack[0], 0, sizeof(charStack));
			}
		}
	}
	if (((digitalRead(4)==1) && (n_buttonStatus==1))) {
		n_dotFinished = 1;
		n_buttonRelease = millis();
		analogWrite(6, 0);
		digitalWrite(11, LOW);
		digitalWrite(A0, LOW);
		n_buttonStatus = 0;
		n_alreadyDot = 0;
		n_alreadyDash = 0;
		n_alreadyChar = 0;
		indexer++;
		n_indexer++;
	}
}
void DF_JieShouMoShi() {
	oled.setTextSize(1);
	oled.setCursor(0,48);
	oled.println("Receive");
	oled.display();
	digitalWrite(A0, HIGH);
	if (((digitalRead(A1)==0) && (n_buttonStatus==0))) {
		n_buttonStatus = 1;
		n_buttonPress = millis();
		analogWrite(6, 5);
		digitalWrite(11, LOW);
	}
	if (((digitalRead(A1)==1) && (n_buttonStatus==1))) {
		n_buttonStatus = 0;
		n_dotFinished = 1;
		n_buttonRelease = millis();
		analogWrite(6, 0);
		digitalWrite(11, LOW);
	}
}
/* void DF_oeldXianShi() {
	if ((n_dotFinished==1)) {
		n_dotFinished = 0;
		n_AnJianShiJian = (n_buttonRelease - n_buttonPress);
		if ((n_AnJianShiJian<200)) {
			oled.setCursor((n_indexer * 8), 0);
			oled.print(".");
			oled.display();
			charStack[indexer] = '.';
			indexer++;
			n_indexer += 1;
		}
		else if ((n_AnJianShiJian<500)) {
			oled.setCursor((n_indexer * 8), 0);
			oled.print("-");
			oled.display();
			charStack[indexer] = '-';
			n_indexer += 1;
			indexer++;
		}
		else {
		}
	}
} */


void throwTooManySymbolsError() {
	indexer = 0;
	memset(&charStack[0], 0, sizeof(charStack));
	oled.setTextSize(1);
	oled.clearDisplay(); // clears the screen and buffer
	oled.setCursor(0, 0);
	oled.println("Too many morse");
	oled.println("code symbols.");
	oled.println("Press any button");
	oled.print("to dismiss.");
	oled.display();
}

void throwUnknownCharacterError() {
	indexer = 0;
	memset(&charStack[0], 0, sizeof(charStack));
	oled.setTextSize(1);
	oled.clearDisplay(); // clears the screen and buffer
	oled.setCursor(0, 0);
	oled.println("No character ");
	oled.println("for this combo.");
	oled.println("Wait 2 second");
	oled.print("to dismiss.");
	delay(2000);
	oled.clearDisplay(); // clears the screen and buffer
	oled.display();
}


//Can't really pass char arrays. Instead, the pointer gets passed, thus char compare[] turns into char * compare
void checkString(char * compare) {
	//Unwrap the pointer to the array into a "real" array
	char buf[5];
	for (int i = 0; i < 5; i++) {
		buf[i] = compare[i];
	}
	int correctIndex = -1;
	for (int iterator = 0; iterator < 26; iterator++) {
		char buf2[5];
		strcpy_P(buf2, (char*)pgm_read_word(&(morse_table[iterator])));
		if (strcmp(buf, buf2) == 0) {
			correctIndex = iterator;
		}
	}
	//Did not find the character. This is an error.
	if (correctIndex == -1) {
		throwUnknownCharacterError();
	}
	else {
		char characterBuffer = pgm_read_byte(alphabet + correctIndex);
		oled.setTextSize(2);
		n_indexer -= 1;
		oled.setCursor((n_indexer * 8), 10);
		oled.print(characterBuffer);
		oled.display();
		n_indexer += 2;
	}
}
