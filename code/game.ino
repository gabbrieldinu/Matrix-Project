#include "LedControl.h"
#include <EEPROM.h>
#include <LiquidCrystal.h>


const byte dinPin =12;
const byte clockPin =A2;
const byte loadPin =10;
const int lcdContrastPin = 5;
const int lcdBrightnessPin = 6;
const byte matrixSize =8;
int xMovementGame;
int yMovementGame; 
bool joyMovedGame=false;

// lcd display pins
const byte rs =9;
const byte en =8;
const byte d4 =7;
const byte d5 =A1;
const byte d6 =A0;
const byte d7 =4;

//joystick pins
const byte pinSW = 2;
const byte pinX = A4;
const byte pinY = A5;

unsigned long timerGame;
unsigned long timerGameDifficulty=0;
unsigned long timerGameLevel=0;
unsigned long timerGameSequence = 0;
unsigned long timerBetweenSequences = 0;

const int buzzerPin = 3;
int buzzerVolume = 10;
int buzzerFreqOnX = 1500;
int buzzerFreqOnY = 2000;
int lcdBrightness = 4;
int matrixBrightness = 1;
int lcdContrast = 5;

int buttonReleased;
int currentHorizontalPosition =0;
int currentVerticalPosition = 1;
int gameMenuPosition;

const char about[] = " Dinu Gabriel githublink   https//     github.com  /gabbriel    dinu                ";
       
const char howToPlay[] = " First level:  watch the  sequence of  lit squares and recreate Secnd level Similar to    level one  but squares  are dots     Third level Similar to  two but now  movement is blind        First diff  The sequence will be lit at all times Second diff Only current dot is lit. Third diff   Only newdot lit";          
int xValue = 0;
int yValue = 0;
int xMovement = 0;
int yMovement = 0;
const int minThreshold=300;
const int maxThreshold=800;
bool joyMovedOnY;
bool joyMovedOnX;
int menuPosition=0;
int settingsMenuPosition=0;
int barLevelMenuPosition=0;
int currentMenu=1;
int valueToAdjust=0;
char adjustmentName[20];
int adjustmentElementPos=0;
int longTextPosition=0;
int initialLongTextPosition=0;
int gameLevel;
int gameDifficulty;
int gameCurrentSeqPosition;
int gameLastSeqPosition;
int gameSequenceLength = 1;
int gameState = 0;
int gamePlayerX = 0;
int gamePlayerY = 0;
int gamePlayerSequencePosition;
int gameScore;
int gameScoreIncrement = 0;
int gameTimeIncrement = 0;
int gameSequenceShowInterval;
int gameMatrixLimit;
int gameMatrixIncrement;
int gameTimeRemaining;
int gamePlayerName[5] = {65, 65, 65, 65, 65};
bool gameHighscoreChanged = false;
bool gameLost = false;
int gameLimit;
unsigned long gameIntroTimer;
int lastSecond;
int pianoPosition = 0;

struct point
{
  int x,y;
}sequence[15], gamePlayer;

LedControl lc = LedControl(dinPin, clockPin, loadPin,1);
LiquidCrystal lcd(rs,en,d4,d5,d6,d7);

const int menuOptionsNumber = 8;
const int settingsMenuOptionsNumber = 4;
const int maxNumbersOfChars = 20;

int currentBrightness=1;
unsigned int timerButtonPress;
int buttonState;
int lastButtonState;


void displayImage( byte image[8]) {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      lc.setLed(0, i, j, bitRead(image[i], 7 - j));
    }
  }
}

struct AdjustableSetting
{
  char name[13];
  int value;
  int increment;
  int pin;
};

struct Highscore
{
  char name[5];
  int score;
} highscores[6];


AdjustableSetting menuSettingsOptions[settingsMenuOptionsNumber]= {
  {"Contrast", 10 , 10, lcdContrastPin},
  {"Brightness", 4 , 25, lcdBrightnessPin},
  {"Mat Bright",5,4, 0},
  {"Volume",10,10,buzzerPin}
};

void setup() 
{

  

  pinMode(pinSW, INPUT_PULLUP);
  pinMode(lcdContrastPin, OUTPUT);
  pinMode(lcdBrightnessPin, OUTPUT);
  lcd.begin(16,2);// Print a message to the LCD.
  Serial.begin(9600);

  byte specialChar[8];


  EEPROM.get(208 ,specialChar);
  lcd.createChar(0, specialChar);

  EEPROM.get(200  ,specialChar);
  lcd.createChar(1, specialChar);

  EEPROM.get(216  ,specialChar);
  lcd.createChar(2, specialChar);

  EEPROM.get(224  ,specialChar);
  lcd.createChar(3, specialChar);

  lc.shutdown(0, false);
  lc.clearDisplay(0);
  
  getSettingsFromEEPROM();
  lc.setIntensity(0, menuSettingsOptions[2].value);
  
  

  getHighscoresFromEEPROM();
  sortHighscoresDesc();
  lc.setIntensity(0, menuSettingsOptions[2].value + menuSettingsOptions[2].increment);
  
  analogWrite(menuSettingsOptions[0].pin,menuSettingsOptions[0].value * menuSettingsOptions[0].increment);
  analogWrite(menuSettingsOptions[1].pin, menuSettingsOptions[1].value * menuSettingsOptions[1].increment );

  }
