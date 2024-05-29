#include "escprinterble.h"
#include "BLEDevice.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! or your board doesn't have one
#endif

EscPos esc;

const int buttonPin = 19;
const int ledPin = 25;
int buttonState = LOW;
int randResult = 999;
int tossResult[] = { 0, 0, 0, 0, 0, 0 };

// Define a struct to hold hexagrams
struct Hexagrams {
  String simplifiedHexagram;
  String changedHexagram;
};

const String shortDescriptions[] = {
  "Force",
  "The Flow",
  "Beginning",
  "Recklessness",
  "Waiting",
  "Conflict",
  "Leadership",
  "Union",
  "Restraint",
  "Respect",
  "Peace",
  "Stagnation",
  "Fellowship",
  "Posperity",
  "Modesty",
  "Enthusiasm",
  "Pursuit",
  "Decay",
  "Ascention",
  "Contemplation",
  "Chomping",  // Biting through
  "Grace",
  "Splitting",
  "Revival",        // retrurning? rebirth?
  "Innocence",      //Serendipity
  "Dominance",      //Authority?
  "Consumption",    //Absorption?
  "Imbalance",      // instability
  "Vulnerability",  //Peril? The Abyss?
  "Adherence",      // Clinging? Radiance?
  "Influence",      // Persuasion
  "Stability",      // Endurance, Persevering?
  "Withdrawal",     //Retreat
  "Great Power",
  "Progress",  //Clarity
  "Eclipse",   // Obscurity
  "Family",    // Alliance
  "Opposition",
  "Obstacles",       //Obstruction
  "Deliverance",     //Salvation
  "Decline",         //Damage,
  "Augmentation",    //Increase
  "Transformation",  //Breakthrough
  "Temptation",      // Reversal?
  "Accumulation",    //Gathering,
  "Effort",          // Pushing Against
  "Oppression",      // Exhaustion, Confinement
  "Fertility",       //Abundance?
  "Revolution",
  "Nourishment",         //Hospitality
  "Shock",               //Intimidation?
  "Tranquility",         //Stillness?, Serenity?
  "Development",         //Advancement?
  "Compliance",          //Conformity
  "Luxury",              //Fullness
  "Transition",          //Travels, Exile?
  "Clarity",             //Penetration? Like th sun?
  "Joy",                 //Happiness
  "Evaporation",         //Dissolution, Vanishing
  "Limitation",          //Restraint
  "Authenticity",        //Sincerity, Genuineness?
  "Prudence",            // Cautiousness? Vigilance
  "Culmination",         //Conclusion, Climax
  "Unfinished Business"  // Incompleteness
};

const String longDescriptions[] = {
  "Forceful beginning", "Resolves differences", "Great possession", "Powerful presence", "Humble success", "Conflict resolution", "Nourishing development", "Effortless flow",
  "Keeping still", "Joyful expansion", "Peaceful tranquility", "Expansion of spirit", "Completion of cycles", "Self-imposed limits", "Loyal adherence", "Approaching opportunity",
  "Dissolving problems", "Gaining benefits", "Taming power", "Excess abundance", "Doubt and caution", "Crossing great waters", "Elegant simplicity", "Brightness returning",
  "Collective power", "Return of energy", "Contemplative reflection", "Steadfast determination", "Sustaining energy", "Initial difficulty", "Change and adaptation", "Youngest daughter",
  "Clinging fire", "Wandering spirit", "Prevailing strength", "Retreat for renewal", "Gentle influence", "Joyful clarity", "Radiance and warmth", "Exhaustion and renewal", "Union of forces", "Great breakthrough",
  "Spreading influence", "Penetrating insight", "Limiting influence", "Youthful folly", "Energetic vitality", "Mounting success", "Dispersing doubts", "Disciplinary action", "Bold daring", "Manifesting desires",
  "Tension and release", "Looking within", "Protective shield", "Guiding force", "Action and movement", "Inner strength", "Cultivated field", "Strong foundation", "Peaceful harmony", "Surging ahead", "Adorning beauty", "Watchful awareness"
};

struct Descriptions {
  String shortSimplified;
  String longSimplified;
  String shortChanged;
  String longChanged;
};


