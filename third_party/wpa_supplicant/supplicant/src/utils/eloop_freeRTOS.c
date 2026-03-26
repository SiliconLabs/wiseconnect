/*
 * Event loop - empty template (basic structure, but no OS specific operations)
 * Copyright (c) 2002-2005, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "includes.h"

#include "common.h"
#include "eloop.h"
#include "sl_dl_list.h"

struct eloop_data eloop;

int eloop_init(void)
{
	memset(&eloop, 0, sizeof(eloop));
	os_time_t init_time;
	//Calling os_get_time to initialize the time
	os_get_time(&init_time);
	return 0;
}

int eloop_register_read_sock(int sock,
			     void (*handler)(int sock, void *eloop_ctx,
					     void *sock_ctx),
			     void *eloop_data, void *user_data)
{
	struct eloop_sock *tmp;

	tmp = (struct eloop_sock *)
		os_realloc(eloop.readers,
			(eloop.reader_count + 1) * sizeof(struct eloop_sock));
	if (tmp == NULL)
  {
    wpa_printf(1, "OBE:PREAUTH : tmp is not initialized\n");    
		return -1;
  }

	tmp[eloop.reader_count].sock = sock;
	tmp[eloop.reader_count].eloop_data = eloop_data;
	tmp[eloop.reader_count].user_data = user_data;
	tmp[eloop.reader_count].handler = handler;
	eloop.reader_count++;
	eloop.readers = tmp;
	if (sock > eloop.max_sock)
		eloop.max_sock = sock;
	eloop.reader_table_changed = 1;

	return 0;
}

void eloop_unregister_read_sock(int sock)
{
	int i;

	if (eloop.readers == NULL || eloop.reader_count == 0)
		return;

	for (i = 0; i < eloop.reader_count; i++) {
		if (eloop.readers[i].sock == sock)
			break;
	}
	if (i == eloop.reader_count)
		return;
	if (i != eloop.reader_count - 1) {
		os_memmove(&eloop.readers[i], &eloop.readers[i + 1],
			(eloop.reader_count - i - 1) *
			sizeof(struct eloop_sock));
	}
	eloop.reader_count--;
	eloop.reader_table_changed = 1;
}


//#ifdef SUPPLICANT_ROM
STATIC void eloop_process_pending_signals(void)_OBE_LOW_POWER_RAM_
STATIC void eloop_process_pending_signals(void)
{
	int i;
	if (eloop.signaled == 0)
		return;

  eloop.signaled = 0;

	if (eloop.pending_terminate) {
		eloop.pending_terminate = 0;
	}

	for (i = 0; i < eloop.signal_count; i++) {
		if (eloop.signals[i].signaled) {
			eloop.signals[i].signaled = 0;
			eloop.signals[i].handler(eloop.signals[i].sig,
						 eloop.signals[i].user_data);
		}
	}
}
int os_time_before_ram(struct os_time *a, struct os_time *b) _OBE_LOW_POWER_RAM_
int os_time_before_ram(struct os_time *a, struct os_time *b)
{
	return (a->sec < b->sec) ||
		(a->sec == b->sec && a->usec < b->usec);
}

void eloop_run(void)
{
	struct eloop_timeout *tmp = NULL;
	struct os_reltime tv, now;
	tv.sec = tv.usec = 0;
	now.sec = now.usec = 0;

	while (!eloop.terminate &&
		(eloop.timeout || eloop.reader_count > 0)) {
		if (eloop.timeout) {
			os_get_reltime(&now);
			if (os_reltime_before(&now, &eloop.timeout->time)){
				os_reltime_sub(&eloop.timeout->time, &now, &tv);
			} else
				tv.sec = tv.usec = 0;
			#ifdef SUPPLICANT_PORTING
			// If Eloop Thread Stop flag is set, then return without further processing
			if (os_wait_for_thread_stop_flag(tv.sec, tv.usec)) {
				return;
			}
			#endif
		}
		
		eloop_process_pending_signals();
		
		/* check if some registered timeouts have occurred */
		if (eloop.timeout) {
		tmp = NULL;
			os_get_reltime(&now);
			if (!os_reltime_before(&now, &eloop.timeout->time)) {
				tmp = eloop.timeout;
				eloop.timeout = eloop.timeout->next;
				tmp->handler(tmp->eloop_data,
					     tmp->user_data);
				os_free(tmp);
			}
		}

		eloop.reader_table_changed = 0;
	}

	return;
}

