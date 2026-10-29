// Hardware Pin Definitions
const int RED_PIN = 9;      
const int GREEN_PIN = 10;   
const int BLUE_PIN = 11;    
const int BUTTON_PIN = 2;   
const int LDR_PIN = A0;     

const int DARK_THRESHOLD = 400;      
const int VERY_DARK_THRESHOLD = 150; 

float mood = 10.0;           
bool buttonPressed = false;

unsigned long lastUpdateTimer = 0;
unsigned long darkStartTimer = 0;
bool isDark = false;

void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  unsigned long currentMillis = millis();

  int lightValue = analogRead(LDR_PIN);
  int buttonState = digitalRead(BUTTON_PIN);

  // 1. ระบบกดปุ่ม (Pet)
  if (buttonState == LOW && !buttonPressed) {
    mood += 1.0;
    if (mood > 20.0) mood = 20.0;
    buttonPressed = true;
    delay(50);
  } else if (buttonState == HIGH) {
    buttonPressed = false;
  }

  // 2. ตรวจสอบความมืด
  if (lightValue < DARK_THRESHOLD) {
    if (!isDark) {
      isDark = true;
      darkStartTimer = currentMillis;
    }
  } else {
    isDark = false;
  }

  // 3. ปรับลดค่า Mood ทุกๆ 100 มิลลิวินาที (0.1 วินาที) แทน เพื่อความเนียน
  if (currentMillis - lastUpdateTimer >= 100) {
    float elapsedSeconds = (currentMillis - lastUpdateTimer) / 1000.0;
    lastUpdateTimer = currentMillis;

    if (lightValue < VERY_DARK_THRESHOLD) {
      mood -= (2.0 * elapsedSeconds); // ลดไวเมื่อมืดมาก
    } 
    else if (isDark && (currentMillis - darkStartTimer > 3000)) {
      mood -= (1.5 * elapsedSeconds); // ลดปานกลางเมื่อมืดนานเกิน 3 วิ
    } 
    else if (!isDark) {
      mood -= (0.2 * elapsedSeconds); // ลดปกติเมื่อสว่าง
    }

    if (mood < 0.0) mood = 0.0;
  }

  // 4. เรียกใช้งานฟังก์ชันเปลี่ยนสี ทุกรอบ Loop ไฟจะเปลี่ยนตามค่า mood แบบสมูททันที
  showToyMood(mood);
}

void showToyMood(float state) {
  int redVal = 0;
  int greenVal = 0;
  int blueVal = 0;

  if (state < 10.0) {
    float brightnessInterval = 255.0 / 10.0;
    redVal = 255 - (brightnessInterval * state);
    greenVal = brightnessInterval * state;
    blueVal = 0;
  } else {
    float brightnessInterval = 255.0 / 10.0;
    redVal = 0;
    greenVal = 255 - (brightnessInterval * (state - 10.0));
    blueVal = brightnessInterval * (state - 10.0);
  }

  analogWrite(RED_PIN, redVal);
  analogWrite(GREEN_PIN, greenVal);
  analogWrite(BLUE_PIN, blueVal);
}