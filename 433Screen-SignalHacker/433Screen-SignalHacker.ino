#include <RFM69LPL.h>
#include <SPI.h>
#include <RFM69LPLregisters.h>
#include <EEPROM.h>

#include <UI1306LPL.h>

#define DIO2_T 15
#define DIO2_R 16

#define R_CS 4
#define T_CS 5
#define SD_CS 13

#define BUILTIN_LED 2

float frequency_ISM = 433.92;
uint8_t buttonA = 27;
uint8_t buttonB = 26;     //buttons pins. these work on ESP32, but add whichever ones you like. 
uint8_t buttonUP = 25;    //on intializeDisplay(), they're all set to pinMode(x, INPUT_PULLUP).
uint8_t buttonDOWN = 33;


//custom mode//////////
UI1306LPL speed_60(&speed60, MODE_FUNCTION);
UI1306LPL speed_70(&speed70, MODE_FUNCTION);
UI1306LPL speed_80(&speed80, MODE_FUNCTION);
UI1306LPL speed_90(&speed90, MODE_FUNCTION);
UI1306LPL speed_100(&speed100, MODE_FUNCTION);

UI1306LPL speed_config(NULL, MODE_MENU, "PWM: 190", &speed_60, "PWM: 200", &speed_70, "PWM: 210", &speed_80, "PWM: 230", &speed_90, "PWM: 255", &speed_100);

UI1306LPL car_mode(&carMode, MODE_FUNCTION);
UI1306LPL mac_mode(&macMode, MODE_FUNCTION);

UI1306LPL tesla(&sendTesla, MODE_FUNCTION);
UI1306LPL custom(NULL, MODE_MENU, "Car Mode", &car_mode, "Speed Config", &speed_config, "Tesla", &tesla, "MAC Mode", &mac_mode);
///////////////

RFM69LPL radio_R(R_CS, DIO2_R); 
RFM69LPL radio_T(T_CS, DIO2_T);


UI1306LPL led_analyser(&ledAnalyser, MODE_FUNCTION);
UI1306LPL rec_rep(&recordReplay, MODE_FUNCTION);
UI1306LPL t_config(&transmitterConfig, MODE_FUNCTION);
UI1306LPL r_config(&receiverConfig, MODE_FUNCTION);
UI1306LPL ReplayRecord(NULL, MODE_MENU, "Receiver Config", &r_config, "Blocker Config", &t_config, "Record/Replay", &rec_rep, "RSSI Viewer", &led_analyser);

UI1306LPL spc_analyser(&spectrumAnalyser, MODE_FUNCTION);
UI1306LPL gx_analyser(&graphicAnalyser, MODE_FUNCTION);
UI1306LPL sig_analyser(NULL, MODE_MENU, "Graphic Analyser", &gx_analyser, "Spectrum Analyser", &spc_analyser, "LED Analyser", &led_analyser, "Receiver Config", &r_config);

UI1306LPL send_cst(&sendConstantWave, MODE_FUNCTION);
UI1306LPL signal_jammer(NULL, MODE_MENU, "Send Constant Wave", &send_cst);



UI1306LPL main_menu(NULL, MODE_MENU, "Signal Jammer", &signal_jammer, "Signal Analyser", &sig_analyser, "Custom Modes", &custom, "Replay/Store", &ReplayRecord);


void setup() {
  Serial.begin(115200);
  pinMode(BUILTIN_LED, OUTPUT);
  unselect_SD();

  radio_T.unselect(); // avoid SPI interference between radios
  radio_R.init();
  radio_T.init();
  
  initEEPROM();
  pullEEPROMSettings(); //pull bytes from eeprom into registers
  updateVariables(); //translate registers into variables for quick access to values
  radio_R.updateSettings(); //write all variables into registers (some settings are not saved by eeprom, and need the default value written at start)
  radio_T.readAllRegs(); 
  radio_R.readAllRegs(); printEEPROMSettings();
  radio_R.readAllSettings(); //reads registers and translates into readable form
  
  main_menu.initializeDisplay(SHOW_BOOT_SCREEN);
  showBattery();
}

void loop() {
  main_menu.runMenu();
}
