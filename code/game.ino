#include "LedControl.h"
#include <EEPROM.h>
#include <LiquidCrystal.h>

#define LCD_BRIGHTNESS_ADDRESS 0
#define LCD_CONTRAST_ADDRESS 2
#define MATRIX_BRIGHTNESS_ADDRESS 4
#define HIGHSCORE_FIRST_ADDRESS 20
#define HIGHSCORES_NUMBER 6
#define VOLUME_ADDRESS 6
#define ADJUSTMENT_MAX_VALUE 10
#define ADJUSTMENT_MIN_VALUE 1
#define LONG_TEXT_LIMIT 24
#define ONE_SECOND 1000
#define ONE_AND_HALF_SECOND 1500
#define RESET_OFFSET 200
#define DEBOUNCE_TIME 100
#define UP_ARROW_ADDRESS 200
#define DOWN_ARROW_ADDRESS 208
#define FULL_PANEL_ADDRESS 216
#define DOT_ADDRESS 224
#define CHECK_MARK_ADDRESS 100
#define WRONG_MARK_ADDRESS 108
#define SMILEY_FACE_IMAGE_ADDRESS 116
#define SAD_FACE_IMAGE_ADDRESS 124
#define HAMMER_IMAGE_ADDRESS 132
#define TROPHY_IMAGE_ADDRESS 140
#define HOURGLASS_IMAGE_ADDRESS 148
#define COUNTDOWN_IMAGES_ADDRESS 156
#define MENU_IMAGE_ADDRESS 180
#define BOOK_IMAGE_ADDRESS 188

#define MAX_SEQUENCE_NUMBER 15
#define PLAYER_NAME_LIMIT 5

#define JOYSTICK_MIN_TRESHOLD 300
#define JOYSTICK_MAX_TRESHOLD 800




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
unsigned long timerWelcomeMenu = -1;
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
bool joyMovedOnY;
bool joyMovedOnX;
int menuPosition=0;
int settingsMenuPosition=0;
int barLevelMenuPosition=0;
int currentMenu=1;
int valueToAdjust=0;
char adjustmentName[2 * ADJUSTMENT_MAX_VALUE];
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
int gameLost;
int gameLimit;
unsigned long gameIntroTimer;
int gamePlayerName[PLAYER_NAME_LIMIT] = {65, 65, 65, 65, 65};

int lastSecond;
int pianoPosition = 0;
bool matrix[8][8];

const int menuOptionsNumber = 8;
const int settingsMenuOptionsNumber = 4;
const int maxNumbersOfChars = 20;

unsigned long timerButtonPress;
int buttonState;
int lastButtonState;

// struct to store matrix positions
struct point
{
  int x,y;
}sequence[MAX_SEQUENCE_NUMBER], gamePlayer;

LedControl lc = LedControl(dinPin, clockPin, loadPin,1);
LiquidCrystal lcd(rs,en,d4,d5,d6,d7);



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

  EEPROM.get(DOWN_ARROW_ADDRESS ,specialChar);
  lcd.createChar(0, specialChar);

  EEPROM.get(UP_ARROW_ADDRESS  ,specialChar);
  lcd.createChar(1, specialChar);

  EEPROM.get(FULL_PANEL_ADDRESS  ,specialChar);
  lcd.createChar(2, specialChar);

  EEPROM.get(DOT_ADDRESS  ,specialChar);
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



