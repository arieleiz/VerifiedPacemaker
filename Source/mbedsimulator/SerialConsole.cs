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
using System.Runtime.Remoting.Messaging;
using System.Security.Cryptography.X509Certificates;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace mbedsimulator
{
    class SerialConsole : RichTextBox
    {
        public SerialConsole()
        {
            
        }

        protected override void WndProc(ref Message m)
        {
            switch (m.Msg)
            {
                case (int)Win32Messages.WM_KEYDOWN:
                    return;

                case (int)Win32Messages.WM_KEYUP:
                    return;

                case (int)Win32Messages.WM_CHAR:
                    OnKeyPress(new KeyPressEventArgs((char) m.WParam));
                    return;
            }

            base.WndProc(ref m);
        }
    }
}
