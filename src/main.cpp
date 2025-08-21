#include <Arduino.h>
#include <LiquidCrystal.h>

// creating a array of 52 unsigned 8-bit integers.
uint8_t deck[52]; 

int deckSize = 0;


int playerCards[12];
int dealerCards[12];

int playerCardCount = 0;
int dealerCardCount = 0;

int playerTotal = 0; 
int dealerTotal = 0;        
       

bool gameOver = false;

// Card rank and suit labels
const char *rankLabels[] = {"A","2","3","4","5","6","7","8","9","10","J","Q","K"};
const char *suitLabels[] = {"hearts","spades","clubs","diamonds"};

//Helper functions to get card rank and suit from ID
inline int id_rank(int id) { return id % 13; }   // 0..12
inline int id_suit(int id) { return id / 13; }   // 0..3

// LCD setup
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int hitButtonPin = 7;
const int standButtonPin = 8;


// Creates card suits 
byte heart[8] = {
  B00000,
  B01010,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000,
  B00000
};

byte diamond[8] = {
  B00000,
  B00100,
  B01110,
  B11111,
  B01110,
  B00100,
  B00000,
  B00000
};

byte spade[8] = {
  B00000,
  B00100,
  B01110,
  B11111,
  B10101,
  B01110,
  B11111,
  B00000
};

byte club[8] = {
  B00100,
  B01110,
  B11111,
  B01110,
  B10101,
  B01110,
  B11111,
  B00000
};



// Initializing the playing deck and shuffles it
void initializeDeck() {

  for (int i = 0; i < 52; i++) {
    deck[i] = i;
  }

  deckSize = 52;


  // Fisher-Yates shuffle
  for (int i = deckSize - 1; i > 0; --i) {
    int j = random(i + 1);
    uint8_t tmp = deck[i];
    deck[i] = deck[j];
    deck[j] = tmp;
  }

  
}



// Draws a card from the deck and removes it from the deck
// Returns -1 if no cards left
int drawCard() {

  if (deckSize <= 0) {
    return -1;;
  }

  deckSize--;
  int id = deck[deckSize];
  deck[deckSize] = 0xFF;


  return id;
}



// Obtains the cards value 
int getCardValue(int id) {

  if (id < 0 || id >= 52) return 0;
  int r = id_rank(id);
  if (r == 0) return 11;        // Ace
  if (r >= 10) return 10;       // J Q K
  return r + 1; 
}



// Returns the highest valid Blackjack hand value (Ace counts as 11 or 1).
int getBestHandValue(int cards[], int count) {
 
  int total = 0;
  int aceCount = 0;

  for (int i = 0; i < count; i++) {

    int val = getCardValue(cards[i]);
    
    if (val == 11) {
      aceCount++;
    }

    total += val;
  }

  // Adjust for Aces
  while (total > 21 && aceCount > 0) {

    total -= 10;
    aceCount--;
  }

  return total;
}



// Initilizes player's starting hand
void startingHand() {

  playerCardCount = 0;

  int c1 = drawCard();
  int c2 = drawCard();

  if (c1 >= 0) playerCards[playerCardCount++] = c1;
  if (c2 >= 0) playerCards[playerCardCount++] = c2;

  playerTotal = getBestHandValue(playerCards, playerCardCount);

  
}



// Initilizes dealer's starting hand
void dealerHand() {
  
  dealerCardCount = 0;

  int d1 = drawCard();
  int d2 = drawCard();

  if (d1 >= 0) dealerCards[dealerCardCount++] = d1;
  if (d2 >= 0) dealerCards[dealerCardCount++] = d2;

  dealerTotal = getBestHandValue(dealerCards, dealerCardCount);

  
}



// resets the game 
void resetGame() {

  deckSize = 52;

  playerTotal = 0;
  dealerTotal = 0;

  playerCardCount = 0;
  dealerCardCount = 0;

  gameOver = false;

  initializeDeck();
  startingHand();
  dealerHand();

 

  lcd.clear();
  lcd.print("New Round!");
  delay(1200);
  
  
}



