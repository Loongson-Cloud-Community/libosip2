/*
  The oSIP library implements the Session Initiation Protocol (SIP -rfc3261-)
  Copyright (C) 2001,2002,2003  Aymeric MOIZARD jack@atosc.org
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <osip2/internal.h>
#include <osip2/osip.h>

#include "fsm.h"
#include "xixt.h"

#ifdef OSIP_MT
static struct osip_mutex *ict_fastmutex;
static struct osip_mutex *ist_fastmutex;
static struct osip_mutex *nict_fastmutex;
static struct osip_mutex *nist_fastmutex;
#endif


int
__osip_global_init ()
{
  /* load the fsm configuration */
  __ict_load_fsm ();
  __ist_load_fsm ();
  __nict_load_fsm ();
  __nist_load_fsm ();

  /* load the parser configuration */
  parser_init ();

  /* give a seed for all calls to rand() */
  srand (time (NULL));
  __osip_init_random_number ();

#ifdef OSIP_MT
  ict_fastmutex = osip_mutex_init ();
  ist_fastmutex = osip_mutex_init ();
  nict_fastmutex = osip_mutex_init ();
  nist_fastmutex = osip_mutex_init ();
#endif
  return 0;
}

void
__osip_global_free ()
{
  __ict_unload_fsm ();
  __ist_unload_fsm ();
  __nict_unload_fsm ();
  __nist_unload_fsm ();

#ifdef OSIP_MT
  osip_mutex_destroy (ict_fastmutex);
  osip_mutex_destroy (ist_fastmutex);
  osip_mutex_destroy (nict_fastmutex);
  osip_mutex_destroy (nist_fastmutex);
#endif
}

int
osip_ict_lock (osip_t * osip)
{
#ifdef OSIP_MT
  return osip_mutex_lock (ict_fastmutex);
#else
  return 0;
#endif
}

int
osip_ict_unlock (osip_t * osip)
{
#ifdef OSIP_MT
  return osip_mutex_unlock (ict_fastmutex);
#else
  return 0;
#endif
}

int
osip_ist_lock (osip_t * osip)
{
#ifdef OSIP_MT
  return osip_mutex_lock (ist_fastmutex);
#else
  return 0;
#endif
}

int
osip_ist_unlock (osip_t * osip)
{
#ifdef OSIP_MT
  return osip_mutex_unlock (ist_fastmutex);
#else
  return 0;
#endif
}

int
osip_nict_lock (osip_t * osip)
{
#ifdef OSIP_MT
  return osip_mutex_lock (nict_fastmutex);
#else
  return 0;
#endif
}

int
osip_nict_unlock (osip_t * osip)
{
#ifdef OSIP_MT
  return osip_mutex_unlock (nict_fastmutex);
#else
  return 0;
#endif
}

int
osip_nist_lock (osip_t * osip)
{
#ifdef OSIP_MT
  return osip_mutex_lock (nist_fastmutex);
#else
  return 0;
#endif
}

int
osip_nist_unlock (osip_t * osip)
{
#ifdef OSIP_MT
  return osip_mutex_unlock (nist_fastmutex);
#else
  return 0;
#endif
}

int
__osip_add_ict (osip_t * osip, osip_transaction_t * ict)
{
#ifdef OSIP_MT
  osip_mutex_lock (ict_fastmutex);
#endif
  osip_list_add (osip->osip_ict_transactions, ict, -1);
#ifdef OSIP_MT
  osip_mutex_unlock (ict_fastmutex);
#endif
  return 0;
}

int
__osip_add_ist (osip_t * osip, osip_transaction_t * ist)
{
#ifdef OSIP_MT
  osip_mutex_lock (ist_fastmutex);
#endif
  osip_list_add (osip->osip_ist_transactions, ist, -1);
#ifdef OSIP_MT
  osip_mutex_unlock (ist_fastmutex);
#endif
  return 0;
}

int
__osip_add_nict (osip_t * osip, osip_transaction_t * nict)
{
#ifdef OSIP_MT
  osip_mutex_lock (nict_fastmutex);
#endif
  osip_list_add (osip->osip_nict_transactions, nict, -1);
#ifdef OSIP_MT
  osip_mutex_unlock (nict_fastmutex);
#endif
  return 0;
}

