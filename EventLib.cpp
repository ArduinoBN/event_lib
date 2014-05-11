#include "Arduino.h"
#include "EventLib.h"

EventLib::EventLib(){
	first_event = NULL;
	last_event = NULL;
}

void EventLib::add_listener(void *event_data,
                  int (*test_func)(void *),
                  void (*handle_func)(void *)) {
  struct _event_listener *new_listener = create_event_listener(event_data, test_func, handle_func);
  add_listener_to_list(new_listener);
}

void EventLib::add_listener(void *event_data,
                  int (*test_func)(void *),
                  void (*handle_func)(void *),
                  int flags) {
  struct _event_listener *new_listener = create_event_listener(event_data, test_func, handle_func, flags);
  add_listener_to_list(new_listener);
}

struct EventLib::_event_listener * EventLib::create_event_listener(
                  void *event_data,
                  int (*test_func)(void *),
                  void (*handle_func)(void *)) {
  struct _event_listener *new_listener = (event_listener *) malloc(sizeof(event_listener));
  new_listener->event_data = event_data;
  new_listener->test_func = test_func;
  new_listener->handle_func = handle_func;
  new_listener->recurrent = 0;
  new_listener->count = 0;
}

struct EventLib::_event_listener * EventLib::create_event_listener(void *event_data,
                  int (*test_func)(void *),
                  void (*handle_func)(void *),
                  int flags) {
  struct _event_listener *new_listener = create_event_listener(event_data, test_func, handle_func);
  if(flags & RECURRENT_EVENT) new_listener->recurrent = 1;
}

void EventLib::add_listener_to_list(struct _event_listener *new_event) {
  struct _event_node *new_node = (struct _event_node *) malloc(sizeof(struct _event_node));
  if(!first_event) {
    first_event = new_node;
  } else {
    last_event->next = new_node;
    new_node->prev = last_event;
  }
  last_event = new_node;
}

void EventLib::remove_listener_from_list(struct _event_node *node) {
  if(node == first_event)
    first_event = node->next;
  if(node == last_event)
    last_event = node->prev;

  if(node->prev)
    node->prev->next = node->next;
  if(node->next)
    node->next->prev = node->prev;
  free(node);
}

void EventLib::check_and_handle_event(struct _event_node *node) {
  struct _event_listener *listener = node->event;
  void *event_data = listener->event_data;
  if(listener->test_func(event_data)) {
    remove_listener_from_list(node);
    listener->handle_func(event_data);
    listener->count++;
    if(listener->recurrent)
      add_listener_to_list(listener);
  }
}

void EventLib::event_loop() {
  struct _event_node *curr = first_event;
  while(curr) {
    check_and_handle_event(curr);
    curr = curr->next;
  }
}

int __EventLib__timeOut(void * data) {
	struct EventLib::timeOutStruct *toData = (struct EventLib::timeOutStruct *)data;
	int ret = 0;
	unsigned long curr = millis();
	if(toData->next <= curr) {
		ret = 1;
		toData->next = curr + toData->interval;
	}
	return ret;
}

void EventLib::set_timeout_to_run(unsigned int interval, void(*cb)(void *), int flags) {
	//TODO: decide how to free() the struct
	struct EventLib::timeOutStruct *toStruct = (struct EventLib::timeOutStruct*) malloc(sizeof(struct EventLib::timeOutStruct));
	toStruct->interval = interval;
	toStruct->next = millis() + interval;
	add_listener((void *)toStruct, __EventLib__timeOut, cb, flags);
}

void EventLib::set_timeout_to_run(unsigned int interval, void(*cb)(void *)) {
	set_timeout_to_run(interval, cb, 0);
}

void EventLib::recurrently_set_timeout_to_run(unsigned int interval, void(*cb)(void *)) {
	set_timeout_to_run(interval, cb, RECURRENT_EVENT);
}

int __EventLib__pin_status(void * data) {
	struct EventLib::pinStatus *toData = (struct EventLib::pinStatus *)data;
	int ret = 0;
	byte curr;
	if(toData->last_status != toData->status_to_run) {
		byte curr = digitalRead(toData->pin_num);
		if(curr == toData->status_to_run ) {
			ret = 1;
		}
	}
	toData->last_status = curr;
	return ret;
}

void EventLib::run_on_status(int pin_num, byte status, void(*cb)(void *), int flags) {
	//TODO: decide how to free() the struct
	struct EventLib::pinStatus *toStruct = (struct EventLib::pinStatus*) malloc(sizeof(struct EventLib::pinStatus));
	toStruct->pin_num = pin_num;
	toStruct->status_to_run = status;
	toStruct->last_status = digitalRead(pin_num);
	add_listener((void *)toStruct, __EventLib__pin_status, cb, flags);
}

void EventLib::run_on_status(int pin_num, byte state, void(*cb)(void *)) {
	run_on_status(pin_num, state, cb, 0);
}

void EventLib::recurrently_run_on_status(int pin_num, byte state, void(*cb)(void *)) {
	run_on_status(pin_num, state, cb, RECURRENT_EVENT);
}

int __EventLib__pin_unbounced_status(void *data) {
	struct EventLib::pinStatus *toData = (struct EventLib::pinStatus *)data;
	int ret = 0;
	byte curr;
	if(millis() >= toData->bounce_retry) {
		byte curr = digitalRead(toData->pin_num);
		if(curr == toData->status_to_run ) {
			ret = 1;
		}
	}
	toData->last_status = curr;
	return ret;
}

void __EventLib__pin_status_add_retry(void *data) {
	struct EventLib::pinStatus *toStruct = (struct EventLib::pinStatus*) malloc(sizeof(struct EventLib::pinStatus));
	toStruct->bounce_retry = millis() + BOUNCE_RETRY_TIME;
	toStruct->obj->add_listener((void *)toStruct, __EventLib__pin_unbounced_status, toStruct->cb, 0);
}

void EventLib::run_on_unbounced_status(int pin_num, byte status, void(*cb)(void *), int flags) {
	//TODO: decide how to free() the struct
	struct EventLib::pinStatus *toStruct = (struct EventLib::pinStatus*) malloc(sizeof(struct EventLib::pinStatus));
	toStruct->pin_num = pin_num;
	toStruct->status_to_run = status;
	toStruct->last_status = digitalRead(pin_num);
	toStruct->cb = cb;
	toStruct->obj = this;
	add_listener((void *)toStruct, __EventLib__pin_status, __EventLib__pin_status_add_retry, flags);
}

void EventLib::run_on_unbounced_status(int pin_num, byte state, void(*cb)(void *)) {
	run_on_unbounced_status(pin_num, state, cb, 0);
}

void EventLib::recurrently_run_on_unbounced_status(int pin_num, byte state, void(*cb)(void *)) {
	run_on_unbounced_status(pin_num, state, cb, RECURRENT_EVENT);
}
