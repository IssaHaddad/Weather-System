#include <stdio.h>
#include <RS232.h>
#include <string.h>

#define CLI() asm("cli \n");         /*Clear I Bit in CCR enables interrupts*/
void porth_isr(void);
/*interrupt pragma*/
#pragma interrupt_handler porth_isr
/*initialize vector table with interrupt source address - PORTH_ INTERRUPT*/
#pragma abs_address: 0x3E4C
void (*PORTH_interrupt_vector[])()={porth_isr};
#pragma end_abs_address 

int flag1=0;
int flag2=0;
int flag3=0;
int flag4=0;
int cost1;
int cost2;
int cost3;
int cost4;

int which_key(void){
int X;
while(1){
PORTA = 0XFE; // Col 0 -PA0
X = PORTA;
if (X == 0xEE)return 0x01;
if (X == 0xDE)return 0x04;
if (X == 0xBE)return 0x07;
if (X == 0x7E)return 0x0E;
PORTA = 0XFD; //Col 1 - PA1
X = PORTA;
if (X == 0xED)return 0x02;
if (X == 0xDD)return 0x05;
if (X == 0xBD)return 0x08;
if (X == 0x7D)return 0x00;
PORTA = 0XFB;
X = PORTA;
if (X == 0xEB)return 0x03;
if (X == 0xDB)return 0x06;
if (X == 0xBB)return 0x09;
if (X == 0x7B)return 0x0F;
PORTA = 0XF7;
X = PORTA;
if (X == 0xE7)return 0x0A;
if (X == 0xD7)return 0x0B;
if (X == 0xB7)return 0x0C;
if (X == 0x77)return 0x0D;
}
}

void delay10seconds()
{
int i;
 for (i =0; i<10; i++)
 {
  delay_01sec();
 }
}

void delay_100us()
{
 int i;
 for (i=0;i<10;i++)
 {
  delay_10usec();
 }
}

void cashier(int tableno)
{
 int digit1,digit2, change1,change2, cost,payment1,remain,payment2;
 char buffer[15];
 clr_disp();
 if (tableno==1)
 {
  cost=cost1;
 }
 else if (tableno==2)
 {
  cost=cost2;
 }
  else if (tableno==3)
 {
  cost=cost3;
 }
 else
 {
  cost=cost4;
 }
 sprintf(buffer,"bill: %d",cost);
 lcd_print_txt(buffer,LINE1);
 digit1=which_key();
 delay_05sec();
 digit2=which_key();
 delay_05sec();
 payment1=(digit1*10)+digit2;
 change1=payment1-cost;
 if (change1>=0)
 {
 sprintf(buffer,"change: %d",change1);
 lcd_print_txt(buffer,LINE2);
  delay10seconds();
 clr_disp();
 }
 else
 {
  remain=change1*(-1);
  sprintf(buffer,"pay %d extra",remain);
  lcd_print_txt(buffer,LINE2);
  digit1=which_key();
  delay_05sec();
  digit2=which_key();
  delay_05sec();
  payment2=(digit1*10)+digit2;
  change2=payment2-remain;
  clr_disp();
  sprintf(buffer,"pay %d extra",remain);
  lcd_print_txt(buffer,LINE1);
  sprintf(buffer,"change: %d",change2);
  lcd_print_txt(buffer,LINE2);
 }
 delay10seconds();
 clr_disp();
}

void Prepare_Meal()
{
 PORTE=0x04;
 delay10seconds();
 PORTE =0x00;
 delay10seconds();
}

void Prepare_Drink()
{
 PTP = 0xFF; // open ptp cw
 PORTB = 0x01;
 delay10seconds();
 PORTB = 0x00;
 delay10seconds();
}