int
__osip_add_nist (osip_t * osip, osip_transaction_t * nist)
{
#ifdef OSIP_MT
  osip_mutex_lock (nist_fastmutex);
#endif
  osip_list_add (osip->osip_nist_transactions, nist, -1);
#ifdef OSIP_MT
  osip_mutex_unlock (nist_fastmutex);
#endif
  return 0;
}

int osip_remove_transaction (osip_t * osip, osip_transaction_t * tr)
{
  int i = -1;
  if (tr==NULL)
    return -1;
  if (tr->ctx_type==ICT)
    i = __osip_remove_ict_transaction(osip, tr);
  else if (tr->ctx_type==IST)
    i = __osip_remove_ist_transaction(osip, tr);
  else if (tr->ctx_type==NICT)
    i = __osip_remove_nict_transaction(osip, tr);
  else if (tr->ctx_type==NIST)
    i = __osip_remove_nist_transaction(osip, tr);
  else return -1;
  return i;
}

int
__osip_remove_ict_transaction (osip_t * osip, osip_transaction_t * ict)
{
  int pos = 0;
  osip_transaction_t *tmp;

#ifdef OSIP_MT
  osip_mutex_lock (ict_fastmutex);
#endif
  while (!osip_list_eol (osip->osip_ict_transactions, pos))
    {
      tmp = osip_list_get (osip->osip_ict_transactions, pos);
      if (tmp->transactionid == ict->transactionid)
	{
	  osip_list_remove (osip->osip_ict_transactions, pos);
#ifdef OSIP_MT
	  osip_mutex_unlock (ict_fastmutex);
#endif
	  return 0;
	}
      pos++;
    }
#ifdef OSIP_MT
  osip_mutex_unlock (ict_fastmutex);
#endif
  return -1;
}

int
__osip_remove_ist_transaction (osip_t * osip, osip_transaction_t * ist)
{
  int pos = 0;
  osip_transaction_t *tmp;

#ifdef OSIP_MT
  osip_mutex_lock (ist_fastmutex);
#endif
  while (!osip_list_eol (osip->osip_ist_transactions, pos))
    {
      tmp = osip_list_get (osip->osip_ist_transactions, pos);
      if (tmp->transactionid == ist->transactionid)
	{
	  osip_list_remove (osip->osip_ist_transactions, pos);
#ifdef OSIP_MT
	  osip_mutex_unlock (ist_fastmutex);
#endif
	  return 0;
	}
      pos++;
    }
#ifdef OSIP_MT
  osip_mutex_unlock (ist_fastmutex);
#endif
  return -1;
}

int
__osip_remove_nict_transaction (osip_t * osip, osip_transaction_t * nict)
{
  int pos = 0;
  osip_transaction_t *tmp;

#ifdef OSIP_MT
  osip_mutex_lock (nict_fastmutex);
#endif
  while (!osip_list_eol (osip->osip_nict_transactions, pos))
    {
      tmp = osip_list_get (osip->osip_nict_transactions, pos);
      if (tmp->transactionid == nict->transactionid)
	{
	  osip_list_remove (osip->osip_nict_transactions, pos);
#ifdef OSIP_MT
	  osip_mutex_unlock (nict_fastmutex);
#endif
	  return 0;
	}
      pos++;
    }
#ifdef OSIP_MT
  osip_mutex_unlock (nict_fastmutex);
#endif
  return -1;
}

int
__osip_remove_nist_transaction (osip_t * osip, osip_transaction_t * nist)
{
  int pos = 0;
  osip_transaction_t *tmp;

#ifdef OSIP_MT
  osip_mutex_lock (nist_fastmutex);
#endif
  while (!osip_list_eol (osip->osip_nist_transactions, pos))
    {
      tmp = osip_list_get (osip->osip_nist_transactions, pos);
      if (tmp->transactionid == nist->transactionid)
	{
	  osip_list_remove (osip->osip_nist_transactions, pos);
#ifdef OSIP_MT
	  osip_mutex_unlock (nist_fastmutex);
#endif
	  return 0;
	}
      pos++;
    }
#ifdef OSIP_MT
  osip_mutex_unlock (nist_fastmutex);
#endif
  return -1;
}

