const float NORMAL_MIN_TEMPERATURE = 25.0;
const float NORMAL_MAX_TEMPERATURE = 50.0;
const float MIN_TEMPERATURE = 30.0;
const float MAX_TEMPERATURE = 80.0;
const float NORMAL_MIN_CURRENT = 1.0;
const float NORMAL_MAX_CURRENT = 3.0;
const float MIN_CURRENT = 0.0;
const float MAX_CURRENT = 5.0;
const float NORMAL_MIN_VOLTAGE = 220.0;
const float NORMAL_MAX_VOLTAGE = 250.0;
const float MIN_VOLTAGE = 200.0;
const float MAX_VOLTAGE = 300.0;
const float NORMAL_MIN_ACCELERATION = 0.1; //measured in g(s)
const float NORMAL_MAX_ACCELERATION = 2.0; 
const float MIN_ACCELERATION = 0.0;
const float MAX_ACCELERATION = 5.0;

const float WEIGHT_TEMPERATURE = 3.0;
const float WEIGHT_CURRENT = 1.0;
const float WEIGHT_VOLTAGE = 2.0;
const float WEIGHT_ACCELERATION = 4.0;

// Function to map a value to a scaled score
float mapToScaledScore(float value, float minVal, float normalMin, float normalMax, float maxVal, float weight) {
//mapToScaledScore(temperatureReading, MIN_TEMPERATURE, NORMAL_MIN_TEMPERATURE, NORMAL_MAX_TEMPERATURE, MAX_TEMPERATURE, WEIGHT_TEMPERATURE);
    if (value < minVal) {
        return 0.0; // Below acceptable range
    } else if (value > maxVal) {
        return 0; // Above acceptable range
    } else if (value < normalMin) {
        float percentage = (value - minVal) / (normalMin - minVal) * 100;
        //Serial.println("percentage: " + percentage);
        float scalePercentage = map(percentage, 0, 100, 0, 69);
        //Serial.println("Scaled below normal percentage: " + String(scalePercentage));
        return scalePercentage * weight / 10; // Below normal range
    } else if (value > normalMax) {
        float percentage = (value - normalMax) / (maxVal - normalMax) * 100;
        float scalePercentage = map(percentage, 0, 100, 0, 50);
         //Serial.println("Scaled above normal percentage: " + String(scalePercentage));
        return scalePercentage * weight / 10; // Above normal range
    } else if (value >= normalMin && value <= normalMax ){
        float percentage = (value - normalMin) / (normalMax - normalMin) * 100;
        float scalePercentage = map(percentage, 0, 100, 70, 100);
        //Serial.println("normal percentage: " + String(percentage));
           // Serial.println("Scaled normal percentage: " + String(scalePercentage));
        return scalePercentage * weight / 10; // Within normal range
    }
}

float calculateOverallCondition(float temperatureReading, float currentReading, float voltageReading, float accelerationReading) {
      
    float temperatureScore = mapToScaledScore(temperatureReading, MIN_TEMPERATURE, NORMAL_MIN_TEMPERATURE, NORMAL_MAX_TEMPERATURE, MAX_TEMPERATURE, WEIGHT_TEMPERATURE);
    float currentScore = mapToScaledScore(currentReading, MIN_CURRENT, NORMAL_MIN_CURRENT, NORMAL_MAX_CURRENT, MAX_CURRENT, WEIGHT_CURRENT);
    float voltageScore = mapToScaledScore(voltageReading, MIN_VOLTAGE, NORMAL_MIN_VOLTAGE, NORMAL_MAX_VOLTAGE, MAX_VOLTAGE, WEIGHT_VOLTAGE);
    float accelerationScore = mapToScaledScore(accelerationReading, MIN_ACCELERATION, NORMAL_MIN_ACCELERATION, NORMAL_MAX_ACCELERATION, MAX_ACCELERATION, WEIGHT_ACCELERATION);
    float overallScore = temperatureScore + currentScore + voltageScore + accelerationScore;
    
    return overallScore; 
}

String getCondition(float overallCondition) {
  if (overallCondition >= 0 && overallCondition <= 20) {
    return "Very Bad"; 
  } else if (overallCondition > 20 && overallCondition <= 40) {
    return "Bad";
  } else if (overallCondition > 40 && overallCondition <= 60 ) {
    return "Good";
  } else if (overallCondition > 60 && overallCondition <= 80 ) {
    return "Very Good";
  } else if (overallCondition > 80 && overallCondition <= 100) {
    return "Excellent";
  }
}
