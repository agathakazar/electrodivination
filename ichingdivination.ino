#include "escprinterble.h" // https://github.com/rnrobles/esc-thermal-printer-ble
#include "BLEDevice.h"
#include "EspEasyServoEx.h" // https://github.com/tanakamasayuki/EspEasyUtils
#include <EspCapaSens.h>
#include "Max7219Animation.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! or your board doesn't have one
#endif

EscPos esc;

//led matrix stuff
const uint8_t DATA_PIN = 19;  // DIN
const uint8_t CS_PIN = 5;    // CS
const uint8_t CLK_PIN = 18;   // CLK
const uint64_t CUSTOM_BITMAP = 0x5555aaaa5555aaaa;

Max7219Animation display(DATA_PIN, CLK_PIN, CS_PIN);

//capasensor stuff
const int left1 = 32;
const int left2 = 33;
const int right1 = 12;
const int right2 = 14;
const float handThreshold = 20.0;
const int debounceCount = 5;
const int debounceDelay = 50;

capasensor capleft(left1, left2, 1.5);
capasensor capright(right1, right2, 1.5);

//other stuff

int buttonState = LOW;
int randResult = 999;
int tossResult[] = { 0, 0, 0, 0, 0, 0 };

// Servo initialize
EspEasyServoEx servo(LEDC_CHANNEL_0, GPIO_NUM_13);



// Define a struct to hold hexagrams
struct Hexagrams {
  String simplifiedHexagram;
  String changedHexagram;
};

const String shortDescriptions[] = {
  "Force", //1
  "Flow", //2
  "Beginning", //3
  "Recklessness", //4 Immaturity
  "Waiting", //5
  "Conflict", //6
  "Discipline", //7 Leadership
  "Union", //8
  "Restraint", //9
  "Caution", //10
  "Harmony", //11
  "Stagnation", //12
  "Community", //13 Fellowship?
  "Wealth", //14
  "Modesty", //15
  "Enthusiasm", //16
  "Following", //17
  "Repair", //18
  "Advances", //19
  "Contemplation", //20
  "Reformation",  // 21 Discernment, Biting through
  "Grace", //22
  "Disintegration", //23 Collapse, Ruin
  "Revival",        //24 retrurning? rebirth?
  "Serendipity",      //25 Innocence
  "Willpower",      //26 Authority? Dominance
  "Consumption",    //27 Absorption?
  "Imbalance",      //28 instability
  "Vulnerability",  //29 Danger? Peril?
  "Clarity",      //30 Clinging? Radiance? 
  "Initiative",      //31 Persuasion, Influence
  "Consistency",      //32 Endurance, Persevering? Stability?
  "Withdrawal",     //33 Retreat
  "Power", //34 Great Power
  "Progress",  //35 
  "Ignorance",   //36 Obscurity
  "Companions",    //37 Confidants, 
  "Opposition", //38
  "Obstacles",       //39 Obstruction
  "Liberation",     //40 Salvation
  "Loss",         //41 Damage, Decline
  "Gain",    //42 Increase
  "Transformation",  //43 Breakthrough
  "Temptation",      //44 Reversal?
  "Contraction",    //45 Gathering
  "Ascension",          //46 Pushing Against
  "Oppression",      //47 Exhaustion, Confinement
  "The Source",       //48 The well, 
  "Metamorphosis",      //49
  "Rejuvenation",         // 50 
  "Shock",               // 51 Intimidation?
  "Stillness",         // 52 , Serenity?
  "Development",         // 53 Advancement?
  "Compliance",          // 54 Conformity
  "Abundance",              // 55 Fullness
  "Transition",          // 56 Travels, Exile?
  "Penetration",             // 57 Penetration? Like th sun?
  "Joy",                 // 58 Happiness
  "Expansion",         // 59 Dissolution, Vanishing, Evaporation
  "Limitation",          // 60 Restraint
  "Authenticity",        // 61 Sincerity, Genuineness?
  "Vigilance",            // 62 Cautiousness? Vigilance, Prudence
  "Completion",         // 63 Conclusion, Climax
  "Incompleteness"  // 64 Incompleteness, Unfinished Business
};

const String longDescriptions[] = {
  "Creative Forces", //1
  "Natural Flow", //2
  "Difficult Beginnings", //3
  "Acquiring Experience", //4
  "Calculated Inaction", //5
  "Stress and Tention", //6
  "Group Action", //7
  "Coordination and Unity", //8
  "Minor Obstruction", //9
  "Proceeding Cautiously", //10
  "Peace and Harmony", //11
  "Standstill and Decadence", //12
  "Companionship With People", //13
  "Possession in Great Measure", //14
  "Temperance and Moderation", //15
  "Enthusiastic Self-confidence", //16
  "Following and Being Followed", //17
  "Work On What Has Been Spoiled", //18
  "Progressive Advances", //19
  "Observation and Introspection", //20
  "Figuring It Out", //21
  "Elegance and Vanity", //22
  "Collapse and Ruin", //23
  "Cyclic Repetition", //24
  "Freedom from Error", //25
  "Energy Under Control", //26
  "Appropriate Nourishment", //27
  "Great Excess", //28
  "Perilous Abyss", //29
  "Radiance of Comprehension", //30
  "Influence of Stimulation", //31
  "Constancy in the Midst of Change", //32
  "Retreat and Disassociation", //33
  "Great Power", //34
  "Advance of Consciousness", //35
  "Clouded Perception", //36
  "Kinship", //37
  "Mutual Alienation", //38
  "Impasse", //39
  "Release of Tension", //40
  "Compensating Sacrifice", //41
  "Augmentation", //42
  "Breakthrough", //43
  "Passing Temptation", //44
  "Gathering Together", //45
  "Rising and Advancing", //46
  "Tiresome Restriction", //47
  "Access to the Source", //48
  "Radical Change", //49
  "Reconstruction", //50
  "Sudden Change", //51
  "Keeping Stillness", //52
  "Gradual Progress", //53
  "Conformity and Propriety", //54
  "Luxuriant Fullness ", //55
  "Wandering and Travel", //56
  "Penetrating Winds", //57
  "Self-indulgence", //58
  "Catharsis", //59
  "Restrictive Regulations", //60
  "Inner Truth", //61
  "Things Small", //62
  "A state of Climax", //63
  "Unfinished Business" //64
};

