// Author: Davide Messori
// Eserczio verifica 21/02/2019

#include "io430.h"

/*******************************************************************************
*** Define constants
*******************************************************************************/

/* Leds definitions
   Led1             */
#define LED1 BIT7
#define LED1DIR P4DIR
#define LED1OUT P4OUT

// Led2
#define LED2 BIT0
#define LED2DIR P1DIR
#define LED2OUT P1OUT

/* Switches definitions
   Sw1                  */
#define SW1    BIT1
#define SW1DIR P2DIR
#define SW1REN P2REN
#define SW1OUT P2OUT
#define SW1IN  P2IN

// Sw2
#define SW2    BIT1
#define SW2DIR P1DIR
#define SW2REN P1REN
#define SW2OUT P1OUT
#define SW2IN  P1IN

/*******************************************************************************
*** Function prototipes
*******************************************************************************/

// Inits every components
void initAll    (void);

// Sets the leds output
void led1       (int state);

// Gets the switches states
int  getSw1     (void);

// Removes bouncing from switches
int  debSw1     (void);

// Gets events on switches
int  eventSw1   (void);

// FSM controlling the led2 with 2 switches
void FSM_Led2   (void);
void led1(int state);
void led2(int state);

/*******************************************************************************
*** Main Program
*******************************************************************************/

int main( void )
{
    // Stop watchdog timer to prevent time out reset
    WDTCTL = WDTPW + WDTHOLD;	

    initAll();
    
    while (1)
    { 
      led1(!debSw1());
      led2(!debSw2());    }
}

/*******************************************************************************
*** Init every component
*******************************************************************************/
//------------------------------------------------------------------------------
void initAll()
{
    LED1OUT &= ~LED1;     // Turn Led1 off (left unchanged other bits)
    LED1DIR |=  LED1;     // Set LED1 pin as output

    SW1DIR  &= ~SW1;      // Set SW1 pin as input
    SW1OUT  |=  SW1;      // Turn on pull up Resistor
    SW1REN  |=  SW1;
    
    LED2OUT &= ~LED2;     // Turn Led1 off (left unchanged other bits)
    LED2DIR |=  LED2;     // Set LED1 pin as output

    SW2DIR  &= ~SW2;      // Set SW1 pin as input
    SW2OUT  |=  SW2;      // Turn on pull up Resistor
    SW2REN  |=  SW2;
}
//------------------------------------------------------------------------------

/*******************************************************************************
*** Accende il led corrispondente se state = 1
*** Spegne  il led corrispondente se state = 0
*******************************************************************************/
//------------------------------------------------------------------------------
void led1(int state)
{
  if (state == 0)
    LED1OUT &= ~LED1;     // Turn Led2 off (left unchanged other bits)
  else if (state == 1)
    LED1OUT |= LED1;      // Turn Led2 on (left unchanged other bits)
}

void led2(int state)
{
  if (state == 0)
    LED2OUT &= ~LED2;     // Turn Led2 off (left unchanged other bits)
  else if (state == 1)
    LED2OUT |= LED2;      // Turn Led2 on (left unchanged other bits)
}
//------------------------------------------------------------------------------

/*******************************************************************************
*** Restituisce lo stato corrente dello switch corrispondente
*** 0 se lo sw è rilasciato
*** 1 se lo sw è premuto
*******************************************************************************/
//------------------------------------------------------------------------------
int getSw1(void)
{    
    return (SW1IN & SW1) != 0;
}

int getSw2(void)
{    
    return (SW2IN & SW2) != 0;
}
//------------------------------------------------------------------------------

/*******************************************************************************
*** Elimina l'effetto rimbalzo dallo switch corrispondente
*******************************************************************************/
//------------------------------------------------------------------------------
int debSw1(void)
{
    static unsigned char readings1     = 0x1; 
    static unsigned char currentState1 = 1;	 
    
    readings1 <<= 1;			
    readings1  |= getSw1();	
    
    if      (readings1 == 0x00) currentState1 = 0;
    else if (readings1 == 0xff) currentState1 = 1;
    
    return currentState1;
}

int debSw2(void)
{
    static unsigned char readings2     = 0x1; 
    static unsigned char currentState2 = 1;	 
    
    readings2 <<= 1;			
    readings2  |= getSw2();	
    
    if      (readings2 == 0x00) currentState2 = 0;
    else if (readings2 == 0xff) currentState2 = 1;
    
    return currentState2;
}
//------------------------------------------------------------------------------

/*******************************************************************************
*** Ritorna 1 se lo sw corrispondente è rilasciato, 0 se è premuto,
*** 2 se è avvenuto un evento pressione, 3 se è avvenuto un evento rilascio.
*******************************************************************************/
//------------------------------------------------------------------------------
int eventSw1(void)
{
  typedef enum State { PREMUTO, RILASCIATO } State;
  
  static State lastState1   = RILASCIATO;
  int actualState1 = debSw1();
  
  switch (lastState1)
  {
    case PREMUTO:
      if (actualState1 == 0)
        return 0;
      else if (actualState1 == 1)
      {
        lastState1 = RILASCIATO;
        return 3;
      }
      break;
      
    case RILASCIATO:
      if (actualState1 == 1)
        return 1;
      else if (actualState1 == 0)
      {
        lastState1 = PREMUTO;
        return 2;
      }
      break;
  }
  return lastState1;          // In case some errors occur (and for no warnings)
}