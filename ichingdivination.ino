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

void setup() {
  // initialize the pushbutton pin as an input:
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);


  Serial.begin(115200);

  while (!Serial)
    ;  // wait for serial port to connect.

  // You can change service, characteristic UUID for a compatible esc BLE printer
  char *service = "000018F0-0000-1000-8000-00805F9B34FB";
  char *characteristic = "00002AF1-0000-1000-8000-00805F9B34FB";
  esc = EscPos(service, characteristic);
  esc.start();

  if (esc.connect()) {
    Serial.println("Connected.");
  }
}

void printYingYang(int yarr) {
  Serial.println("Printing line corresponding to " + String(yarr));
  esc.align(ALIGN_CENTER);
  if (yarr == 6) {
    esc.println("---x---");
  } else if (yarr == 9) {
    esc.println("---o---");
  } else if (yarr == 7) {
    esc.println("-------");
  } else if (yarr == 8) {
    esc.println("--- ---");
  }
}

String findMatch(int arr[]) {
  int simplifiedArray[6];
  int changedArray[6];

  // Preprocess the input array according to the rules provided
  for (int i = 0; i < 6; i++) {
    if (inputArray[i] == 6) {
      simplifiedArray[i] = 8;
      changedArray[i] = 8;  // Change 6 to 8 in changedArray
    } else if (inputArray[i] == 9) {
      simplifiedArray[i] = 7;
      changedArray[i] = 9;  // Change 9 to 7 in changedArray
    } else {
      simplifiedArray[i] = inputArray[i];
      changedArray[i] = inputArray[i];
    }
  }


  // Define arrays to store match arrays and their corresponding descriptions
  const int allArrays[][6] = {
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


  const String descriptions[] = {
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

  // Get the length of the arrays
  int numArrays = sizeof(matchArrays) / sizeof(matchArrays[0]);

  // Compare the given array with each match array
  for (int i = 0; i < numArrays; i++) {
    bool match = true;
    for (int j = 0; j < 6; j++) {
      // Compare each element of the given array with corresponding elements in the match array
      if (arr[j] != matchArrays[i][j]) {
        match = false;
        break;  // Exit loop early if there is a mismatch
      }
    }

    // If a match is found, return the corresponding description
    if (match) {
      return descriptions[i];
    }
  }

  // If no match is found, return an empty string or some default value
  return "";
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

    Serial.println("The correct array sequence is " + int(tossResult));

    // Printing the reversed array
    //esc.println("The array after reversing it:");
    for (int i = 0; i < length; i++) {
      printYingYang(tossResult[i]);
    }



    Serial.println("Printed yingyang lines");
    esc.feed(2);


    delay(3000);                // Wait for 3 seconds to debounce the button
    digitalWrite(ledPin, LOW);  // Turn off LED
  }
}
