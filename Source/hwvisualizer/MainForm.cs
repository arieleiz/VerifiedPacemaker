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
using System.Windows.Forms;
using System.IO;
using System.IO.Ports;
using System.Management;

namespace hwvisualizer
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
            initComs();
        }

        private void serial_ErrorReceived(object sender, System.IO.Ports.SerialErrorReceivedEventArgs e)
        {

        }

        static private readonly char[] LINEFEED = new char[] { '\r'};
        static private readonly string MAGIC_PREFIX = "*** STARTING AS ";

        private void serial_DataReceived(object sender, System.IO.Ports.SerialDataReceivedEventArgs e)
        {
            var p = (SerialPort)sender;
            lock(this)
            {
                string buf = p.ReadExisting();
                OutputDisplay od = null;
                StreamWriter sw = null;
                if (_map.TryGetValue(p, out od))
                {
                    lock (od)
                    {
                        _text = od.buffer;
                        od.buffer = "";
                    }
                }

                _text = _text + buf;
               
                _logs.TryGetValue(p, out sw);

                for(;;)
                {
                    int pos = _text.IndexOf('\n');
                    if(pos < 0)
                        break;

                    string line = _text.Substring(0, pos + 1).TrimEnd(LINEFEED);
                    _text = _text.Substring(pos + 1);
                    if (line.StartsWith(MAGIC_PREFIX))
                    {
                        if (od != null)
                        {
                            od.Tag = null;
                            od.Invoke((Action)(() => { od.reset(); od.Visible = false; _hider[od].Visible = true; }));
                        }
                        string mode = "";
                        if (line[MAGIC_PREFIX.Length] == 'P') // pacemaker
                        {
                            od = output1;
                            mode = "pacemaker";
                        }
                        else
                        {
                            od = output2;
                            mode = "heart";
                        }
                        _map[p] = od;
                        od.Tag = p;
                        od.Invoke((Action)(() => { od.reset(); od.start();  od.Visible = true; _hider[od].Visible = false; }));
                        if (sw != null)
                        {
                            try
                            {
                                sw.Close();
                            }
                            catch
                            {
                            }
                        }
                        string path = Path.Combine(Path.GetDirectoryName(System.Diagnostics.Process.GetCurrentProcess().MainModule.FileName),
                                                   string.Format("{0}_{1}.log", mode, DateTime.Now.ToString("yyyyMMdd_HHmmss")));
                        sw = new StreamWriter(path);
                        _logs[p] = sw;
                    }

                    if (od != null)
                    {
                        lock (od)
                        {
                            od.addLine(line);
                        }
                    }
                    if(sw != null)
                    {
                        sw.WriteLine("{0} {1}", DateTime.Now.Subtract(_refdt).TotalMilliseconds, line);
                    }
                }

                if (od != null)
                {
                    lock (od)
                    {
                        od.buffer = _text;
                    }
                }
            }
        }

        private string[] getMBEDPorts()
        {
            using (var sps = new ManagementObjectSearcher("SELECT * FROM Win32_PnPEntity WHERE (Caption LIKE '%MBED%') AND (Caption LIKE '%SERIAL%')"))
            {
                string[] portnames = SerialPort.GetPortNames();
                var ports = sps.Get().Cast<ManagementBaseObject>();
                return (from n in portnames
                             from p in ports 
                            where p["Caption"].ToString().Contains(String.Format("({0})", n)) select n).ToArray();
            }
        }

        private void btnRefresh_Click(object sender, EventArgs e)
        {
            initComs();
        }

        private void initComs()
        {
            output1.Visible = false;
            output2.Visible = false;
            lblWaitPM.Visible = true;
            lblWaitHM.Visible = true;
            _hider[output1] = lblWaitPM;
            _hider[output2] = lblWaitHM;

            string[] ports = getMBEDPorts();
            if (ports.Length > 0)
            {
                reopen(serial1, ports[0]);
                if (ports.Length > 1)
                    reopen(serial2, ports[1]);
            }
        }

        private void reopen(SerialPort s, string p)
        {
            if (s.IsOpen)
                s.Close();
            s.PortName = p;
            s.Open();
        }

        private void output1_KeyPress(object sender, KeyPressEventArgs e)
        {
            var sp = (SerialPort)((Control)sender).Tag;
            if(sp == null)
                return;

            byte[] buffer = new byte[1];
            buffer[0] = (byte)(char)e.KeyChar;
            sp.Write(buffer, 0, 1);
        }

        private static ASCIIEncoding ascii = new ASCIIEncoding();
        private string _text = "";
        Dictionary<SerialPort, OutputDisplay> _map = new Dictionary<SerialPort, OutputDisplay>();
        Dictionary<SerialPort, StreamWriter> _logs = new Dictionary<SerialPort, StreamWriter>();
        Dictionary<OutputDisplay, Label> _hider = new Dictionary<OutputDisplay, Label>();
        DateTime _refdt = DateTime.Now;
    }
}