// Checks for Blackjack immediately after the initial deal.
// - Calculates player and dealer totals to see if either has a natural 21.
// - Applies standard casino "peek" rule: if dealer shows Ace or 10, they check for Blackjack.
// - Displays the outcome (player win, dealer win, or push) on the LCD.
// - Ends the round early if a Blackjack is found, otherwise play continues.
void checkBlackJack(){
  
  playerTotal = getBestHandValue(playerCards, playerCardCount);
  dealerTotal = getBestHandValue(dealerCards, dealerCardCount);

  bool playerBJ = (playerTotal == 21 && playerCardCount == 2);
  bool dealerBJ = (dealerTotal == 21 && dealerCardCount == 2);

  int dealerUpcard = dealerCards[0];
  int dealerUpVal = getCardValue(dealerUpcard);

  if (playerBJ) {

    if (dealerUpVal == 11 || dealerUpVal == 10) {  

      // Dealer peeks
      if (dealerBJ) {
        lcd.clear();
        lcd.setCursor(0, 0); lcd.print("BLACKJACK!");
        lcd.setCursor(0, 1); lcd.print("It's a Push!");
      } 
      else {
        lcd.clear();
        lcd.setCursor(0, 0); lcd.print("BLACKJACK!");
        lcd.setCursor(0, 1); lcd.print("You Win!");
      }
    } 
    else {
      // Dealer doesn’t peek, player wins immediately
      lcd.clear();
      lcd.setCursor(0, 0); lcd.print("BLACKJACK!");
      lcd.setCursor(0, 1); lcd.print("You Win!");
    }

    delay(3000);
    gameOver = true;
    resetGame();
    
  }

  // If dealer has blackjack and player does not
  if (dealerBJ) {

    lcd.clear();
    lcd.setCursor(0, 0); lcd.print("BLACKJACK!");
    lcd.setCursor(0, 1); lcd.print("Dealer Wins!");
    delay(3000);
    gameOver = true;
    resetGame();
    
  }

}



// determines whether player total is a bust or not
void isBust(int total) {

  if (total > 21) {
    lcd.clear();
    lcd.print("You Busted!");
    delay(900);
    gameOver = true;
    resetGame();
    checkBlackJack();
    return; 
  }

}



// displays the cards onto the lcd
void displayCard(int id) {

  if (id < 0 || id >= 52) {
    lcd.print("??");
    return;
  }
  int r = id_rank(id);
  int s = id_suit(id);

  // print rank
  lcd.print(rankLabels[r]);

  if (s == 0) lcd.write(byte(0)); // hearts
  else if (s == 1) lcd.write(byte(2)); // spades -> glyph index 2
  else if (s == 2) lcd.write(byte(3)); // clubs  -> glyph 3
  else if (s == 3) lcd.write(byte(1)); // diamonds -> glyph 1
}



// displays the player's or dealer's hand on a single line
void displayHand(int cards[], int count, int row) {

  lcd.setCursor(0, row);
  for (int i = 0; i < count; i++) {
    displayCard(cards[i]);
    if (i < count - 1) lcd.print(" ");
  }
}



// determines if there is a player win, dealer win, bust, or push
void determineWinner() {
  
  playerTotal = getBestHandValue(playerCards, playerCardCount);
  dealerTotal = getBestHandValue(dealerCards, dealerCardCount);

  lcd.clear();
  
  if (playerTotal > 21) {
    lcd.print("You Busted!");
  } 
  else if (dealerTotal > 21) {
    lcd.print("Dealer Busts!");
  } 
  else if (playerTotal > dealerTotal) {
    lcd.print("You Win!");
  } 
  else if (playerTotal < dealerTotal) {
    lcd.print("Dealer Wins!");
  } 
  else {
    lcd.print("It's a Push!");
  }

  delay(3000);
  gameOver = true;
  resetGame();
}



