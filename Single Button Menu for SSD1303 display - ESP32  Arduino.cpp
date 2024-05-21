/****************************************************************
*
* SIMPLE MENU with SUBMENUS using a SINGLE button
*
* SINGLE click:   Go down in MENU or SUBMENU items
* LONG click:     Enter SUBMENU or RUN Command when inside SUBMENU
* DOUBLE click:   when inside SUBMENU, goes back to MAIN MENU
*
* Go to the end of the code to define what each
* SUBMENU item does after a LONG button press.
*
* Functions to be executed are defined at the end of the sketch
*
****************************************************************/

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Button2.h"


#define BUTTON_PIN  13  // This is the pin that the physical button is attached to

Button2 button;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);



int MainMenu    = 0;
int SubMenu     = 0;
int SubMenuItem = 0;


String HomeMenu[]  = {
  "WiFi",
  "RF",
  "IR",
  "RFID",
  "NFC",      // this one doesn't have submenus so it'll show empty
  "Sleep"     // this one doesn't have submenus so it'll show empty
};

// This is the amount of items in the MAIN MENU
int const TotalHomeItems = sizeof(HomeMenu) / sizeof(HomeMenu[0]);


int const MaxSubMenuItems = 6;    // <----- needs to be hardcoded (maximum amount of submenu items possible)


String SubItems[TotalHomeItems][MaxSubMenuItems] = {

  /*********************************************************************
  * Unused items must be filled with "NULL" AT THE END of the array
  * the count of items must match MaxSubMenuItems (variable set above)
  * 
  * The array NEEDS to start with 1, not 0 so we know when the sub-menu
  * is requested, because 0 means "no sub-menu to show"
  *********************************************************************/

  {"1", "Connect", "List", "Scan", "Jammer", "NULL"},   // items for submenu 1 (WiFi)
  {"2", "Favorite", "List", "Clone", "BLAST!", "Wow!"},   // items for submenu 2 (RF)
  {"3", "Favorite", "List", "Clone", "BLAST!", "NULL"},   // items for submenu 3 (IR)
  {"4", "Turn Off", "NULL", "NULL", "NULL", "NULL"}   // items for submenu 4 (Sleep)

};

int SubMenuCount  = 0;    // set this to zero until we actually request a sub-menu


void ShowMenu (int Home, int Sub, int Item) {

  Serial.println("Request to ShowMenu: " + String(Home) + ", " + String(Sub) + ", " + String(Item) );

  display.clearDisplay();
	display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.setTextSize(2);

  if (Sub == 0 && Item == 0) {    // we are navigating in the MAIN MENU

    for (byte idx = Home; idx < TotalHomeItems; idx++) {
      if ( MainMenu == idx) {
        display.print("> ");
        Serial.print("> ");
      }

      display.println(HomeMenu[idx]);
      Serial.println(HomeMenu[idx]);
      //////// print on screen //////////
    }

  }



/**************************************
* Check if a SUB MENU was requested
**************************************/

  if ( Sub > 0 ) {    // We entered a SUB MENU

    for(int i=Sub; i < MaxSubMenuItems; i++)  {

      if (SubItems[Home][i] != "NULL") {
        if ( Sub == i ) {
          Serial.print("> ");
          display.print("> ");
        }
        Serial.println(SubItems[Home][i]);
        display.println(SubItems[Home][i]);
      }

    }

  }

  /***********************************
  * Display menu on the screen
  * Either a MAIN MENU or a SUB MENU
  ***********************************/
  display.display();
}





void setup()
{
  Serial.begin(115200);
  delay(50);

  button.begin(BUTTON_PIN);

  button.setClickHandler(click);
  button.setLongClickDetectedHandler(longClick);
  button.setDoubleClickHandler(DoubleClick);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  // Start by showing the MAIN MENU
  ShowMenu(0, 0, 0);

}






void DoubleClick(Button2& btn) {

//  Serial.println("DOUBLE click");
  if ( SubMenu > 0 ) {
      Serial.println("Go back HOME! ");
      SubMenu   = 0;
      MainMenu  = 0;
      ShowMenu(0, 0, 0);
  }

}


