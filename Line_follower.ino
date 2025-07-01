#define IR_LEFT A1               // Czujnik IR po lewej stronie
#define IR_RIGHT A0              // Czujnik IR po prawej stronie
#define ULTRASONIC_ECHO A2       // Pin Echo dla czujnika ultradźwiękowego
#define ULTRASONIC_TRIGGER A3    // Pin Trigger dla czujnika ultradźwiękowego
#define SERVO_PIN A5             // Pin sterujący serwomechanizmem
#define MOTOR_A_EN 10            // Pin Enable1 dla sterownika L298
#define MOTOR_A_IN1 9            // Pin IN1 dla silnika A
#define MOTOR_A_IN2 8            // Pin IN2 dla silnika A
#define MOTOR_B_IN1 7            // Pin IN1 dla silnika B
#define MOTOR_B_IN2 6            // Pin IN2 dla silnika B
#define MOTOR_B_EN 5             // Pin Enable2 dla sterownika L298

int thresholdDistance = 10;      // Minimalna odległość do przeszkody w cm
int leftDistance, rightDistance, frontDistance; // Odległości od przeszkód

bool ignoreRightOnce = false;    // Flaga ignorowania przeszkód z prawej strony
bool ignoreLeftOnce = false;     // Flaga ignorowania przeszkód z lewej strony
unsigned long ignoreStartTime;   // Czas rozpoczęcia ignorowania przeszkody
const unsigned long ignoreDuration = 3000; // Czas ignorowania przeszkody (ms)


void setup() {
    Serial.begin(9600);          // Inicjalizacja portu szeregowego
    pinMode(IR_LEFT, INPUT);     // Czujnik IR po lewej stronie jako wejście
    pinMode(IR_RIGHT, INPUT);    // Czujnik IR po prawej stronie jako wejście
    pinMode(ULTRASONIC_ECHO, INPUT); // Czujnik ultradźwiękowy - Echo
    pinMode(ULTRASONIC_TRIGGER, OUTPUT); // Czujnik ultradźwiękowy - Trigger
    pinMode(SERVO_PIN, OUTPUT);  // Serwomechanizm jako wyjście
    pinMode(MOTOR_B_EN, OUTPUT); // Sterownik silnika B - Enable
    pinMode(MOTOR_A_IN1, OUTPUT);// Sterownik silnika A - IN1
    pinMode(MOTOR_A_IN2, OUTPUT);// Sterownik silnika A - IN2
    pinMode(MOTOR_B_IN1, OUTPUT);// Sterownik silnika B - IN1
    pinMode(MOTOR_B_IN2, OUTPUT);// Sterownik silnika B - IN2
    pinMode(MOTOR_A_EN, OUTPUT); // Sterownik silnika A - Enable

    analogWrite(MOTOR_B_EN, 170); // Ustawienie początkowej prędkości silnika B
    analogWrite(MOTOR_A_EN, 170); // Ustawienie początkowej prędkości silnika A

    for (int angle = 70; angle <= 145; angle += 5) {
        moveServo(SERVO_PIN, angle); // Kalibracja serwa w zakresie 70°-145°
    }
    for (int angle = 145; angle >= 0; angle -= 5) {
        moveServo(SERVO_PIN, angle); // Kalibracja serwa w zakresie 145°-0°
    }
    for (int angle = 0; angle <= 70; angle += 5) {
        moveServo(SERVO_PIN, angle); // Kalibracja serwa w zakresie 0°-70°
    }

    frontDistance = readUltrasonic(); // Pierwszy odczyt odległości z przodu
    delay(500);                      // Odstęp czasowy dla stabilizacji czujników
}



void loop() {
    frontDistance = readUltrasonic(); // Odczyt odległości z przodu
    Serial.print("Front Distance="); Serial.println(frontDistance); // Wyświetlenie odległości w konsoli

    if (ignoreLeftOnce && (millis() - ignoreStartTime > ignoreDuration)) {
        ignoreLeftOnce = false; // Wyłączenie ignorowania lewej strony
    }
    if (ignoreRightOnce && (millis() - ignoreStartTime > ignoreDuration)) {
        ignoreRightOnce = false; // Wyłączenie ignorowania prawej strony
    }

    if (!ignoreRightOnce && (digitalRead(IR_RIGHT) == 1) && (digitalRead(IR_LEFT) == 0)) {
        turnRight(); // Skręt w prawo, gdy prawy czujnik wykrywa linię
    } else if (!ignoreLeftOnce && (digitalRead(IR_RIGHT) == 0) && (digitalRead(IR_LEFT) == 1)) {
        turnLeft();  // Skręt w lewo, gdy lewy czujnik wykrywa linię
    } else if ((digitalRead(IR_RIGHT) == 1) && (digitalRead(IR_LEFT) == 1)) {
        stopMovement(); // Zatrzymanie, gdy oba czujniki wykrywają linię
    } else {
        if (frontDistance > thresholdDistance) {
            moveForward(); // Ruch do przodu przy braku przeszkód
        } else {
            checkSides(); // Analiza boków przy przeszkodzie
        }
    }
    delay(10); // Krótka przerwa
}


void moveServo(int pin, int angle) {
    int pwm = (angle * 11) + 500; // Obliczenie sygnału PWM dla danego kąta
    digitalWrite(pin, HIGH);      // Ustawienie pinu w stan wysoki
    delayMicroseconds(pwm);       // Wysłanie impulsu PWM
    digitalWrite(pin, LOW);       // Ustawienie pinu w stan niski
    delay(50);                    // Krótka przerwa między sygnałami
}


