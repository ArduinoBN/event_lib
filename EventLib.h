#ifndef EVENTLIB_H
#define EVENTLIB_H
#include "Arduino.h"

class EventLib {

	typedef struct _event_listener{
		void *event_data;
		int (*test_func)(void *);
		void (*handle_func)(void *);
	} event_listener;

	//typedef struct _event_node{
	//	struct _event_listener *event;
	//	struct _event_node *next.
	//	struct _event_node *last.
	//};

	public:
		EventLib();
		void add_listener(void *event_data, int (*test_func)(void *), void (*handle_func)(void *));
		void check_and_handle_event(struct _event_listener *listener);
		void set_timeout_to_run(unsigned int, void(*)(void *));
		void event_loop();
	private:
		event_listener *listeners[10];
		int curr_listener;
		//struct _event_node *first_event;
		//struct _event_node *last_event;
};

#endif
