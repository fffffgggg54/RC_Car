#include "base-sensor.cpp"

class ServoLogger :	public Sensor{

public:

	begin(uint pin, const char* taskname, int priority = 1, uint memory = 256){
		
		pinMode(pin, INPUT);

		servoQueue = xQueueCreate(6, sizeof( uint ));

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

		if(xQueueReceive(this->queue, &local, 0) == pdFALSE){
            // queue recv failed
        }

		return local;
	}

protected:

 	QueueHandle_t servoQueue;

	void task(void*){
		
		uint now, prev;

		int high, low;

		while(true){
		
			if( xQueueReceive(this->servoQueue, &now 10) == pdTRUE){ // wait upto 10 ticks for queue message
																// queue returns (t) when rising edge, and (-t) for now falling edge
				if(now < 0){
					high = -(now + prev);  // now - prev, but now is negative
				}else{
					low = now + prev; // now - prev, but prev is negative
				}
					
				int angle = high / (high + low) * 180; // assuming servo only goes upto 180 deg

				xQueuePushBack(this->queue, angle);

				b = a;
			}
		}
	}

	void on_rising(void){
        xQueuePushBackFromISR(this->servoQueue,  xTaskGetTickCountFromISR());
	}

	void on_falling(void){
        xQueuePushBackFromISR(this->servoQueue, -xTaskGetTickCountFromISR());
	}

private:
	using Sensor::read; // revoke public status of read() so nobody can use it
}