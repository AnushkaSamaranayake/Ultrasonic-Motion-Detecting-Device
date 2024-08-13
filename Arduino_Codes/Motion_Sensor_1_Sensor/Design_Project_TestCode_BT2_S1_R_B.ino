#define TRIG_PIN 9
#define ECHO_PIN 8
#define BUZZER_PIN 13

long initialDistance, currentDistance;
long Total_Distance = 0;
int loop_no = 0;
int error = 3;

bool Device_State;
bool Buzzer_State = false;
bool Reset_State =  false;

char Incoming_value = 0;

int readDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  int duration = pulseIn(ECHO_PIN, HIGH);
  int distance = duration * 0.034 / 2;
  return distance;
}

// Function to control the buzzer
void controlBuzzer() {
  
  digitalWrite(BUZZER_PIN, HIGH);
  delay(1000); // Increase the delay to make the buzzer sound audible
  digitalWrite(BUZZER_PIN, LOW);
  delay(1000); // Delay between buzzer on/off states
}

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Get initial distance
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  initialDistance = pulseIn(ECHO_PIN, HIGH) * 0.017;

  Serial.print("Initial Distance = ");
  Serial.println(initialDistance);
}

void loop() {
  if (Serial.available() > 0) {
    Incoming_value = Serial.read();
    Serial.print(Incoming_value);
    Serial.print("\n");
    if (Incoming_value == '1') {
        Device_State = true;
    } else if (Incoming_value == '0') {
        Device_State = false;
        Buzzer_State = false;
    } else if (Incoming_value == 'r') {
        Reset_State = true;
    } else if (Incoming_value == 'b'){
      Buzzer_State = false;
    }
  }

  if (Reset_State == true){
      // Get initial distance
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    initialDistance = pulseIn(ECHO_PIN, HIGH) * 0.017;

    Serial.print("Initial Distance = ");
    Serial.println(initialDistance);
    Reset_State = false;
  }

  if (Device_State == true) {
    if (Buzzer_State == false) {
      int distance = readDistanceCM();

      Serial.print("Distance = ");
      Serial.println(distance);

      Total_Distance = Total_Distance + distance;
      loop_no = loop_no + 1;
      Serial.println(loop_no);

      if (loop_no == 10) {
        long Mean_Distance = Total_Distance / loop_no;

        Serial.print("Mean Distance = ");
        Serial.println(Mean_Distance);

        if ((initialDistance + error) >= Mean_Distance && (initialDistance - error) <= Mean_Distance) {
          Serial.print("Mean Distance = ");
          Serial.println(Mean_Distance);
        } else if ((initialDistance - error) >= Mean_Distance) {
          Serial.print("Mean Distance changed = ");
          Serial.println(Mean_Distance);

          Buzzer_State = true;
        }
        loop_no = 0;
        Total_Distance = 0;
      }
    }
    
    if (Buzzer_State == true) {
      controlBuzzer(); // Call the function to control the buzzer

      if (Serial.available() > 0) {
        Incoming_value = Serial.read();
        Serial.print(Incoming_value);
        Serial.print("\n");
        if (Incoming_value == '0') {
          Device_State = false;
          Buzzer_State = false;
        }
      }
    }
  }
  
  delay(250); // Adjust the delay between distance measurements as per your requirements
}