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

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);
  // Set motor pins as outputs
  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  pinMode(motor2pin1, OUTPUT);
  pinMode(motor2pin2, OUTPUT);
}

void loop() {
  start();
}

void start(){
  int distance = read();
  makeDec(distance);
  delay(100);
}


void solveMaze() {
  const int WALL_THRESHOLD = 10; // Distance to consider as a wall (in cm)
  const int FORWARD_SPEED = 100; // Adjust as needed
  const int TURN_SPEED = 50; // Adjust as needed

  while (true) {
    setSpeed(FORWARD_SPEED);
    
    if (read() > WALL_THRESHOLD) {
      // No wall in front, keep going
      delay(100);
    } else {
      // Wall in front, try to turn right
      setSpeed(0);
      turnRight(90);
      
      if (read() > WALL_THRESHOLD) {
        // Right turn possible, continue
        delay(100);
      } else {
        // Can't turn right, try left
        turnLeft(180);
        
        if (read() > WALL_THRESHOLD) {
          // Left turn possible, continue
          delay(100);
        } else {
          // Dead end, turn around
          turnRight(90);
        }
      }
    }
  
    // Check if we've reached the goal
    // if (isGoalReached()) {
    //   setSpeed(0);
    //   break;
    // }
  }
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