void sortHighscoresDesc()
{
  for (int i = 0 ; i < 5; i++)
  {
    for (int j = i+1;j<6;j++)
    {
      if (highscores[i].score < highscores[j].score)
      {
        Highscore aux = highscores[i];
        highscores[i] = highscores[j];
        highscores[j] = aux;

      }
    }
  }
}

void updateHighscores()
{
  for (int i = 0, j =20 ;i<6;i++,j+=7)
  {
    EEPROM.put(j,highscores[i].name);
    EEPROM.put(j+5,highscores[i].score);
  }
}

void getHighscoresFromEEPROM()
{
  for (int i = 0, j =20 ;i<6;i++,j+=7)
  {
    EEPROM.get(j,highscores[i].name);
    EEPROM.get(j+5,highscores[i].score);
  }
}
void resetHighScores()
{
  char null[5] = "     ";
  int zero = 0;
  for (int i = 0, j =20 ;i<6;i++,j+=7)
  {
    EEPROM.put(j,null);
    EEPROM.put(j+5,zero);
  }
  
}

void getSettingsFromEEPROM()
{
   EEPROM.get(0, menuSettingsOptions[0].value);
  EEPROM.get(2, menuSettingsOptions[1].value);
  EEPROM.get(4, menuSettingsOptions[2].value);
  EEPROM.get(6, menuSettingsOptions[3].value);

}
// Menu
char menuOptions[menuOptionsNumber][maxNumbersOfChars] = {
  "---- Menu ----",
  " Start Game",
  " Settings ",
  " Highscore ",
  "How to play",
  "About",
  "Reset Scores",
  ""
};



bool matrix[8][8];

int i = 0;

void buzz(int freq)
{
  if (menuSettingsOptions[3].value > 0)
  tone(buzzerPin, freq);
}
void stopBuzz()
{
  noTone(buzzerPin);
}


int checkJoyMovementOnY()
{
  int dir=0;
  yValue = analogRead(pinY);
   if (yValue < minThreshold && joyMovedOnY == false ) 
  {
    dir= 1;
    joyMovedOnY = true;
  }

  if (yValue >= minThreshold && yValue <= maxThreshold && joyMovedOnY == true)
  {
    joyMovedOnY = false;
  }

  if (yValue > maxThreshold && joyMovedOnY == false ) 
  {
    
    joyMovedOnY = true;
    dir = -1;
  }
if (dir == 0)
  {
    stopBuzz();
  }
  else
  {
    buzz(buzzerFreqOnY);
  }    
  
  return dir;
}
int checkJoyMovementOnX()
{
  int dir=0;
  xValue = analogRead(pinX);
  if (xValue >= minThreshold && xValue <= maxThreshold && joyMovedOnX==true)
  {
    joyMovedOnX = false;
  }
   if (xValue < minThreshold && joyMovedOnX == false ) 
  {
    dir = 1;
    joyMovedOnX = true;
  }

  if (xValue > maxThreshold && joyMovedOnX == false ) 
  {
    
    joyMovedOnX = true;
    dir = -1;
  }
if (dir == 0)
  {
    stopBuzz();
  }
  else
  {
    buzz(buzzerFreqOnX);
  }
  
  return dir;
}


void saveAdjustmentsToEEPROM()
{
  EEPROM.put(0, menuSettingsOptions[0].value);
  EEPROM.put(2, menuSettingsOptions[1].value);
  EEPROM.put(4, menuSettingsOptions[2].value);
  EEPROM.put(6, menuSettingsOptions[3].value);
}

void displayLongTextDownArrow()
{
  lcd.setCursor(15, 1);
  lcd.write(byte(0));
}

void displayLongTextUpArrow()
{
  lcd.setCursor(15, 0);
  lcd.write(byte(1));
}


void displayCurrentVerticalPosition()
{
  lcd.setCursor(0, currentVerticalPosition%2);
  lcd.write(byte(3));
}

void displayLongText(char text[])
{
  
  int i,lastCharacterPosition, remainingCharacters, iterator,
  xMovement = checkJoyMovementOnX(), 
  yMovement = checkJoyMovementOnY();

  iterator = longTextPosition;
  remainingCharacters = strlen(text) - longTextPosition;
  
  displayCurrentVerticalPosition();
  if (longTextPosition < 24)
  {
    displayLongTextDownArrow();
  }
  if (longTextPosition >= 24 && remainingCharacters > 24)
  {
    displayLongTextUpArrow();
    displayLongTextDownArrow();
  }
  if (remainingCharacters < 24)
  {
    displayLongTextUpArrow();
  }
  if (remainingCharacters > 0)
  {
    if ( remainingCharacters < 14)
      lastCharacterPosition = remainingCharacters;
    else
      lastCharacterPosition = 14;
    for (i = 2; i < lastCharacterPosition ; i ++)
    {
      lcd.setCursor(i, 0);
      iterator++;
      lcd.print(text[iterator]);
    }
  }
  remainingCharacters -= 12;
  if (remainingCharacters > 0)
  {
    if ( remainingCharacters < 14)
      lastCharacterPosition = strlen(text);
    else
      lastCharacterPosition = 14;
    for (i = 2; i < lastCharacterPosition ; i ++)
    {
      lcd.setCursor(i, 1);
      iterator++;
      lcd.print(text[iterator]);
      
    }
  }

  if (currentVerticalPosition == 0)
  {
    if (displayLeftMenu(xMovement))
      {
        currentMenu = 1;
        currentVerticalPosition = menuPosition;
        menuPosition = 0;
        lcd.clear();
      }
  }
  
  int limit;
  if (strlen(text)>200)
    limit = 24;
  else
  {
    limit = 4;
  }
  if (yMovement == -1 && currentVerticalPosition < limit)
  {
    if (currentVerticalPosition % 2 == 1)
       longTextPosition += 25;
    currentVerticalPosition ++;
    lcd.clear();
   
  }
 
  if (yMovement == 1 && currentVerticalPosition > 0)
  
      {
        if (currentVerticalPosition % 2 == 0)
        {
          longTextPosition -= 25;

        }
          
       currentVerticalPosition --;
        lcd.clear();
      }   
 
  
}