// Once player stands, dealer will show their 2nd card and draw cards until they reach 17, above 17 or bust
void dealerTurn() {

  lcd.clear();
  lcd.print("Dealer Shows:");
  displayHand(dealerCards, dealerCardCount, 1);
  delay(2000);

  dealerTotal = getBestHandValue(dealerCards, dealerCardCount);

  while (dealerTotal < 17) {

    int newCard = drawCard();

    if (newCard >= 0 && dealerCardCount < 12) {
      dealerCards[dealerCardCount++] = newCard;
      dealerTotal = getBestHandValue(dealerCards, dealerCardCount);
    } 

    lcd.clear();
    lcd.print("Dealer draws:");
    displayHand(dealerCards, dealerCardCount, 1);
    delay(1300);


    if (dealerTotal > 21) {
      lcd.clear();
      lcd.print("Dealer Busts!");
      delay(1000);
      determineWinner(); // Let the winner be determined properly
      return;
    }
  }
  determineWinner();
}



// Handles player input for Hit or Stand. Determines whether hit or stand button is pressed.
// - Hit: draw a card, update player's hand, check for bust.
// - Stand: end player turn and let dealer play.
void hitStand() {

  if(gameOver){
    return;
  }

  while (true) {
    lcd.clear();
    lcd.print("Hit or Stand?");
    delay(250);

    int hitButtonState = digitalRead(hitButtonPin);
    int standButtonState = digitalRead(standButtonPin);

    if (hitButtonState == LOW) {

      int newCard = drawCard();

      if (newCard >= 0 && playerCardCount < 12) {
        playerCards[playerCardCount++] = newCard;
        playerTotal = getBestHandValue(playerCards, playerCardCount);
      } 
      

      lcd.clear();
      lcd.print("You chose: HIT");
      delay(1000);
      

      lcd.clear();
      lcd.print("Player card:");
      displayHand(playerCards, playerCardCount, 1);
      delay(2000);


      isBust(playerTotal);
    } 
    else if (standButtonState == LOW) {

      lcd.clear();
      lcd.print("You chose: STAND");
      delay(1500);

      lcd.clear();
      lcd.print("Dealer card:");
      displayHand(dealerCards, dealerCardCount, 1);
      delay(1200);

      dealerTurn();
      return;

    }
  }
}



// Starts new round by dealing cards to both player and dealer. 
//  1. Displays the player's hand and the dealer's first card.
//  2. Checks for immediate Blackjack after dealing.
//  3. If no Blackjack, allows player to hit or stand.
void playRound() {

  lcd.clear();
  lcd.print("Dealing cards...");
  delay(1000);


  // Show player's starting hand
  lcd.clear();
  lcd.print("Player:");
  displayHand(playerCards, playerCardCount, 1);
  delay(2000);

  // Show dealer's first card only
  lcd.clear();
  lcd.print("Dealer:");
  lcd.setCursor(0,1);
  displayCard(dealerCards[0]);
  lcd.print(" ?");
  delay(2000);

  // Check for blackjack immediately after dealing
  checkBlackJack();
  if (gameOver){
    return;
  }

  // let player play
  hitStand();

}



// Initalizes the LCD, buttons, and starts the game
void setup() {

  Serial.begin(9600);
  randomSeed(analogRead(0));

  pinMode(hitButtonPin, INPUT_PULLUP);
  pinMode(standButtonPin, INPUT_PULLUP);

  lcd.begin(16, 2);
  lcd.createChar(0, heart);
  lcd.createChar(1, diamond);
  lcd.createChar(2, spade);
  lcd.createChar(3, club);

  lcd.clear();
  lcd.print("Welcome to");
  lcd.setCursor(0, 1);
  lcd.print(" Blackjack ");
  delay(1500);
  
  resetGame();
  checkBlackJack();

  
}



// Calls the playRound function repeatedly to start the game
void loop() {

  playRound();

}
