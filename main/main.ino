#include <DS3231.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x20, 16, 2);

// Clock
DS3231 clk;
RTClib rtc;
int hour, minute, sec;
#define buzzer 8
#define NUM_SETTINGS 3
#define NUM_WEEKDAYS 7
#define NUM_HOURS 24
#define NUM_MINS 60
const int settings[NUM_SETTINGS] = {2, 18, 19};
const String days_of_week[NUM_WEEKDAYS] = {"MON", "TUE", "WED", "THU", "FRI", "SAT", "SUN"};
bool prev_setting_state[NUM_SETTINGS];
volatile int settings_mode;
volatile int settings_col;
volatile byte settings_val[NUM_SETTINGS];

// constants
enum operations {
    ADD = 0,
    SUB = 1,
    MULT = 2,
    DIV = 3
};
enum gameType {
    MENTAL_MATH = 0,
    COUNTING = 1
};
#define LEN_NUMS 8
#define NUM_OPS 3
#define START_BUTTON 29
String op_chars[NUM_OPS] = {"+","-","*"};
int num_order[LEN_NUMS];
String correct_order = "";

// Keypad
long target_num;
int temp;
char input;
int waiting, running_count;
bool r;
unsigned long my_time, prev_time;

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {42,  52, 50, 46}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {44, 40, 48}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
int cursorColumn = 0;

int ledPin[4] = {4, 5, 6, 7};
bool game_not_completed = true;
bool game_started = false;
int game;
int number_of_tries = 0;

bool isAlarmOn = false;
bool h12Flag;
bool pmFlag;
bool century = false;

void setup()
{
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  DateTime now = rtc.now();
  randomSeed(now.year() + now.hour() + now.minute() + now.second());
  Wire.begin();
  
  pinMode(ledPin[0], OUTPUT);
  pinMode(ledPin[1], OUTPUT);
  pinMode(ledPin[2], OUTPUT);
  pinMode(ledPin[3], OUTPUT);
  pinMode(buzzer, OUTPUT);
  
  for(int i = 0; i < LEN_NUMS; i++) {
    pinMode(START_BUTTON-i, INPUT);
  }
  for(int i = 0; i < NUM_SETTINGS; i++) {
    pinMode(settings[i], INPUT);
    prev_setting_state[i] = false;
  }
  settings_mode = 0;
  settings_col = -1;
  
  // interrupts
  attachInterrupt(digitalPinToInterrupt(settings[0]), settingMode1, FALLING);
  attachInterrupt(digitalPinToInterrupt(settings[1]), settingMode2, FALLING);
  attachInterrupt(digitalPinToInterrupt(settings[2]), settingMode3, FALLING);

  isAlarmOn = false;
  clk.turnOnAlarm(1);
  byte alarmDay, alarmHour, alarmMinute, alarmSecond, alarmBits;
  bool alarmDy, alarmH12Flag, alarmPmFlag;
  clk.getA1Time(alarmDay, alarmHour, alarmMinute, alarmSecond, alarmBits, alarmDy, alarmH12Flag, alarmPmFlag);
  Serial.println(String(alarmDay) + " " + String(alarmHour) + " " + String(alarmMinute) + " " + String(alarmSecond));
  settings_val[0] = alarmDay - 1;
  settings_val[1] = alarmHour;
  settings_val[2] = alarmMinute;
}

void loop() {
  main_loop();
}

void main_loop() {
  Serial.println(String(clk.getHour(h12Flag, pmFlag)) + ":" + String(clk.getMinute()));
  Serial.println(isAlarmOn);
  if(!clk.checkIfAlarm(1) && !isAlarmOn) {
    if(settings_mode == 0) {
      print_time();
      delay(1000);
    } else if (settings_mode < 4){
      display_alarm();
      delay(1000);
    } else {
      save_settings();
      settings_mode = 0;
    }
  } else {
    isAlarmOn = true;
    buzz_on();
    game_loop();
  }
}
//int game_num=0;
void game_loop()
{
  if(!game_started) {
    game = random(0, 3);
//    game = game_num;
    game_started = true;
    number_of_tries = 0;
  }
  if(game_not_completed) {
    game_not_completed = !get_game();
    if(!game_not_completed) {
      delay(1000);
      lcd.clear();
      buzz_off(); 
      isAlarmOn = false; 
      game_started = false;  
      game_not_completed = true;
//      game_num = (game_num+1)%4;
    } else {
      number_of_tries += 1;
    }
    if(number_of_tries > 3) {
      String msg[] = {"You have failed", "4 times.", "Hopefully, you", "are awake now."};
      lcd_print(msg, 4, 1500);
      delay(2000);
      lcd.clear();
      // back to displaying time     
      buzz_off(); 
      isAlarmOn = false; 
      game_started = false;  
      game_not_completed = true;
    }
  }
}