// EEPROM helper functions
void getSettingsFromEEPROM()
{
   EEPROM.get(0, menuSettingsOptions[0].value);
  EEPROM.get(2, menuSettingsOptions[1].value);
  EEPROM.get(4, menuSettingsOptions[2].value);
  EEPROM.get(6, menuSettingsOptions[3].value);

}
void getHighscoresFromEEPROM()
{
  for (int i = 0, j =20 ;i<6;i++,j+=7)
  {
    EEPROM.get(j,highscores[i].name);
    EEPROM.get(j+5,highscores[i].score);
  }
}
void saveAdjustmentsToEEPROM()
{
  EEPROM.put(0, menuSettingsOptions[0].value);
  EEPROM.put(2, menuSettingsOptions[1].value);
  EEPROM.put(4, menuSettingsOptions[2].value);
  EEPROM.put(6, menuSettingsOptions[3].value);
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




// buzzer helper functions
void buzz(int freq)
{
  if (menuSettingsOptions[3].value > 0)
  tone(buzzerPin, freq);
}
void stopBuzz()
{
  noTone(buzzerPin);
}

// joystick helper functions
int checkJoyMovementOnY()
{
  int dir=0;
  yValue = analogRead(pinY);
   if (yValue < JOYSTICK_MIN_TRESHOLD && joyMovedOnY == false ) 
  {
    dir= -1;
    joyMovedOnY = true;
  }

  if (yValue >= JOYSTICK_MIN_TRESHOLD && yValue <= JOYSTICK_MAX_TRESHOLD && joyMovedOnY == true)
  {
    joyMovedOnY = false;
  }

  if (yValue > JOYSTICK_MAX_TRESHOLD && joyMovedOnY == false ) 
  {
    
    joyMovedOnY = true;
    dir = 1;
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
  if (xValue >= JOYSTICK_MIN_TRESHOLD && xValue <= JOYSTICK_MAX_TRESHOLD && joyMovedOnX==true)
  {
    joyMovedOnX = false;
  }
   if (xValue < JOYSTICK_MIN_TRESHOLD && joyMovedOnX == false ) 
  {
    dir = -1;
    joyMovedOnX = true;
  }

  if (xValue > JOYSTICK_MAX_TRESHOLD && joyMovedOnX == false ) 
  {
    
    joyMovedOnX = true;
    dir = 1;
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


// display guiding symbols
void displayDownArrow()
{
  lcd.setCursor(15, 1);
  lcd.write(byte(0));
}

void displayUpArrow()
{
  lcd.setCursor(15, 0);
  lcd.write(byte(1));
}

void displayAdjustmentSigns()
{
  lcd.setCursor(2,1);
  lcd.print("-");
  lcd.setCursor(13,1);
  lcd.print("+");
}

void displayCurrentVerticalPosition()
{
  lcd.setCursor(0, currentVerticalPosition%2);
  lcd.write(byte(3));
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
void displayLevelBar(int value)
{
  int offset = 3;
  for (int i = offset;i < value + offset;i++)
  {
      lcd.setCursor(i,1);
      lcd.write(byte(2));
  }
  
}


void writeToScreen(char s1[], char s2[])
{
  lcd.setCursor(2,0);
  lcd.print(s1);
  lcd.setCursor(2,1);
  lcd.print(s2);
}


// used for about/how to play sections to display 24 chars at a time ( 12 on each line)
void displayLongText(char text[])
{
  
  int i,lastCharacterPosition, remainingCharacters, iterator, offset = 2,
  xMovement = checkJoyMovementOnX(), 
  yMovement = checkJoyMovementOnY();

  iterator = longTextPosition;
  remainingCharacters = strlen(text) - longTextPosition;
  
  displayCurrentVerticalPosition();
  if (longTextPosition < LONG_TEXT_LIMIT)
  {
    displayDownArrow();
  }
  if (longTextPosition >= LONG_TEXT_LIMIT && remainingCharacters > LONG_TEXT_LIMIT)
  {
    displayUpArrow();
    displayDownArrow();
  }
  if (remainingCharacters < LONG_TEXT_LIMIT)
  {
    displayUpArrow();
  }
  if (remainingCharacters > 0)
  {
    if ( remainingCharacters < LONG_TEXT_LIMIT / 2 + offset)
      lastCharacterPosition = remainingCharacters;
    else
      lastCharacterPosition = LONG_TEXT_LIMIT / 2 + offset;
    for (i = offset; i < lastCharacterPosition ; i ++)
    {
      lcd.setCursor(i, 0);
      iterator++;
      lcd.print(text[iterator]);
    }
  }
  remainingCharacters -= LONG_TEXT_LIMIT / 2;
  if (remainingCharacters > 0)
  {
    if ( remainingCharacters < LONG_TEXT_LIMIT / 2 + offset)
      lastCharacterPosition = strlen(text);
    else
      lastCharacterPosition = LONG_TEXT_LIMIT / 2 + offset;
    for (i = offset; i < lastCharacterPosition ; i ++)
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
  if (strlen(text) > LONG_TEXT_LIMIT * 5)
    limit = LONG_TEXT_LIMIT;
  else
  {
    limit = LONG_TEXT_LIMIT / 6;
  }
  if (yMovement == -1 && currentVerticalPosition < limit)
  {
    if (currentVerticalPosition % 2 == 1)
       longTextPosition += LONG_TEXT_LIMIT + 1;
    currentVerticalPosition ++;
    lcd.clear();
   
  }
 
  if (yMovement == 1 && currentVerticalPosition > 0)
  
      {
        if (currentVerticalPosition % 2 == 0)
        {
          longTextPosition -= LONG_TEXT_LIMIT + 1;

        }
          
       currentVerticalPosition --;
        lcd.clear();
      }   
 
  
}


// Main menu function

void mainMenu()
{
  // check for joystick movement
  xMovement = checkJoyMovementOnX();
  yMovement = checkJoyMovementOnY();

  // store current position in case the user moves right/left so when he can return to the chosen menu option
  // ( if he enters settings, when he comes back he will be positioned on settings instead of first menu option)
  menuPosition = currentVerticalPosition;
  
  // if user is on line 0 display current and next option, if on line 1 display current position and last position
  if (currentVerticalPosition % 2 == 0)
    writeToScreen(menuOptions[currentVerticalPosition],menuOptions[currentVerticalPosition+1]);
  else
    writeToScreen(menuOptions[currentVerticalPosition-1],menuOptions[currentVerticalPosition]);

  // show user menu position
  displayCurrentVerticalPosition();

  // position on Start Game option
  if ( currentVerticalPosition == 1)
  {
    // if joy moves right, prepare for next menu
    if (displayRightMenu(xMovement))
    {
      lcd.clear();
      currentVerticalPosition = 0;
      currentHorizontalPosition = 1;
      
      timerGameLevel = millis();
      currentMenu = 7;
      gameMenuPosition = 1;
    }
  }

  // position on Settings option
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
  // position Highscore option
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

  // position on About/How to play
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

  // position on Reset Highscores
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
  
  // if joy moves down and there are available options, go down
  if(yMovement == -1 && currentVerticalPosition < menuOptionsNumber - 1 )
  {
    currentVerticalPosition++;
    lcd.clear();
  }
  
  // if joy moves up and there are available options, go up
  if(yMovement == 1 && currentVerticalPosition > 1)
  {
        currentVerticalPosition--;
      lcd.clear();
  }
      
}

// settings Menu function
void settingsMenu()
{
  // store current position in case the user moves right/left so when he can return to the chosen menu option
  // ( if he enters settings, when he comes back he will be positioned on settings instead of first menu option)
  if (currentVerticalPosition % 2 == 0)
    writeToScreen(menuSettingsOptions[currentVerticalPosition].name,menuSettingsOptions[currentVerticalPosition+1].name);
  else
    writeToScreen(menuSettingsOptions[currentVerticalPosition-1].name,menuSettingsOptions[currentVerticalPosition].name);
  
  displayCurrentVerticalPosition();

  // if you can move down, display down arrow
  if (currentVerticalPosition < 2)
  {
    displayDownArrow();
  }
  // if you can move down, display up arrow
  if (currentVerticalPosition > 1 )
  {
    displayUpArrow();
  }

  // check joy movement on any axis
  xMovement = checkJoyMovementOnX();
  yMovement = checkJoyMovementOnY();

  // if joy moves down and there are available options, go down
  if(yMovement == -1 && currentVerticalPosition < settingsMenuOptionsNumber - 1)
  {
    currentVerticalPosition++;
    lcd.clear();
  }

  // if joy moves down and there are available options, go down
  if(yMovement == 1 && currentVerticalPosition > 0)
  {
      currentVerticalPosition--;
      lcd.clear();
  }
      
  // back to main menu and saved any changed values to EEPROM
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
  
  // display adjustmenu menu for current position ( general function that changes based on position) 
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

// general function that display setting name and a progress bar that edits the value of the setting
//        receives: name ( array of chars) and pos (int)
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
    displayDownArrow();
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
    displayUpArrow();
    displayAdjustmentSigns();
    
    // if position on LCD contrast, brightness or matrix brightness
    if ( pos < 3)
    {
      // if on LCD contrast or brightnes, write to pins
      if ( pos < 2)
        analogWrite(menuSettingsOptions[pos].pin,menuSettingsOptions[pos].value*menuSettingsOptions[pos].increment);
      
      // if on matrix brightness, set matrix intensity
      if (pos == 2)
        lc.setIntensity(0,menuSettingsOptions[pos].value + menuSettingsOptions[pos].increment);
      
      // if joy moves left, decrease the value if bigger than minimum
      if (xMovement == -1 &&  menuSettingsOptions[pos].value > ADJUSTMENT_MIN_VALUE )
      {
        menuSettingsOptions[pos].value --;
        lcd.clear();
      }

      // if joy moves left, increase the value if lower than maximum
      if (xMovement == 1 && menuSettingsOptions[pos].value < ADJUSTMENT_MAX_VALUE)
      {
        menuSettingsOptions[pos].value ++;
        lcd.clear();
      }
    }

    // if position on volume
    if ( pos == 3)
    {
     
      // if joy moves left, decrease the value if bigger than minimum
      if (xMovement == 1 &&  menuSettingsOptions[pos].value < ADJUSTMENT_MAX_VALUE)
      {
        menuSettingsOptions[pos].value = ADJUSTMENT_MAX_VALUE;
        lcd.clear();
      }

      // if joy moves left, increase the value if lower than maximum
      if (xMovement == -1 && menuSettingsOptions[pos].value > ADJUSTMENT_MIN_VALUE)
      {
        menuSettingsOptions[pos].value = ADJUSTMENT_MIN_VALUE - 1;
        lcd.clear();
      }
    }
  }
  
  
}

// Highscore screen from main menu

void displayHighscore( int position, int line)
{
  char name[5];
  strcpy(name,highscores[position].name);
  lcd.setCursor(3, line);
  lcd.print(char('1'+position));

  for (int i=0;i<strlen(name);i++)
  {
    lcd.setCursor(i+5,line);
    lcd.print(name[i]);
  }
  lcd.setCursor(10,line);
  lcd.print(":");
  lcd.setCursor(11,line);
  lcd.print(highscores[position].score);
}

void highscoreScreen()
{
  int xMovement, yMovement;
  xMovement = checkJoyMovementOnX();
  yMovement = checkJoyMovementOnY();

  if( currentVerticalPosition % 2 == 0)
  {
    displayHighscore(currentVerticalPosition, 0);
    displayHighscore(currentVerticalPosition + 1, 1);
  }
  else
  {
    displayHighscore(currentVerticalPosition - 1, 0);
    displayHighscore(currentVerticalPosition, 1);
    
  }
  displayCurrentVerticalPosition();
  
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

// Highscore helper functions
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


void resetHighScores()
{
  char null[5] = "     ";
  int zero = 0;
  for (int i = 0, j = HIGHSCORE_FIRST_ADDRESS ;i < HIGHSCORES_NUMBER;i++,j+=7)
  {
    EEPROM.put(j,null);
    EEPROM.put(j+5,zero);
  }
  
}


// Awesome Game

// Game helper functions

// Turn off all leds from matrix
void turnOnEntireMatrix()
{
  for (int row = 0; row < matrixSize; row++) 
  {
    for (int col = 0; col < matrixSize; col++) 
    {
     lc.setLed(0, row, col, true);
    }
  }
}

// Turn on all leds from matrix
void turnOffEntireMatrix()
{
  for (int row = 0; row < matrixSize; row++) 
  {
    for (int col = 0; col < matrixSize; col++) 
    {
      lc.setLed(0, row, col, false);
    }
  }
}

// display user position on X axis
void displayCurrentHorizontalPosition()
{
  // pos 1 means Back button 
  // sign 1 means up arrow
  // logic is initially assume user is on back button
  // then verify if user actually is an option so i have fewer verifications
  int pos = 1, sign = 1, offset = 3;

  // if on level or difficulty screen
  if ( gameMenuPosition == 1 || gameMenuPosition == 2)
  {
    if (currentHorizontalPosition > 0)
      {
        pos = offset + (offset * currentHorizontalPosition);
        sign = 0;
      }
  }

  // if on end-game screen, name introduction screen
  if (gameMenuPosition == 5)
  {
    int saveOptionPosition = 12;

    if (currentHorizontalPosition < 5)
      {
        pos = offset + currentHorizontalPosition;
        sign = 0;
      }
    else
    {
      pos = saveOptionPosition;
    }
  }
  
  lcd.setCursor(pos , 0);
  lcd.write(byte(sign));
}


//display level choosing screen
void displayGameLevel()
{
  int xMovement = checkJoyMovementOnX();
  int yMovement = checkJoyMovementOnY();
  unsigned long currentTime = millis() - timerGameLevel;

  // 1.5 second prompt screen
  if ( currentTime < ONE_AND_HALF_SECOND)
  {
      lcd.setCursor(2, 0);
      lcd.print("Choose Game");
      lcd.setCursor(5, 1);
      lcd.print("Level");
  }

  // clear for smooth transition
  if ( currentTime > ONE_AND_HALF_SECOND && currentTime < ONE_AND_HALF_SECOND + RESET_OFFSET)
  {
    lcd.clear();
  }

  // actual choosing level screen
  if ( currentTime > ONE_AND_HALF_SECOND + RESET_OFFSET)
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

    // if on the BACK option check for joy up movement and exit
    if (currentHorizontalPosition == 0 && yMovement == 1)
    {
      lcd.clear();
      currentMenu = 1;
      currentVerticalPosition = 1;
    }

    // if joy moves right and not at the end, move the indicator right
    if ( xMovement == 1 && currentHorizontalPosition < 3)
    {
      currentHorizontalPosition++;
      lcd.clear();
    }

    // if joy moves left and not at the end, move the indicator left
    if ( xMovement == -1 && currentHorizontalPosition > 0 )
    {
      {
        currentHorizontalPosition--;
        lcd.clear();
      }
    }

    // if joy moves down while on a game level option,
    // set up variables for chosen level
    // and move to difficulty screen
    if (yMovement == -1 && currentHorizontalPosition > 0)
    {
      gameLevel = currentHorizontalPosition;

      // at level 1 there will be 2x2 squares (so 3 each line/ column)
      if (gameLevel == 1)
      {
        gameMatrixLimit = 6;
        gameMatrixIncrement = 3;
      }
      else
      // at level 2 or 3 there will dots ( 8 the matrix size )
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

// similar logic to game level
void displayGameDifficulty()
{
  int xMovement = checkJoyMovementOnX();
  int yMovement = checkJoyMovementOnY();

  if ( millis() - timerGameDifficulty < ONE_AND_HALF_SECOND)
  {
      lcd.setCursor(2, 0);
      lcd.print("  Set Game");
      lcd.setCursor(3, 1);
      lcd.print("Difficulty");
  }

  if ( millis() - timerGameDifficulty > ONE_AND_HALF_SECOND && millis() - timerGameDifficulty < ONE_AND_HALF_SECOND + RESET_OFFSET)
  {
    lcd.clear();
  }

  if ( millis() - timerGameDifficulty > ONE_AND_HALF_SECOND + RESET_OFFSET)
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
    
  // once user chooses a difficulty, there will be different variables values
  // difficulty 1: 
  //        - each sequence complete will add 1 score point multiplied by level and 5 seconds
  //        - sequence time between dots/squares will be 2 seconds
  //        - but only a maximum sequence of 9 will be available
  //        - the sequence will be displayed as a whole until last dot/square is shown
  // difficulty 2: 
  //        - each sequence complete will add 3 score point multiplied by level and 4 seconds
  //        - sequence time between dots/squares will be 1.5 seconds
  //        - but only a maximum sequence of 12 will be available
  //        - the sequence will only show a dot at a time
  //        ( for example: dot1 shown for x time, turned off, then dot2 shown for x time etc.)
  // difficulty 3: 
  //        - each sequence complete will add 5 score point multiplied by level and 5 seconds
  //        - sequence time between dots/squares will be 2 seconds
  //        - a sequence of 15 will be available
  //        - now the sequence will only show the new dot position added
  //       ( so for a sequence of dot1,dot2, the player will only see dot3, not the previous ones)
    if (yMovement == -1 && currentHorizontalPosition > 0)
    {
      gameDifficulty = currentHorizontalPosition;
      if (gameDifficulty == 1)
      {
        gameScoreIncrement = 1;
        gameTimeIncrement = 5;
        gameSequenceShowInterval = 2 * ONE_SECOND;
        gameLimit = 9;
      }
      if (gameDifficulty == 2)
      {
        gameScoreIncrement = 3;
        gameTimeIncrement = 4;
        gameSequenceShowInterval = ONE_AND_HALF_SECOND;
        gameLimit = 12;
      }
      if (gameDifficulty == 3)
      {
        gameScoreIncrement = 5;
        gameTimeIncrement = 3;
        gameSequenceShowInterval = ONE_SECOND;
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

// Game helper functions

void displayGameScore()
{
  lcd.setCursor(0, 0);
  lcd.print("Score: ");
  lcd.setCursor(6,0);
  lcd.print(gameScore);
  
}
void displayGameTimeRemaining(int time)
{
  lcd.setCursor(0, 1);
  lcd.print("Time:");
  lcd.setCursor(6,1);
  lcd.print(time); 
}
void displayInGameDifficulty()
{
  lcd.setCursor(10, 0);
  lcd.print("Dif:");
  lcd.setCursor(15,0);
  lcd.print(gameDifficulty); 
}

void displayWelcomeMenu()
{
  lcd.setCursor(2, 0);
  lcd.print("Welcome to");
  lcd.setCursor(4,1);
  lcd.print("EI GAME");
}

void displayInGameLevel()
{
  lcd.setCursor(10, 1);
  lcd.print("Lvl:");
  lcd.setCursor(15,1);
  lcd.print(gameLevel); 
}

void displayCountdown(byte image[][8],int pos)
{
  displayImage(image[pos]);
  lcd.setCursor(0,0);
  lcd.print(" Game starts in");
  lcd.setCursor(8,1);
  lcd.print(char('3'-pos));
}

void turnOnPixel(int posX, int posY) 
{
  lc.setLed(0, posX, posY, true);
}

void turnOffPixel(int posX, int posY)
{
  lc.setLed(0, posX, posY, false);
}

// a square means current position, the one underneath, the left one
// and the one underneath the left one
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

void displayWonGameMessage()
{
  lcd.setCursor(0, 0);
  lcd.print("Spot on memory");
  lcd.setCursor(0, 1);
  lcd.print("Level won");
}

void displayLostGameWrongChoiceMessage()
{
  lcd.setCursor(0, 0);
  lcd.print("Wrong dot chosen");
  lcd.setCursor(0, 1);
  lcd.print(" Game over");
}

void displayLostGameTimeMessage()
{
  lcd.setCursor(0, 0);
  lcd.print(" Time expired");
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

// Game menu

void gameMenu()
{
  // game level choosing menu
  if (gameMenuPosition == 1)
  {
    displayGameLevel();
  }

  // game difficulty choosing menu
  if( gameMenuPosition == 2)
  {
    displayGameDifficulty();
  }

  // actual game
  if (gameMenuPosition == 3)
  {
    unsigned long currentTime = millis() - gameIntroTimer;
    byte countDown[3][8];
    EEPROM.get(COUNTDOWN_IMAGES_ADDRESS,countDown);

    if ( currentTime < ONE_SECOND)
    {
      displayCountdown(countDown,0);
      buzz(1000);
    }
    if ( currentTime > ONE_SECOND &&  currentTime < ONE_SECOND + RESET_OFFSET)
    {
      lcd.clear();
      stopBuzz();
      turnOffEntireMatrix();
    }
    if ( currentTime > ONE_SECOND + RESET_OFFSET &&  millis() - currentTime < 2 * ONE_SECOND + RESET_OFFSET)
    {
      buzz(1000);
      displayCountdown(countDown,1);
    }
    if ( millis() - gameIntroTimer > 2 * ONE_SECOND + RESET_OFFSET &&  millis() - gameIntroTimer < 2 * (ONE_SECOND + RESET_OFFSET))
    {
      lcd.clear();
      stopBuzz();
      turnOffEntireMatrix();
    }
    if ( millis() - gameIntroTimer >  2 * (ONE_SECOND + RESET_OFFSET) && millis() - gameIntroTimer < 3 * ONE_SECOND + 2* RESET_OFFSET)
    {
      buzz(1000);
      displayCountdown(countDown,2);
    }
    if ( millis() - gameIntroTimer > 3 * ONE_SECOND + 2* RESET_OFFSET && millis() - gameIntroTimer < 3 * (ONE_SECOND + RESET_OFFSET))
    {
      turnOffEntireMatrix();
      stopBuzz();
      lcd.clear();
      timerGame = millis();
    }
    if ( millis() - gameIntroTimer > 3 * (ONE_SECOND + RESET_OFFSET) + RESET_OFFSET)
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

// Game logic functions

// increase the number of dots/squares to remember
// increase score based of level, difficulty and time remaining
// choose a new random dot/square randomly, making sure it is different from the rest
void increaseSequence()
{
  lcd.clear();
  gameScore += gameScoreIncrement + (gameSequenceLength*gameLevel) + (gameTimeRemaining * gameLevel);
  timerGameSequence += gameTimeIncrement;
  displayGameScore();
  displayGameTimeRemaining(timerGameSequence);
  displayInGameLevel();  
  displayInGameDifficulty();
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

// display a checkmark if sequence is correct or a big X if wrong
void displayBetweenSequenceStatus(byte image[8], unsigned long timer)
{
  bool functionOver = false;
  while (functionOver == false)
  {
    if (millis() - timer < ONE_SECOND / 2)
    {
      turnOffEntireMatrix();
    }
    if ( millis() - timer > ONE_SECOND / 2 && millis() - timer < ONE_AND_HALF_SECOND)
    {
      displayImage(image);
    }
    if (millis() - timer > ONE_AND_HALF_SECOND && millis() - timer < ONE_AND_HALF_SECOND + RESET_OFFSET)
    {
      turnOffEntireMatrix();
      
    }
    if (millis() - timer > ONE_AND_HALF_SECOND + RESET_OFFSET)
    {
      functionOver = true;
      
    }
  }
}

// name introductions screen
void displayNameIntroduction()
{
  displayCurrentHorizontalPosition();
  lcd.setCursor(2, 1);
  lcd.print("<");
  lcd.setCursor(8, 1);
  lcd.print(">");
  lcd.setCursor(10, 1);
  lcd.print("save");

  // print the char from array of ints
  for ( int i = 0, j = 3; i < 5; i++, j ++)
  {
    lcd.setCursor(j, 1);
    lcd.print((char)(gamePlayerName[i]));
  }
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
  int currentSecond = (millis() - timerGame) / ONE_SECOND;
  if ( currentSecond < 2)
  {
     if (gameLost == 0)
    {
      byte image[8];
      EEPROM.get(SMILEY_FACE_IMAGE_ADDRESS,image);
      displayImage(image);
      displayWonGameMessage();
    }
    else
    {
      byte image[8];
      EEPROM.get(SAD_FACE_IMAGE_ADDRESS,image);
      if ( gameLost == 1)
      {
        displayLostGameTimeMessage();
      }
      if ( gameLost == 2)
      {
        displayLostGameWrongChoiceMessage();
      }
      displayImage(image);
      
    }
  }
 
  if (currentSecond > 2)
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
    EEPROM.get(TROPHY_IMAGE_ADDRESS,image);
    displayImage(image);

    if (millis() - timerGame < ONE_AND_HALF_SECOND)
    {
      if (gameScore > highscores[0].score)
        displayAllHighscoresBeatMessage();
      else
        displayHighscoreBeatMessage();
    }

    if (millis() - timerGame > ONE_AND_HALF_SECOND && millis() - timerGame < ONE_AND_HALF_SECOND + RESET_OFFSET)
    {
      lcd.clear();
    }

    if (millis() - timerGame > ONE_AND_HALF_SECOND + RESET_OFFSET && millis() - timerGame < 2 * (ONE_AND_HALF_SECOND + RESET_OFFSET))
    {
      displayRequestNameMessage();
    }

    if (millis() - timerGame > 2 * (ONE_AND_HALF_SECOND + RESET_OFFSET) && millis() - timerGame < 2 * ONE_AND_HALF_SECOND + 3 *RESET_OFFSET)
    {
      lcd.clear();
    }
    if (millis() - timerGame > 2 * ONE_AND_HALF_SECOND + 3 *RESET_OFFSET)
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

        if ( yMovement == 1 && currentHorizontalPosition == 5)
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

    for (int i=0;i<PLAYER_NAME_LIMIT;i++)
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
    unsigned long currentTime = millis() - timerGame;
    if (currentTime < ONE_SECOND)
    {
        displayNoHighscoreMessage();
    }

    if (currentTime > ONE_SECOND && currentTime < ONE_SECOND + RESET_OFFSET)
    {
      lcd.clear();
    }

    if (currentTime > ONE_SECOND + RESET_OFFSET && currentTime < 2 * ONE_SECOND + RESET_OFFSET)
    {
      displayEncouragingMessage();
    }

    if ( currentTime > 2 * ONE_SECOND + RESET_OFFSET)
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
  displayGameTimeRemaining(gameTimeRemaining);
  displayInGameLevel();
  displayInGameDifficulty();

  // sequence state
  if (gameState == 1)
  {
    
    int currentSecond = (millis() - timerGame) / gameSequenceShowInterval;
    
    // use the ,,current second" as iterator through sequence
    // it's not really whole second, depending on difficulty
    // so if the sequence length is not reached, keep displaying it
    if ( currentSecond < gameSequenceLength)
    {
      // every time currentSecond increases, increase the sequence position
      if (currentSecond > gameCurrentSeqPosition )
      {
        // game specific logic
        // if difficulty is 2, turn off the last pixel before lighting up the next one
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

      // game specific logic
      // if difficulty is 1 or 2 show the next sequence position
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

      // game specific logic
      // if difficulty is 3 only show the new dot
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

    // turn off matrix for smoothnesss
    if (currentSecond >= gameSequenceLength  && currentSecond <= gameSequenceLength < 1)
      turnOffEntireMatrix();
    
    // once sequence is shown, game state changes and playr may move and choose
    if (currentSecond >= gameSequenceLength + 1)
      {
        gamePlayerSequencePosition = 0;
        timerGame = millis();
        gamePlayerX = 0;
        gamePlayerY = 0;
        gameState = 2;
      }
  }

  // player state
  if ( gameState  == 2)
  {

    int xMovement = checkJoyMovementOnX();
    int yMovement = checkJoyMovementOnY();
    int initialX, initialY;
    int currentSecond = (millis() - timerGame) / ONE_SECOND;
    byte swReading = digitalRead(pinSW);

    // remember initial positions to turn them off at the end
    initialX = gamePlayerX;
    initialY = gamePlayerY;
    
    // refresh rate of a second
    if ( lastSecond < currentSecond)
    {
      lcd.clear();
    }

    lastSecond = currentSecond;
    gameTimeRemaining = timerGameSequence - currentSecond;

    // display current player position depending on level
    if (gameLevel == 1)
      turnOnSquare(gamePlayerX,gamePlayerY);

    if (gameLevel == 2)
      turnOnPixel(gamePlayerX,gamePlayerY);
    

    // on level 3, show the initial position if at the start of
    if ( gameLevel == 3 && gamePlayerSequencePosition == 0)
    {
      turnOnPixel(0,0);
    }

    // joystick movement
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
      EEPROM.get(HOURGLASS_IMAGE_ADDRESS,image);
      buzz(500);
      displayBetweenSequenceStatus(image,millis());
      stopBuzz();
      gameLost = 2;
      endGame();
    }

    // check for button press
    if (swReading != lastButtonState)
    {
      timerButtonPress = millis();
    }
    
    // check for debounce
    if ((millis() - timerButtonPress) > DEBOUNCE_TIME) 
    {
      if (swReading != buttonState) 
      {
        buttonState = swReading;

        if (buttonState == HIGH) 
        {
          // if button pressed check if the current choice is correct
          // if it is continue until the end of sequence
          if ( gamePlayerX == sequence[gamePlayerSequencePosition].x && gamePlayerY == sequence[gamePlayerSequencePosition].y)
          {
            turnOffEntireMatrix();

            // game logic
            // on level 3 display the correct position
            if (gameLevel ==  3)
            {
                turnOnPixel(gamePlayerX,gamePlayerY);
            }
            gamePlayerSequencePosition ++;
          }
          // otherwise end the game with wrong choice message
          else
          {
            byte image[8];
            EEPROM.get(WRONG_MARK_ADDRESS,image);
            buzz(500);
            displayBetweenSequenceStatus(image,millis());
            stopBuzz();
            gameLost = 1;
            endGame();
          }

          // if current choice position is the same as sequence length
          if (gamePlayerSequencePosition >= gameSequenceLength)
          {
            byte image[8];
            // if sequence didn't reach the level limit, increase it
            if ( gameSequenceLength < gameLimit)
            {
              EEPROM.get(CHECK_MARK_ADDRESS,image);
              buzz(2000);
              displayBetweenSequenceStatus(image,millis());
              stopBuzz();
              increaseSequence();
              gameCurrentSeqPosition = 0;
              timerGame = millis();
              gameState = 1;
            }
            // if sequence is on level limit, game is won
            else
            {
              EEPROM.get(CHECK_MARK_ADDRESS,image);
              displayBetweenSequenceStatus(image,millis());
              gameLost = 0;
              endGame();
            }
            
          }
        }
      }
    
      
    }
    lastButtonState = swReading;

    // game logic
    // turn off the last position if level is lower than 3 (where you dont see movemnt)
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

  randomSeed(millis());

  // main menu
  if (currentMenu == 1)
  {
    // show intro message only the first time main menu is entered
    if (timerWelcomeMenu == -1)
    {
      timerWelcomeMenu = millis();
    }
    if ( millis() - timerWelcomeMenu < ONE_AND_HALF_SECOND )
    {
      displayWelcomeMenu();
    }
    if ( millis() - timerWelcomeMenu > ONE_AND_HALF_SECOND && millis() - timerWelcomeMenu < ONE_AND_HALF_SECOND + RESET_OFFSET)
    {
      lcd.clear();
    }
    if ( millis() - timerWelcomeMenu > ONE_AND_HALF_SECOND + RESET_OFFSET)
    {
      byte image[8];
      EEPROM.get(MENU_IMAGE_ADDRESS,image);
      displayImage(image);
      mainMenu();
    }
  }
  else
  {
    // settings menu
    if (currentMenu == 2)
    {
      byte image[8];
      EEPROM.get(HAMMER_IMAGE_ADDRESS,image);
      displayImage(image);
      settingsMenu();
    }
    else
    {
      // adjustment menu
      if (currentMenu == 3)
      {
        adjustmentMenu(adjustmentName,adjustmentElementPos);
      }
      else
      {
        // how to play / about menu
        if (currentMenu == 4)
        {
          byte image[8];
          EEPROM.get(BOOK_IMAGE_ADDRESS,image);
          displayImage(image);

          if( menuPosition == 4)
            displayLongText(howToPlay);
          else
            displayLongText(about);
        }
        else
        {
          // highscore menu
          if (currentMenu == 6)
          {
            byte image[8];
            EEPROM.get(TROPHY_IMAGE_ADDRESS,image);
            displayImage(image);
            highscoreScreen();
          }
          else
          {
            // game menu
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
