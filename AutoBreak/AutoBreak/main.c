#define F_CPU	16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define TRIG 6
#define ECHO 7
#define SOUND_VELOCITY 340UL

#define M0_HIGH (PORTB |= 0x01)
#define M0_LOW	(PORTB &= 0xfe)
#define M1_HIGH (PORTB |= 0x02)
#define M1_LOW	(PORTB &= 0xfd)
#define M2_HIGH	(PORTB |= 0x04)
#define M2_LOW	(PORTB &= 0xFB)

void Port_Init()
{
	DDRB = 0xff; //Motor
	DDRC = 0x0f; //RGB
	DDRA = 0x01; // Buzzer
	DDRE = ((DDRE | (1<<TRIG)) & ~(1<<ECHO)); // Super Sonic
	DDRF = 0x00;
}

void MotorGo()
{
	M0_LOW; M1_HIGH; M2_HIGH;
}

void MotorBack()
{
	M0_HIGH; M1_LOW; M2_HIGH;
	_delay_ms(100);
	M2_LOW;
	_delay_ms(5);
}

void MotorSTOP()
{
	M0_HIGH; M1_HIGH;
}

void SuperSonic(unsigned int *distance)
{
	TCCR1B = 0x03;
	PORTE &= ~(1<<TRIG);
	_delay_us(10);
	PORTE |= (1<<TRIG);
	_delay_us(10);
	PORTE &= ~(1<<TRIG);
	while(!(PINE & (1<<ECHO)));
	TCNT1 = 0x0000;
	while(PINE & (1<<ECHO));
	TCCR1B = 0x00;
	*distance = (unsigned int)(SOUND_VELOCITY * (TCNT1 * 4 / 2) / 1000);
}

void RGB_ctrl(unsigned char RGB)
{	
	PORTC = RGB;
}

void Buzzer_ctrl(int on)
{
	if (on == 1)	PORTA = 0x01;
	else PORTA = 0x00;
}

int TouchSensor()
{
	if((PIND|0xfe) == 0xff)
		return 1;
	else
		return 0;
}

int Button()
{
	if((PINF|0xfe) == 0xff)
		return 0;
	else
		return 1;
}

int main(void)
{
	Port_Init();
	
	unsigned int distance = 0;
	double delay = 0;
	int flag = 1;
	RGB_ctrl(0x00);
	
    while (1) 
    {
		SuperSonic(&distance);
		
		if(Button())
		{
			flag = 1;
		}
		
		if(flag == 1)
		{
			MotorGo();
			
			if(TouchSensor() && distance < 100)
			{
				flag = 0;
				Buzzer_ctrl(0);
				_delay_ms(200);
				
				RGB_ctrl(0x01);
				Buzzer_ctrl(1);
				MotorSTOP();
				_delay_ms(2000);
				
				RGB_ctrl(0x00);
				Buzzer_ctrl(0);
				MotorBack();
				_delay_ms(700);
				MotorSTOP();
			}
			else if(distance < 100)
			{
				RGB_ctrl(0x01);
				delay = 200;
				
				MotorSTOP();
				_delay_ms(1000);
				
				Buzzer_ctrl(1);
				_delay_ms(delay);
				Buzzer_ctrl(0);
			}else if(distance < 200)
			{
				RGB_ctrl(0x01);
				delay = 300;
				
				Buzzer_ctrl(1);
				_delay_ms(delay);
				Buzzer_ctrl(0);
			}else if(distance < 300)
			{
				RGB_ctrl(0x02);
				delay = 400;
				
				Buzzer_ctrl(1);
				_delay_ms(delay);
				Buzzer_ctrl(0);
			}
			else if(distance < 400)
			{
				RGB_ctrl(0x02);
				delay = 500;
				
				Buzzer_ctrl(1);
				_delay_ms(delay);
				Buzzer_ctrl(0);
			}else if(distance < 500)
			{
				RGB_ctrl(0x04);
				delay = 600;
				
				Buzzer_ctrl(1);
				_delay_ms(delay);
				Buzzer_ctrl(0);
			}
			else
			{
				RGB_ctrl(0x00);
				Buzzer_ctrl(0);
			}
		}	
    }
}
