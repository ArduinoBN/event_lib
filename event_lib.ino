
// EVENT_LIB

typedef struct _event_listener{
  void *event_data;
  int (*test_func)(void *);
  void (*handle_func)(void *);
} event_listener;

event_listener *listeners[10];
int curr_listener = 0;

void add_listener(void *event_data,
                  int (*test_func)(void *),
                  void (*handle_func)(void *)) {
  listeners[curr_listener] = (event_listener *) malloc(sizeof(event_listener));
  listeners[curr_listener]->event_data = event_data;
  listeners[curr_listener]->test_func = test_func;
  listeners[curr_listener]->handle_func = handle_func;
  curr_listener++;
}

void check_and_handle_event(struct _event_listener *listener) {
  void *event_data = listener->event_data;
  if(listener->test_func(event_data)) {
    listener->handle_func(event_data);
  }
}

void event_loop() {
  for(int i=0; i<curr_listener; i++) {
    event_listener *listener = listeners[i];
    check_and_handle_event(listener);
  }
}



//////////////////////////////////////////////
// Program using the event_lib
//////////////////////////////////////////////

#define LED 13
#define INTERVAL 500

typedef struct {
  unsigned long last_event_time;
} blink_data;

blink_data led_data;

void setup() {
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  led_data.last_event_time = millis();
  add_listener((void *) &led_data, test_a, handle_a);
}

void loop() {
  event_loop();
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
