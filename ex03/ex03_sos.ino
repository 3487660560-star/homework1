const int ledPin = 2;

// 时间定义（单位：ms）
const unsigned long shortOn = 150;
const unsigned long shortOff = 200;
const unsigned long longOn = 600;
const unsigned long longOff = 200;
const unsigned long letterPause = 300;
const unsigned long sosPause = 2000;

// SOS序列状态管理
enum State { S_SHORT1, S_SHORT2, S_SHORT3,
             PAUSE_S_O,
             O_LONG1, O_LONG2, O_LONG3,
             PAUSE_O_S,
             S_SHORT4, S_SHORT5, S_SHORT6,
             SOS_END_PAUSE };
State currentState = S_SHORT1;

unsigned long previousMillis = 0;
bool ledState = LOW;

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
  Serial.println("SOS signal started");
}

void loop() {
  unsigned long currentMillis = millis();
  unsigned long elapsed = currentMillis - previousMillis;

  switch (currentState) {
    // 第一个S：三次短闪
    case S_SHORT1:
    case S_SHORT2:
    case S_SHORT3:
      if (!ledState) { // 亮灯阶段
        digitalWrite(ledPin, HIGH);
        ledState = HIGH;
        previousMillis = currentMillis;
      } else if (elapsed >= shortOn) { // 亮够时间，灭灯
        digitalWrite(ledPin, LOW);
        ledState = LOW;
        previousMillis = currentMillis;
        if (currentState == S_SHORT1) currentState = S_SHORT2;
        else if (currentState == S_SHORT2) currentState = S_SHORT3;
        else currentState = PAUSE_S_O;
      }
      break;

    case PAUSE_S_O:
      if (elapsed >= shortOff) { // 灭灯时间到，进入O的三次长闪
        previousMillis = currentMillis;
        currentState = O_LONG1;
      }
      break;

    // O：三次长闪
    case O_LONG1:
    case O_LONG2:
    case O_LONG3:
      if (!ledState) {
        digitalWrite(ledPin, HIGH);
        ledState = HIGH;
        previousMillis = currentMillis;
      } else if (elapsed >= longOn) {
        digitalWrite(ledPin, LOW);
        ledState = LOW;
        previousMillis = currentMillis;
        if (currentState == O_LONG1) currentState = O_LONG2;
        else if (currentState == O_LONG2) currentState = O_LONG3;
        else currentState = PAUSE_O_S;
      }
      break;

    case PAUSE_O_S:
      if (elapsed >= shortOff) { // 灭灯时间到，进入第二个S的三次短闪
        previousMillis = currentMillis;
        currentState = S_SHORT4;
      }
      break;

    // 第二个S：三次短闪
    case S_SHORT4:
    case S_SHORT5:
    case S_SHORT6:
      if (!ledState) {
        digitalWrite(ledPin, HIGH);
        ledState = HIGH;
        previousMillis = currentMillis;
      } else if (elapsed >= shortOn) {
        digitalWrite(ledPin, LOW);
        ledState = LOW;
        previousMillis = currentMillis;
        if (currentState == S_SHORT4) currentState = S_SHORT5;
        else if (currentState == S_SHORT5) currentState = S_SHORT6;
        else currentState = SOS_END_PAUSE;
      }
      break;

    case SOS_END_PAUSE:
      if (elapsed >= sosPause) { // SOS结束，长停顿后重新开始
        previousMillis = currentMillis;
        currentState = S_SHORT1;
        Serial.println("SOS cycle completed, restarting...");
      }
      break;
  }
}
