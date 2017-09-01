/* See: https://github.com/kasom/LoadCellZProbe */

#include "HX711-multi.h"

#define CHANNEL_COUNT 3

// clock pin to the load cell amp
#define HX711_CLK_PIN PA7
// data pins to the load cell amp
#define HX711_DATAL_PINS {PA0,PA1,PA2}

// Active low
#define LED_PIN PB12
// Active high
#define OUTPUT_PIN PB0

// Active low
#define VERBOSE_OUTPUT_ENABLE PA9
// Active low
#define NOISE_READ_ENABLE PA10

#define SAFETY_MARGIN 8
#define MIN_N_NOISE_SAMPLE 80*10

// ******************************************************************

byte DOUTS[CHANNEL_COUNT] = HX711_DATAL_PINS;

long int results[CHANNEL_COUNT];

HX711MULTI scales(CHANNEL_COUNT, DOUTS, HX711_CLK_PIN);

void setOutputActive(bool isActive) {
  if (isActive) {
    digitalWrite(LED_PIN, LOW); // Active low
    digitalWrite(OUTPUT_PIN, HIGH); // Active height
  } else {
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(OUTPUT_PIN, LOW);
  }
}

class ZProbe {
  private:
    long rawAvg[CHANNEL_COUNT];
    long noiseRange;
    int nNoiseSample;

    HX711MULTI scales;

  public:
    ZProbe(HX711MULTI scales):scales(scales) {
      readNoise();
    }
  
    bool isTriggered() {
      long raws[CHANNEL_COUNT];
      long sumAdj=0;
      scales.readRaw(raws);

      bool verboseMode=(digitalRead(VERBOSE_OUTPUT_ENABLE)==0);

      for (int i=0;i<CHANNEL_COUNT;i++) {
          sumAdj+=raws[i]-rawAvg[i];
      }

      long sumDivNoise=sumAdj/noiseRange;
      
      if (verboseMode) {
        Serial.print("RAW\t");
        for (int i=0;i<CHANNEL_COUNT;i++) {
          Serial.print(raws[i]);
          Serial.print("\t");
        }
        Serial.print("ADJ\t");
        for (int i=0;i<CHANNEL_COUNT;i++) {
          Serial.print(raws[i]-rawAvg[i]);
          Serial.print("\t");
        }
        Serial.print("SUM\t");
        Serial.print(sumAdj);
        Serial.print("/Noise\t");
        Serial.println(sumDivNoise);
        
        if (sumDivNoise!=0) {
          Serial.print("Triggered ");
          Serial.println(sumDivNoise);
        }
      }

      return sumDivNoise!=0;
    }

    void readNoise() {
      long long sum[CHANNEL_COUNT];
      long rawsMin[CHANNEL_COUNT];
      long rawsMax[CHANNEL_COUNT];
      long raws[CHANNEL_COUNT];

      setOutputActive(true);

      Serial.println("Reading noise samples...");

      scales.readRaw(raws); // throw away the first reading

      for (int i=0;i<CHANNEL_COUNT;i++) {
        scales.readRaw(raws);
        sum[i]=raws[i];
        rawsMin[i]=raws[i];
        rawsMax[i]=raws[i];
      }

      int noiseSampleCount=0;
      
      for (noiseSampleCount=0;noiseSampleCount<MIN_N_NOISE_SAMPLE-1  || (digitalRead(NOISE_READ_ENABLE)==0);noiseSampleCount++) {
        scales.readRaw(raws);

        for (int i=0;i<CHANNEL_COUNT;i++) {
          if (rawsMin[i]>raws[i]) rawsMin[i]=raws[i];
          if (rawsMax[i]<raws[i]) rawsMax[i]=raws[i];
          sum[i]+=raws[i];
        }
      }

      nNoiseSample=noiseSampleCount+1;

      long sumRange=0;

      noiseRange=0;

      for (int i=0;i<CHANNEL_COUNT;i++) {
        noiseRange+=SAFETY_MARGIN*(rawsMax[i]-rawsMin[i]);
        
        rawAvg[i]=sum[i]/nNoiseSample;
      }

      Serial.print("N Noise Samples: ");
      Serial.println(nNoiseSample);
      
      Serial.print("Noise floor:");
      Serial.print(" range=");
      Serial.println(noiseRange);

      setOutputActive(false);
    }
};

ZProbe *zProbe;

void setup() {
  Serial.begin(115200);
  Serial.flush();

  pinMode(LED_PIN,OUTPUT);
  pinMode(OUTPUT_PIN,OUTPUT);

  pinMode(VERBOSE_OUTPUT_ENABLE,INPUT_PULLUP);
  pinMode(NOISE_READ_ENABLE,INPUT_PULLUP);

  zProbe=new ZProbe(scales);
}

void loop() {

  setOutputActive(zProbe->isTriggered());

  if (digitalRead(NOISE_READ_ENABLE)==0) {
    zProbe->readNoise();
  } else if (Serial.available()>0) {   //on serial data (any data) re-read noise floor
    while (Serial.available()) {
      Serial.read();
    }
    zProbe->readNoise();
  } 
}
