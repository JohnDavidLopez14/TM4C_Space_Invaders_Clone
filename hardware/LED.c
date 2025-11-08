#include "LED.h"


#define LED_COUNTER_RELOAD 0x4C4B40F // 1 HZ
#define INFINITE_DURATION 0

static uint32_t LED_Counter = 0;

typedef struct{
    bool active;
    uint32_t start_time;
    uint32_t duration;
    uint32_t frequency;
    uint32_t next_toggle;
    uint8_t led_mask;
} LedBlink_t;

static LedBlink_t PB4_BlinkTask = {
    .active = false,
    .led_mask = PB4
};

static LedBlink_t PB5_BlinkTask = {
    .active = false,
    .led_mask = PB5
};

void LED_On(uint8_t led_mask){
    led_mask &= PIN_MASK;  // mask PB4:5
    if (led_mask){
			GPIO_PORTB_DATA_BITS_R[led_mask] = led_mask;
			//GPIO_PORTB_DATA_R |= led_mask;
    }
}

void LED_Off(uint8_t led_mask){
    led_mask &= PIN_MASK;
    if (led_mask){
			GPIO_PORTB_DATA_BITS_R[led_mask] = 0; // can also use ~led_mask
			//GPIO_PORTB_DATA_R &= ~led_mask;
    }
}

static void LED_Blink(uint8_t led_mask){
    led_mask &= PIN_MASK;
    if (led_mask){
	    GPIO_PORTB_DATA_BITS_R[led_mask] ^= led_mask; // blink led
    }
}

static void ModifyTimerifAllInactive(bool activeState){
    if (!(PB4_BlinkTask.active | PB5_BlinkTask.active)){ // if no events are active, ie timer isn't runnig
        if (activeState)
            Timer4_Enable();
        else if (!activeState)
            Timer4_Disable();
    }
}

static void BlinkEvent_Start(LedBlink_t *BlinkTask, uint32_t duration, uint32_t frequency){
    ModifyTimerifAllInactive(true); // start timer
    BlinkTask->active = true;
    BlinkTask->start_time = LED_Counter;
    BlinkTask->duration = duration;
    BlinkTask->frequency = frequency;
    BlinkTask->next_toggle = LED_Counter + frequency;
}

void PB4_Blink_Start(uint32_t duration, uint32_t frequency){
    BlinkEvent_Start(&PB4_BlinkTask, duration, frequency);
}

void PB5_Blink_Start(uint32_t duration, uint32_t frequency){
    BlinkEvent_Start(&PB5_BlinkTask, duration, frequency);
}

bool PB4_Get_State(void){
    return PB4_BlinkTask.active;
}

bool PB5_Get_State(void){
    return PB5_BlinkTask.active;
}

static void LED_Blink_Event(LedBlink_t *BlinkTask){
    if (BlinkTask->active){
        if (LED_Counter >= BlinkTask->next_toggle){ // if the counter is still greater than the next toggle
            if (BlinkTask->duration != INFINITE_DURATION && LED_Counter - BlinkTask->start_time >= BlinkTask->duration){ // ends the duration if not infinite and the delta is greater than duration
                BlinkTask->active = false;
                LED_Off(BlinkTask->led_mask);
            } else { // if it is not over, blink the led and schedule the next blink
                LED_Blink(BlinkTask->led_mask);
                BlinkTask->next_toggle += BlinkTask->frequency;
            }
        }
    }
}

void Clear_All_LED_Events(void){
    PB4_BlinkTask.active = false;
    PB5_BlinkTask.active = false;
    LED_Off(PIN_MASK);
}

static void LED_Event_Timer(void){
    LED_Blink_Event(&PB4_BlinkTask);
    LED_Blink_Event(&PB5_BlinkTask);
    ModifyTimerifAllInactive(false);
		LED_Counter++;
}

void LED_Init(void){
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB;
    while             ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R1) == 0);  // Poll until ready
    GPIO_PORTB_CR_R    |= PIN_MASK;                                 // allow GPIOAFSEL, GPIOPUR, and GPIODEN bits to be written
    GPIO_PORTB_AMSEL_R &= ~PIN_MASK;                                // clear analog functions
    GPIO_PORTB_PCTL_R  &= ~0x00FF0000;                                // clear alternate 
    GPIO_PORTB_DIR_R   |= PIN_MASK;                                 // pins as output
    GPIO_PORTB_AFSEL_R &= ~PIN_MASK;                                // clear alternate functions
    GPIO_PORTB_DEN_R   |= PIN_MASK;                                 // enable digital for all pins

    // clear output pins
    GPIO_PORTB_DATA_BITS_R[PIN_MASK] = 0;
		//GPIO_PORTB_DATA_R &= ~PIN_MASK; // pretty sure DATA_BITS_R is a safer way of doing this
    Timer4_Init(LED_Event_Timer, LED_COUNTER_RELOAD);
}