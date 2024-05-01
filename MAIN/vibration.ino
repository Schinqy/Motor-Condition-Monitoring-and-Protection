

// Function to calculate RMS value of an array of float values
float calculateRMS(float data[], int numSamples) {
  float sumSq = 0.0;
  for (int i = 0; i < numSamples; i++) {
    sumSq += pow(data[i], 2);
  }
  float rms = sqrt(sumSq / numSamples);
  return rms;
}

// Function to calculate baseline RMS values
void calculateBaselineRMS() {
  for (int i = 0; i < NUM_SAMPLES; i++) {
    sensors_event_t event;
    accel.getEvent(&event);
    baselineRMS_X += pow(event.acceleration.x, 2);
    baselineRMS_Y += pow(event.acceleration.y, 2);
    baselineRMS_Z += pow(event.acceleration.z, 2);
    delay(10); // Adjust delay based on sampling frequency
  }
  baselineRMS_X = sqrt(baselineRMS_X / NUM_SAMPLES);
  baselineRMS_Y = sqrt(baselineRMS_Y / NUM_SAMPLES);
  baselineRMS_Z = sqrt(baselineRMS_Z / NUM_SAMPLES);
}

// Function to determine if vibration change is significant
bool isVibrationChangeSignificant(float rmsX, float rmsY, float rmsZ) {
  float delta_X = abs(rmsX - baselineRMS_X);
  float delta_Y = abs(rmsY - baselineRMS_Y);
  float delta_Z = abs(rmsZ - baselineRMS_Z);
  
  return (delta_X > RMS_THRESHOLD || delta_Y > RMS_THRESHOLD || delta_Z > RMS_THRESHOLD);
}

float calculateOverallRMS(float accX, float accY, float accZ) {
    // Calculate the square of each component
    float sqX = accX * accX;
    float sqY = accY * accY;
    float sqZ = accZ * accZ;

    // Calculate the mean square
    float meanSq = (sqX + sqY + sqZ) / 3.0;

    // Return the square root of the mean square
    return sqrt(meanSq);
}

float calculateAccelerationChange(float accX, float accY, float accZ, float baselineRMS_X, float baselineRMS_Y, float baselineRMS_Z) {
    float currentRMS = calculateOverallRMS(accX, accY, accZ);
    float baselineRMS = calculateOverallRMS(baselineRMS_X, baselineRMS_Y, baselineRMS_Z);

    if (baselineRMS == 0) {
        // Avoid division by zero
        return 0.0;
    }

    float change = abs(currentRMS - baselineRMS);
    return change;
}
