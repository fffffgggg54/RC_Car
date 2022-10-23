//#include "sensor.h"

class ServoLog : public Sensor{

public:

	virtual void begin(uint pin){

		this->queue = xQueueCreate(3, sizeof(int));

		pinMode(pin, INPUT);

		attachInterruptArg(digitalPinToInterrupt(pin), this->interrupt, this, CHANGE);
	}

	virtual int read(){

		/*
		//Serial.print( uxQueueSpacesAvailable(queue) );
		//Serial.print(' ');
		//if( uxQueueSpacesAvailable(queue) >= 3 ){
		//	Serial.println("queue available");
			int t1 = -1, t2 = -1, t3 = -1;
			if(xQueueReceive(this->queue, &t1, 1) != pdPASS){Serial.println("fail 1");};
			if(xQueueReceive(this->queue, &t2, 1) != pdPASS){Serial.println("fail 2");};
			if(xQueueReceive(this->queue, &t3, 1) != pdPASS){Serial.println("fail 3");};

			Serial.println(String(t1) +' '+ String(t2) +' '+ String(t3));

			float out;
			if(t1 > 0){
				out = (t1 + t2)/(t1 - t3);
			}else{
				out = (t2 + t3)/(t3 - t1);
			}
			Serial.println("got: " + String(out));
			return out;
		}else{
			Serial.println("waiting for queue");
		}
		*/

		int value = -1;

		xQueueReceive(this->queue, &value, 0);

		return value;

	}

	virtual void WRAP(ServoLog, interrupt){
		
		t1 = t2;
		t2 = t3;
		t3 = xTaskGetTickCountFromISR();

		if(digitalRead(this->pin) == HIGH && t3 != t1){
			xQueuePushBack(this->queue, (t2 - t1) * 255/(t3 - t1));
		}
		
	}

protected:
	int t1;
	int t2;
	int t3;
};