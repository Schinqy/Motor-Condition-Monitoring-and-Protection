  static bool overcurrentSent = false;
  static bool overloadSent = false;
  static bool unbalancedVoltageSent = false;
  static bool unbalancedCurrentSent = false;
  static bool unbalancedCurrentPos2Sent = false;
  static bool unbalancedCurrentNeg2Sent = false;
  static bool overVoltageSent = false;
  static bool underVoltageSent = false;
  static bool singlePhasingSent = false;
  static bool overFrequencySent = false;
  static bool underFrequencySent = false;
  static bool highTemperatureSent = false;
  static bool excessiveVibrationSent = false;
  static bool phaseSequenceFailureSent = false;
  boolean breakerStatusSent = false;
  
void motorProtection() {


  // Check for overcurrent
  //trip instantly!!
  if (currentR > I50set && currentR < 1000 || currentY > I50set && currentY < 1000 || currentB > I50set && currentB < 1000) { //if cre
    turnOffMotor();
    if (!overcurrentSent) {
      Serial.println("Overcurrent Protection Operated");
      sendSMSx(PHONE_NUMBER, "Overcurrent Protection Operated");
      displayError("Overcurrent Protection Operated");
    
      overcurrentSent = true;
      errorFlag = 1;
    }
  }
//*********************^^^^^^^^^^^^^^^^^^^^^^^^^^*************************^^^^^^^^^^^^^^^^^^^^^^^
  //Check for overload
  if (calculateOverloadTime()) {
   //If there is an overload ,a trip signal will be sent after a set time
    turnOffMotor();
    if (!overloadSent) {
      Serial.println("Overload Protection Operated");
      sendSMSx(PHONE_NUMBER, "Overload Protection Operated");
     overloadSent = true;
     displayError("Overload Protection Operated");
     errorFlag = 1;
    }
  }


//*********************^^^^^^^^^^^^^^^^^^^^^^^^^^*************************^^^^^^^^^^^^^^^^^^^^^^^
  float averageVoltage = (voltageR + voltageY + voltageB)/3;
  // Check if unbalanced voltage is detected
  if (isUnbalancedVoltage(voltageR, voltageY, voltageB, averageVoltage)) {
    if (!unbalancedVoltageSent) {
      Serial.println("Unbalanced Supplies Protection Operated");
      sendSMSx(PHONE_NUMBER,"Unbalanced Supplies Protection Operated(Voltage)" );
      unbalancedVoltageSent = true;
      displayError("Unbalanced Supplies Protection Operated(Voltage)");
      errorFlag = 1;
    }
  }

//*********************^^^^^^^^^^^^^^^^^^^^^^^^^^*************************^^^^^^^^^^^^^^^^^^^^^^^
  //Check for unbalanced currents
  // send an alarm if inbalance is >5%
  // Check if unbalanced voltage is detected
  float averageCurrent = (currentR + currentY + currentB)/3 ;
  if (isUnbalancedCurrent(currentR, currentY, currentB, averageCurrent)) {
    if (!unbalancedCurrentSent) {
      Serial.println("Unbalanced Supplies Protection Operated(Current)");
      sendSMSx(PHONE_NUMBER,"Unbalanced Supplies Protection Operated(Current)");
      unbalancedCurrentSent = true;
      displayError("Unbalanced Supplies Protection Operated(Current)");
      errorFlag = 1;
    }
  }
//*********************^^^^^^^^^^^^^^^^^^^^^^^^^^*************************^^^^^^^^^^^^^^^^^^^^^^^
// Calculate the deviation of each current from the average current
float currentRDeviation = abs(currentR - averageCurrent);
float currentYDeviation = abs(currentY - averageCurrent);
float currentBDeviation = abs(currentB - averageCurrent);

// Calculate the maximum deviation among the three currents
float maxCurrentDeviation = max(max(currentRDeviation, currentYDeviation), currentBDeviation);

// Calculate the imbalance percentage
float imbalancePercentage = maxCurrentDeviation / averageCurrent;

// Trip if imbalance > 20%
if (imbalancePercentage > 0.2 && currentB <1000) {
    // Turn off the motor
    //turnOffMotor();
    if (!unbalancedCurrentPos2Sent) {
      Serial.println("Unbalanced Supplies Operated : Current");
      sendSMSx(PHONE_NUMBER, "Unbalanced Supplies Operated : +ve Current");
      unbalancedCurrentPos2Sent = true;
      displayError("Unbalanced Supplies Operated : +ve Current");
      errorFlag = 1;
    }
} else if (imbalancePercentage < -0.2) {
    // Turn off the motor
    turnOffMotor();
    if (!unbalancedCurrentNeg2Sent) {
      Serial.println("Unbalanced Supplies Operated : Current ");
      sendSMSx(PHONE_NUMBER,"Unbalanced Supplies Operated : -ve Current" );
      unbalancedCurrentNeg2Sent = true;
      displayError("Unbalanced Supplies Operated : -ve Current");
      errorFlag = 1;
    }
}
//*********************^^^^^^^^^^^^^^^^^^^^^^^^^^*************************^^^^^^^^^^^^^^^^^^^^^^^
// Check for overvoltage
if (voltageR > 1.1 * Vrated && voltageR < 1000 || voltageY > 1.1 * Vrated && voltageY < 1000|| voltageB > 1.1 * Vrated && voltageB < 1000) {
    // Delay before tripping 
    // delay(5000);
    // Turn off the motor
    turnOffMotor();
    if (!overVoltageSent) {
      Serial.println("Voltage Protection Operated : Over Voltage");
      sendSMSx(PHONE_NUMBER, "Voltage Protection Operated : Over Voltage");
      overVoltageSent= true;
      displayError("Voltage Protection Operated : Over Voltage");
      errorFlag = 1;
    }
}
//*********************^^^^^^^^^^^^^^^^^^^^^^^^^^*************************^^^^^^^^^^^^^^^^^^^^^^^
// Check for undervoltage
if (voltageR < 0.9 * Vrated || voltageY < 0.9 * Vrated || voltageB < 0.9* Vrated) {
    // Delay before tripping 
    // delay(5000);
    // Turn off the motor
    turnOffMotor();
    if (!underVoltageSent) {
      Serial.println("Voltage Protection Operated : Under Voltage ");
      sendSMSx(PHONE_NUMBER, "Voltage Protection Operated : Under Voltage");
      underVoltageSent = true;
       displayError("Voltage Protection Operated : Under Voltage");
      errorFlag = 1;
    }
}

//*********************^^^^^^^^^^^^^^^^^^^^^^^^^^*************************^^^^^^^^^^^^^^^^^^^^^^^
 // Check for single phasing
if ((currentR == 0 || currentB == 0 ||currentB == 0)) {
    // Turn off the motor (trip instantaneously)
     turnOffMotor();
    if (!singlePhasingSent) {
      Serial.println("Single Phasing Protection Operated");
      sendSMSx(PHONE_NUMBER, "Single Phasing Protection Operated");
      singlePhasingSent = true;
         displayError("Single Phasing Protection Operated");
      errorFlag = 1;
    }
}

//*********************^^^^^^^^^^^^^^^^^^^^^^^^^^*************************^^^^^^^^^^^^^^^^^^^^^^^
  // Check for Hz
  if (frequencyR > 1.1 * FrequencySet && frequencyR <1000){
    turnOffMotor();
    if (!overFrequencySent) {
      Serial.println("Frequency Protection Operated : Over Frequency");
      sendSMSx(PHONE_NUMBER,"Frequency Protection Operated : Over Frequency" );
      overFrequencySent = true;
         displayError("Frequency Protection Operated : Over Frequency");
      errorFlag = 1;
    }
  }

if (frequencyR < 0.9 * FrequencySet && frequencyR < 1000){
    turnOffMotor();
    if (!underFrequencySent) {
      Serial.println("Frequency Protection Operated : Under Frequency");
      sendSMSx(PHONE_NUMBER, "Frequency Protection Operated : Under Frequency");
      underFrequencySent= true;
       displayError("Frequency Protection Operated : Under Frequency");
      errorFlag = 1;
    }
  }

//*********************^^^^^^^^^^^^^^^^^^^^^^^^^^*************************^^^^^^^^^^^^^^^^^^^^^^^
  // Check for high temperature
  if (temperatureC > TemperatureSet) {
    turnOffMotor();
    if (!highTemperatureSent) {
      Serial.println("Temperature Protection Operated");
      sendSMSx(PHONE_NUMBER, "Temperature Protection Operated");
      highTemperatureSent = true;
      displayError("Temperature Protection Operated");
      errorFlag = 1;
    }
  }

//*********************^^^^^^^^^^^^^^^^^^^^^^^^^^*************************^^^^^^^^^^^^^^^^^^^^^^^
  // Check for excessive vibration
  
  if (isVibrationChangeSignificant(rmsX, rmsY, rmsZ)) {
    turnOffMotor();
    if (!excessiveVibrationSent) {
      Serial.println("Vibration Protection Operarated");
      sendSMSx(PHONE_NUMBER, "Vibration Protection Operated ");
      excessiveVibrationSent= true;
      displayError("Vibration Protection Operated");
      errorFlag = 1;
    }
  }

//*********************^^^^^^^^^^^^^^^^^^^^^^^^^^*************************^^^^^^^^^^^^^^^^^^^^^^^
  // Check for phase sequence failure
  if (PhaseFailureIndicatorVolatge > 100) {
    turnOffMotor();
    if (!phaseSequenceFailureSent) {
      Serial.println("Phase Sequence Protection Operated");
      sendSMSx(PHONE_NUMBER, "Phase Sequence Protection Operated");
      phaseSequenceFailureSent = true;
     
    }
  }

//*********************^^^^^^^^^^^^^^^^^^^^^^^^^^*************************^^^^^^^^^^^^^^^^^^^^^^^
  // Check for phase sequence failure
  if (BreakerStatusIndicatorVolatge > 100) {
    if (!breakerStatusSent) {
      Serial.println("BREAKER STATUS : CLOSE");
      sendSMSx(PHONE_NUMBER, "BREAKER STATUS : CLOSE");
      breakerStatusSent = true;
    }
  }
}