bool displayRightMenu(int xValue)
{
  lcd.setCursor(14, currentVerticalPosition%2);
  lcd.print(">");
  if (xValue == 1)
    return true;
  return false;
}
bool displayLeftMenu(int xValue)
{
  lcd.setCursor(1, currentVerticalPosition%2);
  lcd.print("<");
  if (xValue == -1)
    return true;
  return false;

}

void writeToScreen(char s1[], char s2[])
{
  lcd.setCursor(2,0);
  lcd.print(s1);
  lcd.setCursor(2,1);
  lcd.print(s2);
}

void displayLevelBar(int value)
{
  int offset = 3;
  for (int i = offset;i < value + offset;i++)
  {
      lcd.setCursor(i,1);
      lcd.write(byte(2));
  }
  
}

void mainMenu()
{
  xMovement = checkJoyMovementOnX();
  yMovement = checkJoyMovementOnY();
  menuPosition = currentVerticalPosition;
  if (currentVerticalPosition % 2 == 0)
    writeToScreen(menuOptions[currentVerticalPosition],menuOptions[currentVerticalPosition+1]);
  else
    writeToScreen(menuOptions[currentVerticalPosition-1],menuOptions[currentVerticalPosition]);
  displayCurrentVerticalPosition();
  if ( currentVerticalPosition == 1)
  {
    displayRightMenu(xMovement);
    if (xMovement == 1)
    {
      lcd.clear();
      currentVerticalPosition = 0;
      currentHorizontalPosition = 1;
      
      timerGameLevel = millis();
      currentMenu = 7;
      gameMenuPosition = 1;
    }
  }
  if (currentVerticalPosition == 2)
  {
      
      if (displayRightMenu(xMovement))
        {
          lcd.clear();
          currentMenu = 2;
          menuPosition = currentVerticalPosition;
          currentVerticalPosition = 0;
          
        }
  }
  if (currentVerticalPosition == 3)
  {
    if (displayRightMenu(xMovement))
    {
      menuPosition = currentVerticalPosition;
      currentVerticalPosition = 0;
      lcd.clear();
      currentMenu = 6;
    }
  }
  if ( currentVerticalPosition == 4 || currentVerticalPosition == 5)
  {
      if(displayRightMenu(xMovement))
        {
          currentMenu = 4;
          menuPosition = currentVerticalPosition;
          currentVerticalPosition = 0;
          lcd.clear();
        }
  }

  if (currentVerticalPosition == 6)
  {
     if(displayRightMenu(xMovement))
        {
          resetHighScores();
          getHighscoresFromEEPROM();
          currentMenu = 1;
          currentVerticalPosition = 1;
          lcd.clear();
        }
  }
  
  if(yMovement == -1 && currentVerticalPosition < menuOptionsNumber - 1 )
  {
    currentVerticalPosition++;
    lcd.clear();
  }
  else
  {
    if(yMovement == 1 && currentVerticalPosition > 1)
    {
        currentVerticalPosition--;
      lcd.clear();
    }
      
  }

    
}

void settingsMenu()
{
   if (currentVerticalPosition % 2 == 0)
    writeToScreen(menuSettingsOptions[currentVerticalPosition].name,menuSettingsOptions[currentVerticalPosition+1].name);
  else
    writeToScreen(menuSettingsOptions[currentVerticalPosition-1].name,menuSettingsOptions[currentVerticalPosition].name);
  
  displayCurrentVerticalPosition();
  if (currentVerticalPosition < 2)
  {
    displayLongTextDownArrow();
  }
  if (currentVerticalPosition > 1 )
  {
    displayLongTextUpArrow();
  }
  xMovement = checkJoyMovementOnX();
  yMovement = checkJoyMovementOnY();

  if(yMovement == -1 && currentVerticalPosition < settingsMenuOptionsNumber - 1)
  {
    currentVerticalPosition++;
    lcd.clear();
  }
  else
  {
    if(yMovement == 1 && currentVerticalPosition > 0)
    {
      currentVerticalPosition--;
      lcd.clear();
    }
      
  }
  
  if (currentVerticalPosition == 0)
  {
    if (displayLeftMenu(xMovement))
      {
        lcd.clear();
        currentMenu = 1;
        saveAdjustmentsToEEPROM();
        currentVerticalPosition = menuPosition;
      }
  }
  
  
    if (displayRightMenu(xMovement))
      {
        currentMenu = 3;
        for (int i = 0; i < 14; i++)
          adjustmentName[i] = menuSettingsOptions[currentVerticalPosition].name[i];
        adjustmentElementPos = currentVerticalPosition;
        currentVerticalPosition = 0;
        menuPosition = 0;
        lcd.clear();
      }
  
}

