/*
  The oSIP library implements the Session Initiation Protocol (SIP -rfc2543-)
  Copyright (C) 2001  Aymeric MOIZARD jack@atosc.org
  
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

#ifdef OSIP_MT

#include <stdio.h>

#include <osip/port.h>
#include <osip/thread.h>

#ifdef WIN32
#include <process.h>
#endif

/********************************/
/* Methods from port_thread.c   */
/********************************/
/* launch a new thread.                               */
/* INPUT :  int stacksize | size of the stack (for VxWorks only).*/
/* INPUT :  void *(*func)(void *) | .*/
/* INPUT :  void *arg | arg is the parameter for func.*/
/* OUTPUT:  pthread_t *thread | thread ID.   */
/* returns SIP_NULL on error */
#ifndef __VXWORKS_OS__
#if defined(HAVE_PTHREAD_H) || defined(HAVE_PTH_PTHREAD_H)
sthread_t *
sthread_create(int stacksize, sthread_t *thread, void *(*func)(void *), void *arg)
{
  int i;
  if (thread==NULL)
    thread = (sthread_t *) smalloc(sizeof(sthread_t));
  
  i = pthread_create(thread,NULL
		     ,func
		     ,(void *)arg);
  if (i!=0)
    {
    DEBUG(fprintf(stdout,"<port_thread.c> Error while creating a new thread %i\n",i));
    return NULL;
    }
  return thread;
}

int sthread_setpriority(sthread_t *thread, int priority)
{ return 0; }
int sthread_join(sthread_t *thread)
{ if (thread==NULL) return -1;return pthread_join(*thread,NULL); }
void sthread_exit()
{ pthread_exit(NULL); }
#endif
#endif

#ifdef WIN32
sthread_t *sthread_create(int stacksize, sthread_t *thread,
                          void *(*func)(void *),  void *arg)
{
    if (thread==NULL)
      thread = (sthread_t *) smalloc(sizeof(sthread_t));

    thread->h = _beginthread((void *)func, stacksize, arg);

    if (thread->h == 0)
    {
    DEBUG(fprintf(stdout,"<port_thread.c> Error while creating a new thread\n"));
    return NULL;
    }

  return thread;
}

int sthread_setpriority(sthread_t *thread, int priority)
{ 
    return 0;
}

int sthread_join(sthread_t *thread)
{  
  if (thread==NULL) return -1;
   if (GetThreadPriority((HANDLE)thread->h) != THREAD_PRIORITY_ERROR_RETURN)
       TerminateThread((HANDLE)thread->h, 0);

   return(0);
}

void sthread_exit()
{ 
    _endthread(); 
}
#endif

#ifndef __VXWORKS_OS__
#ifdef __PSOS__
sthread_t *sthread_create(int stacksize, sthread_t *thread,
                          void *(*func)(void *),  void *arg)
{

  if (thread==NULL)
    thread = (sthread_t *) smalloc(sizeof(sthread_t));

  if (t_create("sip", 150, stacksize, 0, 0, &thread->tid) != 0) {
      sfree(thread);
      return(NULL);
  }

  if (t_start(thread->tid, T_PREEMPT | T_ISR, func, 0) != 0) {
      sfree(thread);
      return(NULL);
  }

  return thread;
}

int sthread_setpriority(sthread_t *thread, int priority)
{ 
  unsigned long oldprio;
  if (thread==NULL) return -1;
  t_setpri(thread->tid, priority, &oldprio);
  return 0;
}

int sthread_join(sthread_t *thread)
{  
  if (thread==NULL) return -1;
  t_delete(thread->tid);
  
  return(0);
}
void sthread_exit()
{ 
   t_delete(0);
}
#endif
#endif

#ifdef __VXWORKS_OS__
sthread_t 
sthread_create(int stacksize, sthread_t *thread, void *(*func)(void *), void *arg)
{
  if (thread!=NULL)
    thread = (sthread_t *) smalloc(sizeof(sthread_t));
  thread->id = taskSpawn(NULL,5,0,stacksize,(FUNCPTR)func,(int)arg,0,0,0,0,0,0,0,0,0);
  if (thread->id<0) sfree(thread);
  return thread;
}
int
sthread_setpriority(sthread_t *thread, int priority)
{
  if (thread==NULL) return -1;
  taskPrioritySet(thread->id, 1);
  return 0;
}
int
sthread_join(sthread_t *thread)
{
  if (thread==NULL) return -1;
  return taskDelete(thread->id);
}
void
sthread_exit()
{
  /*??*/
}
#endif

#endif
