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

namespace hwvisualizer
{
    class SerialConsole : RichTextBox
    {
        public SerialConsole()
        {
            BackColor = System.Drawing.Color.MidnightBlue;
            DetectUrls = false;
            Font = new System.Drawing.Font("Consolas", 8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            ForeColor = System.Drawing.Color.White;
            Location = new System.Drawing.Point(7, 143);
            Name = "rtSerial";
            ReadOnly = true;
            Size = new System.Drawing.Size(926, 298);
            TabIndex = 1;
            Text = "";
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