void displayAdjustmentSigns()
{
  lcd.setCursor(2,1);
  lcd.print("-");
  lcd.setCursor(13,1);
  lcd.print("+");
}
void adjustmentMenu(char name[], int pos)
{ 
  
  writeToScreen(name,"");
  displayLevelBar(menuSettingsOptions[pos].value);
  displayCurrentVerticalPosition();

  xMovement = checkJoyMovementOnX();
  yMovement = checkJoyMovementOnY();

  if(yMovement == -1 && currentVerticalPosition < 1)
  {
    currentVerticalPosition ++;
    lcd.clear();
  }
  if(yMovement == 1 && currentVerticalPosition > 0 )
  {
    currentVerticalPosition -- ;
    lcd.clear();
  }

  if (currentVerticalPosition == 0)
  {
    displayLongTextDownArrow();
     if (displayLeftMenu(xMovement))
    {
      currentMenu = 2 ;
      currentVerticalPosition = 0;
      if ( pos ==3)
       EEPROM.put(6,menuSettingsOptions[pos].value);
      lcd.clear();
    }
    

  }
  if (currentVerticalPosition == 1)
  {
    displayLongTextUpArrow();
    displayAdjustmentSigns();
    if ( pos == 0 || pos == 1)
    {
      analogWrite(menuSettingsOptions[pos].pin,menuSettingsOptions[pos].value*menuSettingsOptions[pos].increment);
      
      if (xMovement == -1 &&  menuSettingsOptions[pos].value > 1)
      {
        menuSettingsOptions[pos].value --;
        lcd.clear();
      }
      if (xMovement == 1 && menuSettingsOptions[pos].value < 10)
      {
        menuSettingsOptions[pos].value ++;
        lcd.clear();
      }
    }
    if (pos == 2)
    {
      lc.setIntensity(0,menuSettingsOptions[pos].value + menuSettingsOptions[pos].increment);
      if (xMovement == -1 &&  menuSettingsOptions[pos].value > 1)
      {
        menuSettingsOptions[pos].value --;
        lcd.clear();
      }
      if (xMovement == 1 && menuSettingsOptions[pos].value < 10)
      {
        menuSettingsOptions[pos].value ++;
        lcd.clear();
      }
    }
    if ( pos == 3)
    {
     
      if (xMovement == 1 &&  menuSettingsOptions[pos].value < 10)
      {
        menuSettingsOptions[pos].value = 10;
        lcd.clear();
      }
      if (xMovement == -1 && menuSettingsOptions[pos].value > 0)
      {
        menuSettingsOptions[pos].value = 0;
        lcd.clear();
      }
    }
  }
  
  
}
// Highscore

void displayHighscores( int position)
{
  char name[5];

  //   String scoreString = String(highscores[position].score);
  // scoreString.toCharArray(score, 4);
  strcpy(name,highscores[position].name);
    lcd.setCursor(3, 0);
    lcd.print(char('1'+position));
    for (int i=0;i<strlen(name);i++)
    {
      lcd.setCursor(i+5,0);
      lcd.print(name[i]);
    }
    lcd.setCursor(10,0);
    lcd.print(":");
    lcd.setCursor(11,0);
    lcd.print(highscores[position].score);

    position++;
    // scoreString = String(highscores[position].score);
    // scoreString.toCharArray(score, 4);
    strcpy(name,highscores[position].name);
    lcd.setCursor(3, 1);
    lcd.print(char('1'+position));
    for (int i=0;i<strlen(name);i++)
    {
      lcd.setCursor(i+5,1);
      lcd.print(name[i]);
    }
    lcd.setCursor(10,1);
    lcd.print(":");
    lcd.setCursor(11,1);
    lcd.print(highscores[position].score);
    


}

void highscoreScreen()
{
  int xMovement, yMovement;
  xMovement = checkJoyMovementOnX();
  yMovement = checkJoyMovementOnY();

  if( currentVerticalPosition % 2 == 0)
  {
    displayHighscores(currentVerticalPosition);
  }
  else
  {
    displayHighscores(currentVerticalPosition - 1);
  }
  if (currentVerticalPosition == 0)
  {
    if (displayLeftMenu(xMovement))
    {
  
      lcd.clear();
      currentMenu = 1;
      currentVerticalPosition = menuPosition;
      menuPosition = 0;
    }
  }
  displayCurrentVerticalPosition();
  if ( yMovement == -1 && currentVerticalPosition < 5)
  {
    currentVerticalPosition ++;
    lcd.clear();
  }

  if ( yMovement == 1 && currentVerticalPosition > 0)
  {
    currentVerticalPosition --;
    lcd.clear();
  }


  
}

// Awesome Game

void turnOnEntireMatrix()
{
  for (int row = 0; row < matrixSize; row++) 
  {
    for (int col = 0; col < matrixSize; col++) {
     lc.setLed(0, row, col, true);
  }}}