#if 0
/* this method is made obsolete because it contains bugs and is also
   too much limited.
   any call to this method should be replace this way:

   //osip_distribute(osip, evt);
   osip_transaction_t *transaction = osip_find_transaction_and_add_event(osip, evt);

   if (i!=0) // in case it's a new request
     {
        if (evt is an ACK)
            evt could be an ACK for INVITE (not handled by oSIP)
        else if ( evt is a 200 for INVITE)
           evt could be a retransmission of a 200 for INVITE (not handled by oSIP)
        else if (evt is a new request)  == not a ACK and not a response
	  {
           transaction = osip_create_transaction(osip, evt);
           if (transaction==NULL)
             printf("failed to create a transaction\");
          }
    }
    else
    {
    // here, the message as been taken by the stack.
    }
*/


/* finds the transaction context and add the sipevent in its fifo. */
/* USED ONLY BY THE TRANSPORT LAYER.                               */
/* INPUT : osip_t *osip | osip. contains the list of tr. context*/
/* INPUT : osip_event_t* sipevent | event to dispatch.               */
osip_transaction_t *
osip_distribute_event (osip_t * osip, osip_event_t * evt)
{
  osip_transaction_t *transaction = NULL;
  int i;
  osip_fsm_type_t ctx_type;

  if (EVT_IS_INCOMINGMSG (evt))
    {
      /* event is for ict */
      if (MSG_IS_REQUEST (evt->sip))
	{
	  if (0 == strcmp (evt->sip->cseq->method, "INVITE")
	      || 0 == strcmp (evt->sip->cseq->method, "ACK"))
	    {
#ifdef OSIP_MT
	      osip_mutex_lock (ist_fastmutex);
#endif
	      transaction =
		osip_transaction_find (osip->osip_ist_transactions, evt);
#ifdef OSIP_MT
	      osip_mutex_unlock (ist_fastmutex);
#endif
	    }
	  else
	    {
#ifdef OSIP_MT
	      osip_mutex_lock (nist_fastmutex);
#endif
	      transaction =
		osip_transaction_find (osip->osip_nist_transactions, evt);
#ifdef OSIP_MT
	      osip_mutex_unlock (nist_fastmutex);
#endif
	    }
	}
      else
	{
	  if (0 == strcmp (evt->sip->cseq->method, "INVITE")
	      || 0 == strcmp (evt->sip->cseq->method, "ACK"))
	    {
#ifdef OSIP_MT
	      osip_mutex_lock (ict_fastmutex);
#endif
	      transaction =
		osip_transaction_find (osip->osip_ict_transactions, evt);
#ifdef OSIP_MT
	      osip_mutex_unlock (ict_fastmutex);
#endif
	    }
	  else
	    {
#ifdef OSIP_MT
	      osip_mutex_lock (nict_fastmutex);
#endif
	      transaction =
		osip_transaction_find (osip->osip_nict_transactions, evt);
#ifdef OSIP_MT
	      osip_mutex_unlock (nict_fastmutex);
#endif
	    }
	}
      if (transaction == NULL)
	{
	  if (EVT_IS_RCV_STATUS_1XX (evt)
	      || EVT_IS_RCV_STATUS_2XX (evt)
	      || EVT_IS_RCV_STATUS_3456XX (evt) || EVT_IS_RCV_ACK (evt))
	    {			/* event MUST be ignored! */
	      /* EXCEPT FOR 2XX THAT MUST BE GIVEN TO THE CORE LAYER!!! */

	      /* TODO */

	      OSIP_TRACE (osip_trace
			  (__FILE__, __LINE__, OSIP_WARNING, NULL,
			   "transaction does not yet exist... %x callid:%s\n",
			   evt, evt->sip->call_id->number));
	      osip_message_free (evt->sip);
	      osip_free (evt);	/* transaction thread will not delete it */
	      return NULL;
	    }

	  /* we create a new context for this incoming request */
	  if (0 == strcmp (evt->sip->cseq->method, "INVITE"))
	    ctx_type = IST;
	  else
	    ctx_type = NIST;

	  i = osip_transaction_init (&transaction, ctx_type, osip, evt->sip);
	  if (i == -1)
	    {
	      osip_message_free (evt->sip);
	      osip_free (evt);	/* transaction thread will not delete it */
	      return NULL;
	    }
	}
      evt->transactionid = transaction->transactionid;

      evt->transactionid = transaction->transactionid;
      osip_fifo_add (transaction->transactionff, evt);
      return transaction;
    }
  else
    {
      OSIP_TRACE (osip_trace
		  (__FILE__, __LINE__, OSIP_BUG, NULL,
		   "wrong event type %x\n", evt));
      return NULL;
    }
}
#endif