void Order(char item[], int tableno)
{
 if (strcmp(item,"1") == 0 | strcmp(item,"2") == 0 | strcmp(item,"3") == 0 |strcmp(item,"4") == 0)
  {
  Prepare_Meal();
   if (tableno==1){ cost1 = 20.0;}else if (tableno==2){ cost2 = 20.0;}else if (tableno==3){ cost3 = 20.0;}else { cost4 = 20.0;}
        }
  else if (strcmp(item,"5") == 0 | strcmp(item,"6") == 0 | strcmp(item,"7") == 0 |strcmp(item,"8") == 0)
  {
  Prepare_Drink();
   if (tableno==1){ cost1 = 10.0;}else if (tableno==2){ cost2 = 10.0;}else if (tableno==3){ cost3 = 10.0;}else { cost4 = 10.0;}
  }
  
}

void Menu_disp()
{
 SCI0Setup(9600);
 Transmit_SCI0("Menu:");
 Transmit_SCI0("\r\n");
 Transmit_SCI0("Item 1: Cheese Sandwich, cost = 20$");
 Transmit_SCI0("\r\n");
 Transmit_SCI0("Item 2: Club Snadwich, cost = 20$");
 Transmit_SCI0("\r\n");
 Transmit_SCI0("Item 3: Chicken Burger, cost = 20$");
 Transmit_SCI0("\r\n");
 Transmit_SCI0("Item 4: Beef Burger, cost = 20$");
 Transmit_SCI0("\r\n");
 Transmit_SCI0("Item 5: Vanilla Milkshake, cost = 10$");
 Transmit_SCI0("\r\n");
 Transmit_SCI0("Item 6: Chocolate Milkshake, cost = 10$");
 Transmit_SCI0("\r\n");
 Transmit_SCI0("Item 7: Strawberry Smoothie, cost = 10$");
 Transmit_SCI0("\r\n");
 Transmit_SCI0("Item 8: Kiwi Smoothie, cost = 10$");
 Transmit_SCI0("\r\n");
 Transmit_SCI0("\r\n");
 Transmit_SCI0("Please enter your choice from the menu");
 Transmit_SCI0("\r\n");
}

void enable_int(void)
{
           CLI();                                            /*Turn interrupt system on*/
}


