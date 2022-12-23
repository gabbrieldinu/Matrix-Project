
# Game Description
  This may seem quite familiar. It is sequence-memory game, but with a twist. Or twists I may add. This version has uunique levels and difficulties, with an awesome menu. Well that seems basic, what can be so special about a few levels or difficulties based on a straight-forward game? Well I am gonna let you decide. Let's dive into the levels first:
  - Level One : The sequence will be 2x2 squares
  - Level Two : The sequence will be 1x1 dots
  - Level Three : The sequence will also be 1x1 dots, but this time the player may not see where he moves and each time he will guess a dot from the sequence that one will light up

The three difficulties:
  - Difficulty One: The sequence will light one at a time and will stay lit until the entire sequence is shown
  - Difficulty Two: The sequence will light one at a time, but each dot dissapears before another one is lit
  - Difficulty Three: The sequence will only light the new dot ( so if you have a sequence of 3 dots, it will only show the 4th and so on)

The main game inspiration is level 2, difficulty 2.

# Game development

But how was it possible to make such an interesting and complex game? Can it even be explained? Not sure, but I will try my best:

  - I started developing the initial game with the logic from level 2 and difficulty 2
    -- For that I needed to the game to have 2 separate states
    -- In the First State the sequence will be shown
      --- For that every second a dot will be lit ( and the previous stopped) until the end of sequence
    -- In the Second State the player may move using the joystick
      --- Lighting up the current dot position and after detecting joystick movement turning it off and moving the player position ( within matrix limits)
    -- After that was working, I needed to keep track of the score and also time remaining
    -- Now I was able to start implementing that end game scenarios:
       --- Choosing the wrong sequence and losing the game
       --- Time expiring and losing the game
       --- Getting the sequence right and increasing it until the limit
       --- Checking if a highscore was beat
       --- Checking if all highscores were beat
       --- Checking if no highscore was beat
       --- If any highscore was beat, I needed a name introduction screen, that would after exiting save the name and score of player
    -- After that was done it came the most important part: choosing the right images for EACH state, where I went a bit too wild. Some may call it crazy, I call it diverse and innovative. But hey geniuses are usually misunderstood.
    -- That was pretty much everything related to the game, oh wait. I forgot. After that I made sure I have a lot of fun the game. You should too!
    



# Game Instructions

-Game:
* joystick UP the player moves UP
* joystick DOWN the player moves DOWN
* joystick RIGHT the player moves RIGHT
* joystick LEFT the player moves LEFT
* the button locks the sequence chosen
-Menu:
* to scroll UP/DOWN move the joystick UP/DOWN
* to select an option move the joystick RIGHT
* to go back to the last option move the joystick LEFT
* at any time the possible actions will be present on the screen accordingly

#Hardware Components:
1x Breadboard medium
1x Breadboard small
1x Arduino UNO board
1x Passive Buzzer
1x LCD1602
1x 8x8 LED Matrix
1x Matrix Driver
1x Joystick
1x Button
1x 100k Ohm Resistor
1x 100 Ohm Resistor
42x Wires
![breadboard](https://user-images.githubusercontent.com/74376628/209407894-635662e0-650b-4092-a687-7c793e4ff40f.jpg)
Link to game menu demo:https://www.youtube.com/watch?v=vwJlXSEQM5E
Link to game demo:https://www.youtube.com/watch?v=Kpo195rfc8o