int
osip_find_transaction_and_add_event (osip_t * osip, osip_event_t * evt)
{
  osip_transaction_t *transaction = __osip_find_transaction (osip, evt, 1);

  if (transaction == NULL)
    return -1;
  return 0;
}

osip_transaction_t *
osip_find_transaction (osip_t * osip, osip_event_t * evt)
{
#ifdef OSIP_MT
  OSIP_TRACE (osip_trace
	      (__FILE__, __LINE__, OSIP_BUG, NULL,
	       "\n\n\n\nYou are using a multithreaded application, but this method is not allowed! Use osip_find_transaction_add_add_event() instead.\n\n\\n"));
#endif
  return __osip_find_transaction (osip, evt, 0);
}

osip_transaction_t *
__osip_find_transaction (osip_t * osip, osip_event_t * evt, int consume)
{
  osip_transaction_t *transaction = NULL;
  osip_list_t *transactions = NULL;

#ifdef OSIP_MT
  struct osip_mutex *mut = NULL;
#endif

  if (evt == NULL || evt->sip == NULL || evt->sip->cseq == NULL)
    return NULL;

  if (EVT_IS_INCOMINGMSG (evt))
    {
      if (MSG_IS_REQUEST (evt->sip))
	{
	  if (0 == strcmp (evt->sip->cseq->method, "INVITE")
	      || 0 == strcmp (evt->sip->cseq->method, "ACK"))
	    {
	      transactions = osip->osip_ist_transactions;
#ifdef OSIP_MT
	      mut = ist_fastmutex;
#endif
	    }
	  else
	    {
	      transactions = osip->osip_nist_transactions;
#ifdef OSIP_MT
	      mut = nist_fastmutex;
#endif
	    }
	}
      else
	{
	  if (0 == strcmp (evt->sip->cseq->method, "INVITE")
	      || 0 == strcmp (evt->sip->cseq->method, "ACK"))
	    {
	      transactions = osip->osip_ict_transactions;
#ifdef OSIP_MT
	      mut = ict_fastmutex;
#endif
	    }
	  else
	    {
	      transactions = osip->osip_nict_transactions;
#ifdef OSIP_MT
	      mut = nict_fastmutex;
#endif
	    }
	}
    }
  else if (EVT_IS_OUTGOINGMSG (evt))
    {
      if (MSG_IS_RESPONSE (evt->sip))
	{
	  if (0 == strcmp (evt->sip->cseq->method, "INVITE")
	      || 0 == strcmp (evt->sip->cseq->method, "ACK"))
	    {
	      transactions = osip->osip_ist_transactions;
#ifdef OSIP_MT
	      mut = ist_fastmutex;
#endif
	    }
	  else
	    {
	      transactions = osip->osip_nist_transactions;
#ifdef OSIP_MT
	      mut = nist_fastmutex;
#endif
	    }
	}
      else
	{
	  if (0 == strcmp (evt->sip->cseq->method, "INVITE")
	      || 0 == strcmp (evt->sip->cseq->method, "ACK"))
	    {
	      transactions = osip->osip_ict_transactions;
#ifdef OSIP_MT
	      mut = ict_fastmutex;
#endif
	    }
	  else
	    {
	      transactions = osip->osip_nict_transactions;
#ifdef OSIP_MT
	      mut = nict_fastmutex;
#endif
	    }
	}
    }
  if (transactions == NULL)
    return NULL;		/* not a message??? */

#ifdef OSIP_MT
  osip_mutex_lock (mut);
#endif
  transaction = osip_transaction_find (transactions, evt);
  if (consume == 1)
    {				/* we add the event before releasing the mutex!! */
      if (transaction != NULL)
	{
	  osip_transaction_add_event (transaction, evt);
#ifdef OSIP_MT
	  osip_mutex_unlock (mut);
#endif
	  return transaction;
	}
    }
#ifdef OSIP_MT
  osip_mutex_unlock (mut);
#endif

  return transaction;
}

