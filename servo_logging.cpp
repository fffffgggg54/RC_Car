#include "base-sensor.cpp"

class ServoLogger :	public Sensor{

public:

	begin(uint pin, const char* taskname, int priority = 1, uint memory = 256){
		
		pinMode(pin, INPUT);

		xQueue = xQueueCreate(6, sizeof( uint ));

		attachInterrupt(digitalPinToInterrupt(pin), this->on_rising, RISING);
		attachInterrupt(digitalPinToInterrupt(pin), this->on_falling, FALLING);

		xTaskCreate(
            this->task,
            taskname,
            memory,
            NULL,
            priority,
            NULL
        );
	}

	float getAngle(){

		// returns the angle of servo (in degrees)
		// returns -1 if fails to aquire lock [though can be changed later to wait till lock is available]

		float local = -1;

		if(xSemaphoreTake(this->lock) == pdTRUE){

			local = this->angle;
			xSemaphoreGive(this->lock);
		}
		return local;
	}

protected:

	float angle;
 	QueueHandle_t xQueue;

	void task(void*){
		
		uint now, prev;

		int high, low;

		while(true){
		
			if( xQueueReceive(this->xQueue, &now 10) == pdTRUE){ // wait upto 10 ticks for queue message
																// queue returns (t) when rising edge, and (-t) for now falling edge
				if(now < 0){
					high = -(now + prev);  // now - prev, but now is negative
				}else{
					low = now + prev; // now - prev, but prev is negative
				}

				if( xSemaphoreTake(this->lock) == pdTRUE){
					
					this->angle = high / (high + low) * 180; // assuming servo only goes upto 180 deg
					xSemaphoreGive(this->lock);
				}

				b = a;
			}
		}
	}

	void on_rising(void){
		xQueueSendToBackFromISR(this->xQueue,  xTaskGetTickCountFromISR());
	}

	void on_falling(void){
		xQueueSendToBackFromISR(this->xQueue, -xTaskGetTickCountFromISR());
	}

private:
	using Sensor::read; // revoke public status of read() so nobody can use it
}