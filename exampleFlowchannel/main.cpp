#include "mbed.h"
#include "SDP3x.h"

DigitalOut myled1(LED1);
DigitalOut myled2(LED2);
InterruptIn tacho_int(PA_11);
PwmOut fan(PA_8);

DigitalIn sdaDummy01(PA_9, PullUp);        // Enabling PulLUp Resistors
DigitalIn sclDummy01(PA_10, PullUp);       // on both I2C Interfaces
I2C i2c(PA_10, PA_9);                      // I2C Channel 01
InterruptIn sdp3x_int(PB_4);               // IRQn Sensor 01

SDP3xClass SDP3x(&i2c);

volatile uint32_t t1,t2,t3,t4,t5;
volatile uint8_t ready=0, sdp3x_int_ready=0;

void tacho_rise() {
  if(ready==0)      { t1 = us_ticker_read(); ready=1; }
  else if(ready==2) { t3 = us_ticker_read(); ready=3; }
}

void tacho_fall() {
  if(ready==1) { t2=us_ticker_read(); ready = 2; }
}

void sdp3x_fall() {
  sdp3x_int_ready = 1;
}

void fantest(float pwm) {
  fan.write(pwm);
  wait(2);

  uint16_t diffpressure, temperature, scalefactor;
  uint8_t ret = SDP3x.readTriggeredMeasurement(&diffpressure, &temperature, &scalefactor);
  if(ret)
    printf("error: %d\n", ret);
  else
    printf("%f %f %f\n", pwm, (float)diffpressure/(float)scalefactor, (float)temperature/(float)200);
}

void testSoftReset() {
  uint8_t ret = 0;

  ret = SDP3x.softReset();
  if(ret)
    printf("error: %d\n", ret);
}

void testIdent() {
  uint8_t ret = 0;
  uint32_t pn;
  uint64_t sn;

  ret = SDP3x.readIdent(&pn, &sn);
  if(ret)
    printf("error: %d\n", ret);
  else
    printf("product number: %08x serial number: %016llx\n", pn, sn);

  ret = SDP3x.readIdent(&pn);
  if(ret)
    printf("error: %d\n", ret);
  else
    printf("product number: %08x\n", pn);

  if(SDP3x.isSDP31())
    printf("found SDP31\n");
  else if(SDP3x.isSDP32())
    printf("found SDP32\n");
}

void testContinuousMeasurement() {
  uint8_t ret = 0;
  uint16_t diffpressure, temperature, scalefactor;

  ret = SDP3x.startContinuousMeasurement();
  if(ret) {
    printf("error: %d\n", ret);
    return;
  }

   // wait for data conversion in sensor
  wait_ms(SDP3X_MEASUREMENT_CONT_START_MS);

  t1 = us_ticker_read();
  ret = SDP3x.readValues(&diffpressure, &temperature, &scalefactor);
  t2 = us_ticker_read();
  if(ret) {
    printf("error: %d\n", ret);
    return;
  } else
    printf("diff_press: %fPa raw_diff_press: %d temperature: %f캜 scale: %d measurement (transfer) time: %ld탎\n", (float)diffpressure/(float)scalefactor, diffpressure, (float)temperature/(float)200, scalefactor, t2-t1);

  t1 = us_ticker_read();
  ret = SDP3x.readValues(&diffpressure, &temperature, &scalefactor);
  t2 = us_ticker_read();
  if(ret) {
    printf("error: %d\n", ret);
    return;
  } else
    printf("diff_press: %fPa raw_diff_press: %d temperature: %f캜 scale: %d measurement (transfer) time: %ld탎\n", (float)diffpressure/(float)scalefactor, diffpressure, (float)temperature/(float)200, scalefactor, t2-t1);

  t1 = us_ticker_read();
  ret = SDP3x.readValues(&diffpressure, &temperature, &scalefactor);
  t2 = us_ticker_read();
  if(ret) {
    printf("error: %d\n", ret);
    return;
  } else
    printf("diff_press: %fPa raw_diff_press: %d temperature: %f캜 scale: %d measurement (transfer) time: %ld탎\n", (float)diffpressure/(float)scalefactor, diffpressure, (float)temperature/(float)200, scalefactor, t2-t1);

  ret = SDP3x.stopContinuousMeasurement();
  if(ret) {
    printf("error: %d\n", ret);
    return;
  }
}

