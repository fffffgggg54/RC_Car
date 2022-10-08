
#define xQueuePushBackFromISR(queue, value) if(uxQueueSpacesAvailable(queue) == 0){ xQueueReceiveFromISR(queue); } xQueueSendToBackFromISR(queue, value)
#define xQueuePushBack(queue, value) if(uxQueueSpacesAvailable(queue) == 0){ xQueueReceive(queue); } xQueueSendToBack(queue, value)

class Sensor{

    /*

        Base `Sensor` class.
        implements a simple analogRead() based sensor, such as a potentiometer at the desired pin.
        for additional functionality, extend this class and implement more stuff

    */

protected:  // making this private makes extending classes a bit harder
    QueueHandle_t queue;

public:
    uint pin;

    // the constructor, initializing the queue
    Sensor(){ 
        this->queue = xQueueCreate(5, sizeof(int)); // value type `int`
    }

    // create and begin task
    begin(uint pin, const char* taskname, int priority = 1, uint memory = 256){
    
        this->pin = pin;
        pinMode(pin, INPUT);

        xTaskCreate(
            this->task,
            taskname,
            memory,
            NULL,
            priority,
            NULL
        );
    }


    // the task to be run
    // feel free to add other functions in the class and call them from here
    void task(void*){

        while(true){

            xQueuePushBackFromISR(
                analogRead(this->pin);
            );

            vTaskDelay(500 / portTICK_PERIOD_MS);

        }
    }

    // an example access-function, to be accessed from the outside
    int read(){

        int value = -1;
        if(xQueueReceive(this->queue, &value, 0) == pdFALSE){
            // queue recv failed
        }
        return value;
    }

}


/* Usage Example:

void setup(){

    Sensor knob;
    
    knob.begin(
        5,            // pin number
        "knob5_task"  // task name
    );

    // later, from another task,
    Serial.println( knob.read() );
}
*/