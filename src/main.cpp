#include <Arduino.h>

#include <LiquidCrystal.h>

String deck[52];
int deckSize = 52;

String starting1, starting2;

String dealer1, dealer2;

String playerCards[12];
String dealerCards[12];

int playerCardCount;
int dealerCardCount;

int playerTotal = 0; 
int dealerTotal = 0;        
       

bool gameOver = false;

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


// Initializing the deck
void initializeDeck() {
  String suits[] = {"hearts", "spades", "clubs", "diamonds"};
  int index = 0;
  for (int i = 1; i < 14; i++) {
    for (int j = 0; j < 4; j++) {
      if (i == 1) {
        deck[index] = "A of " + suits[j];
      } else if (i == 11) {
        deck[index] = "J of " + suits[j];
      } else if (i == 12) {
        deck[index] = "Q of " + suits[j];
      } else if (i == 13) {
        deck[index] = "K of " + suits[j];
      } else {
        deck[index] = String(i) + " of " + suits[j];
      }
      index++;
    }
  }
}



// 1 deck game so draw cards until deck empty
String drawCard() {
  if (deckSize == 0) {
    return "No cards left!";
  }

  int randomIndex = random(deckSize);
  String card = deck[randomIndex];

  Serial.print("Drew card: ");
  Serial.println(card);

  if(card == ""){
    Serial.print("EMPTY: ");
  
  }

  for (int i = randomIndex; i < deckSize - 1; i++) {
    
    // testing for uninitialized slots
    if (deck[i] == "") {
      Serial.print("Uninitialized slot at: ");
      Serial.println(i);
    }
    
    deck[i] = deck[i + 1];
  }

  deck[deckSize - 1] = "";
  deckSize--;
  Serial.print("Deck size now: ");
  Serial.println(deckSize);
  return card;
}



// obtaining the cards value 
int getCardValue(String card) {
  
  card.toLowerCase();
  int spaceIndex = card.indexOf(" ");
  String rank = card.substring(0, spaceIndex);

  Serial.print("Parsing card: ");
  Serial.println(card);

  if (rank == "a") {
    return 11;
  } else if (rank == "k" || rank == "q" || rank == "j") {
    return 10;
  } else {
    return rank.toInt();
  }
}



// Helper to get best hand value (Ace as 11 or 1)
int getBestHandValue(String cards[], int count) {
 
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

  starting1 = drawCard();
  starting2 = drawCard();

  playerCards[playerCardCount++] = starting1;
  playerCards[playerCardCount++] = starting2;

  playerTotal = getBestHandValue(playerCards, playerCardCount); 

  Serial.print("Player cards: ");
  for (int i = 0; i < playerCardCount; i++) {
    Serial.print(playerCards[i]);
    Serial.print(", ");
  }
  Serial.println();
  

}



// Initilizes dealer's starting hand
void dealerHand() {
  
  dealerCardCount = 0;

  dealer1 = drawCard();
  dealer2 = drawCard();

  dealerCards[dealerCardCount++] = dealer1;
  dealerCards[dealerCardCount++] = dealer2;

  dealerTotal = getBestHandValue(dealerCards, dealerCardCount);
  
  Serial.print("Dealer cards: ");
  for (int i = 0; i < dealerCardCount; i++) {
    Serial.print(dealerCards[i]);
    Serial.print(", ");
  }
  Serial.println();
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
  delay(900);

  playerTotal = getBestHandValue(playerCards, playerCardCount);
  dealerTotal = getBestHandValue(dealerCards, dealerCardCount);

  
}



// determines if player or dealer has blackjack already

void checkBlackJack(){
  
  if (playerTotal == 21 && playerCardCount == 2) {
    lcd.clear();
    lcd.print("BLACKJACK! You win!");
    delay(2000);
    gameOver = true;
    resetGame();
    return;
  }

  if(dealerTotal == 21 && dealerCardCount == 2){
    lcd.clear();
    lcd.print("BLACKJACK! Dealer win!");
    delay(2000);
    gameOver = true;
    resetGame();
    return;
  }

}



// determines whether player bust
void isBust(int total) {

  if (total > 21) {
    lcd.clear();
    lcd.print("You Busted!");
    delay(900);
    resetGame();
    checkBlackJack();
  }

}



