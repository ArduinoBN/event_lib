#ifndef EVENTLIB_H
#define EVENTLIB_H
#include "Arduino.h"

class EventLib {
	#define RECURRENT_EVENT 0b0001

	typedef struct _event_listener{
		void *event_data;
		int (*test_func)(void *);
		void (*handle_func)(void *);
		int recurrent;
		int count;
	} event_listener;

	typedef struct _event_node{
		struct _event_listener *event;
		struct _event_node *next;
		struct _event_node *prev;
	};

	public:
		struct timeOutStruct {
			unsigned long next;
			unsigned int interval;
		};
		EventLib();
		void add_listener(void *, int (*)(void *), void (*)(void *));
		void add_listener(void *, int (*)(void *), void (*)(void *), int);
		void set_timeout_to_run(unsigned int, void(*)(void *));
		void set_timeout_to_run(unsigned int, void(*)(void *), int);
		void set_timeout_to_recurrently_run(unsigned int, void(*)(void *));
		void event_loop();
	private:
		event_listener *listeners[10];
		struct _event_node *first_event;
		struct _event_node *last_event;
		void check_and_handle_event(struct _event_node *);
		void add_listener_to_list(struct _event_listener *);
		struct _event_listener * create_event_listener(void *, int (*)(void *), void (*)(void *));
		struct _event_listener * create_event_listener(void *, int (*)(void *), void (*)(void *), int);
		void remove_listener_from_list(struct _event_node *);
};

#endif
