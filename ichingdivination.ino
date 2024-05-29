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

// Define a struct to hold descriptions
struct Descriptions {
  String simplifiedDescription;
  String changedDescription;
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

String constructUrl(int tossResult[], String simplifiedDescription) {
  // Convert tossResult array to a single string
  String hexagram = "";
  for (int i = 0; i < 6; i++) {
    hexagram += String(tossResult[i]);
  }

  // Construct the URL with the hexagram and simplifiedDescription
  String url = "http://wengu.tartarie.com/wg/wengu.php?l=Yijing&tire=" + hexagram + "&no=" + simplifiedDescription + "&lang=en";

  return url;
}

Descriptions findMatches(int arr[]) {

  // Define arrays to store match arrays and their corresponding descriptions
  const int yarrowArrays[][6] = {
    { 7, 7, 7, 7, 7, 7 },
    { 7, 7, 7, 7, 7, 8 },
    { 7, 7, 7, 7, 8, 7 },
    { 7, 7, 7, 7, 8, 8 },
    { 7, 7, 7, 8, 7, 7 },
    { 7, 7, 7, 8, 7, 8 },
    { 7, 7, 7, 8, 8, 7 },
    { 7, 7, 7, 8, 8, 8 },
    { 7, 7, 8, 7, 7, 7 },
    { 7, 7, 8, 7, 7, 8 },
    { 7, 7, 8, 7, 8, 7 },
    { 7, 7, 8, 7, 8, 8 },
    { 7, 7, 8, 8, 7, 7 },
    { 7, 7, 8, 8, 7, 8 },
    { 7, 7, 8, 8, 8, 7 },
    { 7, 7, 8, 8, 8, 8 },
    { 7, 8, 7, 7, 7, 7 },
    { 7, 8, 7, 7, 7, 8 },
    { 7, 8, 7, 7, 8, 7 },
    { 7, 8, 7, 7, 8, 8 },
    { 7, 8, 7, 8, 7, 7 },
    { 7, 8, 7, 8, 7, 8 },
    { 7, 8, 7, 8, 8, 7 },
    { 7, 8, 7, 8, 8, 8 },
    { 7, 8, 8, 7, 7, 7 },
    { 7, 8, 8, 7, 7, 8 },
    { 7, 8, 8, 7, 8, 7 },
    { 7, 8, 8, 7, 8, 8 },
    { 7, 8, 8, 8, 7, 7 },
    { 7, 8, 8, 8, 7, 8 },
    { 7, 8, 8, 8, 8, 7 },
    { 7, 8, 8, 8, 8, 8 },
    { 8, 7, 7, 7, 7, 7 },
    { 8, 7, 7, 7, 7, 8 },
    { 8, 7, 7, 7, 8, 7 },
    { 8, 7, 7, 7, 8, 8 },
    { 8, 7, 7, 8, 7, 7 },
    { 8, 7, 7, 8, 7, 8 },
    { 8, 7, 7, 8, 8, 7 },
    { 8, 7, 7, 8, 8, 8 },
    { 8, 7, 8, 7, 7, 7 },
    { 8, 7, 8, 7, 7, 8 },
    { 8, 7, 8, 7, 8, 7 },
    { 8, 7, 8, 7, 8, 8 },
    { 8, 7, 8, 8, 7, 7 },
    { 8, 7, 8, 8, 7, 8 },
    { 8, 7, 8, 8, 8, 7 },
    { 8, 7, 8, 8, 8, 8 },
    { 8, 8, 7, 7, 7, 7 },
    { 8, 8, 7, 7, 7, 8 },
    { 8, 8, 7, 7, 8, 7 },
    { 8, 8, 7, 7, 8, 8 },
    { 8, 8, 7, 8, 7, 7 },
    { 8, 8, 7, 8, 7, 8 },
    { 8, 8, 7, 8, 8, 7 },
    { 8, 8, 7, 8, 8, 8 },
    { 8, 8, 8, 7, 7, 7 },
    { 8, 8, 8, 7, 7, 8 },
    { 8, 8, 8, 7, 8, 7 },
    { 8, 8, 8, 7, 8, 8 },
    { 8, 8, 8, 8, 7, 7 },
    { 8, 8, 8, 8, 7, 8 },
    { 8, 8, 8, 8, 8, 7 },
    { 8, 8, 8, 8, 8, 8 }
  };

  const String matchDescriptions[] = {
    "1",
    "43",
    "14",
    "34",
    "9",
    "5",
    "26",
    "11",
    "10",
    "58",
    "38",
    "54",
    "61",
    "60",
    "41",
    "19",
    "13",
    "49",
    "30",
    "55",
    "37",
    "63",
    "22",
    "36",
    "25",
    "17",
    "21",
    "51",
    "42",
    "3",
    "27",
    "24",
    "44",
    "28",
    "50",
    "32",
    "57",
    "48",
    "18",
    "46",
    "6",
    "47",
    "64",
    "40",
    "59",
    "29",
    "4",
    "7",
    "33",
    "31",
    "56",
    "62",
    "53",
    "39",
    "52",
    "15",
    "12",
    "45",
    "35",
    "16",
    "20",
    "8",
    "23",
    "2"
  };


  String simplifiedDescription;
  String changedDescription;
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
      simplifiedDescription = matchDescriptions[i];
    }

    if (changedMatch) {
      changedDescription = matchDescriptions[i];
    }
  }

  if (!contains6or9) {
    changedDescription = simplifiedDescription;
  }

  return { simplifiedDescription, changedDescription };
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

    Descriptions descriptions = findMatches(tossResult);

    // Debugging output to check descriptions
    Serial.print("descriptions.simplifiedDescription value is: ");
    Serial.print(descriptions.simplifiedDescription);
    Serial.print("an descriptions.changedDescription value is: ");
    Serial.println(descriptions.changedDescription);

    // Print descriptions
    if (descriptions.changedDescription == descriptions.simplifiedDescription) {
      Serial.println("Simplified Array: " + descriptions.simplifiedDescription);
    } else {
      Serial.println("Simplified Array: " + descriptions.simplifiedDescription + ", changing to: " + descriptions.changedDescription);
    }
    Serial.println("Printed description lines");
    esc.feed(1);

    // print url qr code
    String url = constructUrl(tossResult, descriptions.simplifiedDescription);
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