void CountSubMenuItems() {
    SubMenuCount = 0;

    for(int i=1; i < MaxSubMenuItems; i++)  {
      if (SubItems[MainMenu][i] != "NULL") {
        SubMenuCount++;
      }
    }

    Serial.println("debug: submenu > 0");

    if ( SubMenu <= SubMenuCount ) {
      SubMenu++;
    } 
    if ( SubMenu > SubMenuCount ) {
      SubMenu = 1;
    }
}



void loop()
{
  button.loop();
}


void click(Button2& btn) {

  if ( SubMenu == 0 ) {
    if ( MainMenu < TotalHomeItems ) {
      MainMenu++;
    } 
    if ( MainMenu == TotalHomeItems ) {
      MainMenu = 0;
    }
  }

  if ( SubMenu > 0 ) {
    CountSubMenuItems();
  }

  ShowMenu (MainMenu, SubMenu, SubMenuItem);

}

void longClick(Button2& btn) {

	/***************************************************
	*
	* Here's where you define what a long press will do
	* when inside a sub-menu. It's the "final action".
	*
	***************************************************/


  // Functions for WiFi Menu (main menu 1)
  if ( MainMenu == 0 ) {
    if ( SubMenu == 1 ) {
        Serial.println("LONG PRESS - Connect to WiFi network" );	// action for WiFi > Connect
    }
    if ( SubMenu == 2 ) {
      Serial.println("LONG PRESS - Show saved networks" );			// action for WiFi > List
    }
    if ( SubMenu == 3 ) {
      Serial.println("LONG PRESS - Do a simple scan" );				// action for WiFi > Scan
    }
    if ( SubMenu == 4 ) {
      Serial.println("LONG PRESS - Run WiFi JAMMER!");				// action for WiFi > Jammer
    }
  }

  // Functions for RF Menu (main menu 2)
  if ( MainMenu == 1 ) {
    if ( SubMenu == 1 ) {
        Serial.println("LONG PRESS - Send code for favorite RF device" );	// action for RF > Favorite
    }
    if ( SubMenu == 2 ) {
	/********************************************************************************
	This is an example. You could call a function or do anything else here.
	Example action for RF > List (will display a list of fake devices for 3 seconds)
	********************************************************************************/
      Serial.println("LONG PRESS - Show saved devices (code)" );
      display.clearDisplay();
	    display.setTextColor(WHITE);
      display.setCursor(0,0);
      display.setTextSize(1);
      display.println("Bedroom Fan\n");
      display.println("Car Lock\n");
      display.println("Main Room Shades\n");
      display.println("Living Room LED");
      display.display();
      delay(5000);
    }
    if ( SubMenu == 3 ) {
      Serial.println("LONG PRESS - Clone a new RF device" );			// action
    }
    if ( SubMenu == 4 ) {
      Serial.println("LONG PRESS - BLAST RF signals");					// action
    }
  }

  // Functions for IR Menu (main menu 3)
  if ( MainMenu == 2 ) {
    if ( SubMenu == 1 ) {
        Serial.println("LONG PRESS - Send code for favorite IR device" );	// action
    }
    if ( SubMenu == 2 ) {
      Serial.println("LONG PRESS - Show saved devices (code)" );			// action
    }
    if ( SubMenu == 3 ) {
      Serial.println("LONG PRESS - Clone a new IR device" );				// action
    }
    if ( SubMenu == 4 ) {
      Serial.println("LONG PRESS - BLAST IR signals (TV-B-Gone)");			// action
    }
  }

  // Functions for RFID Menu (main menu 4)
  if ( MainMenu == 3 ) {
    Serial.println("LONG PRESS - RFID");							// action
  }


  if ( SubMenu > 0 ) {
      ShowMenu(MainMenu, SubMenu, 0);
  }

  if ( SubMenu == 0 ) {
    ShowMenu(MainMenu, 1, 0);
    SubMenu = 1;
  }

}

/* Aresnations?! */