#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Define OLED display size
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Define OLED reset pin
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Define button pins based on the schematic
#define BUTTON_UP 14    // GPIO14
#define BUTTON_DOWN 12  // GPIO12
#define BUTTON_OK 13    // GPIO13

// Menu variables
int menuIndex = 0;
const char* menuItems[] = {"Scan", "Show", "Deauth"};
int menuItemsCount = sizeof(menuItems) / sizeof(menuItems[0]);

enum MenuState { MAIN_MENU, SCAN_MENU, SHOW_MENU, DEAUTH_MENU };
MenuState currentState = MAIN_MENU;

// WiFi variables
String selectedSSID = "";
String selectedBSSID = "";

// Function to scan networks
void scanNetworks() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Scanning...");
  display.display();

  int n = WiFi.scanNetworks();
  if (n == 0) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("No networks found");
    display.display();
    delay(2000);
  } else {
    for (int i = 0; i < n; ++i) {
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print(i + 1);
      display.print(": ");
      display.print(WiFi.SSID(i));
      display.display();
      delay(2000);

      // Save selected network details
      selectedSSID = WiFi.SSID(i);
      selectedBSSID = WiFi.BSSIDstr(i);
    }
  }
}

// Function to show selected network
void showNetwork() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Selected Network:");
  display.setCursor(0, 20);
  display.print(selectedSSID);
  display.display();
  delay(5000);
}

// Function to deauthenticate selected network
void deauthNetwork() {
  if (selectedSSID == "") {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("No network selected");
    display.display();
    delay(2000);
    return;
  }

  // Note: The ESP32 doesn't have native deauther support like ESP8266.
  // This is a placeholder for deauthentication functionality.
  // You'll need to implement or include an appropriate library or code for deauth.

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Deauthenticating:");
  display.setCursor(0, 20);
  display.print(selectedSSID);
  display.display();
  delay(5000);
}

// Function to display the main menu
void displayMainMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Menu:");
  for (int i = 0; i < menuItemsCount; i++) {
    display.setCursor(0, (i + 1) * 10);
    if (i == menuIndex) {
      display.print(">");
    }
    display.print(menuItems[i]);
  }
  display.display();
}

// Function to display a back option in submenus
void displayBackOption(const char* title) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print(title);
  display.setCursor(0, 10);
  display.print("> Back");
  display.display();
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.display();
  delay(2000);
  display.clearDisplay();

  // Initialize buttons
  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
  pinMode(BUTTON_OK, INPUT_PULLUP);

  // Display initial menu
  displayMainMenu();
}

void loop() {
  if (digitalRead(BUTTON_UP) == LOW) {
    if (currentState == MAIN_MENU) {
      menuIndex = (menuIndex - 1 + menuItemsCount) % menuItemsCount;
      displayMainMenu();
    }
    delay(200);
  }

  if (digitalRead(BUTTON_DOWN) == LOW) {
    if (currentState == MAIN_MENU) {
      menuIndex = (menuIndex + 1) % menuItemsCount;
      displayMainMenu();
    }
    delay(200);
  }

  if (digitalRead(BUTTON_OK) == LOW) {
    switch (currentState) {
      case MAIN_MENU:
        switch (menuIndex) {
          case 0: // Scan
            currentState = SCAN_MENU;
            scanNetworks();
            displayBackOption("Scan");
            break;
          case 1: // Show
            currentState = SHOW_MENU;
            showNetwork();
            displayBackOption("Show");
            break;
          case 2: // Deauth
            currentState = DEAUTH_MENU;
            deauthNetwork();
            displayBackOption("Deauth");
            break;
        }
        break;
      case SCAN_MENU:
      case SHOW_MENU:
      case DEAUTH_MENU:
        currentState = MAIN_MENU;
        displayMainMenu();
        break;
    }
    delay(200);
  }
}
