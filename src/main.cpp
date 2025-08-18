#include <Arduino.h>

#include <LiquidCrystal.h>

String deck[52];
int deckSize = 52;
String starting1, starting2; // Global player
String dealer1, dealer2;     // Global dealer
int dealerTotal = 0;         // Global
int playerTotal = 0;         // Global
bool gameOver = false;

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int hitButtonPin = 7;
const int standButtonPin = 8;

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

void initializeDeck() {
  String suits[] = {" hearts", " spades", " clubs", " diamonds"};
  int index = 0;
  for (int i = 1; i < 14; i++) {
    for (int j = 0; j < 4; j++) {
      if (i == 1) {
        deck[index] = "Ace of" + suits[j];
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

int getCardValue(String card) {
  card.toLowerCase();

  if (card.indexOf("ace") != -1) {
    return 11;
  } else if (card.indexOf("king") != -1 ||
             card.indexOf("queen") != -1 ||
             card.indexOf("jack") != -1) {
    return 10;
  } else {
    int spaceIndex = card.indexOf(" ");
    if (spaceIndex > 0) {
      String numStr = card.substring(0, spaceIndex);
      return numStr.toInt();
    }
  }
  return 0;
}

void startingHand() {
  starting1 = drawCard();
  starting2 = drawCard();
  playerTotal = getCardValue(starting1) + getCardValue(starting2);
  Serial.print("Player Cards: ");
  Serial.print(starting1);
  Serial.print(", ");
  Serial.println(starting2);
  Serial.print("Player Total: ");
  Serial.println(playerTotal);
}

void dealerHand() {
  dealer1 = drawCard();
  dealer2 = drawCard();
  dealerTotal = getCardValue(dealer1) + getCardValue(dealer2);
  Serial.print("Dealer Cards: ");
  Serial.print(dealer1);
  Serial.print(", ");
  Serial.println(dealer2);
  Serial.print("Dealer Total: ");
  Serial.println(dealerTotal);
}

void resetGame() {
  deckSize = 52;
  playerTotal = 0;
  dealerTotal = 0;
  gameOver = false;

  initializeDeck();
  startingHand();
  dealerHand();

  lcd.clear();
  lcd.print("New Round!");
  delay(1000);
}

void isBust(int total) {
  if (total > 21) {
    lcd.clear();
    lcd.print("You Busted!");
    delay(1000);
    resetGame();
  }
}

void dealerTurn() {
  while (dealerTotal < 17) {
    String newCard = drawCard();
    dealerTotal += getCardValue(newCard);

    lcd.clear();
    lcd.print("Dealer draws:");
    lcd.setCursor(0, 1);
    displayCard(newCard);
    delay(1500);

    Serial.print("Dealer draws: ");
    Serial.println(newCard);
    Serial.print("Dealer Total: ");
    Serial.println(dealerTotal);

    if (dealerTotal > 21) {
      lcd.clear();
      lcd.print("Dealer Busts!");
      delay(1000);
      resetGame();
      return;
    }
  }
  determineWinner();
}

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

void displayCard(String card) {
  if (card.indexOf("hearts") != -1) {
    lcd.print(card.substring(0, card.indexOf(" ")));
    lcd.write(byte(0));
  } else if (card.indexOf("diamonds") != -1) {
    lcd.print(card.substring(0, card.indexOf(" ")));
    lcd.write(byte(1));
  } else if (card.indexOf("spades") != -1) {
    lcd.print(card.substring(0, card.indexOf(" ")));
    lcd.write(byte(2));
  } else if (card.indexOf("clubs") != -1) {
    lcd.print(card.substring(0, card.indexOf(" ")));
    lcd.write(byte(3));
  }
}

void hitStand() {
  while (true) {
    lcd.clear();
    lcd.print("Hit or Stand?");
    delay(1500);

    int hitButtonState = digitalRead(hitButtonPin);
    int standButtonState = digitalRead(standButtonPin);

    if (hitButtonState == LOW) {
      String newCard = drawCard();
      playerTotal += getCardValue(newCard);

      lcd.clear();
      lcd.print("You chose: HIT");
      lcd.setCursor(0, 1);
      displayCard(newCard);
      delay(1500);

      Serial.print("Player draws: ");
      Serial.println(newCard);
      Serial.print("Player Total: ");
      Serial.println(playerTotal);

      isBust(playerTotal);
    } else if (standButtonState == LOW) {
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

  Serial.print("Starting Cards: ");
  Serial.print(starting1);
  Serial.print(", ");
  Serial.println(starting2);
  Serial.print("Player Total: ");
  Serial.println(playerTotal);

  lcd.clear();
  lcd.print("Dealer:");
  lcd.setCursor(9, 0);
  displayCard(dealer1);
  delay(3000);

  Serial.print("Dealer's First Card: ");
  Serial.println(dealer1);
  Serial.print("Dealer Total (Hidden): ");
  Serial.println(getCardValue(dealer1));

  hitStand();
}
