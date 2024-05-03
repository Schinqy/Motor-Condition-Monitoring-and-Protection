

void sendCommand(const String& command, int expectedOrder) {
  if (orderNum == expectedOrder && millis() - lastCommandTime >= commandDelay) {
    sim800Serial.println(command);
    Serial.println(command);
    if (sim800Serial.available()) {
      String response = sim800Serial.readString();
      Serial.println("GSM say:" + response);
    }
    
    lastCommandTime = millis();
    orderNum++;
    if (orderNum > 3) {
      orderNum = 0;
    }
  }
}

void sendSMS(const String& phoneNumber, const String& message) {
  sendCommand("AT+CMGF=1", 0); // Set SMS mode to text
  sendCommand("AT+CMGS=\"" + phoneNumber + "\"", 1); // No delay after AT+CMGS
  sendCommand(message, 2);
  sendCommand(String((char)26), 3); // Ctrl+Z to send the message
}


void printConditionAndSendSMS(float overallCondition) {
  if (overallCondition >= 0 && overallCondition <= 20 && !veryBadPrinted) {
    Serial.println("Condition: Very Bad");
    sendSMS(PHONE_NUMBER, "Condition: Very Bad");
    veryBadPrinted = true;
  } else if (overallCondition > 20 && overallCondition <= 40 && !badPrinted) {
    Serial.println("Condition: Bad");
    sendSMS(PHONE_NUMBER, "Condition: Bad");
    badPrinted = true;
  } else if (overallCondition > 40 && overallCondition <= 60 && !goodPrinted) {
    Serial.println("Condition: Good");
    sendSMS(PHONE_NUMBER, "Condition: Good");
    goodPrinted = true;
  } else if (overallCondition > 60 && overallCondition <= 80 && !veryGoodPrinted) {
    Serial.println("Condition: Very Good");
    sendSMS(PHONE_NUMBER, "Condition: Very Good");
    veryGoodPrinted = true;
  } else if (overallCondition > 80 && overallCondition <= 100 && !excellentPrinted) {
    Serial.println("Condition: Excellent");
    sendSMS(PHONE_NUMBER, "Condition: Excellent");
    excellentPrinted = true;
  }
}



void sendSMSEveryXMilliseconds(int XMilliseconds) {
  unsigned long currentMillis = millis(); // Get the current time
  
  // Check if X milliseconds have passed since the last SMS was sent
  if (currentMillis - prevMillis >= XMilliseconds) {
    // Update the previousMillis variable to the current time
    prevMillis = currentMillis;
    
    // Generate the SMS string
    String SMString = "Ir:" + String(currentR, 2) + "|Ib:" + String(currentB,2) + "|Iy:" + String(currentY, 2) + "|Vr:" + String(voltageR,2)
                  + "|Vb:" + String(voltageB, 2) + "|Vy:" + String(voltageY,2) + "|Acc:" + String(accelerationReading,2) + "|Temp:" 
                  + String(temperatureC,2) + "|Score:" + String(overallCondition, 2) + "-" + getCondition(overallCondition);
                  
    // Send the SMS
    sendSMS(PHONE_NUMBER, SMString);
  }
}



//*********************************Send  SMS with blocking delay*************************

void sendCommandx(const String& command)
{
  sim800Serial.println(command);
  delay(1000);

  if (sim800Serial.available())
  {
    String response = sim800Serial.readString();
    Serial.println(response);
  }
}
 

void sendSMSx(const String& phoneNumber, const String& message)
{
  sendCommandx("AT+CMGF=1"); // Set SMS mode to text
  delay(1000);

  sendCommandx("AT+CMGS=\"" + phoneNumber + "\"");
  delay(1000);

  sim800Serial.print(message);
  delay(100);

  sim800Serial.write(26); // Ctrl+Z to send the message
  delay(1000);

  if (sim800Serial.available())
  {
    String response = sim800Serial.readString();
    Serial.println(response);
  }
}