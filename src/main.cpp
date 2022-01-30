#include <Arduino.h>
#include <OneButton.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

#define BUTTON_RED D10
#define BUTTON_YELLOW D4
#define BUTTON_BLUE D3
#define BUTTON_GREEN D9

#define LED_RED 10
#define LED_YELLOW D5
#define LED_BLUE D6
#define LED_GREEN D7

#define EEPROM_ADDR 0

OneButton buttonRed(BUTTON_RED, true);
OneButton buttonBlue(BUTTON_BLUE, true);
OneButton buttonYellow(BUTTON_YELLOW, true);
OneButton buttonGreen(BUTTON_GREEN, true);

// set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 20, 4); 

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
    lcd.print(currentScore);

    lcd.setCursor(0, 1);
    lcd.print("Scorul max: ");
    lcd.print(highScore);
}

void gameOverHandler(bool won)
{
    // store score
    currentScore = (level + 1) * 10 + buttonPresses;
    
    if (currentScore > highScore) {
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
    lcd.backlight();
    lcd.clear();

    highScore = EEPROM.read(EEPROM_ADDR);
    showScore();
    showWelcomeLights();
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
    }

    if (now - lightShowReference > levelInterval)
    {
        gameOverHandler(false);
    }
}

void loop()
{
    buttonRed.tick();
    buttonBlue.tick();
    buttonYellow.tick();
    buttonGreen.tick();

    gameLoop();
}