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
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using System.Reflection;
using mbedsimulatortypes;

namespace mbedsimulator
{
    public partial class MbedSimulatorPane : UserControl, IMbedEmulatorCB
    {
        public MbedSimulatorPane()
        {
            InitializeComponent();
            update_lcd_action = update_lcd_uithread;
            tbState.Text = "Uninitialized";
        }

        public string LogFilePath { get; set; }
        private void MbedSimulatorPane_Load(object sender, EventArgs e)
        {
            relayout();
            ParentForm.FormClosing += ParentForm_FormClosing;
        }

        void ParentForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            stop();
        }

        private void MbedSimulatorPane_Resize(object sender, EventArgs e)
        {
            relayout();
        }

        private void relayout()
        {
            pnlLCD.Width = lcdDisp.Width + pnlLCD.Margin.Horizontal;
            pnlLCD.Height = lcdDisp.Height + pnlLCD.Margin.Vertical;

            pnlLCD.Left = (ClientRectangle.Width - pnlLCD.Width) / 2;

        }

        private void btnStart_Click(object sender, EventArgs e)
        {
            start();
        }

        private void btnStop_Click(object sender, EventArgs e)
        {
            stop();
        }

        private void rtSerial_KeyPress(object sender, KeyPressEventArgs e)
        {
            gotSerialKeypress(e.KeyChar);
        }

        private void gotSerialKeypress(char c)
        {
            _selInpQueue.queueChar(c);
            if (_serial_handler != null)
                _serial_handler(0);
        }

        public void init(mbedsimulatorctr.MbedEmulator emulator)
        {
            foreach(Control c in flowButtons.Controls)
            {
                GPIOElement g = c as GPIOElement;
                if (g != null)
                    g.remove();
            }
            flowButtons.Controls.Clear();
            clear_lcd();
            rtSerial.Text = "";

            _emulator = emulator;
            tbName.Text = emulator.get_info().name;
            tbState.Text = "Initialized";
            btnStart.Enabled = true;

            IEnumerable<Device> devices = genBuiltinDevices().Concat(_emulator.get_info().devices);
            _emulator.get_info().devices = devices.ToArray();

            addDevices(devices);

            if(!string.IsNullOrEmpty(emulator.get_info().vizname))
            {
                var asm = Assembly.LoadFile(emulator.get_info().vizname);
                var s = asm.GetTypes().SingleOrDefault(v => typeof(mbedsimulatortypes.IVisualizer).IsAssignableFrom(v));
                if(s != null)
                {
                    var ci = s.GetConstructor(new Type[0]);
                    _viz = (IVisualizer) ci.Invoke(new object[0]);
                    pnlVis.Controls.Add((Control)_viz);
                }

            }
        }

        private List<Device> genBuiltinDevices()
        {
            List<Device> devices = new List<Device>();
            devices.Add(new Device() { extra = "blue", name = "LED1", pin = PinName.LED1, type = DeviceType.Led });
            devices.Add(new Device() { extra = "blue", name = "LED2", pin = PinName.LED2, type = DeviceType.Led });
            devices.Add(new Device() { extra = "blue", name = "LED3", pin = PinName.LED3, type = DeviceType.Led });
            devices.Add(new Device() { extra = "blue", name = "LED4", pin = PinName.LED4, type = DeviceType.Led });
            return devices;
        }