osip_transaction_t *
osip_create_transaction (osip_t * osip, osip_event_t * evt)
{
  osip_transaction_t *transaction;
  int i;
  osip_fsm_type_t ctx_type;

  if (evt==NULL)
    return NULL;
  if (evt->sip==NULL)
    return NULL;

  /* make sure the request's method reflect the cseq value. */
  if (MSG_IS_REQUEST (evt->sip))
    {
      /* delete request where cseq method does not match
	 the method in request-line */
      if (evt->sip->cseq==NULL
	  || evt->sip->cseq->method==NULL || evt->sip->sip_method==NULL)
	{
	  return NULL;
	}
      if (0 != strcmp (evt->sip->cseq->method, evt->sip->sip_method))
	{
	  OSIP_TRACE (osip_trace
		      (__FILE__, __LINE__, OSIP_WARNING, NULL,
		       "core module: Discard invalid message with method!=cseq!\n"));
	  return NULL;
	}
    }

  if (MSG_IS_ACK (evt->sip)) /* ACK never create transactions */
    return NULL;

  if (EVT_IS_INCOMINGREQ (evt))
    {
      /* we create a new context for this incoming request */
      if (0 == strcmp (evt->sip->cseq->method, "INVITE"))
	ctx_type = IST;
      else
	ctx_type = NIST;
    }
  else if (EVT_IS_OUTGOINGREQ (evt))
    {
      if (0 == strcmp (evt->sip->cseq->method, "INVITE"))
	ctx_type = ICT;
      else
	ctx_type = NICT;
    }
  else
    {
      OSIP_TRACE (osip_trace
		  (__FILE__, __LINE__, OSIP_ERROR, NULL,
		   "Cannot build a transction for this message!\n"));
      return NULL;
    }

  i = osip_transaction_init (&transaction, ctx_type, osip, evt->sip);
  if (i == -1)
    {
      return NULL;
    }
  evt->transactionid = transaction->transactionid;
  return transaction;
}

osip_transaction_t *
osip_transaction_find (osip_list_t * transactions, osip_event_t * evt)
{
  int pos = 0;
  osip_transaction_t *transaction;

  if (EVT_IS_INCOMINGREQ (evt))
    {
      while (!osip_list_eol (transactions, pos))
	{
	  transaction = (osip_transaction_t *) osip_list_get (transactions, pos);
	  if (0 ==
	      __osip_transaction_matching_request_osip_to_xist_17_2_3 (transaction,
								       evt->sip))
	    return transaction;
	  pos++;
	}
    }
  else if (EVT_IS_INCOMINGRESP (evt))
    {
      while (!osip_list_eol (transactions, pos))
	{
	  transaction = (osip_transaction_t *) osip_list_get (transactions, pos);
	  if (0 ==
	      __osip_transaction_matching_response_osip_to_xict_17_1_3 (transaction,
							    evt->sip))
	    return transaction;
	  pos++;
	}
    }
  else				/* handle OUTGOING message */
    {				/* THE TRANSACTION ID MUST BE SET */
      while (!osip_list_eol (transactions, pos))
	{
	  transaction = (osip_transaction_t *) osip_list_get (transactions, pos);
	  if (transaction->transactionid == evt->transactionid)
	    return transaction;
	  pos++;
	}
    }
  return NULL;
}

static int ref_count = 0;
#ifdef OSIP_MT
static struct osip_mutex * ref_mutex = NULL;
#endif

int
increase_ref_count (void)
{
#ifdef OSIP_MT
  if (ref_count == 0)
    ref_mutex = osip_mutex_init();
  /* Here we should assert() that the mutex was really generated. */
  osip_mutex_lock (ref_mutex);
#endif
  if (ref_count == 0)
    __osip_global_init ();
  ref_count++;
#ifdef OSIP_MT
  osip_mutex_unlock (ref_mutex);
#endif

  return 0;
}