void porth_isr(void)
{
clr_disp();
while(!(PIFH & 0x0F));
{
clr_disp();
if (PIFH & 0x08)//sw2 is pressed
{
 if (!flag1)
 {
  char item[5];
  Menu_disp();
  Receive_SCI0(item,0x0D);
  while (strcmp(item,"1")!=0 && strcmp(item,"2")!=0 && strcmp(item,"3")!=0 && strcmp(item,"4")!=0 && strcmp(item,"5")!=0 && strcmp(item,"6")!=0 && strcmp(item,"7")!=0 && strcmp(item,"8")!=0 )
  {
   Transmit_SCI0("your choice of Item is invalid, please choose another item: ");
   Transmit_SCI0("\r\n");
   Receive_SCI0(item,0x0D);
  }
  Order(item,1);
  Transmit_SCI0("You chose an item: ");
  Transmit_SCI0(item);
  flag1 = 1;
  }  // if (!flag1) 
  else
  {
   cashier(1);
   flag1 =0;
  }
}//sw2 if
else if (PIFH & 0x04) //sw3 is pressed
{
 if (!flag2)
 {
  char item[5];
  Menu_disp();
  Receive_SCI0(item,0x0D);
  while (strcmp(item,"1")!=0 && strcmp(item,"2")!=0 && strcmp(item,"3")!=0 && strcmp(item,"4")!=0 && strcmp(item,"5")!=0 && strcmp(item,"6")!=0 && strcmp(item,"7")!=0 && strcmp(item,"8")!=0 )
  {
   Transmit_SCI0("your choice of Item is invalid, please choose another item: ");
   Transmit_SCI0("\r\n");
   Receive_SCI0(item,0x0D);
  }
  Order(item,2);
  Transmit_SCI0("You chose an item: ");
  Transmit_SCI0(item);
  flag2 = 1;
  }  // if (!flag2) 
  else
  {
   cashier(2);
   flag2 =0;
  }
}
else if (PIFH & 0x02) //sw4 is pressed
{
  if (!flag3)
 {
  char item[5];
  Menu_disp();
  Receive_SCI0(item,0x0D);
  while (strcmp(item,"1")!=0 && strcmp(item,"2")!=0 && strcmp(item,"3")!=0 && strcmp(item,"4")!=0 && strcmp(item,"5")!=0 && strcmp(item,"6")!=0 && strcmp(item,"7")!=0 && strcmp(item,"8")!=0 )
  {
   Transmit_SCI0("your choice of Item is invalid, please choose another item: ");
   Transmit_SCI0("\r\n");
   Receive_SCI0(item,0x0D);
  }
  Order(item,3);
  Transmit_SCI0("You chose an item: ");
  Transmit_SCI0(item);
  flag3 = 1;
  }  // if (!flag3) 
  else
  {
   cashier(3);
   flag3 =0;
  }
}
else if (PIFH & 0x01) //sw5 is pressed
{
  if (!flag4)
 {
  char item[5];
  Menu_disp();
  Receive_SCI0(item,0x0D);
  while (strcmp(item,"1")!=0 && strcmp(item,"2")!=0 && strcmp(item,"3")!=0 && strcmp(item,"4")!=0 && strcmp(item,"5")!=0 && strcmp(item,"6")!=0 && strcmp(item,"7")!=0 && strcmp(item,"8")!=0 )
  {
   Transmit_SCI0("your choice of Item is invalid, please choose another item: ");
   Transmit_SCI0("\r\n");
   Receive_SCI0(item,0x0D);
  }
  Order(item,4);
  Transmit_SCI0("You chose an item: ");
  Transmit_SCI0(item);
  flag4 = 1;
  }  // if (!flag4) 
  else
  {
   cashier(4);
   flag4 =0;
  }
}
}//while
PIFH |=0x0F;
}//func
void initialize_ADC(void)
{ ATD0CTL2 = 0x80; // power up ADC, disable interrupts
  delay_05sec(); // wait for ADC to warm up
  ATD0CTL3 = 0x00; // select active background mode
  ATD0CTL4 = 0x85; // 8-bit, sample time 2 ADC clock, prescale of 5,
} 
void initialize_ADC2(void)
{ ATD1CTL2 = 0x80; // power up ADC, disable interrupts
  delay_05sec(); // wait for ADC to warm up
  ATD1CTL3 = 0x00; // select active background mode
  ATD1CTL4 = 0x85; // 8-bit, sample time 2 ADC clock, prescale of 5,
} 
int ADC_CONVERT()
{ ATD0CTL5 = 0x85; // channel no. 5 and right justified
  while(!(ATD0STAT0 & 0x80)); // wait for conversion to finish
  return(ATD0DR0); // get and return the value to the caller
} 
int ADC_CONVERT2()
{ ATD1CTL5 = 0x86; // channel no. 5 and right justified
  while(!(ATD1STAT0 & 0x80)); // wait for conversion to finish
  return(ATD1DR0); // get and return the value to the caller
} 
void main(void)
{

enable_int();  // has to be enabled to allow the timing in the functions to
init_lcd();
                              // Enabling PortH interrupts 
DDRH=0x00;
PPSH &=0xF0; //porth pull polarity select
PIEH |=0x0F; //set porth interrupt enable register
PIFH |=0x0F; //clear porth interrupt flag register
DDRB=DDRJ=DDRP=PTP=DDRM=0xFF;
DDRE=0x04;
PTJ=0x00;
DDRA=0X0F;
PUCR=0XFF;
initialize_ADC();

while(1)  //infinite loop to check the interrupt at all the times
{
 char buffer[17];
 char buffer2[17];
 char *voutbuff;
 int *status;
 int ADValue;
 float temp;
 float vout;
 int potent;
 initialize_ADC2();
 potent=ADC_CONVERT2();
 voutbuff = ftoa(potent,status);
 lcd_print_txt(voutbuff, LINE2);
 if (potent>127)
 {
  PTM = 0xFF;
 }
 else
 {
  PTM = 0x00;
 }
 clr_disp();
  vout = ADC_CONVERT()*0.02;
  // Line2 ADValue*resolution
  temp = vout/0.01; //10/1000.0
  voutbuff = ftoa(temp,status);
  lcd_print_txt(voutbuff, LINE1);
  put_char('C');
  clr_disp();

}//while
}//main
