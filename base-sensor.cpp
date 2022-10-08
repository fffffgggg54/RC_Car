
class Sensor{

    /*

        Base `Sensor` class.
        implements a simple analogRead() based sensor, such as a potentiometer at the desired pin.
        for additional functionality, extend this class and implement more stuff

    */

protected:  // making this private makes extending classes a bit harder
    SemaphoreHandle_t lock;
    int value; // or float or whatever

public:
    uint pin;

    // the constructor, initializing the mutex/lock
    Sensor(){ 
        this->lock = xSemaphoreCreateMutex();
    }

    // create and begin task
    begin(uint pin, const char* taskname, int priority = 1, uint memory = 256){
    
        this->pin = pin;
        pinMode(pin, INPUT);

        this->value = -1;

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

            unlock(this->lock, {

                this->value = analogRead(this->pin);
            })

            vTaskDelay(500 / portTICK_PERIOD_MS);

        }
    }

    // an example access-function, to be accessed from the outside
    int read(){

        int temp_local = -1;

        unlock(this->lock, {

            temp_local = this->value;
        })

        return temp_local;
    }

}


// Usage Example:
#ifdef THIS_IS_A_USAGE_EXAMPLE_DONT_DEFINE_THIS

void setup(){

    Sensor knob;
    
    knob.begin(
        5,            // pin number
        "knob5_task"  // task name
    );

    // later, from another task,
    Serial.println( knob.read() );
}

#endif