void turnOffEntireMatrix()
{
  for (int row = 0; row < matrixSize; row++) {
for (int col = 0; col < matrixSize; col++) {
lc.setLed(0, row, col, false);
}}}
void displayCurrentHorizontalPosition()
{
  int poz = 1, sign = 1;
  if ( gameMenuPosition == 1 || gameMenuPosition == 2)
  {
    if (currentHorizontalPosition > 0)
      {
        poz = 3 + (3*currentHorizontalPosition);
        sign = 0;
      }
  }

  if (gameMenuPosition == 5)
  {
    if (currentHorizontalPosition < 5)
      {
        poz = 3 + currentHorizontalPosition;
        sign = 0;
      }
    else
    {
      poz = 12;
    }
  }
  
    
  lcd.setCursor(poz , 0);
  lcd.write(byte(sign));
}



void displayGameLevel()
{
  int xMovement = checkJoyMovementOnX();
  int yMovement = checkJoyMovementOnY();
  if ( millis() - timerGameLevel < 3000)
  {
      lcd.setCursor(2, 0);
      lcd.print("Choose Game");
      lcd.setCursor(5, 1);
      lcd.print("Level");
  }
  if ( millis() - timerGameLevel < 3200 && millis() - timerGameLevel > 3000)
  {
    lcd.clear();
  }
  if ( millis() - timerGameLevel > 3200)
  {
      lcd.setCursor(0,1);
      lcd.print("Back");
      lcd.setCursor(6, 1);
      lcd.print("1");

      lcd.setCursor(9, 1);
      lcd.print("2");

      lcd.setCursor(12, 1);
      lcd.print("3");

      displayCurrentHorizontalPosition();

      if (currentHorizontalPosition == 0 && yMovement == 1)
      {
        lcd.clear();
          currentMenu = 1;
          currentVerticalPosition = 1;
      }

      if ( xMovement == 1 && currentHorizontalPosition < 3)
      {
        currentHorizontalPosition++;
        lcd.clear();
      }
      if ( xMovement == -1 && currentHorizontalPosition > 0 )
      {
        {
          currentHorizontalPosition--;
          lcd.clear();
        }
      }

      if (yMovement == -1 && currentHorizontalPosition > 0)
      {
        gameLevel = currentHorizontalPosition;
        if (gameLevel == 1)
        {
          gameMatrixLimit = 6;
          gameMatrixIncrement = 3;
        }
        else
        {
          gameMatrixLimit = 8;
          gameMatrixIncrement = 1;
        }
        gameMenuPosition = 2;
        currentHorizontalPosition = 1;
        lcd.clear();
        timerGameDifficulty = millis();
      }
  }
  

}

void displayGameDifficulty()
{
  int xMovement = checkJoyMovementOnX();
  int yMovement = checkJoyMovementOnY();
  if ( millis() - timerGameDifficulty < 2000)
  {
      lcd.setCursor(2, 0);
      lcd.print("  Set Game");
      lcd.setCursor(3, 3);
      lcd.print("Difficulty");
  }
  if ( millis() - timerGameDifficulty < 2200 && millis() - timerGameDifficulty > 2000)
  {
    lcd.clear();
  }
  if ( millis() - timerGameDifficulty > 2200)
  {
      lcd.setCursor(0,1);
      lcd.print("Back");
      lcd.setCursor(6, 1);
      lcd.print("1");

      lcd.setCursor(9, 1);
      lcd.print("2");

      lcd.setCursor(12, 1);
      lcd.print("3");

      displayCurrentHorizontalPosition();

      if (currentHorizontalPosition == 0 && yMovement == 1)
      {
          lcd.clear();
          gameMenuPosition = 1;
          timerGameLevel = millis();
          currentVerticalPosition = 0;
          currentHorizontalPosition = 0;
      }

      if ( xMovement == 1 && currentHorizontalPosition < 3)
      {
        currentHorizontalPosition++;
        lcd.clear();
      }
      if ( xMovement == -1 && currentHorizontalPosition > 0 )
      {
        {
          currentHorizontalPosition--;
          lcd.clear();
        }
      }

      if (yMovement == -1 && currentHorizontalPosition > 0)
      {
        gameDifficulty = currentHorizontalPosition;
        if (gameDifficulty == 1)
        {
          gameScoreIncrement = 1;
          gameTimeIncrement = 5;
          gameSequenceShowInterval = 3000;
          gameLimit = 9;
        }
        if (gameDifficulty == 2)
        {
          gameScoreIncrement = 3;
          gameTimeIncrement = 4;
          gameSequenceShowInterval = 2000;
          gameLimit = 12;
        }
        if (gameDifficulty == 3)
        {
          gameScoreIncrement = 5;
          gameTimeIncrement = 3;
          gameSequenceShowInterval = 1000;
          gameLimit = 15;
        }
        gameScoreIncrement *= gameLevel;

        gameMenuPosition = 3;
        gameIntroTimer = millis();
        turnOffEntireMatrix();
        gameState = 1;
        gameSequenceLength = 0;
        gameCurrentSeqPosition = 0;
        timerGameSequence = 10;
        increaseSequence();
        gameScore = 0;
        lcd.clear();
        stopBuzz();
      }
  }
  

}

void displayGameScore()
{
  lcd.setCursor(0, 0);
  lcd.print("Score: ");
  lcd.setCursor(8,0);
  lcd.print(gameScore);
  
}
void displayGameTimeRemaining(int time)
{
  lcd.setCursor(0, 1);
  lcd.print("Time: ");
  lcd.setCursor(8,1);
  lcd.print(time); 
}