// Games
bool get_game() {
  switch(game) {
    case 0: return mental_math();
    case 1: return counting();
    case 2: return dec_to_binary();
    case 3: return binary_to_dec();
  }
}
bool mental_math(){
  int answer;
  int operation = random(0, NUM_OPS);
  int numbers[2];
  numbers[0] = random(0, 100);
  numbers[1] = random(0, numbers[0]);
  switch (operation) {
    case ADD: answer = numbers[0] + numbers[1]; break;
    case SUB: answer = numbers[0] - numbers[1]; break;
    case MULT: 
      numbers[0] = random(0, 12);
      numbers[1] = random(0, 12);
      answer = numbers[0] * numbers[1]; 
      break;
    case DIV: answer = numbers[0]/numbers[1]; break;
    default: answer = 0; break;
  }
  // ask question
  String msg[] = {"What is " + String(numbers[0]) + op_chars[operation] + String(numbers[1]) + "?", "answer: "};
  lcd_print(msg, 2, 0);

  String input_ans = poll_keypad();
  return print_result_msg(input_ans.equals(String(answer)));
}
bool counting(){
  for(int i=0; i < LEN_NUMS; i++){
      num_order[i] = i;
  }
  shuffle();
  String curr_count = display_count();

  String msg1[] = {"Put numbers in", "correct order", curr_count, "answer: "};
  lcd_print(msg1, 4, 2000);
  
  String ans="";
  int prev_states[LEN_NUMS];
  for(int i=0; i < LEN_NUMS; i++){
    prev_states[i] = 0;
  }
  while (1){
    for(int i = 0; i < LEN_NUMS; i++){
      int state = digitalRead(START_BUTTON - i);
      if(state) {
        prev_states[i] = state;
      } else {
        if(prev_states[i] == 1){
          lcd.print(num_order[i]);
          ans += String(num_order[i]);
          prev_states[i] = 0;
        }
      }
    }
    if(ans.length() >= LEN_NUMS) {
      break;
    }
  }
  bool isCorrect = true;
  for(int i = 0; i < LEN_NUMS; i++) {
    if(!String(ans[i]).equals(String(i))) {
      isCorrect = false;
      break;
    }
  }

  print_result_msg(isCorrect);
  
  return isCorrect;
}
void shuffle() {
  for(int i=0; i < 1000; i++) {
    int idx1 = random(0, LEN_NUMS);
    int idx2 = random(0, LEN_NUMS);

    int temp = num_order[idx1];
    num_order[idx1] = num_order[idx2];
    num_order[idx2] = temp;
  }
}
String display_count() {
  String count_str = "";
  String c_order[LEN_NUMS];
  for(int i=0; i < LEN_NUMS; i++){
    count_str += String(num_order[i]) + " ";
    c_order[num_order[i]] = String(i);
  }
  return count_str;
}


bool dec_to_binary(){
  int target_dec_num = random(0, 16);
  int temp = target_dec_num;
  String answer = "";

  // converting to binary
  while(temp > 0 || answer.length() < 4) {
    int remainder = temp % 2;
    answer = String(remainder) + answer;
    temp = temp/2;
  }
  Serial.println("Target Number: " + String(target_dec_num) + ", answer: " + String(answer));
  
  String msg[] = {"convert " + String(target_dec_num) + " to 4-", "bit binary: "};
  lcd_print(msg, 2, 0);

  String input_ans = poll_keypad();
  return print_result_msg(input_ans.equals(answer));
}

bool binary_to_dec(){
  target_num = random(0, 16); //for 4 leds, we can have 0 to 15
  temp = target_num;
  
  Serial.println(temp);
  String binary = "";
  
  //turn on LEDs in binary
  if(temp%2==1){
    digitalWrite(ledPin[0], HIGH);
    temp=(temp-1)/2;
    binary = "1" + binary;
  } else{
    digitalWrite(ledPin[0], LOW);
    temp = temp/2;
    binary = "0" + binary;
  }
  
  if ((temp)%2==1){
    //turn on led1
    digitalWrite(ledPin[1], HIGH);
    temp = (temp-1)/2;
    binary = "1" + binary;
  } else {
    digitalWrite(ledPin[1], LOW);
    temp = temp/2;
    binary = "0" + binary;
  }
  
  if ((temp)%2==1){
    //turn on led2
    digitalWrite(ledPin[2], HIGH);
    temp = (temp-1)/2;
    binary = "1" + binary;
  } else {
    digitalWrite(ledPin[2], LOW);
    temp = temp/2;
    binary = "0" + binary;
  }
  
  if ((temp)%2==1){
    //turn on led3
    digitalWrite(ledPin[3], HIGH);
    temp = (temp-1)/2;
    binary = "1" + binary;
  } else {
    digitalWrite(ledPin[3], LOW);
    temp = temp/2;
    binary = "0" + binary;
  }

  String msg[] = {"convert LEDs to", "decimal: "};
  lcd_print(msg, 2, 0);
  
  String answer = poll_keypad();
  
  //turn off lights
  for(int i=0; i < 4; i++) {
    digitalWrite(ledPin[i], LOW);
  }

  return print_result_msg(answer.equals(String(target_num)));
}

