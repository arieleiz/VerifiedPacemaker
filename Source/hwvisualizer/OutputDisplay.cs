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
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace hwvisualizer
{
    public partial class OutputDisplay : UserControl
    {
        public OutputDisplay()
        {
            InitializeComponent();
            KeyPress += (sender, arg) => { };
        }

        public void reset()
        {
            lock(this)
            {
                visualizer1.init();
            }
        }

        public void start()
        {
            lock (this)
            {
                visualizer1.start();
            }
        }

        private void serialLog_KeyPress(object sender, KeyPressEventArgs e)
        {
            OnKeyPress(e);
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            lock(_lines)
            {
                if (_lines.Count == 0)
                    return;
            }

            serialLog.SelectionStart = serialLog.TextLength;

            for(;;)
            {
                string line;

                lock(_lines)
                {
                    if(_lines.Count == 0)
                        break;

                    line = _lines.Dequeue();
                    serialLog.AppendText(line);
                }
                if (serialLog.TextLength > 2000000)
                {
                    serialLog.Text = serialLog.Text.Substring(1000000);
                }
            }

            serialLog.ScrollToCaret();
      
        }

        public void addLine(string line)
        {
            lock(_lines)
            {
                _lines.Enqueue(line);
            }

            visualizer1.addLine(line);
        }

        public string buffer;
        private Queue<string> _lines = new Queue<string>();
    }
}
