#include "mbed.h"
#include "SDP3x.h"

/* 
 * Test mit Matlab:
   s=serial('COM8','Baudrate',115200)
   fopen(s)
   fwrite(s,0.0,'float32'); fread(s,1,'float32') ... fwrite(s,1.0,'float32'); fread(s,1,'float32')
   fclose(s)
 
   for kk=1:100; fwrite(s,0.0,'float32'); while(s.BytesAvailable<4) end, rr(kk)=fread(s,1,'float32'); pause(.1); end plot(rr);
	 
	 for kk=1:100; 
     if kk>20
       fwrite(s,1.0,'float32'); 
     else
       fwrite(s,0.0,'float32'); 
     end
     while(s.BytesAvailable<4) end, 
     rr(kk)=fread(s,1,'float32'); 
     pause(.1); 
   end
*/


// Tiefpass 2.Ordnung mit Zeitkonstanten 0.21 und 0.13 und 100Hz Abtastzeit
#define b1 0.001757246355887f
#define b0 0.001685788948090f
#define a1 -1.879458033475793f
#define a0 0.882901068779770f

float uk_1, uk_2=0.0f, yk_1=0.0f, yk_2=0.0f, yk;


Serial pc(USBTX, USBRX);

PwmOut fan(PA_8);

DigitalIn sdaDummy01(PA_9, PullUp);        // Enabling PulLUp Resistors
DigitalIn sclDummy01(PA_10, PullUp);       // on both I2C Interfaces
I2C i2c(PA_10, PA_9);                      // I2C Channel

InterruptIn sdp3x_int(PB_4);               // IRQn Sensor SDP3x

SDP3xClass SDP3x(&i2c);

Thread thSensor;
Thread thFilter;
osThreadId idThFilter=NULL;
Ticker tckFilter;

float fPressure = 0.0;
float fPressureFiltered = 0.0;
uint16_t iPressure = 0;

struct rxData {
	float soll;
};

union {
	rxData rxStruct;
	uint8_t rxData[sizeof(rxData)];
} rxUnion;

struct txData {
	float ist;
};

union {
	txData txStruct;
	uint8_t txData[sizeof(txData)];
} txUnion;

volatile uint8_t ready=0, sdp3x_int_ready=0;


void sdp3x_fall() {
  sdp3x_int_ready = 1;
}

int softReset() {
  uint8_t ret = 0;

  ret = SDP3x.softReset();
  return ret;
}

uint32_t testIdent() {
  uint8_t ret = 0;
  uint32_t pn;
  uint64_t sn;

  ret = SDP3x.readIdent(&pn, &sn);
  if(ret)
    return 0;
  else
    return pn;
}

void sensor() {
  uint8_t ret = 0;
  uint16_t diffpressure, temperature, scalefactor;

  sdp3x_int.fall(Callback<void()>(sdp3x_fall));

  while(true) {
    sdp3x_int_ready = 0;

    ret = SDP3x.startTriggeredMeasurement();
    if(ret) {
		  iPressure = ret;
      continue;
    }
	
    while(sdp3x_int_ready==0) ;

    ret = SDP3x.readValues(&diffpressure, &temperature, &scalefactor);
    if(ret) {
		  fPressure = 0.0;
		  iPressure = ret;
      return;
    } else {
		  fPressure = (float)diffpressure/(float)scalefactor;
		  iPressure = diffpressure;
      //printf("diff_press: %fPa raw_diff_press: %d temperature: %f°C scale: %d measurement time: %ldµs\n", (float)diffpressure/(float)scalefactor, diffpressure, (float)temperature/(float)200, scalefactor, t2-t1);
		}
	}
}

void cbTckFilter(){
  if(idThFilter != NULL)
    osSignalSet(idThFilter, 0x1);
}

void filter() {	
  //idThFilter = ThisThread::get_id();
	idThFilter = Thread::gettid();
	while(1){
    //ThisThread::flags_wait_all(0x1);
		Thread::signal_wait(0x1,osWaitForever);
		// hier Filter

		//P.Duenow
		uk_1=fPressure;
		yk=-a1*yk_1-a0*yk_2+b1*uk_1+b0*uk_2;
		yk_2=yk_1;
		yk_1=yk;
		uk_2=uk_1;
		//P.Duenow
		
		fPressureFiltered = yk;
  }
}

void cbRxComplete(int event) {
	pc.read(rxUnion.rxData, sizeof(rxUnion), Callback<void(int)>(cbRxComplete));
  fan.write(1.0f-rxUnion.rxStruct.soll);
	
  //txUnion.txStruct.ist = fPressure;
  txUnion.txStruct.ist = fPressureFiltered;
	pc.write(txUnion.txData, sizeof(txUnion), NULL);
}

int main() {
  fan.period_us(50); // 20kHz
	fan.write(1.0);
	pc.baud(115200);

	thSensor.start(Callback<void()>(sensor));
	thFilter.start(Callback<void()>(filter));
	
  tckFilter.attach_us(callback(cbTckFilter), (timestamp_t)10000); // 10ms
	
	pc.read(rxUnion.rxData, sizeof(rxUnion), Callback<void(int)>(cbRxComplete));
	
  while(true) 
		wait(1);
}
