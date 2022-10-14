#define xQueuePushBackFromISR(queue, value) if(uxQueueSpacesAvailable(queue) == 0){ xQueueReceiveFromISR(queue, nullptr, 0); } xQueueSendToBackFromISR(queue, value, pdFALSE)
#define xQueuePushBack(queue, value) if(uxQueueSpacesAvailable(queue) == 0){ xQueueReceive(queue, nullptr, 0); } xQueueSendToBack(queue, value, pdFALSE)
#define WRAP(classname, funcname) static funcname(void* obj){ ((classname*) obj)->__##funcname(); }; void __##funcname

/*
    `xQueuePushBack(queue, value)` pushes a value to a queue. if the queue is full, it automatically removes the first value in the queue
    `xQueuePushBackFromISR()` is basically the same thing
    `WRAP(class, func)` wraps a member function `func` from `class`, so it can be used as a task (note: tasks and freeRTOS stuff only use C functions 
                                                                                                        so C++ functions need to be wrapped.
													also, the wrapped task takes `(void*)this` as an argument)
*/

class Sensor{

	QueueHandle_t queue;
public:
	int pin;

	Sensor(){
		this->queue = xQueueCreate(6, sizeof(int));
	}

	void begin(int pin, int priority=1, int memory=512){
		this->pin = pin;
		Serial.println("Nothing");

		char buf[20];
		sprintf(buf, "SENSOR TASK (pin %d)", pin);

		xTaskCreate(
            this->task,
            buf,
            memory,
            (void*)this,
            priority,
            NULL
        );
	}


	/*
	void static task(void* obj){

		Sensor* self = (Sensor*) obj;
		self->__task();

	}

	void __task(){
	*/

	void WRAP(Sensor, task){
		while(true){

			xQueuePushBack(
				this->queue,
				(void*) analogRead(this->pin)
			);

			vTaskDelay(200);
		}
	}

	int read(int ticks=0){
		int value = -1;

		xQueueReceive(this->queue, &value, ticks);

		return value;
	}
};