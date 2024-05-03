void displayParameters()
{
  lcd.setCursor(0 , 0);
  lcd.print("                    ");
  lcd.setCursor(0 , 1);
  lcd.print("                    ");
  lcd.setCursor(0 , 2);
  lcd.print("                    ");
  lcd.setCursor(0 , 3);
  lcd.print("                    ");
  lcd.setCursor(0 , 0);
  lcd.print("Ir: " + String(currentR, 2) + "A");
  lcd.print(" Vr: "  + String(voltageR,2) + "V");
  lcd.setCursor(0 , 2);
  lcd.print("Ib: " + String(currentR, 2) + "A");
  lcd.print(" Vb: "  + String(voltageB,2) + "V");
  lcd.setCursor(0 , 1);
  lcd.print("Iy: " + String(currentY, 2) + "A");
  lcd.print(" Vy: "  + String(voltageY,2) + "V");
  lcd.setCursor(0 , 3);
  lcd.print(" " + String(temperatureC, 0));
  lcd.print((char)223); // Print the degree symbol
  lcd.print("C"); // Print "C" for Celsius 
  lcd.print(" " + String(accelerationReading,2) + "g");
  lcd.print(" CS:" + String(overallCondition, 0) + "%");                 
}


void displayError(String errorMessage)
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Warning: " + errorMessage);
  errorFlag = 1;
}