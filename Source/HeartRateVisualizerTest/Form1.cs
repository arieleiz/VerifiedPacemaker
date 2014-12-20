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
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using System.Windows.Forms;

namespace HeartRateVisualizerTest
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();

            t1 = new Thread(func1);
            t1.IsBackground = true;
            t2 = new Thread(func2);
            t2.IsBackground = true;
            t3 = new Thread(func3);
            t3.IsBackground = true;
            visualizer.start();
            t1.Start();
            t2.Start();
            t3.Start();
        }

        Thread t1;

        void func1()
        {
            for (int i = 0; i < 100; ++i)
            {
                if (i % 5 != 4)
                {
                    visualizer.addLine("@@@[APACE]");
                }
                visualizer.addLine("@@@[VPACE]");
                Thread.Sleep((int)(800 + (r.NextDouble() - 0.5) * 150));
            }
        }

        Thread t2;

        void func2()
        {
            for (int i = 0; i < 100; ++i)
            {
                if (i % 5 != 4)
                {
                    visualizer.addLine("@@@[ASIGNAL]");
                }
                visualizer.addLine("@@@[VSIGNAL]");
                Thread.Sleep((int)(800 + (r.NextDouble() - 0.5) * 150));
            }
        }

        Thread t3;

        void func3()
        {
            for (int i = 0; i < 100; ++i)
            {
                if (i % 5 != 4)
                {
                    visualizer.addLine("@@@[ASENSE]");
                }
                visualizer.addLine("@@@[VSENSE]");
                Thread.Sleep((int)(800 + (r.NextDouble() - 0.5) * 150));
            }
        }

        Random r = new Random();

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
        }
    }
}
