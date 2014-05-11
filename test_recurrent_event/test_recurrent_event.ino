#define LED 13
#define INTERVAL 500
#include <EventLib.h>

EventLib event;

void setup() {
  pinMode(LED, OUTPUT);
  event.set_timeout_to_recurrently_run(500, handle_a);
}

void loop() {
  event.event_loop();
}

void handle_a(void *data) {
  byte last_state = digitalRead(LED);
  digitalWrite(LED, !last_state);
}
