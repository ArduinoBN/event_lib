#define LED 13
#define INTERVAL 500
#include <EventLib.h>

EventLib event;

typedef struct {
  unsigned long last_event_time;
} blink_data;

blink_data led_data;

void setup() {
  pinMode(LED, OUTPUT);
  led_data.last_event_time = millis();
  event.add_listener((void *) &led_data, test_a, handle_a);
}

void loop() {
  event.event_loop();
}

int test_a(void *data) {
  unsigned long curr_time = millis();
  blink_data *my_data = (blink_data *) data;
  return (curr_time - my_data->last_event_time > INTERVAL);
}

void handle_a(void *data) {
  blink_data *my_data = (blink_data *) data;
  byte last_state = digitalRead(LED);
  my_data->last_event_time = millis();
  digitalWrite(LED, !last_state);
}
