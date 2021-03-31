#include <LiquidCrystal.h>
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
  
}