//*********************************************** CALCULATE CURRENT OVERLOAD TIME ****************************************************************


bool calculateOverloadTime() {
  unsigned long currentMillis = millis();
  unsigned long timeDifference = currentMillis - previousMillis;

  // Check for overload for currentR
  if (currentR > I51set) {
    if (overloadStartTimeR == 0) {
      overloadStartTimeR = currentMillis;
    }
    overloadTimeR += timeDifference;
  } else {
    overloadStartTimeR = 0;
    overloadTimeR = 0;
  }

  // Check for overload for currentY
  if (currentY > I51set) {
    if (overloadStartTimeY == 0) {
      overloadStartTimeY = currentMillis;
    }
    overloadTimeY += timeDifference;
  } else {
    overloadStartTimeY = 0;
    overloadTimeY = 0;
  }

  // Check for overload for currentB
  if (currentB > I51set) {
    if (overloadStartTimeB == 0) {
      overloadStartTimeB = currentMillis;
    }
    overloadTimeB += timeDifference;
  } else {
    overloadStartTimeB = 0;
    overloadTimeB = 0;
  }

  previousMillis = currentMillis;

  // Check if any of the overload times is greater than or equal to the desired duration
  if (overloadTimeR >= overloadThreshold || overloadTimeY >= overloadThreshold  || overloadTimeB >= overloadThreshold ) {
    return true; 
  }

  return false; 
}