void gameMenu()
{
  if (gameMenuPosition == 1)
  {
    displayGameLevel();
  }
  if( gameMenuPosition == 2)
  {
    displayGameDifficulty();
  }
  if (gameMenuPosition == 3)
  {
    byte countDown[3][8];
    EEPROM.get(156,countDown);
    if ( millis() - gameIntroTimer < 1000)
    {
      displayImage(countDown[0]);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(" Game starts in");
      lcd.setCursor(8,1);
      lcd.print("3");
    }
    if ( millis() - gameIntroTimer > 1000 &&  millis() - gameIntroTimer < 2000)
    {
      displayImage(countDown[1]);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(" Game starts in");
      lcd.setCursor(8,1);
      lcd.print("2");
    }
    if ( millis() - gameIntroTimer > 2000 && millis() - gameIntroTimer < 3000)
    {
      displayImage(countDown[2]);
      
      lcd.setCursor(0,0);
      lcd.print(" Game starts in");
      lcd.setCursor(8,1);
      lcd.print("1");
      
    }
    if ( millis() - gameIntroTimer > 3000 && millis() - gameIntroTimer < 3100)
    {
      turnOffEntireMatrix();
      lcd.clear();
      timerGame = millis();
    }
    if ( millis() - gameIntroTimer > 3100)
    {
      game();
    }
    
  }
  
  if ( gameMenuPosition == 4)
  {
    endGameMenu();
  }
  if (gameMenuPosition == 5)
  {
    postGameMenu();
  }
}
void turnOnPixel(int posX, int posY) {
  lc.setLed(0, posX, posY, true);
}
void turnOffPixel(int posX, int posY) {
  lc.setLed(0, posX, posY, false);
}
void turnOnSquare(int posX, int posY)
{
  lc.setLed(0, posX, posY, true);
  lc.setLed(0, posX+1, posY, true);
  lc.setLed(0, posX, posY+1, true);
  lc.setLed(0, posX+1, posY+1, true);
}
void turnOffSquare(int posX, int posY)
{
  lc.setLed(0, posX, posY, false);
  lc.setLed(0, posX+1, posY, false);
  lc.setLed(0, posX, posY+1, false);
  lc.setLed(0, posX+1, posY+1, false);
}


void increaseSequence()
{
  lcd.clear();
  
  gameScore += gameScoreIncrement + (gameSequenceLength*gameLevel) + (gameTimeRemaining * gameLevel);
  timerGameSequence += gameTimeIncrement;
  displayGameScore();
  displayGameTimeRemaining(timerGameSequence);
  int ok=0,x,y;
  while(ok==0)
  {
    ok = 1;
    if ( gameLevel == 1)
    {
      x = random(0,3 )* gameMatrixIncrement;
      y = random(0,3) * gameMatrixIncrement;
    }
    else
    {
      x= random(0,gameMatrixLimit );
      y = random(0,gameMatrixLimit);
    }

    for (int i =0;i<gameSequenceLength;i++)
      if(sequence[i].x == x && sequence[i].y==y)
        {
          ok = 0;
          i = gameSequenceLength;
        }
  }
  sequence[gameSequenceLength].x = x;
  sequence[gameSequenceLength].y = y;
  gameSequenceLength++;
}

void displayBetweenSequenceStatus(byte image[8], unsigned long timer)
{
  bool functionOver = false;
  while (functionOver == false)
  {
    if (millis() - timer < 1000)
    {
      turnOffEntireMatrix();
    }
    if ( millis() - timer > 1000 && millis() - timer < 3000)
    {
      displayImage(image);
    }
    if (millis() - timer > 3000 && millis() - timer < 3600)
    {
      turnOffEntireMatrix();
      
    }
    if (millis() - timer > 3600)
    {
      functionOver = true;
      
    }
  }
}

void displayNameIntroduction()
{
  displayCurrentHorizontalPosition();
  lcd.setCursor(2, 1);
  lcd.print("<");
  lcd.setCursor(8, 1);
  lcd.print(">");
  lcd.setCursor(10, 1);
  lcd.print("save");
  for ( int i = 0, j = 3; i < 5; i++, j ++)
  {
    lcd.setCursor(j, 1);
    lcd.print((char)(gamePlayerName[i]));
  }
}

void displayWonGameMessage()
{
  lcd.setCursor(0, 0);
  lcd.print("Spot on memory");
  lcd.setCursor(0, 1);
  lcd.print("Level won");
}

void displayLostGameMessage()
{
  lcd.setCursor(0, 0);
  lcd.print("Wrong dot chosen");
  lcd.setCursor(0, 1);
  lcd.print(" Game over");
}

void displayHighscoreBeatMessage()
{
  lcd.setCursor(0, 0);
  lcd.print(" You beat");
  lcd.setCursor(0, 1);
  lcd.print("a highscore");
}

void displayRequestNameMessage()
{
  lcd.setCursor(0, 0);
  lcd.print("We need a name");
  lcd.setCursor(0, 1);
  lcd.print("for leaderboard");
}

