#include <Adafruit_LiquidCrystal.h>
#include <LiquidCrystal_I2C.h> // подключаем библиотеку для QAPASS 16x2

LiquidCrystal_I2C LCD(0x27,16,2); // присваиваем имя LCD для дисплея

enum state  {Setup1, Setup2, Timer_game, Deploy};
int current_state = Setup1;
int time_on  = 1;
int time_off = 1;
uint32_t tic = 0;
uint32_t tac = 0;
uint32_t myTimer = 0;
uint32_t timerStop = 0;

bool DEBUG = true;


// Функуия вывода на LCD дисплей чразу в 2 строчки.
void printlsd (String str1, String str2){
  /*lcd_1.clear();
  lcd_1.setCursor(0, 0);
  lcd_1.print(str1);
  lcd_1.setCursor(0, 1);
  lcd_1.print(str2);
 */ 
  LCD.clear();
  LCD.setCursor(0, 0);    // ставим курсор на 1 символ первой строки
  LCD.print(str1);
  LCD.setCursor(0,1);
  LCD.print(str2);
  
  if(DEBUG) {
    Serial.println(String(">>> ") + String(str1) + String(" ") + String(str2) + 
                   String(" | ") + String(tic)  + String(" ") + String(tac));
  }
}

void setup() {
  LCD.init();    // инициализация LCD дисплея
  LCD.backlight(); // включение подсветки дисплея
  Serial.begin(9600);
  pinMode(3, INPUT_PULLUP); // старт
  pinMode(4, INPUT_PULLUP); // стоп
  pinMode(5, INPUT_PULLUP); // верх
  pinMode(6, INPUT_PULLUP); // вниз
}

///////////////////// 2 Vatiant /////////////////////////////
  
void loop() {
  // Set Value
  bool buttonPlus = false;
  bool buttonMinus = false;
  bool buttonStart = false;
  bool buttonStop = false;
  bool timeEvent = false;
  bool pushButton = false;
  
  if (digitalRead(3) == 0 || digitalRead(4) == 0 ||
      digitalRead(5) == 0 || digitalRead(6) == 0   ) {
    delay(200);
    if (digitalRead(5) == 0) buttonPlus = true,  pushButton = true;
    if (digitalRead(6) == 0) buttonMinus = true, pushButton = true;
    if (digitalRead(3) == 0) buttonStart = true, pushButton = true;
    if (digitalRead(4) == 0) buttonStop = true,  pushButton = true;

  }
    
  if (millis() - myTimer >= 1000) {
    myTimer = millis();
    timeEvent = true;
  }
    
  // Set State
  if (current_state == Setup1) {
    if (buttonPlus)          			time_on ++;
    else if (buttonMinus) 				time_on --;
  	else if (buttonStart) 				current_state = Setup2, tic = 0, tac = 0;
		//else if (buttonStop)					continue;
    
    if (time_on < 1)				      time_on = 1; // нужно, чтобы не задавать время с минусом 
    if (timeEvent || pushButton) 	printlsd(String("Setup 1 "), String("time: ") + String(time_on)); // Вывод на экран состояния
  } 
  
  else if (current_state == Setup2) {
    if (buttonPlus)          			time_off ++;
    else if (buttonMinus) 				time_off --;
  	else if (buttonStart) 				current_state = Timer_game, tic = 0, tac = 0;
	  else if (buttonStop)					current_state = Setup1;
    
    if (time_off < 1)				      time_off = 1; // нужно, чтобы не задавать время с минусом 
    if (timeEvent || pushButton) 	printlsd(String("Setup 2 "), String("time: ") + String(time_off)); // Вывод на экран состояния
  } 
  
  else if (current_state == Timer_game) {
    if (tic < time_on){
      if (timeEvent) {
        printlsd(String("Game"), String("time: ") + String(time_on - tic)); // Вывод на экран состояния 
      	tic++;
      }
    } else {
      current_state = Deploy; 
      tic = 0, tac = 0;
    }
  } 
  
	else if (current_state == Deploy) {
    if (tac < time_off){
      if (timeEvent) {
        printlsd(String("deploy"), String("time: ") + String(time_off - tac)); // Вывод на экран состояния 
      	tac++;
      }
    } else {
    	current_state = Timer_game; 
      tic = 0, tac = 0;
    }
  }
  
  
  if((current_state == Timer_game || current_state == Deploy) && buttonStop) {
    if (millis() - timerStop >= 5000) {
      timerStop = millis(); // сбросить таймер
      current_state = Setup1;  
    }

  }
}