void setup() {
  // initialize the pushbutton pin as an input:
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);


  Serial.begin(115200);

  while (!Serial)
    ;  // wait for serial port to connect.

  // You can change service, characteristic UUID for a compatible esc BLE printer
  char* service = "000018F0-0000-1000-8000-00805F9B34FB";
  char* characteristic = "00002AF1-0000-1000-8000-00805F9B34FB";
  esc = EscPos(service, characteristic);
  esc.start();

  if (esc.connect()) {
    Serial.println("Connected.");
  }
}

void printYingYang(int yarr) {
  Serial.println("Printing line corresponding to " + String(yarr));
  esc.align(ALIGN_CENTER);
  switch (yarr) {
    case 6:
      esc.println("---x---");
      break;
    case 9:
      esc.println("---o---");
      break;
    case 7:
      esc.println("-------");
      break;
    case 8:
      esc.println("--- ---");
      break;
    default:
      break;
  }
}

String constructUrl(int tossResult[], String simplifiedHexagram) {
  // Convert tossResult array to a single string
  String hexagram = "";
  for (int i = 0; i < 6; i++) {
    hexagram += String(tossResult[i]);
  }

  // Construct the URL with the hexagram and simplifiedHexagram
  String url = "http://wengu.tartarie.com/wg/wengu.php?l=Yijing&tire=" + hexagram + "&no=" + simplifiedHexagram + "&lang=en";

  return url;
}

