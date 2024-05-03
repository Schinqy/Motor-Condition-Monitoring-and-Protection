void displayMenu() {
  lcd.clear();

  // Print the current menu item and parameter value
  lcd.setCursor(0, 0);
  lcd.print(menuItems[selectedMenuItem]);
  int cursorValue = menuItems[selectedMenuItem].length();
  lcd.setCursor(cursorValue, 0);
 // lcd.print("Value: ");
  lcd.print(": " + String(parameters[selectedMenuItem]));
 switch(selectedMenuItem)
 {
  case 0: lcd.print("A"); break;
  case 1: lcd.print("A"); break;
  case 2: lcd.print("V"); break;
  case 3: lcd.print("A"); break;
  case 4: lcd.print("V"); break;
  case 5: lcd.print("Hz"); break;
  case 6: lcd.print((char)223); // Print the degree symbol
          lcd.print("C"); // Print "C" for Celsius 
          break;
  case 7: lcd.print("g"); break;

      

 }

  lcd.setCursor(0, 2);
  lcd.print("Adjust with UP/DOWN");
  lcd.setCursor(0, 3);
  lcd.print("OK to next ");
 
}
void menuControl() {
  upButton.update();
  downButton.update();
  okButton.update();

  // Handle button presses
  if (upButton.fell()) {
    if (selectedMenuItem == NUM_MENU_ITEMS - 1) {
      parameters[selectedMenuItem] += 0.1; // Increase parameter value by 0.1 for the last menu item
    } else {
      parameters[selectedMenuItem] += 1.0; // Increase parameter value by 1 for other menu items
    }
    displayMenu(); // Update display
  }

  if (downButton.fell()) {
    if (selectedMenuItem == NUM_MENU_ITEMS - 1) {
      parameters[selectedMenuItem] -= 0.1; // Decrease parameter value by 0.1 for the last menu item
    } else {
      parameters[selectedMenuItem] -= 1.0; // Decrease parameter value by 1 for other menu items
    }
    displayMenu(); // Update display
  }

  if (okButton.fell()) {
    selectedMenuItem = selectedMenuItem + 1; // Move to the next parameter
    displayMenu(); // Update display
  }
}
