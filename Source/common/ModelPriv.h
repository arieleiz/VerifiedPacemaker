/*
*  Pacemaker4Mbed (c) 2014 Ariel Eizenberg - arieleiz@seas.upenn.edu.
*  Developed for UPENN CIS 541 course project - verifiable pacemaker.
*
*    This file is part of Pacemaker4Mbed. You can redistribute it and/or modify
*    it under the terms of the Academic Free License 3.0
*	  (http://opensource.org/licenses/AFL-3.0).
*
*/
#pragma once

#include <mbed.h>
#include <rtos.h>

#ifdef DISPATCH_DEBUG
#define DBGSTR(str)	str
#else
#define DBGSTR(str)	NULL
#endif

namespace ModelImpl
{
	// forward declarations
	struct STATE_ENTRY_DATA;
	struct STATE_ENTRY;
	struct TRANSITION_ENTRY_DATA;
	struct TRANSITION_ENTRY;
	struct PROCESS_ENTRY_DATA;
	struct SYNCHRONIZATION_CHANNEL_DATA;
	class SYNCHRONIZATION_CHANNEL_STATE;
	struct SYNCHRONIZATION_CHANNEL;
	struct PROCESS_ENTRY_STATE_DATA;
	struct PROCESS_ENTRY_STATE;
	struct PROCESS_ENTRY;
	class clk;
	class DispatcherPrivate;

	enum ProcessState
	{
		ProcessInit, ProcessRunning, ProcessWaiting, ProcessDeadlocked
	};

	enum StateMode
	{ 
		StateNormal = 0x1, StateUrgent = 0x2, StateCommited = 0x4 
	};

	typedef bool(*func_guard_t)();
	typedef void(*func_update_t)();

	// structs
	class clk
	{
	public:
		static inline int get_tick();

		inline clk() {
			_value = get_tick();
		}

		inline operator int() const {
			return get_tick() - _value;
		}

		inline void zero() {
			_value = get_tick();
		}

		inline void operator = (int value)
		{
			_value = get_tick() - value;
		}

	private:
		volatile unsigned int _value;
	};

	// Private parts of dispatcher
	class DispatcherPrivate
	{
	private:
		friend class Dispatcher;
		friend class clk;
		friend struct PROCESS_ENTRY_THREAD;

		static DispatcherPrivate INSTANCE;

#ifdef DISPATCH_DEBUG
		void init_debug(Serial* dbgserial, Mutex* dbgmutex);
#else
		void init_debug(Serial* , Mutex* ) { }
#endif

		void set_clock_multiplier(int multiplier);
		void add_process(PROCESS_ENTRY_STATE* ps);
		void run();
		static void run_main(const void* arg);

	private: // time
		unsigned inline int get_tick() {
			return _tick;
		}

	private: // channels
		void fire_channel(SYNCHRONIZATION_CHANNEL* channel);
		void set_channel_fire_cb(SYNCHRONIZATION_CHANNEL* channel, void(*func)(SYNCHRONIZATION_CHANNEL* me), void* context);
		void set_channel_action(SYNCHRONIZATION_CHANNEL* channel, PinName pin, SendChannelMode mode, int pulse_length_us = 0);
		void disable_channel_actions(SYNCHRONIZATION_CHANNEL* channel);
		void set_receive_input(SYNCHRONIZATION_CHANNEL* channel, PinName pin, PinMode pull, ReceiveChannelMode rcmode);
		void connect_serial_char(SYNCHRONIZATION_CHANNEL* channel, char c);

	private: // states
		void set_state_enter_cb(STATE_ENTRY* state, void(*func)(STATE_ENTRY* me), void* context);
		void set_state_leave_cb(STATE_ENTRY* state, void(*func)(STATE_ENTRY* me), void* context);

	private: // transitions
		void set_state_pre_execute_cb(TRANSITION_ENTRY* transition, void(*func)(TRANSITION_ENTRY* me), void* context);
		void set_state_post_execute_cb(TRANSITION_ENTRY* transition, void(*func)(TRANSITION_ENTRY* me), void* context);

	private: // processes
		void set_process_start_cb(PROCESS_ENTRY* entry, void(*func)(PROCESS_ENTRY* me), void* context);

