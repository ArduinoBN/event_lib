#include "Arduino.h"
#include "EventLib.h"

EventLib::EventLib(){
	first_event = NULL;
	last_event = NULL;
}

void EventLib::add_listener(void *event_data,
                  int (*test_func)(void *),
                  void (*handle_func)(void *)) {
  struct _event_node *new_node = (struct _event_node *) malloc(sizeof(struct _event_node));
  new_node->event = (event_listener *) malloc(sizeof(event_listener));
  new_node->event->event_data = event_data;
  new_node->event->test_func = test_func;
  new_node->event->handle_func = handle_func;
  if(!first_event) {
    first_event = new_node;
  } else {
    last_event->next = new_node;
    new_node->prev = last_event;
  }
  last_event = new_node;
}

void EventLib::check_and_handle_event(struct _event_listener *listener) {
  void *event_data = listener->event_data;
  if(listener->test_func(event_data)) {
    listener->handle_func(event_data);
  }
}

void EventLib::event_loop() {
  struct _event_node *curr = first_event;
  while(curr) {
    check_and_handle_event(curr->event);
    curr = curr->next;
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

