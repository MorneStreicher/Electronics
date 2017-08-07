

#define MAX_Z 6
#define MAX_X 4
#define MAX_Y 4

byte cube[MAX_Z][MAX_X][MAX_Y];

void pause()
{
 delayMicroseconds(1);
}

void selectRow(byte b)
{
  // Set the storage register to LOW (PD4)
  PORTD = PORTD & B11101111;

  // Push out the byte value, bit by bit
  for (int i = 0; i < 8; i++)
  {
    // Write the data pin (PD3)
    if ((b & 128) > 0)
    {
      PORTD = PORTD | B00001000;
    }
    else
    {
      PORTD = PORTD & B11110111;
    }

    // Send a pulse on the clock pin (PD5)
    PORTD = PORTD | B00100000;
    PORTD = PORTD & B11011111;

    // Set the data pin to 0 - TODO : Might be removed??
    PORTD = PORTD & B11110111;
    
    b = b << 1;
  }

  // Set the storage register to HIGH (PD4)
  PORTD = PORTD | B00010000;
}

void writeBusData(byte busData)
{
  PORTC = (PORTC & B11110000) | ((busData >> 4) & B00001111);
  PORTB = (PORTB & B11110000) | (busData & B00001111);
}

void pulseBusDestination(int dest)
{
  if (dest == 0)
  {
    PORTD = (PORTD & B00111111) | B01000000;
  }

  if (dest == 1)
  {
    PORTD = (PORTD & B00111111) | B10000000;
  }

  pause();

  PORTD = (PORTD & B00111111);
}

void writeRowContent(int z)
{
  byte busData = 0;
  for (int yBus = 0; yBus < 2; yBus++)
  {
    if (cube[z][yBus*2][0]) busData = busData | 128;
    if (cube[z][yBus*2][1]) busData = busData | 64;
    if (cube[z][yBus*2][2]) busData = busData | 32;
    if (cube[z][yBus*2][3]) busData = busData | 16;

    if (cube[z][yBus*2+1][0]) busData = busData | 8;
    if (cube[z][yBus*2+1][1]) busData = busData | 4;
    if (cube[z][yBus*2+1][2]) busData = busData | 2;
    if (cube[z][yBus*2+1][3]) busData = busData | 1;

    writeBusData(busData);
    
    pulseBusDestination(yBus);
  }
}


void setup() {
  Serial.begin(9600);
  
  // Data bus
  DDRC = DDRC | B00001111;
  DDRB = DDRB | B00001111;

  // Row selector and bus destination
  DDRD = DDRD | B11111000;

  //
  cube [0][0][0] = 1;
  for (int i = 0; i < 4; i++)
  {
    cube [0][i][i] = 1;
  }
  //
  selectRow(0);
}

void loop() 
{
  while (1)
  {
    //writeRowContent(0);
    selectRow(4);
    //pulseBusDestination(0);
    //pulseBusDestination(1);

    //pause(); pause();
    
    //writeBusData(B00111000);
    //writeBusData(1);
  }
}