	private:
		DispatcherPrivate();

		void run_steps();
		void clear_signals();
		void clear_normal_step();
		void run_tasks(int types);

		void task_main_imp(volatile PROCESS_ENTRY_STATE* process);
		TRANSITION_ENTRY* find_valid_transition(volatile PROCESS_ENTRY_STATE* process);
		bool can_take_transition(volatile PROCESS_ENTRY_STATE* process, TRANSITION_ENTRY* transition);
		void take_transition(volatile PROCESS_ENTRY_STATE* process, TRANSITION_ENTRY* transition);
		void enter_state(volatile PROCESS_ENTRY_STATE* process, STATE_ENTRY* state);
		void leave_state(volatile PROCESS_ENTRY_STATE* process, STATE_ENTRY* state);

	private: // callbacks
		static void task_main(const void* arg);
		static void ticker_handler();
		static void channel_irq_handler(uint32_t id, gpio_irq_event event);

	private: // clock helpers
		int add_time(int timecur, int timespan);
		int sub_time(int timecur, int timeprev);
		int wall_add(int timespan);
		int wall_diff(int timeprev);
	private:
#ifdef DISPATCH_DEBUG
		Mutex*   _dbglock;
		Serial* _dbgout;
#endif
		int _cnt_proc;
		Mutex _lock;
		Timer _wall;
		osThreadId _dispid;

		Ticker _ticker;
		int _clock_multiplier;
		int _tick;
	};

	class DelayedThread {
	public:
		DelayedThread()
		{
		}

		DelayedThread(void(*task)(void const *argument), void *argument = NULL,
			osPriority priority = osPriorityNormal)
		{
			init(task, argument, priority);
		}

		void init(void(*task)(void const *argument), void *argument = NULL,
			osPriority priority = osPriorityNormal)
		{
			_thread_def.pthread = task;
			_thread_def.tpriority = priority;
			_thread_def.stacksize = DEFAULT_STACK_SIZE;
			_thread_def.stack_pointer = (unsigned char*)malloc(DEFAULT_STACK_SIZE / 3);
			if (_thread_def.stack_pointer == NULL)
			{
				wait(1000000);
			}
			_started = false;
			_argument = argument;
		}

		void start()
		{
			if (_thread_def.pthread == NULL)
				return;

				_started = true;
			_tid = osThreadCreate(&_thread_def, _argument);
		}

		osStatus terminate()
		{
			if (!_started)
				return osOK;
			_started = false;
			osStatus status = osThreadTerminate(_tid);
			free(_stack);
			return status;
		}

		osStatus set_priority(osPriority priority){
			return osThreadSetPriority(_tid, priority);
		}

		osPriority get_priority(){
			return osThreadGetPriority(_tid);
		}

		int32_t signal_set(int32_t signals){
			return osSignalSet(_tid, signals);
		}

		enum State {
			Inactive,           /**< Not created or terminated */
			Ready,              /**< Ready to run */
			Running,            /**< Running */
			WaitingDelay,       /**< Waiting for a delay to occur */
			WaitingInterval,    /**< Waiting for an interval to occur */
			WaitingOr,          /**< Waiting for one event in a set to occur */
			WaitingAnd,         /**< Waiting for multiple events in a set to occur */
			WaitingSemaphore,   /**< Waiting for a semaphore event to occur */
			WaitingMailbox,     /**< Waiting for a mailbox event to occur */
			WaitingMutex,       /**< Waiting for a mutex event to occur */
		};

		State get_state(){
			return ((State)_thread_def.tcb.state);
		}

		static osEvent signal_wait(int32_t signals, uint32_t millisec = osWaitForever){
			return osSignalWait(signals, millisec);
		}

		static osStatus wait(uint32_t millisec){
			return osDelay(millisec);
		}

		static osStatus yield(){
			return osThreadYield();
		}

		static osThreadId gettid(){
			return osThreadGetId();
		}

		~DelayedThread()
		{
			terminate();
		}

	private:
		osThreadId _tid;
		osThreadDef_t _thread_def;
		unsigned char *_stack;
		bool _started;
		void* _argument;
	};

