// Create the LCD menu variable and initialize the LCD (16x2 characters)
LcdMenu lcdMenu(16, 2, MAXMENUITEMS);
LcdButtons lcdButtons(0);

// Create the variables to track RA time, RA display time and HA time
//DayTime RATime;
//DayTime RADisplayTime;
//DayTime HATime;
//DayTime HACorrection;

Mount mount(RAStepsPerDegree, DECStepsPerDegree, &lcdMenu);

void setup() {

  //Serial.begin(38400);
  Serial.begin(57600);
  //BT.begin(9600);

#ifdef DEBUG_MODE
  Serial.println("Hello");
#endif

  // Show a splash screen
  lcdMenu.setCursor(0, 0);
  lcdMenu.printMenu("OpenAstroTracker");
  lcdMenu.setCursor(0, 1);
  lcdMenu.printMenu("     " + version);
  unsigned long now = millis();

  // Not sure if this is neeeded
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);

  // Configure the mount
  // Set the global HA correction
  mount.setHACorrection(h, m, s);

  // Set the stepper motor parameters
  mount.configureRAStepper(FULLSTEP, RAmotorPin1, RAmotorPin2, RAmotorPin3, RAmotorPin4, RAspeed, RAacceleration);
  mount.configureDECStepper(HALFSTEP, DECmotorPin1, DECmotorPin2, DECmotorPin3, DECmotorPin4, DECspeed, DECacceleration);

  // Read persisted values adn set in mount
  inputcal = EEPROM.read(0);
  DayTime haTime = DayTime(EEPROM.read(1), EEPROM.read(2), 0);
  mount.setSpeedCalibration(speed + inputcal / 10000);
#ifdef DEBUG_MODE
  Serial.println("InputCal: " + String(inputcal));
  Serial.println("SpeedCal: " + String(mount.getSpeedCalibration(), 5));
  Serial.println("TRKSpeed: " + String(mount.getSpeed(TRACKING), 5));
#endif

  mount.setHA(haTime);

  // Start the tracker.
  mount.startSlewing(TRACKING);

  // Create the LCD top-level menu items
  lcdMenu.addItem("RA", RA_Menu);
  lcdMenu.addItem("DEC", DEC_Menu);
#ifdef SUPPORT_POINTS_OF_INTEREST
  lcdMenu.addItem("POI", POI_Menu);
#endif
  lcdMenu.addItem("HOME", Home_Menu);
  lcdMenu.addItem("HA", HA_Menu);
#ifdef SUPPORT_HEATING
  lcdMenu.addItem("HEA", Heat_Menu);
#endif
  lcdMenu.addItem("CTRL", Control_Menu);
  lcdMenu.addItem("CAL", Calibration_Menu);
  lcdMenu.addItem("INFO", Status_Menu);

  while (millis() - now < 750) {
    mount.loop();
  }

  lcdMenu.updateDisplay();
  
#ifdef DEBUG_MODE
  Serial.println("SetupDone");
#endif
  
}
