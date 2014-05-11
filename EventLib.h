#ifndef EVENTLIB_H
#define EVENTLIB_H
#include "Arduino.h"

class EventLib {
	#define RECURRENT_EVENT		0b0001
	#define BOUNCE_RETRY_TIME	50

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
		struct pinStatus {
			int pin_num;
			byte status_to_run;
			byte last_status;
			unsigned int bounce_retry;
			void (*cb)(void *);
			EventLib *obj;
		};
		EventLib();
		void add_listener(void *, int (*)(void *), void (*)(void *), int);
		void add_listener(void *, int (*)(void *), void (*)(void *));
		void set_timeout_to_run(unsigned int, void(*)(void *), int);
		void set_timeout_to_run(unsigned int, void(*)(void *));
		void recurrently_set_timeout_to_run(unsigned int, void(*)(void *));
		void run_on_status(int, byte, void(*)(void *), int);
		void run_on_status(int, byte, void(*)(void *));
		void recurrently_run_on_status(int, byte, void(*)(void *));
		void run_on_unbounced_status(int, byte, void(*)(void *), int);
		void run_on_unbounced_status(int, byte, void(*)(void *));
		void recurrently_run_on_unbounced_status(int, byte, void(*)(void *));
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