struct Descriptions {
  String shortSimplified;
  String longSimplified;
  String shortChanged;
  String longChanged;
};


void setup() {
  // let's get serial
  Serial.begin(115200);
  delay(500);


  // let's get led matrix
  display.begin(Max7219Animation::IDLE, 200);  // Start in IDLE with 200ms frame delay (slower for idle)
  display.startTask();

  capleft.begin();
  Serial.println(capleft.getBaseline(), 1);
  capright.begin();
  Serial.println(capright.getBaseline(), 1);

  // You can change service, characteristic UUID for a compatible esc BLE printer
  char* service = "000018F0-0000-1000-8000-00805F9B34FB";
  char* characteristic = "00002AF1-0000-1000-8000-00805F9B34FB";
  esc = EscPos(service, characteristic);
  esc.start();

  if (esc.connect()) {
    Serial.println("Connected.");
  }

  //servo closer to paper
  servo.setTarget(45, 69);
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
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd == "c") {
      capleft.recalibrate();  // defaults: 10 samples, 2000 ms wait
      capright.recalibrate();
    } else if (cmd.startsWith("s")) {
      float newSens = cmd.substring(1).toFloat();
      capleft.setSensitivity(newSens);
      capright.setSensitivity(newSens);
      Serial.print("Sensitivity set to: ");
      Serial.println(newSens);
    }
  }

  static int detectionCounterLeft = 0;
  static int detectionCounterRight = 0;
  bool handDetectedLeft = false;
  bool handDetectedRight = false;
  bool handDetected = false;

  int tossItera = 0;

  if (capright.isHandDetected(handThreshold)) {
    detectionCounterRight++;
    if (detectionCounterRight >= debounceCount) {
        handDetectedRight = true;
    }
} else {
    detectionCounterRight = 0; // Reset counter if right hand not detected
    handDetectedRight = false; // Reset detection state
}

if (capleft.isHandDetected(handThreshold)) {
    detectionCounterLeft++;
    if (detectionCounterLeft >= debounceCount) {
        handDetectedLeft = true;
    }
} else {
    detectionCounterLeft = 0; // Reset counter if left hand not detected
    handDetectedLeft = false; // Reset detection state
}

if (handDetectedRight && handDetectedLeft) {
    //animation.showTwoArrows();
    //delay(500);
    handDetected = true;
      if (esc.connect()){
        display.setMode(Max7219Animation::PROCESSING);
        display.setFrameDelay(100);
      lastButtonPressTime = millis();
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
} else if (handDetectedRight) {
    display.showRightArrow();
    handDetected = false;
} else if (handDetectedLeft) {
    display.showLeftArrow();
    handDetected = false;
} else {
    display.setMode(Max7219Animation::IDLE, 200);
    handDetected = false;
}


  if (handDetected) {
    handDetected = false;

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

    esc.feed(1);
    // Printing the reversed array
    //esc.println("The array after reversing it:");
    for (int i = 0; i < length; i++) {
      printYingYang(tossResult[i]);
    }
    esc.feed(1);

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
      Serial.println("Short hexagram description: " + descriptions.longSimplified);
      esc.align(ALIGN_CENTER);
      esc.println("Experience");
      esc.println (descriptions.longSimplified); //Stable hexagrams use a longer text, so two lines (36 symbols max per line)
      // Serial.println("Long hexagram description:  " + descriptions.longSimplified);
    } else {
      Serial.println(descriptions.shortSimplified + " transforming into " + descriptions.shortChanged);
      // Serial.println("long hexagram description: " + descriptions.longSimplified);
      // Serial.println("Long changed description:  " + descriptions.longChanged);
      esc.align(ALIGN_CENTER);
      esc.effectBold();
      esc.effectDoubleWidth();
      //esc.effectDoubleHeight();
      esc.println(descriptions.shortSimplified);
      esc.effectOff();
      esc.println(" may lead to ");
      esc.effectBold();
      esc.effectDoubleWidth();
      esc.println(descriptions.shortChanged);
      esc.effectOff();
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
    esc.feed(7);

    delay(3000);                // Wait for 3 seconds to debounce the button
    

    //cutcutcut
    servo.setTarget(145, 69);
    delay(1000);
    //return cutter
    servo.setTarget(45, 42);
  }
}