void testContinuousAverageMeasurement() {
  uint8_t ret = 0;
  uint16_t diffpressure, temperature, scalefactor;

  ret = SDP3x.startContinuousAverageMeasurement();
  if(ret) {
    printf("error: %d\n", ret);
    return;
  }

   // wait for data conversion in sensor
  wait_ms(SDP3X_MEASUREMENT_CONT_START_MS);

  t1 = us_ticker_read();
  ret = SDP3x.readValues(&diffpressure, &temperature, &scalefactor);
  t2 = us_ticker_read();
  if(ret) {
    printf("error: %d\n", ret);
    return;
  } else
    printf("diff_press: %fPa raw_diff_press: %d temperature: %f캜 scale: %d measurement (transfer) time: %ld탎\n", (float)diffpressure/(float)scalefactor, diffpressure, (float)temperature/(float)200, scalefactor, t2-t1);

  t1 = us_ticker_read();
  ret = SDP3x.readValues(&diffpressure, &temperature, &scalefactor);
  t2 = us_ticker_read();
  if(ret) {
    printf("error: %d\n", ret);
    return;
  } else
    printf("diff_press: %fPa raw_diff_press: %d temperature: %f캜 scale: %d measurement (transfer) time: %ld탎\n", (float)diffpressure/(float)scalefactor, diffpressure, (float)temperature/(float)200, scalefactor, t2-t1);

  t1 = us_ticker_read();
  ret = SDP3x.readValues(&diffpressure, &temperature, &scalefactor);
  t2 = us_ticker_read();
  if(ret) {
    printf("error: %d\n", ret);
    return;
  } else
    printf("diff_press: %fPa raw_diff_press: %d temperature: %f캜 scale: %d measurement (transfer) time: %ld탎\n", (float)diffpressure/(float)scalefactor, diffpressure, (float)temperature/(float)200, scalefactor, t2-t1);

  ret = SDP3x.stopContinuousMeasurement();
  if(ret) {
    printf("error: %d\n", ret);
    return;
  }
}

void testConstantWaitTriggeredMeasurement() {
  uint8_t ret = 0;
  uint16_t diffpressure, temperature, scalefactor;

  t1 = us_ticker_read();
  ret = SDP3x.readTriggeredMeasurement(&diffpressure, &temperature, &scalefactor);
  t2 = us_ticker_read();
  if(ret) {
    printf("error: %d\n", ret);
    return;
  } else
    printf("raw_diff_press: %d temperature: %f캜 scale: %d measurement and transfer time: %ld탎\n", diffpressure, (float)temperature/(float)200, scalefactor, t2-t1);

  t1 = us_ticker_read();
  ret = SDP3x.readTriggeredMeasurement(&diffpressure, &temperature);
  t2 = us_ticker_read();
  if(ret) {
    printf("error: %d\n", ret);
    return;
  } else
    printf("raw_diff_press: %d temperature: %f measurement and transfer time: %ld탎\n", diffpressure, (float)temperature/(float)200, t2-t1);

  t1 = us_ticker_read();
  ret = SDP3x.readTriggeredMeasurement(&diffpressure);
  t2 = us_ticker_read();
  if(ret) {
    printf("error: %d\n", ret);
    return;
  } else
    printf("raw_diff_press: %d measurement and transfer time: %ld탎\n", diffpressure, t2-t1);
}

void testConstantWaitTriggeredMeasurement2() {
  uint8_t ret = 0;
  uint16_t diffpressure, temperature, scalefactor;
  uint16_t diffpressure2, temperature2;
  uint16_t diffpressure3;

  sdp3x_int.fall(Callback<void()>(sdp3x_fall));
  sdp3x_int_ready=0;

  t1 = us_ticker_read();
  ret = SDP3x.readTriggeredMeasurement(&diffpressure, &temperature, &scalefactor);
  if(ret) {
    printf("error: %d\n", ret);
    return;
  }

  t2 = us_ticker_read();
  ret = SDP3x.readTriggeredMeasurement(&diffpressure2, &temperature2);
  if(ret) {
    printf("error: %d\n", ret);
    return;
  }

  t3 = us_ticker_read();
  ret = SDP3x.readTriggeredMeasurement(&diffpressure3);
  if(ret) {
    printf("error: %d\n", ret);
    return;
  }
  t4 = us_ticker_read();

  if(ret) {
    printf("error: %d\n", ret);
    return;
  } else {
    printf("raw_diff_press: %d scale: %d temperature: %f캜 read time: %ld탎\n", diffpressure, scalefactor, (float)temperature/(float)200, t2-t1);
    printf("raw_diff_press: %d temperature: %f캜 read time: %ld탎\n", diffpressure2, (float)temperature2/(float)200, t3-t2);
    printf("raw_diff_press: %d read time: %ld탎\n", diffpressure3, t4-t3);
  }
}

