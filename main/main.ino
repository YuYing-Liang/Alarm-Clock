#include <LiquidCrystal.h>
#include <Key.h>
#include <Keypad.h>
/*
 *   The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 */
 
// IO initialization
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

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
#define LEN_NUMS 10
#define NUM_OPS 3
String op_chars[NUM_OPS] = {"+","-","×"};
int num_order[LEN_NUMS];
String correct_order = "";

// Keypad
long target_num;
int temp;
char input;
int waiting, running_count, r;
unsigned long my_time, prev_time;

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {8, 7, 6}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );


void setup()
{
  lcd.begin(16, 2);
  Serial.begin(9600);
}

void loop()
{
	// lcd.setCursor(0, 1);
  // lcd.print(millis() / 1000);
  counting();
}

// reset all i/o modules e.g. clear lcd screen, 6-seg displays etc.
void clear_io(){

}

// Games
void get_game() {
  int game = rand() % 4;
  switch(game) {
    case 0: mental_math(); break;
    case 1: counting(); break;
    case 2: dec_to_binary(); break;
    case 3: binary_to_dec(); break;
  }
}
void mental_math(){
  int answer;
  int operation = rand() % NUM_OPS;
  int numbers[2];
  numbers[0] = rand() % 100;
  numbers[1] = rand() % 100;
  switch (operation) {
    case ADD: answer = numbers[0] + numbers[1]; break;
    case SUB: answer = numbers[0] - numbers[1]; break;
    case MULT: 
      numbers[0] = rand() % 12;
      numbers[1] = rand() % 12;
      answer = numbers[0] * numbers[1]; 
      break;
    case DIV: answer = numbers[0]/numbers[1]; break;
    default: answer = 0; break;
  }
  // ask question
  while(1) {
    Serial.println("What is " + String(numbers[0]) + " " + op_chars[operation] + " " + String(numbers[1]) + "?");
    // wait for answer
    int ans;
    while (!Serial.available()) {}
    while (Serial.available()) {
      ans = Serial.readString().toInt();
      if(Serial.read() == 13) break; 
    }
    if(ans == answer) {
      Serial.println("Correct!");
      break;
    } else {
      Serial.println("Incorrect: " + String(ans) + " Try again.");
    }
  }
}
void counting(){
  for(int i=0; i < LEN_NUMS; i++){
      num_order[i] = i;
  }
  shuffle();
  while (1) {
    String answer;
    Serial.println("Please put these numbers in the right order based on location (space separated): ");
    Serial.println(display_count());
    Serial.println("0 1 2 3 4 5 6 7 8 9");
    while (!Serial.available()) {}
    while (Serial.available()) {
      answer = Serial.readString();
      if(Serial.read() == 13) break; 
    }
    answer.trim();
    if(answer.equals(correct_order)) {
      Serial.println("This is correct!");
      break;
    } else {
      Serial.println("Incorrect answer: " + answer + " Please try again");
    }
  }
}
void shuffle() {
  for(int i=0; i < 1000; i++) {
    int idx1 = rand() % LEN_NUMS;
    int idx2 = rand() % LEN_NUMS;

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
  correct_order = "";
  for(int i=0; i < LEN_NUMS; i++){
    correct_order += c_order[i] + " ";
  }
  correct_order.trim();
  return count_str;
}

void dec_to_binary(){
  
}

void binary_to_dec(){
  target_num = random(0, 16); //for 4 leds, we can have 0 to 15
  temp = target_num;
  //turn on LEDs in binary
  if(temp%2==1){
    turn on led0;
    digitalWrite(ledPin0, HIGH);
    temp=(temp-1)/2;
  } else{
    digitalWrite(ledPin0, LOW);
    temp = temp/2;
  }
  
  if ((temp)%2==1){
    turn on led1
    digitalWrite(ledPin1, HIGH);
    temp = (temp-1)/2;
  } else {
    digitalWrite(ledPin1, LOW);
    temp = temp/2;
  }
  
  if ((temp)%2==1){
    turn on led2
    digitalWrite(ledPin2, HIGH);
    temp = (temp-1)/2;
  } else {
    digitalWrite(ledPin2, LOW);
    temp = temp/2;
  }
  
  if ((temp)%2==1){
    turn on led3
    digitalWrite(ledPin3, HIGH);
    temp = (temp-1)/2;
  } else {
    digitalWrite(ledPin3, LOW);
    temp = temp/2;
  }

  input = keypad.getKey();
  waiting = 1;
  start = millis();
  running_count = 0;

  while (waiting == 1){
    my_time = millis();
    if (my_time-start > wait_time or input == "#" or input == "*"){ //condition to exit the wait
      waiting = 0;
    } else if (input != NO_KEY) { // assuming input is a valid digit
      running_count = running_count * 10 + (input-"0");
    }
    // in the case that input == NO_KEY just get the next input
    input = keypad.getKey();
  }
  if (running_count==target_num){
    r = 1; //user input is correct
  } else {
    r = 0; //user input incorrect
  }
  
  //turn off lights
  digitalWrite(ledPin0, LOW);
  digitalWrite(ledPin1, LOW);
  digitalWrite(ledPin2, LOW);
  digitalWrite(ledPin3, LOW);
  
  return r
}