//------------------------------------------------- LCD FUNCTIONS ------------------------------------------------------------------------------
void lcd_print(String lines[], int len, int read_time) {
  for(int i=0; i < len; i+=2){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(lines[i]);
    lcd.setCursor(0,1);
    lcd.print(lines[i+1]);
    if(i+1 != len-1) {
      Serial.println("delay...");
      delay(read_time);
    }
  }
}

bool print_result_msg(bool result) {
  if (result){
    //user input is correct
    String msg[5][2] = {
      {"Your answer is", "correct!"},
      {"Good job!", ""},
      {"Awesome! Have a", "great day :)"},
      {"You rock!", ""},
      {"Amazing!", ""}
    };
    lcd_print(msg[random(0,5)], 2, 0);
    delay(1500);
    return true;
  } else {
    //user input incorrect
    String msg[5][2] = {
      {"Incorrect!", "Try again."},
      {"Why do you even", "try"},
      {"ur a failure", ""},
      {"can u not do", "basic math"},
      {"u r unfit for", "this"}
    };
    lcd_print(msg[random(0,5)], 2, 0);
    delay(2000);
    return false;
  }
}
// ------------------------------------------------ POLLING & INTERRUPTS ------------------------------------------------------------------------
String poll_keypad() {
  input = keypad.getKey();
  waiting = 1;
  unsigned long start = millis();
  running_count = 0;
  String answer = "";
  
  while (waiting){
    my_time = millis();
    if (my_time-start > 50000 or input == '#' or input == '*'){ //condition to exit the wait
      waiting = 0;
      Serial.println("Your answer: " + answer + " Correct answer: " + String(target_num));
    } else if (input != NO_KEY) { // assuming input is a valid digit
      running_count = running_count * 10 + int(input);
      answer += String(input);
      lcd.print(input);
    }
    // in the case that input == NO_KEY just get the next input
    input = keypad.getKey();
  }
  return answer;
}
// ISRs
void settingMode1(){
  if(settings_mode >= 1){
    settings_mode = 4;
    settings_col = -1;   
  } else {      
    settings_mode = 1;
  }
}
void settingMode2(){
  settings_mode = 2;
  settings_col = (settings_col + 1) % 3;
}
void settingMode3(){
  settings_mode = 3;
  switch(settings_col){
    case 0: settings_val[0] = (settings_val[0] + 1) % 7; break;
    case 1: settings_val[1] = (settings_val[1] + 1) % 24; break;
    case 2: settings_val[2] = (settings_val[2] + 5) % 60; break;
  }
}
// ------------------------------------------------ ALARM & CLOCK CODE --------------------------------------------------------------------------
// Alarm & Time
void print_time() {
  String fields[6] = {String(clk.getYear()), String(clk.getMonth(century)), 
                      String(clk.getDate()), String(clk.getHour(h12Flag, pmFlag)), 
                      String(clk.getMinute()), String(clk.getSecond())};
  // formatting
  for(int i=0; i < 6; i++) {
    if(fields[i].toInt() < 10) {
      fields[i] = "0" + fields[i];
    }
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Date: " + fields[0] + "-" + fields[1] + "-" + fields[2]);
  lcd.setCursor(0,1);
  lcd.print("Time: " + fields[3] + ":" + fields[4] + ":" + fields[5]);
}

void buzz_on() {
  digitalWrite(buzzer, HIGH);
}

void buzz_off() {
  digitalWrite(buzzer, LOW);
}

// different state functions
void display_alarm() {  
  // formatting
  String fields[3] = {" " + days_of_week[settings_val[0]], String(settings_val[1]), String(settings_val[2])};
  if(settings_val[1] < 10) {
    fields[1] = "0" + fields[1];
  }
  if(settings_val[2] < 10) {
    fields[2] = "0" + fields[2];
  }
  fields[1] = " " + fields[1] + " ";
  fields[2] = " " + fields[2] + " ";
  if(settings_mode == 2 or settings_mode == 3) {
    fields[settings_col][0] = '*';
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Alarm Details: ");
  lcd.setCursor(0,1);
  lcd.print(fields[0] + " " + fields[1] + ":" + fields[2]);
}
void save_settings() {
  clk.setA1Time(settings_val[0] + 1, settings_val[1], settings_val[2], 0, 0x0, true, false, false);
}