long readUltrasonic() {
    digitalWrite(ULTRASONIC_TRIGGER, LOW); // Trigger na niski stan
    delayMicroseconds(2);                  // Krótka przerwa
    digitalWrite(ULTRASONIC_TRIGGER, HIGH);// Trigger na wysoki stan
    delayMicroseconds(10);                 // Impuls trwający 10 µs
    long duration = pulseIn(ULTRASONIC_ECHO, HIGH); // Czas trwania Echo
    return duration / 29 / 2;              // Przeliczenie na odległość w cm
}


void checkSides() {
    stopMovement();
    delay(100);
    
    for (int angle = 70; angle <= 145; angle += 5) {
        moveServo(SERVO_PIN, angle);
    }
    delay(300);
    leftDistance = readUltrasonic();
    Serial.print("Left Distance="); Serial.println(leftDistance);
    delay(100);

    for (int angle = 145; angle >= 0; angle -= 5) {
        moveServo(SERVO_PIN, angle);
    }
    delay(500);
    rightDistance = readUltrasonic();
    Serial.print("Right Distance="); Serial.println(rightDistance);
    delay(100);

    for (int angle = 0; angle <= 70; angle += 5) {
        moveServo(SERVO_PIN, angle);
    }
    delay(300);

    if (leftDistance < thresholdDistance && rightDistance < thresholdDistance) {
        stopMovement(); 
    } else if (leftDistance > thresholdDistance && rightDistance > thresholdDistance) {
        turnRight();
        delay(360);
        moveForward();
        delay(1283);
        turnLeftOnly();
        delay(620);
        moveForward();
        delay(581);
        turnLeftOnly();
        delay(620);
        
        ignoreLeftOnce = true;  
        ignoreStartTime = millis(); 
    } else {
        evaluateDistances();
    }
}


void evaluateDistances() {  // Funkcja oceniająca, która strona ma większą odległość
    if (rightDistance > leftDistance) {  // Jeśli odległość po prawej stronie jest większa niż po lewej
        turnRight();    // Skręt w prawo
        delay(360);     // Odczekanie 360 ms na wykonanie skrętu
        moveForward();  // Robot rusza do przodu
        delay(1283);    // Odczekanie 1283 ms na kontynuację ruchu do przodu
        turnLeftOnly(); // Skręt tylko w lewo
        delay(620);     // Odczekanie 620 ms na wykonanie skrętu
        moveForward();  // Robot rusza do przodu
        delay(581);     // Odczekanie 581 ms na kontynuację ruchu do przodu
        turnLeftOnly(); // Skręt tylko w lewo
        delay(620);     // Odczekanie 620 ms na wykonanie skrętu
        
        ignoreLeftOnce = true;  // Ustawienie flagi, aby zignorować przyszłe pomiary dla lewej strony
        ignoreStartTime = millis();  // Zapisanie czasu rozpoczęcia ignorowania
    } else {  // Jeśli odległość po lewej stronie jest większa niż po prawej
        turnLeft();    // Skręt w lewo
        delay(360);    // Odczekanie 360 ms na wykonanie skrętu
        moveForward(); // Robot rusza do przodu
        delay(1283);   // Odczekanie 1283 ms na kontynuację ruchu do przodu
        turnRightOnly(); // Skręt tylko w prawo
        delay(620);     // Odczekanie 620 ms na wykonanie skrętu
        moveForward();  // Robot rusza do przodu
        delay(581);     // Odczekanie 581 ms na kontynuację ruchu do przodu
        turnRightOnly(); // Skręt tylko w prawo
        delay(620);     // Odczekanie 620 ms na wykonanie skrętu
        
        ignoreRightOnce = true;  // Ustawienie flagi, aby zignorować przyszłe pomiary dla prawej strony
        ignoreStartTime = millis();  // Zapisanie czasu rozpoczęcia ignorowania
    }
}

void moveForward() {
    digitalWrite(MOTOR_A_IN1, LOW);
    digitalWrite(MOTOR_A_IN2, HIGH);
    digitalWrite(MOTOR_B_IN1, HIGH);
    digitalWrite(MOTOR_B_IN2, LOW);
}

void moveBackward() {
    digitalWrite(MOTOR_A_IN1, HIGH);
    digitalWrite(MOTOR_A_IN2, LOW);
    digitalWrite(MOTOR_B_IN1, LOW);
    digitalWrite(MOTOR_B_IN2, HIGH);
}

void turnRight() {
    digitalWrite(MOTOR_A_IN1, HIGH);
    digitalWrite(MOTOR_A_IN2, LOW);
    digitalWrite(MOTOR_B_IN1, HIGH);
    digitalWrite(MOTOR_B_IN2, LOW);
}

void turnRightOnly() {
    digitalWrite(MOTOR_A_IN1, LOW);
    digitalWrite(MOTOR_A_IN2, LOW);
    digitalWrite(MOTOR_B_IN1, HIGH);
    digitalWrite(MOTOR_B_IN2, LOW);
}

void turnLeft() {
    digitalWrite(MOTOR_A_IN1, LOW);
    digitalWrite(MOTOR_A_IN2, HIGH);
    digitalWrite(MOTOR_B_IN1, LOW);
    digitalWrite(MOTOR_B_IN2, HIGH);
}

void turnLeftOnly() {
    digitalWrite(MOTOR_A_IN1, LOW);
    digitalWrite(MOTOR_A_IN2, HIGH);
    digitalWrite(MOTOR_B_IN1, LOW);
    digitalWrite(MOTOR_B_IN2, LOW);
}

void stopMovement() {
    if (ignoreLeftOnce || ignoreRightOnce) {
        return;
    }

    digitalWrite(MOTOR_A_IN1, LOW);
    digitalWrite(MOTOR_A_IN2, LOW);
    digitalWrite(MOTOR_B_IN1, LOW);
    digitalWrite(MOTOR_B_IN2, LOW);
}
