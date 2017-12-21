String get_gps_reading() {

  float latitude, longitude, rtc_date, rtc_time, speed_kph;

  boolean gps_success = fona.getGPS(&latitude, &longitude, &rtc_date, &rtc_time, &speed_kph);

  if (gps_success) {

    //    Serial.print(str1);
    //    Serial.println(latitude, 6);
    //    Serial.print(str2);
    //    Serial.println(longitude, 6);

    //    Serial.print("GPS speed MPH:");
    //    speed_mph = speed_kph * 0.621371192;
    //    Serial.println(speed_mph);

    //    Serial.print(str3);
    //    Serial.println(rtc_date, 0); //no decimal points
    //    Serial.print(str4);
    //    Serial.println(rtc_time, 0);
    //    Serial.print(str5);
    //    Serial.println(speed_kph);

    //    Serial.print("GPS heading:");
    //    Serial.println(heading);
    //    Serial.print("GPS altitude:");
    //    Serial.println(altitude);

    latlon = String(latitude, 6) + "," + String(longitude, 6);

    speedKPH = String(2.0 * speed_kph, 2);

    String tripStatus;
    if (hasTripped) {
      tripStatus = "YES";
    }
    else {
      tripStatus = "NO";
    }

    String reportString = String(SMSquota) + "sms " + "tripped:" + tripStatus + " date:" + String(rtc_date, 0) + " time:" + String(rtc_time, 0) + " " + googleMapsQueryString + latlon + " spd:" + String(2.0 * speed_kph, 2);

    //return the appropriate string to log to SD card
    if (check_idle_time()) {
      return (reportString + " -> Alert! Truck stationary for more than " + String(int(IDLEDURATION / 60000.0)) + "min");
    } else {
      return (reportString + " -> all normal");
    }
  } else {
    Serial.println(F("Waiting for FONA GPS 3D fix..."));
    return "waiting for GPS fix";
  }
}

bool check_idle_time () {
  //checks whether truck has been stationary for more than IDLEDURATION
  if (speedKPH.toInt() == 0 ) {
    isIdle = true;
  } else {
    isIdle = false;
    lastMovingTime = millis();
  }

  if ((isIdle) && (millis() - lastMovingTime > IDLEDURATION) && (millis() - lastSentAlertSMSTime > 2 * IDLEDURATION)) {
    //send sms alert with link to google maps
    send_sms(BryanMobileNum, "ALERT! Truck stationary for more than " + String(int(IDLEDURATION / 60000.0)) + "min at " + googleMapsQueryString + latlon);
    //    send_sms(BenMobileNum, "ALERT! Truck stationary for more than " + String(int(IDLEDURATION / 60000.0)) + "min at " + googleMapsQueryString + latlon);
    lastSentAlertSMSTime = millis();
    return true;
  } else {
    return false;
  }
}

void initialise_fona() {
  fonaSS.begin(9600); // Reconnect at lower baud, 115200 had issues with SoftwareSerial
  fonaSerial->begin(9600);

  fonaSS.println(F("AT+IPR=9600"));  // Set baud to 9600
  delay(100); // Let the command run

  if (! fona.begin(*fonaSerial)) {
    Serial.println(F("Couldn't find FONA"));
    while (1);
  }
  Serial.println(F("FONA is OK"));

  //Clear out any waiting serial data
  while (fonaSS.available())
  {
    fonaSS.read();
  }

  Serial.println(F("Raitong system activated."));
  send_sms(BryanMobileNum, "Raitong system activated");
  delay(100);
  //  send_sms(BenMobileNum, "Raitong system activated");
  delay(100);

  fonaSS.println(F("AT+CGPS?"));
  delay(100);
  fonaSS.println(F("AT+CGPS=1,1")); //activate GPS tracking
  delay(100);
  fonaSS.println(F("AT+CTZU=1"));
  Serial.println(F("Automatic time zone updating turned on"));
  Serial.println(F("Starting GPS tracking, allow up to 15min to get initial fix"));

  delay(100);

  fonaSerial->print(F("AT+CNMI=2,1\r\n"));  //set up the FONA to send a +CMTI notification when an SMS is received

  fonaSS.println(F("AT"));
  Serial.println(F("FONA ready"));
}