void trigger_eloop_run()
{
	//Clear event to trigger supplicant event loop
	if (sli_clear_event_supplicant_eloop() != SL_STATUS_OK) {
		SL_MGMT_ASSERT(0);
	}

	if ((!eloop.terminate) && (eloop.timeout || (eloop.reader_count > 0)))
	{	
		eloop_run();
	}
}

int eloop_register_timeout(unsigned int secs, unsigned int usecs,
			   void (*handler)(void *eloop_ctx, void *timeout_ctx),
			   void *eloop_data, void *user_data)
{
	struct eloop_timeout *timeout, *tmp, *prev;
	timeout = (struct eloop_timeout *)os_malloc(sizeof(*timeout));
	uint32 ipl_reg = 0;

	if (timeout == NULL)
		return -1;
	os_get_time(&timeout->time);
	timeout->time.sec += secs;
	timeout->time.usec += usecs;
	while (timeout->time.usec >= 1000000) {
		timeout->time.sec++;
		timeout->time.usec -= 1000000;
	}
	timeout->eloop_data = eloop_data;
	timeout->user_data = user_data;
	timeout->handler = handler;
	timeout->next = NULL;

	if (eloop.timeout == NULL) {
		CPF_CORE_ENTER_ATOMIC();
		eloop.timeout = timeout;
		CPF_CORE_EXIT_ATOMIC();
		goto set_eloop_trigger;
	}
	CPF_CORE_ENTER_ATOMIC();
	prev = NULL;
	tmp = eloop.timeout;
	
	while (tmp != NULL) {
		if (os_time_before_ram(&timeout->time, &tmp->time)) //assending order
			break;
		prev = tmp;
		tmp = tmp->next;
	}

	if (prev == NULL) {
		timeout->next = eloop.timeout;
		eloop.timeout = timeout;
	} else {
		timeout->next = prev->next;
		prev->next = timeout;
	}
	CPF_CORE_EXIT_ATOMIC();

set_eloop_trigger:
	//Set event to trigger supplicant event loop
	if (sli_set_event_supplicant_eloop() != SL_STATUS_OK) {
		SL_MGMT_ASSERT(0);
	}
	return 0;
}


int eloop_cancel_timeout(void (*handler)(void *eloop_ctx, void *sock_ctx),
			 void *eloop_data, void *user_data)
{
	struct eloop_timeout *timeout, *prev, *next;
	int removed = 0;
	CPF_CORE_ENTER_ATOMIC();
	prev = NULL;
	timeout = eloop.timeout;
	while (timeout != NULL) {
		next = timeout->next;

		if (timeout->handler == handler &&
		    (timeout->eloop_data == eloop_data ||
		     eloop_data == ELOOP_ALL_CTX) &&
		    (timeout->user_data == user_data ||
		     user_data == ELOOP_ALL_CTX)) {
			if (prev == NULL)
				eloop.timeout = next;
			else
				prev->next = next;
#ifdef ENABLE_UMAC_ROM_PTR
			OS_FREE(timeout);
#else
			os_free(timeout);
#endif /* ENABLE_UMAC_ROM_PTR */
			removed++;
		} else
			prev = timeout;

		timeout = next;
	}
	
	CPF_CORE_EXIT_ATOMIC();
	return removed;
}

