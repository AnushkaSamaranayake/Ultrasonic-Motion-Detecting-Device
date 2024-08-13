#define TRIG_PIN_1 6
#define ECHO_PIN_1 7

#define TRIG_PIN_2 3
#define ECHO_PIN_2 4

#define TRIG_PIN_3 8
#define ECHO_PIN_3 9

#define BUZZER_PIN 10
#define LED_PIN 11

long initialDistance_1,currentDistance_1;
long initialDistance_2,currentDistance_2;
long initialDistance_3,currentDistance_3;

long Total_Distance_1=0;long Total_Distance_2=0;long Total_Distance_3=0;

int loop_no=0;
int error = 3;

bool Device_State;
bool Buzzer_State = false;
bool Set_Buzzer_State = false;
bool Reset_State =  false;

char Incoming_value = 0;

int readDistanceCM(int TRIG_PIN, int ECHO_PIN){
  digitalWrite(TRIG_PIN, LOW);delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  int distance=pulseIn(ECHO_PIN, HIGH)*0.034/2;
  return distance;
}

// Function to control the buzzer
void controlBuzzer() {
  digitalWrite(BUZZER_PIN, HIGH);delay(750); 
  digitalWrite(BUZZER_PIN, LOW);delay(500);
   // Delay between buzzer on/off states
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(TRIG_PIN_1, OUTPUT);pinMode(ECHO_PIN_1, INPUT);
  pinMode(TRIG_PIN_2, OUTPUT); pinMode(ECHO_PIN_2, INPUT);
  pinMode(TRIG_PIN_3, OUTPUT);pinMode(ECHO_PIN_3, INPUT);

  pinMode(BUZZER_PIN, OUTPUT);pinMode(LED_PIN, OUTPUT);

  // Get initial distance
  initialDistance_1=readDistanceCM(TRIG_PIN_1, ECHO_PIN_1);
  initialDistance_2=readDistanceCM(TRIG_PIN_2, ECHO_PIN_2);
  initialDistance_3=readDistanceCM(TRIG_PIN_3, ECHO_PIN_3);

  digitalWrite(BUZZER_PIN, HIGH);digitalWrite(LED_PIN, HIGH);delay(150);digitalWrite(BUZZER_PIN, LOW);digitalWrite(LED_PIN, LOW);

  Serial.print("Initial Distance = ");
  Serial.print(initialDistance_1);Serial.print(" ");Serial.print(initialDistance_2);Serial.print(" ");Serial.println(initialDistance_3);
}

void loop() {

  if (Serial.available() > 0) {
    Incoming_value = Serial.read();
    Serial.print(Incoming_value);Serial.print("\n");   
    if (Incoming_value == '1') {
      Device_State = true;
    } else if (Incoming_value == '0') {
      Device_State = false;
      Buzzer_State = false;
    } else if (Incoming_value == 'r'){
      Reset_State = true;
    } else if (Incoming_value == 'b'){
      Buzzer_State = false;
    }
  }

  if (Reset_State == true){
    // Get initial distance
    initialDistance_1=readDistanceCM(TRIG_PIN_1, ECHO_PIN_1);
    initialDistance_2=readDistanceCM(TRIG_PIN_2, ECHO_PIN_2);
    initialDistance_3=readDistanceCM(TRIG_PIN_3, ECHO_PIN_3);

    digitalWrite(BUZZER_PIN, HIGH);digitalWrite(LED_PIN, HIGH);delay(150);digitalWrite(BUZZER_PIN, LOW);digitalWrite(LED_PIN, LOW);

    Serial.print("Initial Distance = ");
    Serial.print(initialDistance_1);Serial.print(" ");Serial.print(initialDistance_2);Serial.print(" ");Serial.println(initialDistance_3);
    Reset_State = false;
  }
  if (Device_State == true) {
    if (Buzzer_State == false){

      int distance_1 = readDistanceCM(TRIG_PIN_1, ECHO_PIN_1);
      int distance_2 = readDistanceCM(TRIG_PIN_2, ECHO_PIN_2);
      int distance_3 = readDistanceCM(TRIG_PIN_3, ECHO_PIN_3);

      Total_Distance_1 = Total_Distance_1 + distance_1;
      Total_Distance_2 = Total_Distance_2 + distance_2;
      Total_Distance_3 = Total_Distance_3 + distance_3;

      Serial.print("Distance = ");
      Serial.print(distance_1);Serial.print(" ");Serial.print(distance_2);Serial.print(" ");Serial.println(distance_3);

      loop_no = loop_no + 1;
      Serial.println(loop_no);

      if (loop_no == 10) {

        long Mean_Distance_1 = Total_Distance_1 / loop_no;
        long Mean_Distance_2 = Total_Distance_2 / loop_no;
        long Mean_Distance_3 = Total_Distance_3 / loop_no;

        Serial.print("Mean Distance = ");
        Serial.print(Mean_Distance_1);Serial.print(" ");Serial.print(Mean_Distance_2);Serial.print(" ");Serial.println(Mean_Distance_3);

        if (((initialDistance_1 + error) >= Mean_Distance_1 && (initialDistance_1 - error) <= Mean_Distance_1)||
            ((initialDistance_2 + error) >= Mean_Distance_2 && (initialDistance_2 - error) <= Mean_Distance_2)||
            ((initialDistance_3 + error) >= Mean_Distance_3 && (initialDistance_3 - error) <= Mean_Distance_3)) {

          Serial.print("Mean Distance noy changed = ");
          Serial.print(Mean_Distance_1);Serial.print(" ");Serial.print(Mean_Distance_2);Serial.print(" ");Serial.println(Mean_Distance_3);

        } else if (((initialDistance_1 - error) >= Mean_Distance_1)||
                  ((initialDistance_2 - error) >= Mean_Distance_2)||
                  ((initialDistance_3 - error) >= Mean_Distance_3)) {

          Serial.print("Mean Distance changed = ");
          Serial.print(Mean_Distance_1);Serial.print(" ");Serial.print(Mean_Distance_2);Serial.print(" ");Serial.println(Mean_Distance_3);

          Buzzer_State = true;
        }
        
        loop_no = 0;
        Total_Distance_1 = 0; Total_Distance_2 = 0; Total_Distance_3 = 0;
      }
    }
    if (Buzzer_State == true) {
      controlBuzzer(); // Call the function to control the buzzer

      if (Serial.available() > 0) {
        Incoming_value = Serial.read();
        Serial.print(Incoming_value);
        Serial.print("\n");
        if (Incoming_value == '0') {
          Device_State = true;
          Buzzer_State = false;
        }
      }
    }
  }

  delay(250);  // Adjust the delay between distance measurements as per your requirements
}
