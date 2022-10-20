//#include "sensor.h"

class ServoLog : public Sensor{

public:

	void begin(uint pin){

		this->queue = xQueueCreate(6, sizeof(int));

		pinMode(pin, INPUT);

		attachInterruptArg(digitalPinToInterrupt(pin), this->on_falling, this, FALLING);
		attachInterruptArg(digitalPinToInterrupt(pin), this->on_rising,  this, RISING);

	}

	float read(){

		if( uxQueueSpacesAvailable(queue) >= 3 ){
			int t1, t2, t3;
			xQueueReceive(this->queue, &t1, 0);
			xQueueReceive(this->queue, &t2, 0);
			xQueueReceive(this->queue, &t3, 0);

			if(t1 > 0){
				return (t1 + t2)/(t1 - t3);
			}else{
				return (t2 + t3)/(t3 - t1);
			}
		}

		return -1;

	}

	void WRAP(ServoLog, on_rising){
		xQueueSendToBackFromISR(this->queue,  (void*)xTaskGetTickCountFromISR(), 0);
	}

	void WRAP(ServoLog, on_falling){
		xQueueSendToBackFromISR(this->queue, (void*)(-xTaskGetTickCountFromISR()), 0);
	}
};