const int trigPin = 9;
const int echoPin = 11;
int motor1pin1 = 3; // PWM-capable pin for motor 1 forward
int motor1pin2 = 2; // Motor 1 reverse
int motor2pin1 = 5; // PWM-capable pin for motor 2 forward
int motor2pin2 = 4; 
float duration, distance;

const double wheelCirc = 20.42; // Wheel circumference in cm
const double wheelRPM = 100;    // Wheel speed in RPM
const double turnFac = 0.5; 
const double wheelBase = 11.5;    // Turning adjustment factor

const int TOTAL_BACK_STEPS = 5;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);

  // Set motor pins as outputs
  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  pinMode(motor2pin1, OUTPUT);
  pinMode(motor2pin2, OUTPUT);
  
  // not exactly sure if this is right,
  // assuming there can be multiple start dirs
  // might be able to move to loop, idk if it matters though bc it only solves the maze once
  for (int i = 0; i < 4; i ++) {
    if (checkHall()) break;
    turnleft(90);
  }
}

void loop() {
  // unused
}

bool checkHall() {
  const int WALL_THRESHOLD = 10; // Distance to consider as a wall (in cm)
  const int FORWARD_SPEED = 100; // Adjust as needed
  const int TURN_SPEED = 50; // Adjust as needed

  // Go down path forward
  const int foward_start = millis();
  setSpeed(FORWARD_SPEED);
  while (read() > WALL_THRESHOLD)
    delay(100);
  setspeed(0);

  const int foward_time = millis() - forward_start;

  if (isGoalReached())
    return true;

  // look left
  turnleft(90);

  // check the left hall (if it exists)
  if (read() > WALL_THRESHOLD)
    if (checkHall()) return true;
  
  // look right
  turnleft(180);

  // check the right hall (if it exists)
  if (read() > WALL_THRESHOLD)
    if (checkHall()) return true;

  // Those two halls didnt have the exit so we have to
  // backtrack a bit and then check for new halls
  int total_back_time = 0;
  int steps = TOTAL_BACK_STEPS;

  // tbh problaby bad to backtrack depending on motor accuracy
  while (total_back_time > 0) {
    // time some stuff to track
    const int tmp_timer = millis();
    setSpeed(FORWARD_SPEED);
    delay(100);
    total_back_time += millis() - tmp_timer;

    if (read() > WALL_THRESHOLD) {
      setspeed(0);
      break;
    }
    
    if (!(steps--)) {
      steps = TOTAL_BACK_STEPS;
      setspeed(0);

      // check the left hall (if it exists)
      turnleft(90);
      if (read() > WALL_THRESHOLD)
        if (checkHall()) return true;
      
      // check the right hall (if it exists)
      turnleft(180);
      if (read() > WALL_THRESHOLD)
        if (checkHall()) return true;
    }
  }

  return false;
}

void setSpeed(int sp) {
  if(sp < 256 && sp > 0){
    analogWrite(motor1pin1, LOW);  // Motor 1 forward
    digitalWrite(motor1pin2, sp);   // Motor 1 reverse off
    analogWrite(motor2pin1, LOW);  // Motor 2 forward
    digitalWrite(motor2pin2, sp); 
  } else {
    analogWrite(motor1pin1, LOW);  // Motor 1 forward
    digitalWrite(motor1pin2, LOW);   // Motor 1 reverse off
    analogWrite(motor2pin1, LOW);  // Motor 2 forward
    digitalWrite(motor2pin2, LOW);
  }
}

int read(){
  // trigger an ultrasonic pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // read the pulse from the echo pin
  duration = pulseIn(echoPin, HIGH);
  distance = (duration * 0.0343) / 2;
  // distance to the Serial Monitor
  Serial.print("Distance: ");
  Serial.println(distance);
  return distance;
}

void turnLeft(int degs){
  double turnRadius = wheelBase / 2; 
  double arcLength = 2 * PI * turnRadius * (degs / 360.0);
  double wheelSpeed = (wheelRPM * wheelCirc) / 60.0;
  double turnTime = arcLength / wheelSpeed * turnFac;
  Serial.print("Turn time = ");
  Serial.println(turnTime);
  analogWrite(motor1pin1, 255); // Motor 1 forward
  digitalWrite(motor1pin2, LOW); // Motor 1 reverse off
  digitalWrite(motor2pin1, LOW); // Motor 2 forward off
  analogWrite(motor2pin2, 255); // Motor 2 reverse
  //Adjusment becuase welp :|
  double t = turnTime * 0.67;
  delay(t * 10000); // Convert to milliseconds
  // Stop motors after turning
  setSpeed(0);
}

void turnRight(int degs){
  double turnTime = (degs / 360.0) * (wheelCirc / turnFac);
  analogWrite(motor1pin1, LOW); // Motor 1 forward
  digitalWrite(motor1pin2, 255); // Motor 1 reverse off
  digitalWrite(motor2pin1, 255); // Motor 2 forward off
  analogWrite(motor2pin2, LOW); // Motor 2 reverse
  delay(turnTime * 1000); // Convert to milliseconds
  // Stop motors after turning
  setSpeed(0);
}

void makeDec(int dis) {
  if(dis > 20){
    setSpeed(255);
  } else {
    turnLeft(90);
  }
}