void testInterruptTriggeredMeasurement() {
  uint8_t ret = 0;
  uint16_t diffpressure, temperature, scalefactor;

  sdp3x_int.fall(Callback<void()>(sdp3x_fall));
  sdp3x_int_ready=0;

  ret = SDP3x.startTriggeredMeasurement();
  if(ret) {
    printf("error: %d\n", ret);
    return;
  } else
    t1 = us_ticker_read();

  while(sdp3x_int_ready==0) ;

  t2 = us_ticker_read();

  ret = SDP3x.readValues(&diffpressure, &temperature, &scalefactor);
  if(ret) {
    printf("error: %d\n", ret);
    return;
  } else
    printf("diff_press: %fPa raw_diff_press: %d temperature: %f캜 scale: %d measurement time: %ld탎\n", (float)diffpressure/(float)scalefactor, diffpressure, (float)temperature/(float)200, scalefactor, t2-t1);
}

void testInterruptTriggeredMeasurement2() {
  uint8_t ret = 0;
  uint16_t diffpressure, temperature, scalefactor;
  uint16_t diffpressure2, temperature2;
  uint16_t diffpressure3;

  sdp3x_int.fall(Callback<void()>(sdp3x_fall));
  sdp3x_int_ready=0;

  ret = SDP3x.startTriggeredMeasurement();
  if(ret) {
    printf("error: %d\n", ret);
    return;
  } else
    t1 = us_ticker_read();

  while(sdp3x_int_ready==0) ;

  t2 = us_ticker_read();

  ret = SDP3x.readValues(&diffpressure, &temperature, &scalefactor);
  if(ret) {
    printf("error: %d\n", ret);
    return;
  }
  t3 = us_ticker_read();

  printf("raw_diff_press: %d temperature: %f캜 scale: %d measurement time: %ld탎 read time: %ld탎\n", diffpressure, (float)temperature/(float)200, scalefactor, t2-t1, t3-t2);

  sdp3x_int_ready=0;
  ret = SDP3x.startTriggeredMeasurement();
  if(ret) {
    printf("error: %d\n", ret);
    return;
  } else
    t1 = us_ticker_read();

  while(sdp3x_int_ready==0) ;

  t2 = us_ticker_read();

  ret = SDP3x.readValues(&diffpressure2, &temperature2);
  if(ret) {
    printf("error: %d\n", ret);
    return;
  }
  t3 = us_ticker_read();

  printf("raw_diff_press: %d temperature: %f캜 measurement time: %ld탎 read time: %ld탎\n", diffpressure2, (float)temperature2/(float)200, t2-t1, t3-t2);

  sdp3x_int_ready=0;
  ret = SDP3x.startTriggeredMeasurement();
  if(ret) {
    printf("error: %d\n", ret);
    return;
  } else
    t1 = us_ticker_read();

  while(sdp3x_int_ready==0) ;

  t2 = us_ticker_read();

  ret = SDP3x.readValues(&diffpressure3);
  if(ret) {
    printf("error: %d\n", ret);
    return;
  }
  t3 = us_ticker_read();

  printf("raw_diff_press: %d measurement time: %ld탎 read time: %ld탎\n", diffpressure3, t2-t1, t3-t2);
}

int main() {

  fan.period_us(50); // 20kHz
  fan.write(0.5);
  wait(5);

  printf("\nstarting SDPX tests\n");

  printf("\ntest ident\n");
  testIdent();

  printf("\ntest softreset\n");
  testSoftReset();

  printf("\ntest continuous measurement with a starting wait and without following waits\n");
  testContinuousMeasurement();

  printf("\ntest softreset\n");
  testSoftReset();

  printf("\ntest continuous average measurement with a starting wait and without following waits\n");
  testContinuousAverageMeasurement();

  printf("\ntest softreset\n");
  testSoftReset();

  printf("\ntest triggered measurement with constant wait\n");
  testConstantWaitTriggeredMeasurement();

  printf("\ntest softreset\n");
  testSoftReset();

  printf("\ntest triggered measurement with interrupt controled readout\n");
  testInterruptTriggeredMeasurement();
  testInterruptTriggeredMeasurement();
  testInterruptTriggeredMeasurement();

  printf("\ntest softreset\n");
  testSoftReset();

  printf("\ntest triggered measurement with interrupt controled readout - measure transfer time\n");
  testInterruptTriggeredMeasurement2();

  printf("\nfinished SDPX tests\n");

//  for(float i=0.0f; i <1.0f; i+=0.025f)
//    fantest(i);

  while(1) ;

//  tacho_int.rise(Callback<void()>(tacho_rise));
//  tacho_int.fall(Callback<void()>(tacho_fall));
//  while(1) {
//    myled1 = 1;
//    wait_ms(10);
//    myled1 = 0;
//    wait_ms(10);
//    if(ready==3) {
//      if(t2>t1 && t3>t2) {
////        printf("%u %lf\n", t3-t1, ((double)t2-t1)/(double)(t3-t1)*100);
////        printf("%u %u\n", t3-t1, t2-t1);
//        printf("%u\n", t3-t1);
//      }
//      ready = 0;
//    }
//  }
}
