#include <Key.h>
#include <Keypad.h>

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

int convert_to_decimal(in pins, out pins, int wait_time) {
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

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