void
decrease_ref_count (void)
{
#ifdef OSIP_MT
  osip_mutex_lock (ref_mutex);
#endif
  /* assert (ref_count > 0); */
  ref_count--;
  if (ref_count == 0)
    {
#ifdef OSIP_MT
      osip_mutex_unlock (ref_mutex);
      osip_mutex_destroy (ref_mutex);
#endif
      __osip_global_free ();
    }
#ifdef OSIP_MT
  osip_mutex_unlock (ref_mutex);
#endif
}

int
osip_init (osip_t ** osip)
{
  if (increase_ref_count () != 0)
    return -1;

  *osip = (osip_t *) osip_malloc (sizeof (osip_t));
  if (*osip == NULL)
    return -1;			/* allocation failed */

  memset (*osip, 0, sizeof (osip_t));

  (*osip)->osip_ict_transactions = (osip_list_t *) osip_malloc (sizeof (osip_list_t));
  osip_list_init ((*osip)->osip_ict_transactions);
  (*osip)->osip_ist_transactions = (osip_list_t *) osip_malloc (sizeof (osip_list_t));
  osip_list_init ((*osip)->osip_ist_transactions);
  (*osip)->osip_nict_transactions = (osip_list_t *) osip_malloc (sizeof (osip_list_t));
  osip_list_init ((*osip)->osip_nict_transactions);
  (*osip)->osip_nist_transactions = (osip_list_t *) osip_malloc (sizeof (osip_list_t));
  osip_list_init ((*osip)->osip_nist_transactions);

  return 0;
}

void
osip_release (osip_t * osip)
{
  osip_free (osip->osip_ict_transactions);
  osip_free (osip->osip_ist_transactions);
  osip_free (osip->osip_nict_transactions);
  osip_free (osip->osip_nist_transactions);
  osip_free (osip);
  decrease_ref_count ();
}


void
osip_set_application_context (osip_t * osip, void *pointer)
{
  osip->application_context = pointer;
}

void *
osip_get_application_context (osip_t * osip)
{
  if (osip == NULL)
    return NULL;
  return osip->application_context;
}

int
osip_ict_execute (osip_t * osip)
{
  osip_transaction_t *transaction;
  osip_event_t *se;
  int more_event;
  int tr;

  tr = 0;
  while (!osip_list_eol (osip->osip_ict_transactions, tr))
    {
      transaction = osip_list_get (osip->osip_ict_transactions, tr);
      tr++;
      more_event = 1;
      do
	{
	  se = (osip_event_t *) osip_fifo_tryget (transaction->transactionff);
	  if (se == NULL)	/* no more event for this transaction */
	    more_event = 0;
	  else
	    osip_transaction_execute (transaction, se);
	}
      while (more_event == 1);
    }
  return 0;
}

int
osip_ist_execute (osip_t * osip)
{
  osip_transaction_t *transaction;
  osip_event_t *se;
  int more_event;
  int tr;

  tr = 0;
  while (!osip_list_eol (osip->osip_ist_transactions, tr))
    {
      transaction = osip_list_get (osip->osip_ist_transactions, tr);
      tr++;
      more_event = 1;
      do
	{
	  se = (osip_event_t *) osip_fifo_tryget (transaction->transactionff);
	  if (se == NULL)	/* no more event for this transaction */
	    more_event = 0;
	  else
	    osip_transaction_execute (transaction, se);
	}
      while (more_event == 1);
    }
  return 0;
}

int
osip_nict_execute (osip_t * osip)
{
  osip_transaction_t *transaction;
  osip_event_t *se;
  int more_event;
  int tr;

  tr = 0;
  while (!osip_list_eol (osip->osip_nict_transactions, tr))
    {
      transaction = osip_list_get (osip->osip_nict_transactions, tr);
      tr++;
      more_event = 1;
      do
	{
	  se = (osip_event_t *) osip_fifo_tryget (transaction->transactionff);
	  if (se == NULL)	/* no more event for this transaction */
	    more_event = 0;
	  else
	    osip_transaction_execute (transaction, se);
	}
      while (more_event == 1);
    }
  return 0;
}

