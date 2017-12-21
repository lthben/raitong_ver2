void setup_tripButton() {
  pinMode(tripButtonPin, INPUT_PULLUP);
}

void run_tripwire() {
  // read the state of the tripwire value:
  tripState = digitalRead(tripButtonPin);

  // check if the tripwire is activated. If it is, the tripState is HIGH:
  if (tripState == HIGH) { //tripped

    if (!hasTripped) {
      send_sms(BryanMobileNum, "ALERT! Trip wire has been triggered!");
//      send_sms(BenMobileNum, "ALERT! Trip wire has been triggered!");
      hasTripped = true;
      hasTrippedTime = millis();
    }
  } else { //not tripped

    if (hasTripped == true && (millis() - hasTrippedTime > IDLEDURATION)) {
      hasTripped = false;//resets cos setting up will reset the wire state
    }
  }
}

