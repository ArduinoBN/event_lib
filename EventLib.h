#ifndef EVENTLIB_H
#define EVENTLIB_H
#include "Arduino.h"

typedef struct _event_listener{
	void *event_data;
	int (*test_func)(void *);
	void (*handle_func)(void *);
} event_listener;


class EventLib {

	public:
		EventLib();
		void add_listener(void *event_data, int (*test_func)(void *), void (*handle_func)(void *));
		void check_and_handle_event(struct _event_listener *listener);
		void event_loop();
	private:
		event_listener *listeners[10];
		int curr_listener;
};

#endif
