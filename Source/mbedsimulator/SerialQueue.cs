/*
*  Pacemaker4Mbed (c) 2014 Ariel Eizenberg - arieleiz@seas.upenn.edu.
*  Developed for UPENN CIS 541 course project - verifiable pacemaker.
*
*    This file is part of Pacemaker4Mbed. You can redistribute it and/or modify
*    it under the terms of the Academic Free License 3.0
*	  (http://opensource.org/licenses/AFL-3.0).
*
*/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace mbedsimulator
{
    class SerialQueue
    {
        public SerialQueue()
        {
        }

        public int Length()
        {
            lock(_serialInputLock)
            {
                return _queue.Count;
            }
        }

        public void queueChar(char c)
        {
            lock(_serialInputLock)
            {
                _queue.Enqueue(c);
                _evt.Set();
            }
        }

        public bool empty()
        {
            lock (_serialInputLock)
            {
                return _queue.Count > 0;
            }
    }
    
        public bool tryDequeueChar(out char c)
        {
            lock (_serialInputLock)
            {
                if (_queue.Count > 0)
                {
                    c = _queue.Dequeue();
                    return true;
                }
                c = '\0';
                return false;
            }
        }

        public string tryDequeueString()
        {
            lock (_serialInputLock)
            {
                int len = _queue.Count;
                if(len == 0)
                    return null;
                char[] ca = new char[len];
                for (int i = 0; i < len; ++ i )
                    ca[i] = _queue.Dequeue();
                return new string(ca);
            }
        }

        public char dequeueChar()
        {
            for(;;)
            {
                lock(_serialInputLock)
                {
                    if (_queue.Count > 0)
                    {
                        return _queue.Dequeue();
                    }
                }
                _evt.WaitOne();
            }
        }

        private readonly object _serialInputLock = new object();
        private Queue<char> _queue = new Queue<char>();
        private AutoResetEvent _evt = new AutoResetEvent(false);
    }
}