	inline int clk::get_tick() {
		return DispatcherPrivate::INSTANCE.get_tick();
	}

	struct SYNCHRONIZATION_CHANNEL
	{
		const SYNCHRONIZATION_CHANNEL_DATA* data;
		void(*fired)(SYNCHRONIZATION_CHANNEL* channel);
		void* context;
	};

	struct STATE_ENTRY
	{
		const STATE_ENTRY_DATA* data;
		void(*state_enter)(STATE_ENTRY* me);
		void(*state_leave)(STATE_ENTRY* me);
		void* context;
	};
	struct TRANSITION_ENTRY
	{
		const TRANSITION_ENTRY_DATA* data;
		void(*transition_pre_execute)(TRANSITION_ENTRY* me);
		void(*transition_post_execute)(TRANSITION_ENTRY* me);
		void* context;
	};

	struct PROCESS_ENTRY
	{
		const PROCESS_ENTRY_DATA* data;
		void(*process_start)(PROCESS_ENTRY* me);
		void* context;
	};

	static const int MAX_SEND_DATA = 2;

	class SendData : TimerEvent
	{
	public:
		PinName pin;
		SendChannelMode mode;
		int pulse_length_us;
		gpio_t gpio;
		int last_write;

	public:
		SendData()
		{
			pin = NC;
			mode = SendChannelModeNone;
			last_write = 0;
			memset(&gpio, 0, sizeof(gpio));
		}
		virtual ~SendData() { }


		void insert(timestamp_t t) {
			TimerEvent::insert(t);
		}

		virtual void handler();
	};

	class RecvData
	{
	public:
		ReceiveChannelMode mode;
		gpio_irq_t gpio_irq;
		gpio_t gpio;

	public:
		RecvData()
		{
			memset(&gpio, 0, sizeof(gpio));
			memset(&gpio_irq, 0, sizeof(gpio_irq));
			mode = ReceiveChannelModeNone;
		}
		~RecvData() { }
	};

	class SYNCHRONIZATION_CHANNEL_STATE
	{
	public:
		osMutexId mutexId;
		osMutexDef_t mutexDef;
		SendData send[MAX_SEND_DATA];
		RecvData recv;

		bool fired;
		bool prev_fired;


	public: 
		SYNCHRONIZATION_CHANNEL_STATE() 
		{
			fired = prev_fired = false;
		}
	};

	struct SYNCHRONIZATION_CHANNEL_DATA
	{
		const char* name;
		bool urgent;
		bool broadcast;
		SYNCHRONIZATION_CHANNEL_STATE* state;
	};

	struct STATE_ENTRY_DATA
	{
		const char* name;
		TRANSITION_ENTRY* const* transitions;
		StateMode mode;
		func_guard_t guard;
	};

	struct TRANSITION_ENTRY_DATA
	{
		const char* name;
		STATE_ENTRY* target;
		SYNCHRONIZATION_CHANNEL *receive;
		SYNCHRONIZATION_CHANNEL *send;
		func_guard_t	guard;
		func_update_t   update;
	};

	struct PROCESS_ENTRY_THREAD
	{
		PROCESS_ENTRY_THREAD(PROCESS_ENTRY_STATE* p) : thread(DispatcherPrivate::task_main, p) { }

		DelayedThread	    thread;
		Mutex		mutex;

	private:
		PROCESS_ENTRY_THREAD(const PROCESS_ENTRY_THREAD& other);
		PROCESS_ENTRY_THREAD& operator=(const PROCESS_ENTRY_THREAD& other);
	};

	struct PROCESS_ENTRY_DATA
	{
		const char* name;
		//STATE_ENTRY* const* states;
		STATE_ENTRY* initstate;
		PROCESS_ENTRY_THREAD* threadptr;
	};

	struct PROCESS_ENTRY_STATE
	{
		PROCESS_ENTRY* process;
		PROCESS_ENTRY_THREAD* thread;
		const PROCESS_ENTRY_DATA* data;
		STATE_ENTRY* current;
		TRANSITION_ENTRY* next;
		bool stepped;
		bool normal_stepped;
		ProcessState state;
	};

};

using ModelImpl::clk;