int
osip_nist_execute (osip_t * osip)
{
  osip_transaction_t *transaction;
  osip_event_t *se;
  int more_event;
  int tr;

  tr = 0;
  while (!osip_list_eol (osip->osip_nist_transactions, tr))
    {
      transaction = osip_list_get (osip->osip_nist_transactions, tr);
      tr++;
      more_event = 1;
      do
	{
	  se = (osip_event_t *) osip_fifo_tryget (transaction->transactionff);
	  if (se == NULL)	/* no more event for this transaction */
	    more_event = 0;
	  else
	    osip_transaction_execute (transaction, se);
	}
      while (more_event == 1);
    }
  return 0;
}


void
osip_timers_ict_execute (osip_t * osip)
{
  osip_transaction_t *tr;
  int pos = 0;

#ifdef OSIP_MT
  osip_mutex_lock (ict_fastmutex);
#endif
  /* handle ict timers */
  while (!osip_list_eol (osip->osip_ict_transactions, pos))
    {
      osip_event_t *evt;

      tr = (osip_transaction_t *) osip_list_get (osip->osip_ict_transactions, pos);

      if (1 <= osip_fifo_size (tr->transactionff))
	{
	  OSIP_TRACE (osip_trace
		      (__FILE__, __LINE__, OSIP_INFO4, NULL,
		       "1 Pending event already in transaction !\n"));
	}
      else
	{
	  evt = __osip_ict_need_timer_b_event (tr->ict_context, tr->state,
					       tr->transactionid);
	  if (evt != NULL)
	    osip_fifo_add (tr->transactionff, evt);
	  else
	    {
	      evt = __osip_ict_need_timer_a_event (tr->ict_context, tr->state,
						   tr->transactionid);
	      if (evt != NULL)
		osip_fifo_add (tr->transactionff, evt);
	      else
		{
		  evt=__osip_ict_need_timer_d_event(tr->ict_context, tr->state,
						    tr->transactionid);
		  if (evt != NULL)
		    osip_fifo_add (tr->transactionff, evt);
		}
	    }
	}
      pos++;
    }
#ifdef OSIP_MT
  osip_mutex_unlock (ict_fastmutex);
#endif
}

void
osip_timers_ist_execute (osip_t * osip)
{
  osip_transaction_t *tr;
  int pos = 0;

#ifdef OSIP_MT
  osip_mutex_lock (ist_fastmutex);
#endif
  /* handle ist timers */
  while (!osip_list_eol (osip->osip_ist_transactions, pos))
    {
      osip_event_t *evt;

      tr = (osip_transaction_t *) osip_list_get (osip->osip_ist_transactions, pos);

      evt = __osip_ist_need_timer_i_event (tr->ist_context, tr->state,
					   tr->transactionid);
      if (evt != NULL)
	osip_fifo_add (tr->transactionff, evt);
      else
	{
	  evt = __osip_ist_need_timer_h_event (tr->ist_context, tr->state,
					       tr->transactionid);
	  if (evt != NULL)
	    osip_fifo_add (tr->transactionff, evt);
	  else
	    {
	      evt = __osip_ist_need_timer_g_event (tr->ist_context, tr->state,
						   tr->transactionid);
	      if (evt != NULL)
		osip_fifo_add (tr->transactionff, evt);
	    }
	}
      pos++;
    }
#ifdef OSIP_MT
  osip_mutex_unlock (ist_fastmutex);
#endif
}

void
osip_timers_nict_execute (osip_t * osip)
{
  osip_transaction_t *tr;
  int pos = 0;

#ifdef OSIP_MT
  osip_mutex_lock (nict_fastmutex);
#endif
  /* handle nict timers */
  while (!osip_list_eol (osip->osip_nict_transactions, pos))
    {
      osip_event_t *evt;

      tr = (osip_transaction_t *) osip_list_get (osip->osip_nict_transactions, pos);

      evt = __osip_nict_need_timer_k_event (tr->nict_context, tr->state,
					    tr->transactionid);
      if (evt != NULL)
	osip_fifo_add (tr->transactionff, evt);
      else
	{
	  evt = __osip_nict_need_timer_f_event (tr->nict_context, tr->state,
						tr->transactionid);
	  if (evt != NULL)
	    osip_fifo_add (tr->transactionff, evt);
	  else
	    {
	      evt = __osip_nict_need_timer_e_event(tr->nict_context, tr->state,
						   tr->transactionid);
	      if (evt != NULL)
		osip_fifo_add (tr->transactionff, evt);
	    }
	}
      pos++;
    }
#ifdef OSIP_MT
  osip_mutex_unlock (nict_fastmutex);
#endif
}


