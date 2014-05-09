#include "Arduino.h"
#include "EventLib.h"

EventLib::EventLib(){
	curr_listener = 0;
}

void EventLib::add_listener(void *event_data,
                  int (*test_func)(void *),
                  void (*handle_func)(void *)) {
  listeners[curr_listener] = (event_listener *) malloc(sizeof(event_listener));
  listeners[curr_listener]->event_data = event_data;
  listeners[curr_listener]->test_func = test_func;
  listeners[curr_listener]->handle_func = handle_func;
  curr_listener++;
}

void EventLib::check_and_handle_event(struct _event_listener *listener) {
  void *event_data = listener->event_data;
  if(listener->test_func(event_data)) {
    listener->handle_func(event_data);
  }
}

void EventLib::event_loop() {
  for(int i=0; i<curr_listener; i++) {
    event_listener *listener = listeners[i];
    check_and_handle_event(listener);
  }
}

struct timeOutStruct {
	unsigned long next;
	unsigned int interval;
};

int __EventLib__timeOut(void * data) {
	timeOutStruct *toData = (timeOutStruct *)data;
	int ret = 0;
	unsigned long curr = millis();
	if(toData->next <= curr) {
		ret = 1;
		toData->next = curr + toData->interval;
	}
	return ret;
}

void EventLib::set_timeout_to_run(unsigned int interval, void(*cb)(void *)) {
	//TODO: decide how to free() the struct
	struct timeOutStruct *toStruct = (struct timeOutStruct*) malloc(sizeof(struct timeOutStruct));
	toStruct->interval = interval;
	toStruct->next = millis() + interval;
	add_listener((void *)toStruct, __EventLib__timeOut, cb);
}