void displayNoHighscoreMessage()
{
  lcd.setCursor(0, 0);
  lcd.print(" You did not");
  lcd.setCursor(0, 1);
  lcd.print("set a highscore");
}
void displayEncouragingMessage()
{
  lcd.setCursor(0, 0);
  lcd.print("You can do");
  lcd.setCursor(0, 1);
  lcd.print("better than that");
}

void displayAllHighscoresBeatMessage()
{
  lcd.setCursor(0, 0);
  lcd.print("You beat all");
  lcd.setCursor(0, 1);
  lcd.print("highscores");
}
void endGame()
{
  turnOffEntireMatrix();
  gameMenuPosition = 4;
  
  timerGame=millis();
  lcd.clear();
}
void endGameMenu()
{
  int currentSecond = (millis() - timerGame) / 1000;
  if ( currentSecond < 5)
  {
     if (gameLost == false)
    {
      byte image[8];
      EEPROM.get(116,image);
      displayImage(image);
      displayWonGameMessage();
    }
    else
    {   byte image[8];
        EEPROM.get(124,image);
        displayImage(image);
        displayLostGameMessage();
    }
  }
 
  if (currentSecond > 3)
  {
    lcd.clear();
    gameMenuPosition =5;
    timerGame = millis();
    currentHorizontalPosition = 0;
  }
}
void postGameMenu()
{

    if ( gameScore > highscores[5].score)
    {
      byte image[8];
      EEPROM.get(140,image);
      displayImage(image);
      gameHighscoreChanged = true;
      if (millis() - timerGame < 3000)
      {
        if (gameScore > highscores[0].score)
          displayAllHighscoresBeatMessage();
        else
          displayHighscoreBeatMessage();
      }

      if (millis() - timerGame > 3000 && millis() - timerGame < 3200)
      {
        lcd.clear();
      }

      if (millis() - timerGame > 3200 && millis() - timerGame < 6200)
      {
        displayRequestNameMessage();
      }

      if (millis() - timerGame > 6200 && millis() - timerGame < 6400)
      {
        lcd.clear();
      }
      if (millis() - timerGame > 6400)
      {
        
        highscores[5].score = gameScore;
        bool nameIntroduced = false;
        while (! nameIntroduced)
        {
          displayNameIntroduction();
          int xMovement = checkJoyMovementOnX();
          int yMovement = checkJoyMovementOnY();

          if ( xMovement == 1 && currentHorizontalPosition < 5)
          {
            currentHorizontalPosition ++;
            lcd.clear();
          }
          if ( xMovement == -1 && currentHorizontalPosition > 0)
          {
            currentHorizontalPosition --;
            lcd.clear();
        }

        if ( yMovement == -1 && currentHorizontalPosition == 5)
        {
          nameIntroduced = true;
          currentMenu = 1;
          currentVerticalPosition = 1;
          lcd.clear();
        }

        if ( yMovement == 1 && gamePlayerName[currentHorizontalPosition] < 90)
        {
          if (gamePlayerName[currentHorizontalPosition] == 32)
            gamePlayerName[currentHorizontalPosition] = 64;
          gamePlayerName[currentHorizontalPosition] ++;
          lcd.clear();
        }

        if ( yMovement == -1 && gamePlayerName[currentHorizontalPosition] > 64)
        {
          gamePlayerName[currentHorizontalPosition] --;
          if (gamePlayerName[currentHorizontalPosition] == 64)
            gamePlayerName[currentHorizontalPosition] = 32;
          lcd.clear();
        }
      }
      for (int i=0;i<5;i++)
      {
          highscores[5].name[i] = (char)(gamePlayerName[i]);
      }
      sortHighscoresDesc();
      updateHighscores();
      currentMenu = 1;
      currentVerticalPosition = 1;
      lcd.clear();
      
      }
      
    
    
    }
    else
    {
      gameHighscoreChanged = true;
      if (millis() - timerGame < 3000)
      {
          displayNoHighscoreMessage();
      }

      if (millis() - timerGame > 3000 && millis() - timerGame < 3200)
      {
        lcd.clear();
      }

      if (millis() - timerGame > 3200 && millis() - timerGame < 6200)
      {
        displayEncouragingMessage();
      }

      if (millis() - timerGame > 6200)
      {
        currentMenu = 1;
        currentVerticalPosition = 1;
        lcd.clear();
      }
    }
  
  
}
void game() 
{
  displayGameScore();
  if (gameState == 1)
  {
    
    int currentSecond = (millis() - timerGame) / gameSequenceShowInterval;
    
    if ( currentSecond < gameSequenceLength)
    {
      if (currentSecond > gameCurrentSeqPosition )
      { 

        if (gameDifficulty == 2)
        {
          if (gameLevel > 1)
            turnOffPixel(sequence[gameCurrentSeqPosition].x,sequence[gameCurrentSeqPosition].y);
          else
          {
            turnOffSquare(sequence[gameCurrentSeqPosition].x,sequence[gameCurrentSeqPosition].y);
          }
        }
        
        gameCurrentSeqPosition ++;
      }
      if (gameDifficulty < 3)
      {
        if (gameLevel > 1)
        {
          turnOnPixel(sequence[gameCurrentSeqPosition].x,sequence[gameCurrentSeqPosition].y);
        }
        else
        {
          turnOnSquare(sequence[gameCurrentSeqPosition].x,sequence[gameCurrentSeqPosition].y);
        }
      }

      if (gameDifficulty == 3)
      {
       if (currentSecond == gameSequenceLength - 1)
       {
         if (gameLevel > 1)
        {
          turnOnPixel(sequence[gameCurrentSeqPosition].x,sequence[gameCurrentSeqPosition].y);
        }
        else
        {
          turnOnSquare(sequence[gameCurrentSeqPosition].x,sequence[gameCurrentSeqPosition].y);
        }
       }
      }

      
    }
    if (currentSecond >= gameSequenceLength  && currentSecond <= gameSequenceLength < 1)
      {
      
        turnOffEntireMatrix();
      }
    if (currentSecond >= gameSequenceLength + 1)
      {
        unsigned long currentTimes = millis();
        gamePlayerSequencePosition = 0;
        timerGame = currentTimes;
        gamePlayerX = 0;
        gamePlayerY = 0;
        gameState = 2;
      }
  }
  if ( gameState  == 2)
  {
    int xMovement = checkJoyMovementOnX();
    int yMovement = checkJoyMovementOnY();
    int initialX, initialY;
    initialX = gamePlayerX;
    initialY = gamePlayerY;
    byte swReading = digitalRead(pinSW);
    int currentSecond = (millis() - timerGame) / 1000;
    if ( lastSecond < currentSecond)
    {
      lcd.clear();
    }
    lastSecond = currentSecond;
    gameTimeRemaining = timerGameSequence - currentSecond;

    if (gameLevel < 3)
    {
      
        if (gameLevel > 1)
          turnOnPixel(gamePlayerX,gamePlayerY);
        else
        {
          turnOnSquare(gamePlayerX,gamePlayerY);
        }
        
    }

    if ( gameLevel == 3 && gamePlayerSequencePosition == 0)
    {
          turnOnPixel(0,0);
    }

    

    
    displayGameTimeRemaining(gameTimeRemaining);

    if (xMovement == 1 && gamePlayerX > 0)
    {
      gamePlayerX -= gameMatrixIncrement;
    }
    if (xMovement == -1 && gamePlayerX < gameMatrixLimit - 1)
    {
      gamePlayerX += gameMatrixIncrement;
    }
    if (yMovement == 1 && gamePlayerY > 0)
    {
      gamePlayerY -= gameMatrixIncrement;
    }
    if (yMovement == -1 && gamePlayerY < gameMatrixLimit - 1)
    {
      gamePlayerY += gameMatrixIncrement;
    }

    if (gameTimeRemaining < 1 )
    {
      byte image[8];
      EEPROM.get(148,image);
      displayBetweenSequenceStatus(image,millis());
      gameLost = true;
      endGame();
    }
    if (swReading != lastButtonState)
    {
      timerButtonPress = millis();
    }

    if ((millis() - timerButtonPress) > 100) 
    {
      if (swReading != buttonState) 
      {
        buttonState = swReading;
        if (buttonState == HIGH) 
        {
          if ( gamePlayerX == sequence[gamePlayerSequencePosition].x && gamePlayerY == sequence[gamePlayerSequencePosition].y)
          {
            turnOffEntireMatrix();
            if (gameLevel ==  3)
            {
                turnOnPixel(gamePlayerX,gamePlayerY);
            }
            gamePlayerSequencePosition ++;
          }
          else
          {
            byte image[8];
            EEPROM.get(108,image);
            displayBetweenSequenceStatus(image,millis());
            gameLost = true;
            endGame();
            
            
          }

          if (gamePlayerSequencePosition >= gameSequenceLength)
          {
            if ( gameSequenceLength < gameLimit)
            {
              byte image[8];
              EEPROM.get(100,image);
              displayBetweenSequenceStatus(image,millis());
              increaseSequence();
              gameCurrentSeqPosition = 0;
              timerGame = millis();
              gameState = 1;
              
            }
            else
            {
              byte image[8];
              EEPROM.get(100,image);
              displayBetweenSequenceStatus(image,millis());
              endGame();
              gameLost = false;
            }
            
          }
        }
      }
    
      
    }



    lastButtonState = swReading;
      if (gameLevel < 3)
      {
        if (xMovement != 0 || yMovement !=0)  
        {
          if (gameLevel == 1)
          {
            turnOffSquare(initialX,initialY);
          }
          else
          {
            turnOffPixel(initialX,initialY);
          }
        }
      }
  }
  
}



void loop() 
{

  if (currentMenu == 1)
  {
    byte image[8];
    EEPROM.get(180,image);
    displayImage(image);
    mainMenu();
  }
  else
  {
    if (currentMenu == 2)
    {
      byte image[8];
      EEPROM.get(132,image);
      displayImage(image);
      settingsMenu();
    }
    else
    {
      if (currentMenu == 3)
      {
        adjustmentMenu(adjustmentName,adjustmentElementPos);
      }
      else
      {
        if (currentMenu == 4)
        {
        byte image[8];
              EEPROM.get(188,image);
              displayImage(image);
          if( menuPosition == 4)
            displayLongText(howToPlay);
          else
            displayLongText(about);
        }
        else
        {
          if (currentMenu == 6)
          {
            byte image[8];
            EEPROM.get(140,image);
            displayImage(image);
            highscoreScreen();
          }
          else
          {
            if ( currentMenu == 7)
            {
              
              gameMenu();
            }
          
          }
        }
      }
    }
  }
}
    