void
osip_timers_nist_execute (osip_t * osip)
{
  osip_transaction_t *tr;
  int pos = 0;

#ifdef OSIP_MT
  osip_mutex_lock (nist_fastmutex);
#endif
  /* handle nist timers */
  while (!osip_list_eol (osip->osip_nist_transactions, pos))
    {
      osip_event_t *evt;

      tr = (osip_transaction_t *) osip_list_get (osip->osip_nist_transactions, pos);

      evt = __osip_nist_need_timer_j_event (tr->nist_context, tr->state,
					    tr->transactionid);
      if (evt != NULL)
	osip_fifo_add (tr->transactionff, evt);
      pos++;
    }
#ifdef OSIP_MT
  osip_mutex_unlock (nist_fastmutex);
#endif
}

void
osip_set_cb_send_message (osip_t * cf,
			 int (*cb) (osip_transaction_t *, osip_message_t *, char *, int,
				    int))
{
  cf->cb_send_message = cb;
}

void
__osip_message_callback (int type, osip_transaction_t * tr, osip_message_t * msg)
{
  osip_t * config = tr->config;

  if (type >= OSIP_MESSAGE_CALLBACK_COUNT)
    {
      OSIP_TRACE (osip_trace
		  (__FILE__, __LINE__, OSIP_BUG, NULL,
		   "invalid callback type %d\n", type));
      return;
    }
  if (config->msg_callbacks[type] == NULL)
    return;
  config->msg_callbacks[type](type, tr, msg);
}

void
__osip_kill_transaction_callback (int type, osip_transaction_t * tr)
{
  osip_t * config = tr->config;

  if (type >= OSIP_KILL_CALLBACK_COUNT)
    {
      OSIP_TRACE (osip_trace
		  (__FILE__, __LINE__, OSIP_BUG, NULL,
		   "invalid callback type %d\n", type));
      return;
    }
  if (config->kill_callbacks[type] == NULL)
    return;
  config->kill_callbacks[type] (type, tr);
}

void
__osip_transport_error_callback (int type, osip_transaction_t * tr, int error)
{
  osip_t * config = tr->config;

  if (type >= OSIP_TRANSPORT_ERROR_CALLBACK_COUNT)
    {
      OSIP_TRACE (osip_trace
		  (__FILE__, __LINE__, OSIP_BUG, NULL,
		   "invalid callback type %d\n", type));
      return;
    }
  if (config->tp_error_callbacks[type] == NULL)
    return;
  config->tp_error_callbacks[type] (type, tr, error);
}


int
osip_set_message_callback (osip_t * config, int type, osip_message_cb_t cb)
{
  if (type >= OSIP_MESSAGE_CALLBACK_COUNT)
    {
      OSIP_TRACE (osip_trace
		  (__FILE__, __LINE__, OSIP_ERROR, NULL,
		   "invalid callback type %d\n", type));
      return -1;
    }
  config->msg_callbacks[type] = cb;

  return 0;
}

int
osip_set_kill_transaction_callback (osip_t * config, int type,
				    osip_kill_transaction_cb_t cb)
{
  if (type >= OSIP_KILL_CALLBACK_COUNT)
    {
      OSIP_TRACE (osip_trace
		  (__FILE__, __LINE__, OSIP_ERROR, NULL,
		   "invalid callback type %d\n", type));
      return -1;
    }
  config->kill_callbacks[type] = cb;
  return 0;
}

int
osip_set_transport_error_callback (osip_t * config, int type,
				   osip_transport_error_cb_t cb)
{
  if (type >= OSIP_TRANSPORT_ERROR_CALLBACK_COUNT)
    {
      OSIP_TRACE (osip_trace
		  (__FILE__, __LINE__, OSIP_ERROR, NULL,
		   "invalid callback type %d\n", type));
      return -1;
    }
  config->tp_error_callbacks[type] = cb;
  return 0;
}
