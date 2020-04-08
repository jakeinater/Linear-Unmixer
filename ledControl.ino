int redPin = 11;
int bluePin = 10;
int greenPin = 9;

void setup() 
{ 
  pinMode(redPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(greenPin, OUTPUT);
}

void loop() 
{      
  analogWrite(redPin, 0); //ledVal is power out of 255
  analogWrite(greenPin, 70);
  analogWrite(bluePin, 255);
  delay(25);
}
