#include <Arduino.h>
#include <OneButton.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

#define BUTTON_RED 2
#define LED_RED 3

#define BUTTON_YELLOW 4
#define LED_YELLOW 5

#define BUTTON_BLUE 6
#define LED_BLUE 7

#define BUTTON_GREEN 8
#define LED_GREEN 9

#define EEPROM_ADDR 0

#define LCD_CHARS = 16.0

OneButton buttonRed(BUTTON_RED, true);
OneButton buttonBlue(BUTTON_BLUE, true);
OneButton buttonYellow(BUTTON_YELLOW, true);
OneButton buttonGreen(BUTTON_GREEN, true);

// set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

int ledPins[4] = {LED_RED, LED_YELLOW, LED_BLUE, LED_GREEN};

bool gameOver = true;
int interval = 2000;        // to start with
int intervalDecrease = 250; // with how many ms the interval decreases for each level
long lightShowReference;    // timestamp when one of the lights was turned on
long currentColor;
int level = 0;
int buttonPresses = 1; // determines current level
int highScore = 0;
int currentScore = 0;

byte zero[] = {B00000, B00000, B00000, B00000, B00000, B00000, B00000, B00000};
byte one[] = {B10000, B10000, B10000, B10000, B10000, B10000, B10000, B10000};
byte two[] = {B11000, B11000, B11000, B11000, B11000, B11000, B11000, B11000};
byte three[] = {B11100, B11100, B11100, B11100, B11100, B11100, B11100, B11100};
byte four[] = {B11110, B11110, B11110, B11110, B11110, B11110, B11110, B11110};
byte five[] = {B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11111};

void showWelcomeLights()
{
    // Serial.println("welcome");
    for (int i = 0; i < 2; i++)
    {
        digitalWrite(LED_BLUE, HIGH);
        delay(100);

        digitalWrite(LED_BLUE, LOW);
        digitalWrite(LED_YELLOW, HIGH);
        delay(100);

        digitalWrite(LED_YELLOW, LOW);
        digitalWrite(LED_RED, HIGH);
        delay(100);

        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_GREEN, HIGH);
        delay(100);

        digitalWrite(LED_GREEN, LOW);
    }

    delay(200);

    // blink 3 times
    for (int i = 0; i < 3; i++)
    {
        digitalWrite(LED_BLUE, HIGH);
        digitalWrite(LED_YELLOW, HIGH);
        digitalWrite(LED_RED, HIGH);
        digitalWrite(LED_GREEN, HIGH);

        delay(200);

        digitalWrite(LED_BLUE, LOW);
        digitalWrite(LED_YELLOW, LOW);
        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_GREEN, LOW);
        delay(100);
    }
}

void showWinnerLights()
{
    for (int i = 0; i < 3; i++)
    {
        digitalWrite(LED_BLUE, HIGH);
        delay(100);

        digitalWrite(LED_BLUE, LOW);
        digitalWrite(LED_YELLOW, HIGH);
        delay(100);

        digitalWrite(LED_YELLOW, LOW);
        digitalWrite(LED_RED, HIGH);
        delay(100);

        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_GREEN, HIGH);
        delay(100);

        digitalWrite(LED_GREEN, LOW);
    }
}

void showGameOverLights()
{
    // blink 3 times
    for (int i = 0; i < 3; i++)
    {
        digitalWrite(LED_BLUE, HIGH);
        digitalWrite(LED_YELLOW, HIGH);
        digitalWrite(LED_RED, HIGH);
        digitalWrite(LED_GREEN, HIGH);

        delay(200);

        digitalWrite(LED_BLUE, LOW);
        digitalWrite(LED_YELLOW, LOW);
        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_GREEN, LOW);
        delay(100);
    }
}

void showStartGameLights()
{

    digitalWrite(LED_BLUE, HIGH);
    digitalWrite(LED_YELLOW, HIGH);
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, HIGH);

    delay(1000);

    digitalWrite(LED_BLUE, LOW);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, LOW);
    delay(500);
}

