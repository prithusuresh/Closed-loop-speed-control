int irpin = 3 ;
int pwmpin = 9 ;
long currenttime, previoustime, settling = 0;
volatile int revs = 0;
long  maxrpm, rpm, prpm, constrpm = 0;
long n = 3;
int Q = 255;
float cref = 500;
float pref, delQ = 0;
int stability, stabilityfactor = 0;
int mrpm[50] = {};
int crpm[50] = {};
int R = 100;
float constant = 1.62;


int modecalc( int list[], int siz) { //Mode Function to obtain maximum RpM of the motor
  int mode = list[0];
  int modecount = 1;
  for (int pos = 0 ; pos < siz ; pos++) {
    int current = 1;
    for ( int j = pos + 1; j < siz ; j++) {
      if (list[pos] == list[j]) {
        current += 1;
      }
    }
    if (current > modecount) {
      modecount = current;
      mode = list[pos];
      //Serial.println(mode);
    }
    if (modecount > siz - pos) {
      break;
    }
  }
  return mode;
}


void setup() {
  // put your setup code here, to run once:
  pinMode(irpin, INPUT);
  pinMode(pwmpin, OUTPUT);
  Serial.begin(9600);
  attachInterrupt(1, calculate, FALLING);

  analogWrite(pwmpin, 255);
  delay(3000);
  int i = 0;
  while (i < 50) {
    if    (revs >= n) {
      revs = 0;
      currenttime = millis();
      long t = currenttime - previoustime;
      maxrpm = ((60000 * n) / t);
      mrpm[i] = maxrpm;
      i += 1;
    }
  }
  maxrpm = modecalc(mrpm, 50);
  Serial.print("maxrpm = ");
  Serial.println(maxrpm);



  Serial.print("Constant = ");
  Serial.println(constant);
  Serial.println( " ");
  Serial.println(" Enter reference rpm: ");   //RPM at which the motor is to spin
  while (Serial.available() == 0) {
  }

}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void loop() {

  if (Serial.available()) {
    pref = cref;
    cref = Serial.parseInt();
    settling = millis();
    stability = 0;

    if (cref < (0.6 * maxrpm)) {
      n = 1;
    }
    else if (cref < (0.8 * maxrpm)) {   //Variable revolution count to deal with Spikes
      n = 2;
    }
    else if (cref < (maxrpm) ) {
      n = 3;
    }
    stabilityfactor = (cref / (n * 60));

    previoustime = millis();
    revs = 0  ;
  }

  analogWrite(pwmpin, Q);

  if    (revs >= n) {
    revs = 0;
    currenttime = millis();                       //RPM calculating block
    long t = currenttime - previoustime;
    rpm = ((60000 * n) / t);
    previoustime = currenttime;



    if (stability >= stabilityfactor) {
      while  (Serial.available() == 0) {
        if    (revs >= 1) {
          revs = 0;
          currenttime = millis();
          long t = currenttime - previoustime;
          rpm = ((60000 * 1) / t);
          previoustime = currenttime;
          if ( abs(cref - rpm) > 20 && (rpm > cref)) {
            Serial.print(cref);
            Serial.print(" ");
            Serial.print(rpm);
            Serial.print(" ");
            Serial.print(millis() - settling);
            Serial.println(" ");
          }
          if (abs(cref - rpm) < 4) {
            Serial.print(cref);
            Serial.print(" ");
            Serial.print(rpm);
            Serial.print(" ");
            Serial.print(millis() - settling);
            Serial.println(" ");
          }

          if (abs(cref - rpm) > 20 && (cref > rpm) && (rpm >= 0)) {
            break;
          }

        }
      }
    }


    if (abs(cref - rpm) > 4) {

      if ((rpm <= maxrpm) && (rpm > 0)) {
        stability = 0;
        float error = ((cref   - rpm));
        delQ = ((error * 255) / (maxrpm * (constant)));

        Q += delQ;
        Q = constrain(Q, 0, 255);
      
        Serial.print(cref);
        Serial.print(" ");
        Serial.print(rpm);
        Serial.print(" ");
        Serial.print(millis() - settling);
        Serial.println(" ");


        prpm = rpm;
      }
    }

    else {
      stability += 1;

      Serial.print(cref);
      Serial.print(" ");
      Serial.print(rpm);
      Serial.print(" ");
      Serial.print(millis() - settling);
      Serial.println(" ");

      prpm = cref;
    }
  }
}

void calculate() {
  revs += 1;
}
