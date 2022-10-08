#include "base-sensor.cpp"
#include <math.h> // for log(x)

class Thermistor :	public Sensor{

public:

	Thermistor(){
		A = 0; B = 0; C = 0; R = 0;

	}

	begin(uint pin, const char* taskname, int priority = 1, uint memory = 256){
		
		// pin needs to be an analog input
		pinMode(pin, INPUT);

		this->R = R;

		xTaskCreate(
            this->task,
            taskname,
            memory,
            NULL,
            priority,
            NULL
        );
	}

	void setConst(float R, float A, float B, float C){

		// sets constants
		// called only once, prefarably from main "thread"

		this->R = R;
		this->A = A;
		this->B = B;
		this->C = C;
	}

	float read(){
		/*	
			https://create.arduino.cc/projecthub/Marcazzan_M/how-easy-is-it-to-use-a-thermistor-e39321
			https://en.wikipedia.org/wiki/Steinhart%E2%80%93Hart_equation
		
			schematic:
		 
			VCC ─────┬─────
			         │
			        (R)  [Fixed value resistor (this->R)]
	      (to pin)───┤
			        (T)  [Thermistor]
			         │
			GND ─────┴────
		
			returns temprature in celcius
			returns -100 if failed
		*/

		int local;

		if(xQueueReceive(this->queue, &local, 0) == pdFALSE){
            return -100;
        }

        float r = this->R * local/1023; // math (find resistance of thermistor (in Ω))
        float l = log(r);
        float T = 1/(A + (B*l) + (C*l*l*l)); // find temprature of thermistor (in K)

        T = T - 273.15; // convert kelvin to °C

		return T;
	}

	float getResistance(){

		// returns resistance of thermistor (in Ω)
		// returns -1 if failed
		//
		// useful for calibration and settings constants

		int local;

		if(xQueueReceive(this->queue, &local, 0) == pdFALSE){
            return -1;
        }

        return this->R * local/1023;
	}

protected:

 	QueueHandle_t servoQueue;
 	float A, B, C, R;

	void task(void*){

		while(true){
			
			xQueuePushBack(
				this->queue, 
				analogRead(this->pin)
			);
			
			vTaskDelay(500 / portTICK_PERIOD_MS);
		}
	}


private:
	using Sensor::read; // revoke public status of read() so nobody can use it
}