        private void addDevices(IEnumerable<Device> devices)
        {
            foreach (Device d in devices)
            {
                GPIOElement c = null;
                switch (d.type)
                {
                    case DeviceType.PushButton:
                        c = new PushButton();
                        break;
                    case DeviceType.Switch:
                        c = new Switch();
                        break;
                    case DeviceType.Led:
                        c = new LED();
                        break;
                    case DeviceType.Crossover:
                        c = new Crossover();
                        break;
                    case DeviceType.Ground:
                        c = new Ground();
                        break;
                    case DeviceType.Vout:
                        c = new Vout();
                        break;
                }
                if (c != null)
                {
                    c.setDevice(d);
                    flowButtons.Controls.Add(c);
                }
            }
        }
        private void start()
        {
            if (_emulator != null)
            {
                try
                {
                    if (_viz != null)
                        _viz.start();
                    _writer = new StreamWriter(LogFilePath, false);
                    _emulator.start();
                    tmrSerial.Start();
                    btnStart.Enabled = false;
                    btnStop.Enabled = true;
                    btnFuzz.Enabled = true;
                    tbState.Text = "Started";
                }
                catch (Exception e)
                {
                    MessageBox.Show(e.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    btnStart.Enabled = false;
                    btnStop.Enabled = false;
                }
            }
        }

        public void stop()
        {
            if (_emulator != null)
            {
                try
                {
                    _writer.Close();
                    _emulator.stop();
                    tbState.Text = "Stopped";
                }
                catch
                {

                }
                btnStart.Enabled = true;
                btnStop.Enabled = false;
            }
        }

        public void trigger_reset()
        {
            BeginInvoke((Action)(() =>
            {
                stop();
                MessageBox.Show("SYSTEM TRIGGERED REBOOT", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                start();
            })
            );
        }

        public void terminate_sim()
        {
            BeginInvoke((Action) ( () =>
            {
                stop();
                tbState.Text = "CRASHED!!!";
            } )
            );
        }
        public int serial_getc()
        {
            return _selInpQueue.dequeueChar();
        }

        private void tmrSerial_Tick(object sender, EventArgs e)
        {
            string s = _selOutQueue.tryDequeueString();
            if (s == null)
                return;

            rtSerial.SelectionStart = rtSerial.TextLength;
            rtSerial.AppendText(s);
            if (rtSerial.TextLength > 2000000)
            {
                rtSerial.Text = rtSerial.Text.Substring(1000000);
            }

            rtSerial.ScrollToCaret();
        }

        private StringBuilder _line = new StringBuilder();
        
        public void serial_putc(int ch)
        {
            _writer.Write((char)ch);
            if (ch == '\n')
                _writer.Flush();
            
            _selOutQueue.queueChar((char)ch);

            lock (_line)
            {
                _line.Append((char)ch);
                if (ch == '\n')
                {
                    if (_viz != null && _line.Length > 0)
                        _viz.addLine(_line.ToString());
                    _line = new StringBuilder();
                }
            }
        }

        public bool serial_readable()
        {
            return !_selOutQueue.empty();
        }

        private serial_event _serial_handler = null;
        public void serial_attach(serial_event handler, int mode)
        {
            if (mode == 0)
                _serial_handler = handler;
        }

        public void serial_detach()
        {
            _serial_handler = null;
        }

        public void serial_send_break()
        {
            
        }

        public void textlcd_init(int cols, int rows)
        {
            lcdDisp.TextLines = rows;
            lcdDisp.NumberOfCharacters = cols;
            _lcd_cols = cols;
            _lcd_pos = 0;
            _lcdDataStore = new char[cols * rows];
            relayout();
            clear_lcd();
        }

        public void textlcd_locate(int col, int row)
        {
            lock (_lcd_lock)
            {
                _lcd_pos = col + row * _lcd_cols;
            }
        }

        public void textlcd_cls()
        {
            clear_lcd();
        }

        public void textlcd_putc(int ch)
        {
            lock(_lcd_lock)
            {
                _lcdDataStore[_lcd_pos] = (char)ch;
                _lcd_pos = (_lcd_pos + 1) % _lcdDataStore.Length;
            }
            update_lcd();
        }

        private void update_lcd_uithread()
        {
            string str = "";
            lock(_lcd_lock)
            {
                str = new string(_lcdDataStore);
            }

            lcdDisp.Text = str;
        }
    
        private void update_lcd()
        {
            BeginInvoke(update_lcd_action);
        }

        private void clear_lcd()
        {
            lock (_lcd_lock)
            {
                for (int i = 0; i < _lcdDataStore.Length; ++i)
                    _lcdDataStore[i] = ' ';
                _lcd_pos = 0;
            }
            update_lcd();

        }

        GPIOElement findElement(PinName pin)
        {
            foreach(Control c in flowButtons.Controls)
            {
                GPIOElement e = c as GPIOElement;
                if (e.GetDevice().pin == pin)
                    return e;
            }
            return null;
        }

        GPIOElement findElementById(uint id)
        {
            foreach (Control c in flowButtons.Controls)
            {
                GPIOElement e = c as GPIOElement;
                if (e.Id == id)
                    return e;
            }
            return null;
        }

        public  void gpio_write(PinName pin, bool is_out, int value)
        {
            GPIOElement e = findElement(pin);
            if (e == null)
                return;
            e.IsOut = is_out;
            e.Value = value;
        }

        public int gpio_read(PinName pin, bool is_out)
        {
            GPIOElement e = findElement(pin);
            if (e == null)
                return 0;
            e.IsOut = is_out;
            return e.Value;
        }

        public void gpio_irq_connect(uint id, PinName pin, gpio_event handler)
        {
            GPIOElement e = findElement(pin);
            if (e == null)
                return;
            e.connect(id, handler);
        }
        
        public void gpio_irq_disconnect(uint id)
        {
            GPIOElement e = findElementById(id);
            if (e == null)
                return;
            e.disconnect();
        }

        public void gpio_irq_set(uint id, int evt, bool enable)
        {
            GPIOElement e = findElementById(id);
            if (e == null)
                return;
            e.setirq(evt, enable);
        }

        public void gpio_irq_enable(uint id, bool enable)
        {
            GPIOElement e = findElementById(id);
            if (e == null)
                return;
            e.setirq(1, enable);
            e.setirq(2, enable);
        }

        private void btnFuzz_Click(object sender, EventArgs e)
        {
            List<GPIOElement> outs = new List<GPIOElement>();
            foreach(var c in flowButtons.Controls)
            {
                GPIOElement el = c as GPIOElement;
                if(el != null && !el.IsOut)
                    outs.Add(el);
            }

            btnFuzz.Enabled = false;
            btnStopFuzz.Enabled = true;

            Fuzzer fuzzer = new Fuzzer(outs, _stopFuzz, () => {
                            btnStopFuzz.Invoke((Action)(() =>
                            {
                                btnStopFuzz.Enabled = false;
                                btnFuzz.Enabled = true;
                            }));
            });
            (new Thread(fuzzer.fuzzThreadMain)).Start();
        }

        private void btnStopFuzz_Click(object sender, EventArgs e)
        {
            _stopFuzz.Set();
        }

        class Fuzzer
        {
            public Fuzzer(IEnumerable<GPIOElement> gpios, AutoResetEvent stopFuzz, Action onstop)
            {
                _gpios = gpios;
                _stopFuzz = stopFuzz;
                _onstop = onstop;
            }
            public void fuzzThreadMain()
            {
                while (!_stopFuzz.WaitOne(0, false))
                {
                    Thread.Sleep(1 /* ms */);
                    foreach(GPIOElement e in _gpios)
                        e.setValue(_rand.Next(2));
                }
                _onstop();
            }
            private IEnumerable<GPIOElement> _gpios;
            private Random _rand = new Random();
            private AutoResetEvent _stopFuzz;
            private Action _onstop;
        }

        private mbedsimulatorctr.MbedEmulator _emulator = null;

        private SerialQueue _selInpQueue = new SerialQueue();
        private SerialQueue _selOutQueue = new SerialQueue();
        private char[] _lcdDataStore = new char[0];

        private Action update_lcd_action = null;
        private object _lcd_lock = new object();
        private int _lcd_pos = 0, _lcd_cols = 0;
        private StreamWriter _writer;
        private IVisualizer _viz;
        private AutoResetEvent _stopFuzz = new AutoResetEvent(false);
    }
}