Hexagrams findMatches(int arr[]) {

  // Define arrays to store match arrays and their corresponding hexagram
  const int yarrowArrays[][6] = {
    { 7, 7, 7, 7, 7, 7 },  // 1
    { 8, 8, 8, 8, 8, 8 },  // 2
    { 7, 8, 8, 8, 7, 8 },  // 3
    { 8, 7, 8, 8, 8, 7 },  // 4
    { 7, 7, 7, 8, 7, 8 },  // 5
    { 8, 7, 8, 7, 7, 7 },  // 6
    { 8, 7, 8, 8, 8, 8 },  // 7
    { 8, 8, 8, 8, 7, 8 },  // 8
    { 7, 7, 7, 8, 7, 7 },  // 9
    { 7, 7, 8, 7, 7, 7 },  // 10
    { 7, 7, 7, 8, 8, 8 },  // 11
    { 8, 8, 8, 7, 7, 7 },  // 12
    { 7, 8, 7, 7, 7, 7 },  // 13
    { 7, 7, 7, 7, 8, 7 },  // 14
    { 8, 8, 7, 8, 8, 8 },  // 15
    { 8, 8, 8, 7, 8, 8 },  // 16
    { 7, 8, 8, 7, 7, 8 },  // 17
    { 8, 7, 7, 8, 8, 7 },  // 18
    { 7, 7, 8, 8, 8, 8 },  // 19
    { 8, 8, 8, 8, 7, 7 },  // 20
    { 7, 8, 8, 7, 8, 7 },  // 21
    { 7, 8, 7, 8, 8, 7 },  // 22
    { 8, 8, 8, 8, 8, 7 },  // 23
    { 7, 8, 8, 8, 8, 8 },  // 24
    { 7, 8, 8, 7, 7, 7 },  // 25
    { 7, 7, 7, 8, 8, 7 },  // 26
    { 7, 8, 8, 8, 7, 7 },  // 27
    { 8, 7, 7, 7, 7, 8 },  // 28
    { 8, 7, 8, 8, 7, 8 },  // 29
    { 7, 8, 7, 7, 8, 7 },  // 30
    { 8, 8, 7, 7, 7, 8 },  // 31
    { 8, 7, 7, 7, 8, 8 },  // 32
    { 8, 8, 7, 7, 7, 7 },  // 33
    { 7, 7, 7, 7, 8, 8 },  // 34
    { 8, 8, 8, 7, 8, 7 },  // 35
    { 7, 8, 7, 8, 8, 8 },  // 36
    { 7, 8, 7, 8, 7, 7 },  // 37
    { 7, 7, 8, 7, 8, 7 },  // 38
    { 8, 8, 7, 8, 7, 8 },  // 39
    { 8, 7, 8, 7, 8, 8 },  // 40
    { 7, 7, 8, 8, 8, 7 },  // 41
    { 7, 8, 8, 8, 7, 7 },  // 42
    { 7, 7, 7, 7, 7, 8 },  // 43
    { 8, 7, 7, 7, 7, 7 },  // 44
    { 8, 8, 8, 7, 7, 8 },  // 45
    { 8, 7, 7, 8, 8, 8 },  // 46
    { 8, 7, 8, 7, 7, 8 },  // 47
    { 8, 7, 7, 8, 7, 8 },  // 48
    { 7, 8, 7, 7, 7, 8 },  // 49
    { 8, 7, 7, 7, 8, 7 },  // 50
    { 7, 8, 8, 7, 8, 8 },  // 51
    { 8, 8, 7, 8, 8, 7 },  // 52
    { 8, 8, 7, 8, 7, 7 },  // 53
    { 7, 7, 8, 7, 8, 8 },  // 54
    { 7, 8, 7, 7, 8, 8 },  // 55
    { 8, 8, 7, 7, 8, 7 },  // 56
    { 8, 7, 7, 8, 7, 7 },  // 57
    { 7, 7, 8, 7, 7, 8 },  // 58
    { 8, 7, 8, 8, 7, 7 },  // 59
    { 7, 7, 8, 8, 7, 8 },  // 60
    { 7, 7, 8, 8, 7, 7 },  // 61
    { 8, 8, 7, 7, 8, 8 },  // 62
    { 7, 8, 7, 8, 7, 8 },  // 63
    { 8, 7, 8, 7, 8, 7 }   // 64
  };

  const String matchHexagrams[] = {
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "10",
    "11",
    "12",
    "13",
    "14",
    "15",
    "16",
    "17",
    "18",
    "19",
    "20",
    "21",
    "22",
    "23",
    "24",
    "25",
    "26",
    "27",
    "28",
    "29",
    "30",
    "31",
    "32",
    "33",
    "34",
    "35",
    "36",
    "37",
    "38",
    "39",
    "40",
    "41",
    "42",
    "43",
    "44",
    "45",
    "46",
    "47",
    "48",
    "49",
    "50",
    "51",
    "52",
    "53",
    "54",
    "55",
    "56",
    "57",
    "58",
    "59",
    "60",
    "61",
    "62",
    "63",
    "64"
  };


  String simplifiedHexagram;
  String changedHexagram;
  int simplifiedArray[6];
  int changedArray[6];

  // Check if the array contains any 6 or 9
  bool contains6or9 = false;
  for (int i = 0; i < 6; i++) {
    if (arr[i] == 6 || arr[i] == 9) {
      contains6or9 = true;
      break;
    }
  }

  if (contains6or9) {
    for (int i = 0; i < 6; i++) {
      if (arr[i] == 6) {
        simplifiedArray[i] = 8;
        changedArray[i] = 7;
      } else if (arr[i] == 9) {
        simplifiedArray[i] = 7;
        changedArray[i] = 8;
      } else {
        simplifiedArray[i] = arr[i];
        changedArray[i] = arr[i];
      }
    }
  } else {
    for (int i = 0; i < 6; i++) {
      simplifiedArray[i] = arr[i];
      changedArray[i] = arr[i];
    }
  }


  // debug

  Serial.print("Input array is: ");
  for (int i = 0; i < 6; i++) {
    Serial.print(arr[i]);
    Serial.print(" ");
  }
  Serial.println();

  Serial.print("Simplified array is: ");
  for (int i = 0; i < 6; i++) {
    Serial.print(simplifiedArray[i]);
    Serial.print(" ");
  }
  Serial.println();

  Serial.print("An changed array is: ");
  for (int i = 0; i < 6; i++) {
    Serial.print(changedArray[i]);
    Serial.print(" ");
  }
  Serial.println();

  // end debug


  // Compare the given array with each match array
  for (int i = 0; i < 64; i++) {
    bool simplifiedMatch = true;
    bool changedMatch = true;

    for (int j = 0; j < 6; j++) {
      if (simplifiedArray[j] != yarrowArrays[i][j]) {
        simplifiedMatch = false;
      }
      if (changedArray[j] != yarrowArrays[i][j]) {
        changedMatch = false;
      }
    }

    if (simplifiedMatch) {
      simplifiedHexagram = matchHexagrams[i];
    }

    if (changedMatch) {
      changedHexagram = matchHexagrams[i];
    }
  }

  if (!contains6or9) {
    changedHexagram = simplifiedHexagram;
  }

  return { simplifiedHexagram, changedHexagram };
}

