/*
 * A LED game based on the K64F Freedom Board by Polling method
 * @Anantha Krishnan R
 */

/* Including header files */
#include <stdio.h>
#include <stdint.h>
#include "fsl_debug_console.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#include "fsl_common.h"
#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"

/* Defining values */
#define RED 22U
#define GREEN 26U
#define BLUE 21U
#define SW 4U
#define score 50

/* Global variables for values */
unsigned int option, pin=0, cur_led=0, scr=0, dly=5000;

/* Prototyping the functions */
int delay();
int delay1();
int led_flash();
int update();
int retrylevel();

/* Function for a 3s delay */
int delay(int t)
{
	float i;
	for(i=0;i<t;i+=0.01);
	return 0;
}

/* Allows retry option when misses a flash */
int retrylevel()
{
	int y=0;
	printf("\n\r\t *** You missed it *** \n\n\r Your current score is: %d \n\n\r What do you want now? \n\r",scr);
	do
	{
		/* Loops till a valid input is received */
		printf("\r Retry - 1 \n\r Exit - 0 : ");
		scanf("%d",&y);
		if(y==1||y==0)
		{
			/* Prints score when user chooses to exit */
			if(y==0)
			{
				printf("\n\r Your score is: %d",scr);
				printf("\n\r\t Exiting...\n\r");

			}
			break;
		}
	}while(1);
	/* Restarting level if chosen */
	printf("\n\r Ok. Restarting level now..\n\r");
	delay(2000);
	return 0;
}

/* Updates score and level when a hit is received */
int update()
{
	/* Score increased and delay reduced between flashes */
	dly-=500;
	scr+=score;
	printf("\n\r\t !!! You have a hit !!! \n\r");
	if(dly==0)
	{
		printf("\n\r !! Congrats! You completed ALL level !! \n\r Your score is: %d \n",scr);
		printf("\n\r\t Exiting game... Push 'Reset' to start again. \n\r");

	}
	printf("\n\r Continuing game... \n\r");
	delay(2000);
	return 0;
}

/* Checks the change is Pin value when interrupt occurs */
int delay1(int t)
{
	float i;
	for(i=0;i<t;i+=0.01)
	{
		pin=GPIO_ReadPinInput(GPIOA,SW);
		if(pin==0)
		{
			/* Chooses win/fail according to pin value */
			if(cur_led==option)
			{
				update();
				break;
			}
			else
			{
				retrylevel();
				break;
			}
		}
	}
	return 0;
}

/* Writes output to led in R B G order and clears pin value to 0 */
int led_flash()
{
	do
	{
		/* Red LED */
		GPIO_WritePinOutput(GPIOB,RED,0);
		cur_led=1;
		delay1(dly);
		GPIO_WritePinOutput(GPIOB,RED,1);

		/* Blue LED */
		GPIO_WritePinOutput(GPIOB,BLUE,0);
		cur_led=2;
		delay1(dly);
		GPIO_WritePinOutput(GPIOB,BLUE,1);

		/* Green LED */
		GPIO_WritePinOutput(GPIOE,GREEN,0);
		cur_led=3;
		delay1(dly);
		GPIO_WritePinOutput(GPIOE,GREEN,1);
	}while(1);
	/* Loops flashing till button press is received */
	return 0;
}

int main(void)
{
  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_BootClockRUN();
    BOARD_InitBootClocks();
    /* Enable clocks at port */
    CLOCK_EnableClock(kCLOCK_PortB);
    CLOCK_EnableClock(kCLOCK_PortE);
    CLOCK_EnableClock(kCLOCK_PortA);

    /* Init FSL debug console. */
    BOARD_InitDebugConsole();

    gpio_pin_config_t led_config = {kGPIO_DigitalOutput, 1,};
    gpio_pin_config_t sw_config = {kGPIO_DigitalInput, 0,};

    /* Mux PORT as GPIO */
    PORT_SetPinMux(PORTB,RED,kPORT_MuxAsGpio);
    PORT_SetPinMux(PORTE,GREEN,kPORT_MuxAsGpio);
    PORT_SetPinMux(PORTB,BLUE,kPORT_MuxAsGpio);
    PORT_SetPinMux(PORTA,SW,kPORT_MuxAsGpio);

    /* Sets the configuration as output */
    GPIO_PinInit(GPIOB,RED,&led_config);
    GPIO_PinInit(GPIOB,BLUE,&led_config);
    GPIO_PinInit(GPIOE,GREEN,&led_config);
    GPIO_PinInit(GPIOA,SW,&sw_config);

    /* Main menu of Game */

    printf("\n\r\t *** Catch the Color *** \n\r");
    do
    {
    	printf("\n\r\t =============== \n\r");
    	printf("\n\r\t Game has 8 levels and each win gives 50 points. \n\n\r");
    	printf("\n\r\t Game Options: \n\r\t RED LED - 1 \n\r\t BLUE LED - 2 \n\r\t GREEN LED - 3 \n\n\r\t EXIT GAME - 4");
    	printf("\n\r\t =============== \n\r\t Choose one: ");
    	scanf("%d",&option);
    	if((option>0)&&(option<5))
    	{
    		/* Exit game when option = 4 */
    		if(option==4)
    		{
    			printf("\n\r\t Exiting... Push 'Reset' to start again. \n\r");
    		    return 0;
    		}
    		break;
    	}
    	printf("\n\r !! Invalid Input !! \n\r");
    /* Loops till a valid input is received */
    }while(1);

    printf("\n\r\t ** Game will start now... ** \n");
    /* Delay for user before flashing starts */
    delay(2000);
    do
    {
		pin=GPIO_ReadPinInput(GPIOA,SW);
		if(pin)
		{
			delay(1000);
			/* Calling LED flash */
			led_flash();
			break;
		}
		else
		{
			printf("\n\r\t !! You have pressed the switch before flash !!");
			printf("\n\r\t Exiting... Push 'Reset' to start again. \n\r");
			break;
		}
    }while(1);
    return 0;

}
/* End of program */
