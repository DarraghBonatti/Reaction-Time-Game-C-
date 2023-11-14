// Author: Darragh Bonatti Hoare
// Student Number: 121454992

const int START_BUTTON = 5;
const int BUTTON1 =3;
const int BUTTON2 =2;
const int BUZZER =9;
const int PLAYER1GREEN = 13;
const int PLAYER1RED = 12;
const int PLAYER2RED = 11;
const int PLAYER2GREEN = 10;
int Round = 1;
int Player1Score = 0;
int Player2Score = 0;
int Player1WinningRounds[3];
int Player2WinningRounds[3];

unsigned long timer;
//byte stateBeforeBuzzer;
volatile bool allGood = true;
unsigned long Player1Times[3];
unsigned long Player1Differences[3];
unsigned long Player2Times[3];
unsigned long Player2Differences[3];
int IndexForPlayers = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {

  pinMode(BUZZER, OUTPUT); // Set buzzer - pin 9 as an output
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(START_BUTTON, INPUT);
  pinMode(PLAYER1GREEN, OUTPUT);
  pinMode(PLAYER1RED, OUTPUT);
  pinMode(PLAYER2GREEN, OUTPUT);
  pinMode(PLAYER2RED, OUTPUT);

  Serial.begin(9600); // Initialize the serial communication
  
  
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int findChange(){


  byte previousPortDState = PIND;
  byte currentPortDState;

  // Wait for PORTD to change
  while ((previousPortDState & B00001100) == (currentPortDState = PIND & B00001100)) {
    // Do nothing; keep waiting for a change in pins 2 and 3
    // im doing this as pin4 for some reason on my microcontroller is changing so when i dircetly compare all 8 bits of previous and currrent it triggers a change but it wasnt from a player,so i had to
    // loop around while only copmaring bits 3 and 2 i.e pin 3 and pin 2
  }

  // Identify and print the changed bit
  byte changedBit = previousPortDState ^ currentPortDState;

  for (int i = 3; i >= 2; i--) {
    if (bitRead(changedBit, i) == 1) {
      if(i == BUTTON1){
        digitalWrite(PLAYER1GREEN, HIGH);
        digitalWrite(PLAYER2RED, HIGH);
        Player1Score ++;
        Player1WinningRounds[IndexForPlayers] = 1;
        unsigned long Player1Pressed = millis();
        //unsigned long reactionTimePlayer1 = Player1Pressed - timer;
        unsigned long reactionTimePlayer1 = Player1Pressed - timer;
        Player1Times[IndexForPlayers]=reactionTimePlayer1;
        while ((PIND & B00000100) != B00000100) {
          // Do nothing; wait for Player 1's button press
        }
        unsigned long Player2PressedAfterPlayer1 = millis();
        
        unsigned long DifferenceFromP1andP2 = Player2PressedAfterPlayer1 - Player1Pressed;
        Player1Differences[IndexForPlayers] = DifferenceFromP1andP2;
        Player2Times[IndexForPlayers] = Player2PressedAfterPlayer1 - timer;
        Player2Differences[IndexForPlayers]=DifferenceFromP1andP2;
        Serial.println("Player 1 WINS");
        
       

       
  }
      if(i == BUTTON2){
        digitalWrite(PLAYER2GREEN, HIGH);
        digitalWrite(PLAYER1RED, HIGH);
        Player2Score ++;
        Player2WinningRounds[IndexForPlayers] = 1;
        unsigned long Player2Pressed = millis();
        unsigned long reactionTimePlayer2 =Player2Pressed - timer;
        Player2Times[IndexForPlayers] = reactionTimePlayer2;
        while ((PIND & B00001000) != B00001000) {
          // Do nothing; wait for Player 1's button press
        }
        unsigned long Player1PressedAfterPlayer2 = millis();
        unsigned long DifferenceFromP2andP1 = Player1PressedAfterPlayer2 - Player2Pressed;
        Player2Differences[IndexForPlayers]=DifferenceFromP2andP1;
        Player1Times[IndexForPlayers] = Player1PressedAfterPlayer2 - timer;
        Player1Differences[IndexForPlayers]=DifferenceFromP2andP1;
        Serial.println("Player 2 WINS");
       
        
     }
    }
  }
  delay(2000);
  IndexForPlayers++;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// this is a function to find a false go which is when a player presses a button before the buzzer sounds in each round

int findFalseGo(byte stateBeforeBuzzer){
  byte currentPortDState = PIND;


// we XOR the state of port D before the buzzer was called contiunoulsy with an updated state of portd. each time checking the bits from pin 3 and 2 as that is where our buttons are

  byte relevantBitsChanged = stateBeforeBuzzer ^ currentPortDState;
  byte pin3Changed = bitRead(relevantBitsChanged, 3);
  byte pin2Changed = bitRead(relevantBitsChanged, 2);

  if (pin3Changed == 1) {
    Serial.println("Pin 3 (BUTTON1) Went early!");
    allGood = false;
    Player2Score++;
    IndexForPlayers++;
    delay(2000);
    
  }

  if (pin2Changed == 1) {
    Serial.println("Pin 2 (BUTTON2) Went early!");
    allGood = false;
    Player1Score++;
    IndexForPlayers++;
    delay(2000);
    
  }

}


// it prints 9 when player1 cheats and 5 when player 2 cheats, it always prints one as changing!


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// this function is in charge of all the rounds of the game, turning leds off, chossing a random time and turning on and off the buzzer as well as printing start round to screen
// it also checks for a false button press befroe teh buzzer goes off, within the time frame of the random time value to start the buzzer
// so if our random value is 10 sedonds, for the 10 seconds before the buzzer goes off, we are checking the pins of the two buttons for a change. if so, that player looses round, and round is skipped.
void startRound(int Round){
    digitalWrite(PLAYER1GREEN, LOW);
    digitalWrite(PLAYER1RED, LOW);
    digitalWrite(PLAYER2GREEN, LOW);
    digitalWrite(PLAYER2RED, LOW);

    
        byte stateBeforeBuzzer = PIND;
        allGood = true;
        if(Round<=3){
       
        
        unsigned long randomValue = random(1000, 10000);
        Serial.print("Starting Round: ");
        Serial.println(Round);
        unsigned long startRoundTime = millis();
        

        while(millis() - startRoundTime <= randomValue){
          findFalseGo(stateBeforeBuzzer);
          if (allGood == false){
            break;
          }
         
          
        }
        if(allGood==true){
          tone(BUZZER, 2000);
          timer = millis();
          delay(200);
          noTone(BUZZER);
          int changedPin = findChange();
          
        
        }
        Round++;
        
        }

    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// this function just prints the results neatly and turns off the leds after each game


void dealWithEnding(){
  digitalWrite(PLAYER1GREEN,LOW);
  digitalWrite(PLAYER2GREEN,LOW);
  digitalWrite(PLAYER1RED,LOW);
  digitalWrite(PLAYER2RED,LOW);


  if(Player1Score>Player2Score){
    Serial.print("Player 1 Wins Overall with ");
    Serial.print(Player1Score);
    Serial.println(" Wins!");
    int z = 0;
    
    for (int i = 0; i < 3; i++) {
      Serial.println(" ");
      Serial.print("Game ");
      Serial.print(i+1);
      Serial.print(": Time: ");
      
      Serial.print(Player1Times[i]);
      Serial.print(", Difference: ");
      if(Player1WinningRounds[z]==1){
        Serial.print("-");
      }
      Serial.print(Player1Differences[i]);
      if(Player1WinningRounds[z]==1){
        Serial.print(" (WIN)");
      }
      else{
         Serial.print(" (LOSS)");
      }
      z ++;
     
}
  }
  if(Player1Score<Player2Score){
    Serial.print("Player 2 Wins Overall with ");
    Serial.print(Player2Score);
    Serial.println(" Wins!");
    int j = 0;
    for (int i = 0; i < 3; i++) {
      Serial.println(" ");
      Serial.print("Game ");
      Serial.print(i+1);
      Serial.print(": Time: ");
      
      Serial.print(Player2Times[i]);
      Serial.print(", Difference: ");
      if(Player2WinningRounds[j]==1){
        Serial.print("-");
      }
      Serial.print(Player2Differences[i]);
      if(Player2WinningRounds[j]==1){
        Serial.print(" (WIN)");
      }
      else{
         Serial.print(" (LOSS)");
      }
      j ++;
    }
  }
}
    
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// main loop calling all respective functions
void loop() {
      if(digitalRead(START_BUTTON)==HIGH){
        while(Round<=3){
            startRound(Round);

            Round ++;
          }
        dealWithEnding();
      }
}