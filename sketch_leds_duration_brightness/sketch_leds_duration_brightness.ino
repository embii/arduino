#define INTERNAL_LED_PIN 13
#define LED_PIN 9
#define RESISTOR_PIN 10

#define BUTTON_DURATION_PIN 7
#define BUTTON_BRIGHTNESS_PIN 8
const int SHORT_PRESS_TIME = 500; // milliseconds
const int LONG_PRESS_TIME  = 1000; // milliseconds
const int RESISTOR_IDDLE = 5000; // milliseconds
const int RESISTOR_RUNNING  = 2000; // milliseconds


// Variables will change:
int lastButtonState = LOW;  // the previous state from the input pin
int currentButtonState;     // the current reading from the input pin
unsigned long pressedTime  = 0;
unsigned long releasedTime = 0;
int currentBrightnessButtonState = LOW;  // current state from the input pin
int lastBrightnessButtonState = LOW;  // the previous state from the input pin

// int pressMax = 30;
// int pressCount = 0;
unsigned long durationMultiplier = 1800000;
unsigned long calculatedDuration = 0;
unsigned long currentMillis=0;
unsigned long maxMillis = 0;
unsigned long initialDuration = 1000;
short initialBrightness = 20;
short minBrightness = 1;
short maxBrightness = 254;
short brightness = initialBrightness;


void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(RESISTOR_PIN, OUTPUT);
  pinMode(BUTTON_DURATION_PIN, INPUT);
  pinMode(BUTTON_BRIGHTNESS_PIN, INPUT);
  pinMode(INTERNAL_LED_PIN, OUTPUT);
  digitalWrite(INTERNAL_LED_PIN, LOW);
  setTimer(initialDuration);
  Serial.begin(9600);        // initialize serial communication

}

void handleResistor(){
  // static int lastState = LOW;  // the previous state from the input pin
  static int currentState = LOW;     // the current reading from the input pin
  static unsigned long runtime  = 0;
  static unsigned long lastMillis  = 0;
  unsigned long ms = millis();  

  runtime += ms - lastMillis;
  lastMillis = ms;

  if ( currentState == LOW ) {
    if (runtime >= RESISTOR_IDDLE ){
      currentState = HIGH;
      runtime = 0;
      Serial.print("resistor to HIGH\n");
      
    } 
  } else {
    if (runtime >= RESISTOR_RUNNING ){
      currentState = LOW;
      runtime = 0;
      Serial.print("resistor to LOW\n");
    } 
  } 
  digitalWrite(RESISTOR_PIN,currentState );
}

void loop() {
   // read the state of the switch/button:
  currentButtonState = digitalRead(BUTTON_DURATION_PIN);
  // ButtonDuration is released
  if(lastButtonState == HIGH && currentButtonState == LOW){
        releasedTime = millis();
    long pressDuration = releasedTime - pressedTime;
    evalPressReverted(pressDuration);    
  } 
  else if(lastButtonState == LOW && currentButtonState == HIGH) { // Button is pressed
    pressedTime = millis();
  }
  // save the the last state
  lastButtonState = currentButtonState;


  if (millis() < maxMillis) {
    currentBrightnessButtonState = digitalRead(BUTTON_BRIGHTNESS_PIN);
    if(currentBrightnessButtonState) setBrightness();  
    // digitalWrite(LED_PIN, HIGH);
    analogWrite(LED_PIN, brightness);
    lastBrightnessButtonState=currentBrightnessButtonState;    

  }
  else {
    digitalWrite(LED_PIN, LOW);
  }
  
  currentMillis=millis();
  log();
  handleResistor();
}



void log(){
  Serial.print("ButtonDuration state ");
  Serial.print(currentButtonState);
  Serial.print(" BrightnessButton state ");
  Serial.print(currentBrightnessButtonState);
  Serial.print(" millis ");
  Serial.print(currentMillis);
  Serial.print(" max millis ");
  Serial.print(maxMillis);
  Serial.print(" calc dur ");
  Serial.print(calculatedDuration);
  Serial.print(" brightness ");
  Serial.print(brightness);
  Serial.print(" from start ");
  Serial.print(millisToTimeString(currentMillis));
  Serial.print(" end ");
  Serial.print(millisToTimeString(maxMillis));
  Serial.print(" till end ");
  Serial.print(millisToTimeString(maxMillis - currentMillis));
  Serial.print("\n");
  delay(1);
}

String millisToTimeString(unsigned long m){
  char ret[128];
  unsigned long seconds=(m/1000)%60;
  unsigned long minutes=(m/60000)%60;
  unsigned long hours=m/3600000;
  sprintf(ret, "%02lu:%02lu:%02lu", hours, minutes, seconds);
  return(ret);
}

void evalPressReverted(long duration){
    if( duration < SHORT_PRESS_TIME ){ 
      Serial.println("A short press is detected");
      offTimer();
    }
    if( duration > LONG_PRESS_TIME ){
      Serial.println("A long press is detected");
      setTimer(duration);
    }  
    delay(250);
}
void setTimer(long duration){    
    calculatedDuration += duration/1000;
    maxMillis=millis()+ calculatedDuration*durationMultiplier;
    ledblink(calculatedDuration, 100, LED_PIN);
}


void setBrightness(){
  Serial.print(currentBrightnessButtonState);
  Serial.print(lastBrightnessButtonState);
  Serial.print("\n");
  brightness += 5;  
  if (brightness >  32){
    brightness += 10; 
  }   
  if (brightness >  128){
    brightness += 15;
  }    
  // if (brightness <= minBrightness ){
  if (brightness >= maxBrightness ){
    brightness = maxBrightness;
    if (lastBrightnessButtonState!=currentBrightnessButtonState) {
      brightness = minBrightness;
    }      
  }
}
  
// void evalPress(long duration){
//     if( duration < SHORT_PRESS_TIME ){
//       Serial.println("A short press is detected");
//       pressCount++;
//       if(pressCount > pressMax) pressCount=1;
//       updateTimer();
//     }
//     if( duration > LONG_PRESS_TIME ){
//       Serial.println("A long press is detected");
//       offTimer();
//     }  
// }
  
// void updateTimer(){
//      maxMillis=millis()+ pressCount*durationMultiplier;
//      ledblink(pressCount, 100, LED_PIN);
// }

void offTimer(){
    maxMillis=0;
    calculatedDuration =0; 
    brightness = initialBrightness;
}

void ledblink(int times, int lengthms, int pinnum){
  for (int x=0; x<times;x++) {
    analogWrite(pinnum, brightness);
    delay (lengthms);
    digitalWrite(pinnum, LOW);
    delay(lengthms);
  }
}