//************************************************ VOLTAGE IMBALANCES *******************************
bool isUnbalancedVoltage(float voltageR, float voltageY, float voltageB, float averageVoltage) {
    static bool unbalanced = false; // Flag to track if the system is unbalanced
    static unsigned long unbalancedStartTime = 0; // Variable to store the start time of unbalanced voltage
    static unsigned long unbalancedDuration = 0; // Variable to store the total duration of unbalanced voltage
    const unsigned long unbalancedThreshold = 5000; // Time threshold for considering the system continuously unbalanced (in milliseconds)

    unsigned long currentTime = millis(); // Get the current time

    // Check if any phase voltage is outside the threshold of 1% from the average voltage
    if (abs(voltageR - averageVoltage) > 0.01 * averageVoltage ||
        abs(voltageY - averageVoltage) > 0.01 * averageVoltage ||
        abs(voltageB - averageVoltage) > 0.01 * averageVoltage) {
        
        // If the system is not already marked as unbalanced, mark it as unbalanced and set the start time
        if (!unbalanced) {
            unbalanced = true;
            unbalancedStartTime = currentTime;
        }

        // Accumulate the duration of unbalanced voltage
        unbalancedDuration += currentTime - unbalancedStartTime;

        // Check if the time threshold has been reached
        if (unbalancedDuration >= unbalancedThreshold) {
            return true; // System has been continuously unbalanced for the threshold time
        }
    } else {
        // Reset the flag, start time, and duration as the system is now balanced
        unbalanced = false;
        unbalancedStartTime = 0;
        unbalancedDuration = 0;
    }

    return false; // System is not continuously unbalanced
}

//*************************** CURRENT IMBALANCES ***************^^^^^^^^^^^^^^^^^**********

bool isUnbalancedCurrent(float currentR, float currentY, float currentB, float averageCurrent) {
    static bool unbalanced = false; // Flag to track if the system is unbalanced
    static unsigned long unbalancedStartTime = 0; // Variable to store the start time of unbalanced current
    static unsigned long unbalancedDuration = 0; // Variable to store the total duration of unbalanced current
    const unsigned long unbalancedThreshold = 5000; // Time threshold for considering the system continuously unbalanced (in milliseconds)

    unsigned long currentTime = millis(); // Get the current time

    // Check if any phase current is outside the threshold of 5% from the average current
    if (abs(currentR - averageCurrent) > 0.05 * averageCurrent ||
        abs(currentY - averageCurrent) > 0.05 * averageCurrent ||
        abs(currentB - averageCurrent) > 0.05 * averageCurrent) {
        
        // If the system is not already marked as unbalanced, mark it as unbalanced and set the start time
        if (!unbalanced) {
            unbalanced = true;
            unbalancedStartTime = currentTime;
        }

        // Accumulate the duration of unbalanced current
        unbalancedDuration += currentTime - unbalancedStartTime;

        // Check if the time threshold has been reached
        if (unbalancedDuration >= unbalancedThreshold) {
            return true; // System has been continuously unbalanced for the threshold time
        }
    } else {
        // Reset the flag, start time, and duration as the system is now balanced
        unbalanced = false;
        unbalancedStartTime = 0;
        unbalancedDuration = 0;
    }

    return false; // System is not continuously unbalanced
}