int eloop_deplete_timeout(unsigned int req_secs, unsigned int req_usecs,
			  eloop_timeout_handler handler, void *eloop_data,
			  void *user_data)
{
	struct os_reltime now, requested, remaining;
	struct eloop_timeout *tmp;
	tmp = eloop.timeout;

//	dl_list_for_each(tmp, &eloop.timeout, struct eloop_timeout, list) {
		while (tmp != NULL) {

		if (tmp->handler == handler &&
		    tmp->eloop_data == eloop_data &&
		    tmp->user_data == user_data) {
			requested.sec = req_secs;
			requested.usec = req_usecs;
			os_get_reltime(&now);
			os_reltime_sub(&tmp->time, &now, &remaining);
			if (os_reltime_before(&requested, &remaining)) {
				eloop_cancel_timeout(handler, eloop_data,
						     user_data);
				eloop_register_timeout(requested.sec,
						       requested.usec,
						       handler, eloop_data,
						       user_data);
				return 1;
			}
			return 0;
		}

		tmp = tmp->next ;

		}

	return -1;
}

int eloop_is_timeout_registered(void (*handler)(void *eloop_ctx,
						void *timeout_ctx),
				void *eloop_data, void *user_data)
{
	struct eloop_timeout *tmp;

	tmp = eloop.timeout;

	while (tmp != NULL) {
		if (tmp->handler == handler &&
		    tmp->eloop_data == eloop_data &&
		    tmp->user_data == user_data)
			return 1;

		tmp = tmp->next;
	}

	return 0;
}

/* TODO: replace with suitable signal handler */
#if 0
static void eloop_handle_signal(int sig)
{
	int i;

	eloop.signaled++;
	for (i = 0; i < eloop.signal_count; i++) {
		if (eloop.signals[i].sig == sig) {
			eloop.signals[i].signaled++;
			break;
		}
	}
}
#endif

int eloop_register_signal(int sig,
			  void (*handler)(int sig, void *signal_ctx),
			  void *user_data)
{
	struct eloop_signal *tmp;

	tmp = (struct eloop_signal *)
		realloc(eloop.signals,
			(eloop.signal_count + 1) *
			sizeof(struct eloop_signal));
	if (tmp == NULL)
		return -1;

	tmp[eloop.signal_count].sig = sig;
	tmp[eloop.signal_count].user_data = user_data;
	tmp[eloop.signal_count].handler = handler;
	tmp[eloop.signal_count].signaled = 0;
	eloop.signal_count++;
	eloop.signals = tmp;

	/* TODO: register signal handler */

	return 0;
}


int eloop_register_signal_terminate(void (*handler)(int sig, void *signal_ctx),
				    void *user_data)
{
#if 0
	/* TODO: for example */
	int ret = eloop_register_signal(SIGINT, handler, user_data);
	if (ret == 0)
		ret = eloop_register_signal(SIGTERM, handler, user_data);
	return ret;
#endif
	return 0;
}


int eloop_register_signal_reconfig(void (*handler)(int sig, void *signal_ctx),
				   void *user_data)
{
#if 0
	/* TODO: for example */
	return eloop_register_signal(SIGHUP, handler, user_data);
#endif
	return 0;
}

void eloop_terminate(void)
{
	eloop.terminate = 1;
}

void eloop_destroy(void)
{
	struct eloop_timeout *timeout, *prev;
	CPF_CORE_ENTER_ATOMIC();
	timeout = eloop.timeout;
	while (timeout != NULL) {
		prev = timeout;
		timeout = timeout->next;
#ifdef ENABLE_UMAC_ROM_PTR
		OS_FREE(prev);
#else
		os_free(prev);
#endif /* ENABLE_UMAC_ROM_PTR */
	}
	eloop.timeout = 0; /* K60_PORTING */
#ifdef ENABLE_UMAC_ROM_PTR
	OS_FREE(eloop.readers);
	OS_FREE(eloop.signals);
#else
	os_free(eloop.readers);
	os_free(eloop.signals);
#endif /* ENABLE_UMAC_ROM_PTR */
	
	eloop_terminate(); /* K60_PORTING */
	CPF_CORE_EXIT_ATOMIC();
}


int eloop_terminated(void)
{
	return eloop.terminate;
}


void eloop_wait_for_read_sock(int sock)
{
	/*
	 * TODO: wait for the file descriptor to have something available for
	 * reading
	 */
}

/**
*$Log: eloop_none.c,v $
*Revision 1.1.1.1  2012/02/28 07:14:12  kiran.gunda
*WiSe Connet initial import
*
**/ 