Descriptions getDescriptions(String simplifiedHexagram, String changedHexagram) {
  int simplifiedIndex = simplifiedHexagram.toInt() - 1;
  int changedIndex = changedHexagram.toInt() - 1;

  Descriptions descriptions = {
    shortDescriptions[simplifiedIndex],
    longDescriptions[simplifiedIndex],
    shortDescriptions[changedIndex],
    longDescriptions[changedIndex]
  };

  return descriptions;
}

bool buttonPressed = false;
unsigned long lastButtonPressTime = 0;
const unsigned long buttonPressDelay = 3000;  // Adjust the delay time in milliseconds (e.g., 3000 for 3 seconds)

void loop() {
  if (digitalRead(buttonPin) == LOW && !buttonPressed) {
    buttonPressed = true;
    lastButtonPressTime = millis();

    digitalWrite(ledPin, HIGH);  // Turn on LED if button is pressed (connected to ground)

    int tossItera = 0;

    while (tossItera < 6) {
      randResult = random(15);
      Serial.println("Got random number " + String(randResult));

      if (randResult == 0) {
        randResult = 6;
      } else if (1 <= randResult && randResult <= 3) {
        randResult = 9;
      } else if (4 <= randResult && randResult <= 8) {
        randResult = 7;
      } else if (9 <= randResult && randResult <= 15) {
        randResult = 8;
      }
      tossResult[tossItera] = randResult;
      tossItera++;
    }
  }

  if (buttonPressed && millis() - lastButtonPressTime >= buttonPressDelay) {
    buttonPressed = false;
    digitalWrite(ledPin, LOW);

    int length = 6;  // Hardcoded length since the array always has 6 elements

    Serial.println("The initial array is");

    for (int i = 0; i < length; i++) {
      Serial.println(tossResult[i]);
    }

    // Printing the normal array
    // esc.println("The array after randomising is:");
    // for (int i = 0; i < length; i++) {
    //   printYingYang(tossResult[i]);
    // }

    // Reversing the array
    int start = 0;
    int end = length - 1;

    while (start < end) {
      // Swap elements at start and end indices
      int temp = tossResult[start];
      tossResult[start] = tossResult[end];
      tossResult[end] = temp;

      // Move indices towards the middle
      start++;
      end--;
    }

    // debug

    Serial.print("The correct array sequence is: ");
    for (int i = 0; i < 6; i++) {
      Serial.print(tossResult[i]);
      Serial.print(" ");  // Separate elements with a space
    }
    Serial.println();  // Print a newline after all elements are printed
                       // end debug

    // Printing the reversed array
    //esc.println("The array after reversing it:");
    for (int i = 0; i < length; i++) {
      printYingYang(tossResult[i]);
    }
    Serial.println("Printed yingyang lines");

    Hexagrams hexagrams = findMatches(tossResult);

    // Debugging output to check hexagrams
    Serial.print("hexagrams.simplifiedHexagram value is: ");
    Serial.print(hexagrams.simplifiedHexagram);
    Serial.print("an hexagrams.changedHexagram value is: ");
    Serial.println(hexagrams.changedHexagram);

    // Get descriptions
    Descriptions descriptions = getDescriptions(hexagrams.simplifiedHexagram, hexagrams.changedHexagram);
    // Print descriptions
    if (hexagrams.changedHexagram == hexagrams.simplifiedHexagram) {
      Serial.println("Short hexagram description: " + descriptions.shortSimplified);
      esc.align(ALIGN_CENTER);
      esc.println("Experience " + descriptions.shortSimplified);
      // Serial.println("Long hexagram description:  " + descriptions.longSimplified);
    } else {
      Serial.println(descriptions.shortSimplified + " changes into " + descriptions.shortChanged);
      // Serial.println("long hexagram description: " + descriptions.longSimplified);
      // Serial.println("Long changed description:  " + descriptions.longChanged);
      esc.align(ALIGN_CENTER);
      esc.println(descriptions.shortSimplified + " changes into " + descriptions.shortChanged);
    }
    Serial.println("Printed description lines");
    esc.feed(1);

    // print url qr code
    String url = constructUrl(tossResult, hexagrams.simplifiedHexagram);
    Serial.println("Printing constructed url:");
    Serial.println(url);
    esc.align(ALIGN_CENTER);
    esc.println("More info here:");
    esc.codeQR(url, 6);
    esc.feed(2);

    delay(3000);                // Wait for 3 seconds to debounce the button
    digitalWrite(ledPin, LOW);  // Turn off LED
  }
}
