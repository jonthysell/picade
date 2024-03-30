#include <Joystick.h>

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,JOYSTICK_TYPE_GAMEPAD,
  16, 0,                  // Button Count, Hat Switch Count
  true, true, false,     // X and Y, but no Z Axis
  false, false, false,   // No Rx, Ry, or Rz
  false, false,          // No rudder or throttle
  false, false, false);  // No accelerator, brake, or steering

typedef struct
{
  int  port;       // which port is this input on? 0 = PORTB, 1 = PORTF, 2 = PORTD
  char mask;       // bitmask for the input on the port
  boolean pressed; // is this input current pressed?
} input;

// PICADE: Switch to PCB Mappings
// Ignore GPIO for Mini
//-------------------------------------------------------------------
//          /      U                 Button       | GPIO    \
//         /                     1     2     3    |   1      \
// Start  |    L   O   R                          |           |  Esc
//         \                     4     5     6    |   2      /
//          \      D                                        /
//-------------------------------------------------------------------
//           |  Select                              Enter  |


input inputs[] = {
  // 0 is PORTB
  // 1 is PORTF
  // 2 is PORTD
  // 3 is PORTC

  /* Joystick UDLR on Port B-0123 */
  { 0, B00000001, 0 }, // UP
  { 0, B00000010, 0 }, // DOWN
  { 0, B00000100, 0 }, // LEFT
  { 0, B00001000, 0 },  //RIGHT

  /* Buttons 123456 on Port F-01-4567 */
  { 1, B10000000, 0 }, // Button 1
  { 1, B01000000, 0 }, // Button 2
  { 1, B00100000, 0 }, // Button 3
  { 1, B00010000, 0 }, // Button 4
  { 1, B00000010, 0 }, // Button 5
  { 1, B00000001, 0 }, // Button 6
  
  /* GPIO 12/Maxi Buttons 78 on Port D-67 */
  { 2, B01000000, 0 }, // GPIO 1 / Button 7
  { 2, B10000000, 0 }, // GPIO 2 / Button 8

  /* GPIO 3456 on Port B-456 C-6 */
  { 0, B00010000, 0 }, // GPIO 3 / Button 9
  { 0, B00100000, 0 }, // GPIO 4 / Button 10
  { 0, B01000000, 0 }, // GPIO 5 / Button 11
  { 3, B01000000, 0 }, // GPIO 6 / Button 12

  /* Front/Side Buttons on Port D-0123 */
  { 2, B00000001, 0 }, // 1UP Start / Button 13 
  { 2, B00000010, 0 }, // 1UP Select/Coin / Button 14
  { 2, B00000100, 0 }, // Enter / Button 15
  { 2, B00001000, 0 }  // Esc / Button 16

};

// PICADE: Default Mappings
// Ignore GPIO for Mini
//-------------------------------------------------------------------
//          /      Up                Button       | GPIO    \
//         /                     1     2     3    |   7      \
//   13   |    Le  O   Ri                         |           |  16
//         \                     4     5     6    |   8      /
//          \      Do                                       /
//-------------------------------------------------------------------
//           |    14                                  15   |

void setup() {
  /*
    Setup the ports to be inputs will pullup resistors enabled
  */
  DDRB = DDRD = DDRF = B00000000; // set ports B, D, F to be inputs
  PORTB = PORTD = B11111111; // set the pullup resistors on ports B, D
  PORTC = B01000000; // set the pullup resistors on port C
  PORTF = B11110011; // set the pullup resistors on port F

  // Initialize Joystick Library
  Joystick.begin();
  Joystick.setXAxisRange(-1, 1);
  Joystick.setYAxisRange(-1, 1);
}

void loop() {
  /*
    Check the state of each input, if it has changed then output the corresponding keypress
  */
  boolean changed = false;  // has any input changed?
  char pinStates[4] = {PINB, PINF, PIND, PINC}; // read the current port states and store in 0, 1, 2 indexed array to match our port ids above

  // loop through each input
  for(int i = 0; i < sizeof(inputs) / sizeof(input); i++)
  {
    // test for current state of this input
    boolean test = ~pinStates[inputs[i].port] & inputs[i].mask;

    if(test != inputs[i].pressed) // has this input changed state since the last time we checked?
    {
      changed = true; // something changed, we should remember that
      inputs[i].pressed = test; // update our state map so we know what's happening with this key in future

      switch (i) {
        case 0: // UP
          if (test) {
            Joystick.setYAxis(-1);
          } else {
            Joystick.setYAxis(0);
          }
          break;
        case 1: // DOWN
            if (test) {
              Joystick.setYAxis(1);
            } else {
              Joystick.setYAxis(0);
            }
            break;
        case 2: // LEFT
            if (test) {
              Joystick.setXAxis(-1);
            } else {
              Joystick.setXAxis(0);
            }
            break;
        case 3: // RIGHT
          if (test) {
            Joystick.setXAxis(1);
          } else {
            Joystick.setXAxis(0);
          }
          break;
        default: // Buttons 1 - 16
          Joystick.setButton(i - 4, test);
          break;
      }
    }
  }

  if(changed) // if an input changed we'll add a small delay to avoid "bouncing" of the input
    delay(10);
}