//displays the cards onto the lcd
void displayCard(String card) {

  card.toLowerCase();
  int spaceIndex = card.indexOf(" ");
  String rank = card.substring(0, spaceIndex);

  Serial.print("Displaying card: ");
  Serial.println(card);

  if (rank == "a") lcd.print("A");
  else if (rank == "j") lcd.print("J");
  else if (rank == "q") lcd.print("Q");
  else if (rank == "k") lcd.print("K");
  else lcd.print(rank);

  if (card.indexOf("hearts") != -1) lcd.write(byte(0));
  else if (card.indexOf("diamonds") != -1) lcd.write(byte(1));
  else if (card.indexOf("spades") != -1) lcd.write(byte(2));
  else if (card.indexOf("clubs") != -1) lcd.write(byte(3));
}



void displayHand(String cards[], int count, int row) {

  lcd.setCursor(0, row);
  for (int i = 0; i < count; i++) {
    displayCard(cards[i]);
    lcd.print(" ");
  }
}



// determines whether the winner of the round or tie
void determineWinner() {
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
  resetGame();
  checkBlackJack();
}



// Once player stands, dealer will delt its cards
void dealerTurn() {

  while (dealerTotal < 17) {

    String newCard = drawCard();
    dealerCards[dealerCardCount++] = newCard;
    dealerTotal = getBestHandValue(dealerCards, dealerCardCount);

    // debugging print
    Serial.print("Dealer cards: ");
    for (int i = 0; i < dealerCardCount; i++) {
      Serial.print(dealerCards[i]);
      Serial.print(", ");
    }
    Serial.println();

    lcd.clear();
    lcd.print("Dealer draws:");
    delay(1000);

    //lcd.setCursor(0, 1);
    //displayHand(dealerCards, dealerCardCount);
    //displayCard(newCard);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Dealer card:");
    displayHand(dealerCards, dealerCardCount, 1);
    delay(1500);


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



// ask the player if they want to hit or stand
void hitStand() {

  if(gameOver){
    return;
  }

  while (true) {
    lcd.clear();
    lcd.print("Hit or Stand?");
    delay(1500);

    int hitButtonState = digitalRead(hitButtonPin);
    int standButtonState = digitalRead(standButtonPin);

    if (hitButtonState == LOW) {

      String newCard = drawCard();
      playerCards[playerCardCount++] = newCard;
      playerTotal = getBestHandValue(playerCards, playerCardCount);

     // debugging print
      Serial.print("Player cards: ");
      for (int i = 0; i < playerCardCount; i++) {
        Serial.print(playerCards[i]);
        Serial.print(", ");
      }
      Serial.println();

      lcd.clear();
      lcd.print("You chose: HIT");
      delay(1000);
      //lcd.setCursor(0, 1);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Player card:");
      displayHand(playerCards, playerCardCount, 1);
      delay(1500);

      //displayHand(playerCards, playerCardCount);
      //displayCard(newCard);
      //delay(1500);


      isBust(playerTotal);
    } 
    else if (standButtonState == LOW) {

      lcd.clear();
      lcd.print("You chose: STAND");
      delay(1500);

      // see how this functions works
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Dealer card:");
      displayHand(dealerCards, dealerCardCount, 1);
      delay(1200);

      dealerTurn();
      break;

    }
  }
}



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
  lcd.clear();
  lcd.print("Dealing cards...");
  delay(1500);

  initializeDeck();
  startingHand();
  dealerHand();

  playerTotal = getBestHandValue(playerCards, playerCardCount);
  dealerTotal = getBestHandValue(dealerCards, dealerCardCount);

  checkBlackJack(); // <-- Only call here, after both hands are dealt

  
}



void loop() {

  /*
  lcd.clear();
  lcd.print("Your cards:");
  lcd.setCursor(0, 1);
  displayCard(starting1);
  lcd.setCursor(8, 1);
  displayCard(starting2);
  delay(3000);
  

  hitStand();
  
  lcd.clear();
  lcd.print("Dealer:");
  lcd.setCursor(9, 0);
  displayCard(dealer1);
  delay(3000);
  
  */

 
  // 1. Show Player's 2 cards
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Player card:");
  displayHand(playerCards, playerCardCount, 1);
  delay(2000);

  // 2. Show Dealer's 1st card
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Dealer card:");
  lcd.setCursor(0, 1);
  displayCard(dealerCards[0]);
  lcd.print(" ?");
  delay(1500);

  // 3. Player's turn
  hitStand();

  

  
}
