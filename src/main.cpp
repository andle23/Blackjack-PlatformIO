#include <Arduino.h>

#include <LiquidCrystal.h>

String deck[52];
int deckSize = 52;

String starting1, starting2;

String dealer1, dealer2;

String playerCards[12];
String dealerCards[12];

int playerCardCount = 0;
int dealerCardCount = 0;

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
  String suits[] = {" hearts", " spades", " clubs", " diamonds"};
  int index = 0;
  for (int i = 1; i < 14; i++) {
    for (int j = 0; j < 4; j++) {
      if (i == 1) {
        deck[index] = "Ace of " + suits[j];
      } else if (i == 11) {
        deck[index] = "Jack of " + suits[j];
      } else if (i == 12) {
        deck[index] = "Queen of " + suits[j];
      } else if (i == 13) {
        deck[index] = "King of " + suits[j];
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

  for (int i = randomIndex; i < deckSize - 1; i++) {
    deck[i] = deck[i + 1];
  }

  deckSize--;
  return card;
}



// Helper to get best hand value (Ace as 11 or 1)
int getBestHandValue(String cards[], int count) {
 
  int total = 0;
  int aceCount = 0;

  for (int i = 0; i < count; i++) {
    int val = getCardValue(cards[i]);
    if (val == 11) aceCount++;
    total += val;
  }

  // Adjust for Aces
  while (total > 21 && aceCount > 0) {
    total -= 10;
    aceCount--;
  }
  return total;
}



// obtaining the cards value 
int getCardValue(String card) {
  
  card.toLowerCase();
  card = card.substring(0); // forces Arduino to actually update
  card.toLowerCase();   // now modifies

  if (card.indexOf("ace") != -1) {
    return 11;
  } 
  else if (card.indexOf("king") != -1 || card.indexOf("queen") != -1 || card.indexOf("jack") != -1) {
    return 10;
  } 
  else {
    int spaceIndex = card.indexOf(" ");
    if (spaceIndex > 0) {
      String numStr = card.substring(0, spaceIndex);
      return numStr.toInt();
    }
  }
  return 0;
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



// Initilizes player's starting hand
void startingHand() {

  playerCardCount = 0;

  starting1 = drawCard();
  starting2 = drawCard();

  playerCards[playerCardCount++] = starting1;
  playerCards[playerCardCount++] = starting2;

  playerTotal = getBestHandValue(playerCards, playerCardCount);
  

  checkBlackJack(); // <-- check for blackjack
}



// Initilizes dealer's starting hand
void dealerHand() {
  
  dealerCardCount = 0;

  dealer1 = drawCard();
  dealer2 = drawCard();

  dealerCards[dealerCardCount++] = dealer1;
  dealerCards[dealerCardCount++] = dealer2;

  dealerTotal = getBestHandValue(dealerCards, dealerCardCount);
  
  checkBlackJack(); // <-- check for blackjack
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

  // POTENTIAL ADDITION for playerTotal and dealerTotal
}



// determines whether player bust
void isBust(int total) {

  if (total > 21) {
    lcd.clear();
    lcd.print("You Busted!");
    delay(900);
    resetGame();
  }

}



//displays the cards onto the lcd
void displayCard(String card) {
  
  card.toLowerCase();  // make sure lowercase
  int spaceIndex = card.indexOf(" ");
  String rank = card.substring(0, spaceIndex);

  lcd.print(rank);

  if (card.indexOf("hearts") != -1) lcd.write(byte(0));
  else if (card.indexOf("diamonds") != -1) lcd.write(byte(1));
  else if (card.indexOf("spades") != -1) lcd.write(byte(2));
  else if (card.indexOf("clubs") != -1) lcd.write(byte(3));
}



// determines whether the winner of the round or tie
void determineWinner() {
  lcd.clear();
  if (playerTotal > dealerTotal) {
    lcd.print("You Win!");
  } else if (playerTotal < dealerTotal) {
    lcd.print("Dealer Wins!");
  } else {
    lcd.print("It's a Push!");
  }
  delay(3000);
  resetGame();
}



// Once player stands, dealer will delt its cards
void dealerTurn() {

  while (dealerTotal < 17) {

    String newCard = drawCard();
    dealerCards[dealerCardCount++] = newCard;
    dealerTotal += getCardValue(newCard); // POTENTIAL CHANGE

    lcd.clear();
    lcd.print("Dealer draws:");
    lcd.setCursor(0, 1);
    displayCard(newCard);
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
      playerTotal += getCardValue(newCard); // POTENTIAL CHANGE

      lcd.clear();
      lcd.print("You chose: HIT");
      lcd.setCursor(0, 1);
      displayCard(newCard);
      delay(1500);


      isBust(playerTotal);
    } 
    else if (standButtonState == LOW) {
      lcd.clear();
      lcd.print("You chose: STAND");
      delay(1500);
      dealerTurn();
      break;
    }
  }
}



void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));
  initializeDeck();
  startingHand();
  dealerHand();

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
}



void loop() {
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

  

  
}