void resetGameParams()
{
    gameOver = true;
    lightShowReference = 0;
    interval = 2000;
    buttonPresses = 1;
    level = 0;
    currentScore = 0;
}

void showScore()
{
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Scorul tau: ");
    lcd.print(String(currentScore));

    lcd.setCursor(0, 1);
    lcd.print("Scorul max: ");
    lcd.print(String(highScore));
}

void gameOverHandler(bool won)
{
    // store score
    currentScore = (level + 1) * 10 + buttonPresses;

    if (currentScore > highScore)
    {
        highScore = currentScore;
        EEPROM.write(EEPROM_ADDR, currentScore);
    }

    showScore();
    resetGameParams();

    if (won)
    {
        showWinnerLights();
    }
    else
    {
        showGameOverLights();
    }
}

void turnOnOffLed(int led, int state)
{
    digitalWrite(led, state);
}

void buttonPress(int ledColor)
{
    if (gameOver)
    {
        return;
    }

    if (currentColor != ledColor)
    {
        gameOverHandler(false);
    }
    else
    {
        buttonPresses++;
        lightShowReference = 0;
        turnOnOffLed(currentColor, LOW);
        delay(100);
    }
}

void clickRed()
{
    // Serial.println("click red");
    buttonPress(LED_RED);
}

void clickBlue()
{
    // Serial.println("click blue");
    if (gameOver)
    {
        // start game
        resetGameParams();
        showStartGameLights();
        gameOver = false;

        showScore();
    }
    else
    {
        buttonPress(LED_BLUE);
    }
}

void clickYellow()
{
    // Serial.println("click yellow");
    buttonPress(LED_YELLOW);
}

void clickGreen()
{
    // Serial.println("click green");
    buttonPress(LED_GREEN);
}

void setup()
{
    // Serial.begin(9600);

    // setup LED mode as output
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_YELLOW, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);

    buttonRed.attachClick(clickRed);
    buttonBlue.attachClick(clickBlue);
    buttonYellow.attachClick(clickYellow);
    buttonGreen.attachClick(clickGreen);

    lcd.init();
    lcd.createChar(0, zero);
    lcd.createChar(1, one);
    lcd.createChar(2, two);
    lcd.createChar(3, three);
    lcd.createChar(4, four);
    lcd.createChar(5, five);

    lcd.backlight();
    lcd.clear();

    highScore = EEPROM.read(EEPROM_ADDR);
    showScore();
    showWelcomeLights();
}

void updateProgressBar(unsigned long count, unsigned long totalCount, int lineToPrintOn)
{
    double factor = totalCount / 80.0;
    int percent = (count + 1) / factor;
    int number = percent / 5;
    int remainder = percent % 5;
    if (number > 0)
    {
        for (int j = 0; j < number; j++)
        {
            lcd.setCursor(j, lineToPrintOn);
            lcd.write(5);
        }
    }

    lcd.setCursor(number, lineToPrintOn);
    lcd.write(remainder);

    if (number < 16) // If using a 20 character display, this should be 20!
    {
        for (int j = number + 1; j <= 16; j++) // If using a 20 character display, this should be 20!
        {
            lcd.setCursor(j, lineToPrintOn);
            lcd.write(0);
        }
    }
}

void gameLoop()
{
    if (gameOver)
    {
        return;
    }

    long now = millis();

    if (lightShowReference == 0)
    {
        currentColor = ledPins[random(4)];
        lightShowReference = now;

        turnOnOffLed(currentColor, HIGH);
    }

    if (buttonPresses > 0 && buttonPresses % (4 + level) == 0)
    {
        level++;
        buttonPresses = 1;
    }

    int levelInterval = interval - intervalDecrease * level;

    if (levelInterval == 250)
    {
        gameOverHandler(true);

        return;
    }

    long time_passed = now - lightShowReference;

    if (time_passed > levelInterval)
    {
        gameOverHandler(false);
        
        return;
    }

    updateProgressBar(levelInterval - time_passed, levelInterval, 1);
}

void loop()
{
    buttonRed.tick();
    buttonBlue.tick();
    buttonYellow.tick();
    buttonGreen.tick();

    gameLoop();
}