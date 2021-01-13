/***** Variables to customize *****/
// Your GPIO pin number connected to trig pin
const int Trig = ;
// Your GPIO pin number connected to echo pin
const int Echo = ;
// Your GPIO pin number connected to first green led
const int Green_1 = ;
// Your GPIO pin number connected to second green led
const int Green_2 = ;
// Your GPIO pin number connected to first yellow led
const int Yellow_1 = ;
// Your GPIO pin number connected to second yellow led
const int Yellow_2 = ;
// Your GPIO pin number connected to first red led
const int Red_1 = ;
// Your GPIO pin number connected to second red led
const int Red_2 = ;
// Your GPIO pin number connected to buzzer
const int Buzzer = ;
/**********************************/


int Value = 0;

int Measure_the_distance(){
  long Time;
  long Distance;

  digitalWrite(Trig, LOW);
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig, LOW);

  Time = pulseIn(Echo, HIGH);
  Distance = Time / 58;

  return Distance;
}


void setup() {
  Serial.begin (115200);
  pinMode(Trig, OUTPUT);
  pinMode(Echo, INPUT);
  
  pinMode(Green_1, OUTPUT);
  pinMode(Green_2, OUTPUT);
  pinMode(Yellow_1, OUTPUT);
  pinMode(Yellow_2, OUTPUT);
  pinMode(Red_1, OUTPUT);
  pinMode(Red_2, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  
  digitalWrite(Green_1, LOW);
  digitalWrite(Green_2, LOW);
  digitalWrite(Yellow_1, LOW);
  digitalWrite(Yellow_2, LOW);
  digitalWrite(Red_1, LOW);
  digitalWrite(Red_2, LOW);
  digitalWrite(Buzzer, LOW);
}

void loop() {
  Value = Measure_the_distance();
  if (Value > 100){
    digitalWrite(Green_1, LOW);
    digitalWrite(Green_2, LOW);
    digitalWrite(Yellow_1, LOW);
    digitalWrite(Yellow_2, LOW);
    digitalWrite(Red_1, LOW);
    digitalWrite(Red_2, LOW);
    digitalWrite(Buzzer, LOW);
  }
  if(Value <= 100 && Value > 80){
    digitalWrite(Green_1, HIGH);
    digitalWrite(Green_2, LOW);
    digitalWrite(Yellow_1, LOW);
    digitalWrite(Yellow_2, LOW);
    digitalWrite(Red_1, LOW);
    digitalWrite(Red_2, LOW);
    digitalWrite(Buzzer, LOW);
  }
  if(Value <= 80 && Value > 60){
    digitalWrite(Green_1, HIGH);
    digitalWrite(Green_2, HIGH);
    digitalWrite(Yellow_1, LOW);
    digitalWrite(Yellow_2, LOW);
    digitalWrite(Red_1, LOW);
    digitalWrite(Red_2, LOW);
    digitalWrite(Buzzer, LOW);
  }
  if(Value <= 60 && Value > 40){
    digitalWrite(Green_1, HIGH);
    digitalWrite(Green_2, HIGH);
    digitalWrite(Yellow_1, HIGH);
    digitalWrite(Yellow_2, LOW);
    digitalWrite(Red_1, LOW);
    digitalWrite(Red_2, LOW);
    digitalWrite(Buzzer, LOW);
  }
  if(Value <= 40 && Value > 25){
    digitalWrite(Green_1, HIGH);
    digitalWrite(Green_2, HIGH);
    digitalWrite(Yellow_1, HIGH);
    digitalWrite(Yellow_2, HIGH);
    digitalWrite(Red_1, LOW);
    digitalWrite(Red_2, LOW);
    digitalWrite(Buzzer, LOW);
  }
  if(Value <= 25 && Value > 10){
    digitalWrite(Green_1, HIGH);
    digitalWrite(Green_2, HIGH);
    digitalWrite(Yellow_1, HIGH);
    digitalWrite(Yellow_2, HIGH);
    digitalWrite(Red_1, HIGH);
    digitalWrite(Red_2, LOW);
    digitalWrite(Buzzer, HIGH);
  }
  if(Value <= 10 && Value > 5){
    digitalWrite(Green_1, HIGH);
    digitalWrite(Green_2, HIGH);
    digitalWrite(Yellow_1, HIGH);
    digitalWrite(Yellow_2, HIGH);
    digitalWrite(Red_1, HIGH);
    digitalWrite(Red_2, HIGH);
    digitalWrite(Buzzer, HIGH);
  }
  if(Value <= 5){
    digitalWrite(Buzzer, HIGH);
    digitalWrite(Green_1, LOW);
    delay(50);
    digitalWrite(Green_2, LOW);
    digitalWrite(Green_1, HIGH);
    delay(50);
    digitalWrite(Yellow_1, LOW);
    digitalWrite(Green_2, HIGH);
    delay(50);
    digitalWrite(Yellow_2, LOW);
    digitalWrite(Yellow_1, HIGH);
    delay(50);
    digitalWrite(Red_1, LOW);
    digitalWrite(Yellow_2, HIGH);
    delay(50);
    digitalWrite(Red_2, LOW);
    digitalWrite(Red_1, HIGH);
    delay(50);
    digitalWrite(Red_2, HIGH);
    delay(50);
  }
}
