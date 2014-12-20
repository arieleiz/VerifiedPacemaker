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
using System.Windows.Forms.DataVisualization.Charting;

namespace HeartRateVisualizer
{
    public partial class Visualizer : UserControl, mbedsimulatortypes.IVisualizer
    {
        const int interval  = 50;

        public Visualizer()
        {
            InitializeComponent();

            init();
        }

        public void init()
        {
            chart.Series.Clear();
            _series[0] = chart.Series.Add("Pace");
            _series[1] = chart.Series.Add("Signal");
            _series[2] = chart.Series.Add("Sense");
            _series[0].LegendText = "Pace";
            _series[1].LegendText = "Signal";
            _series[2].LegendText = "Sense";

            for(int i = 0; i < _adata.Length; ++ i)
                _adata[i] = _vdata[i] = false;

            foreach (var s in _series)
                s.ChartType = SeriesChartType.Line;

            chart.ChartAreas[0].AxisY.Minimum = -1.5;
            chart.ChartAreas[0].AxisY.Maximum = 2.5;
            chart.ChartAreas[0].AxisY.Interval = 1;
            chart.ChartAreas[0].AxisY.MajorGrid.Enabled = false;
            chart.ChartAreas[0].AxisY.MajorTickMark.Enabled = false;
            chart.ChartAreas[0].AxisY.Crossing = 0;
            chart.ChartAreas[0].AxisY.IntervalType = DateTimeIntervalType.Number;
            chart.ChartAreas[0].AxisY.IntervalOffset = 0;

            chart.ChartAreas[0].AxisX.MajorGrid.Enabled = false;
            chart.ChartAreas[0].AxisX.IntervalType = DateTimeIntervalType.Number;
            chart.ChartAreas[0].AxisX.IntervalOffset = 0;
            chart.ChartAreas[0].AxisX.MajorTickMark.Interval = (1000.0 * _pixelsperPoints / interval);
            chart.ChartAreas[0].AxisX.MajorTickMark.Enabled = true;
            chart.ChartAreas[0].AxisX.MinorTickMark.TickMarkStyle = TickMarkStyle.AcrossAxis;
            chart.ChartAreas[0].AxisX.MinorTickMark.Interval = (1000.0 * _pixelsperPoints / interval) / 10;
            chart.ChartAreas[0].AxisX.MinorTickMark.Enabled = true;
            chart.ChartAreas[0].AxisX.MinorTickMark.TickMarkStyle = TickMarkStyle.OutsideArea;
            chart.ChartAreas[0].AxisX.Crossing = 0;
            chart.ChartAreas[0].AxisY.CustomLabels.Add(-1.1, -0.9, "A");
            chart.ChartAreas[0].AxisY.CustomLabels.Add(1.9, 2.1, "V");

            for (int i = 0; i < (_maxPoints * _pixelsperPoints); ++i)
                for (int j = 0; j < 3; ++j)
                    _series[j].Points.Add(0);

            for (int i = 0; i <= (((_maxPoints * _pixelsperPoints) * interval) / (_pixelsperPoints * 100)); ++i)
            {
                var lbl = chart.ChartAreas[0].AxisX.CustomLabels.Add(((i * 100.0 * _pixelsperPoints / interval)) - 10,
                                (i * 100.0 * _pixelsperPoints / interval) + 10, "");
                if (i % 10 == 0)
                {
                    lbl.Text = (i / 10).ToString();
                }
                lbl.GridTicks = GridTickTypes.TickMark;
            }
        }

        private void Visualizer_Load(object sender, EventArgs e)
        {

 
        }

        public void start()
        {
            ParentForm.FormClosing += ParentForm_FormClosing;
            timer.Interval = interval;
            timer.Start();
        }

        public void stop()
        {
            timer.Stop();
            ParentForm.FormClosing -= ParentForm_FormClosing;
        }

        void ParentForm_FormClosing(object sender, FormClosingEventArgs e)
        {
           stop();
        }

        public void addLine(string line)
        {
            if (!line.StartsWith("@@@"))
                return;
            int spos = line.IndexOf('[', 3);
            if(spos < 0)
                return;
            int epos = line.IndexOf(']', spos + 1);
            if (epos < 0)
                return;
            string type = line.Substring(spos + 1, epos - spos - 1);
            switch(type)
            {
                case "APACE":
                    _adata[0] = true;
                    break;
                case "VPACE":
                    _vdata[0] = true;
                    break;

                case "ASIGNAL":
                    _adata[1] = true;
                    break;
                case "VSIGNAL":
                    _vdata[1] = true;
                    break;

                case "ASENSE":
                    _adata[2] = true;
                    break;
                case "VSENSE":
                    _vdata[2] = true;
                    break;            
            }
        }

        private void timer_Tick(object sender, EventArgs e)
        {
            if (_points >= _maxPoints)
            {
                for (int i = 0; i < (_maxPoints * _pixelsperPoints); ++i)
                {
                    for (int j = 0; j < 3; ++j)
                        _series[j].Points[i].YValues[0] = 0.0;
                }
                _points = 0;
            }

            for (int i = 0; i < 3; ++i)
            {
                _series[i].Points[_points * _pixelsperPoints + 0].YValues[0] = (_adata[i] ? -1 : 0);
                _series[i].Points[_points * _pixelsperPoints + 1].YValues[0] = (_adata[i] ? -1 : 0);
                _adata[i] = false;
            }
            for (int i = 0; i < 3; ++i)
            {
                _series[i].Points[_points * _pixelsperPoints + 2].YValues[0] = (_vdata[i] ? 2 : 0);
                _series[i].Points[_points * _pixelsperPoints + 3].YValues[0] = (_vdata[i] ? 2 : 0);
                _vdata[i] = false;
            }
            for (int i = 0; i < 3; ++i)
                _series[i].Points[_points * _pixelsperPoints + 4].YValues[0] = 0;
            _points += 1;
            chart.Refresh();
        }

        private void chart_Resize(object sender, EventArgs e)
        {

        }


        Series[] _series = new Series[3];
        volatile bool[] _adata = new bool[3];
        volatile bool[] _vdata = new bool[3];
        const int _maxPoints = 140;
        int _points = 0;
        int _pixelsperPoints = 5;

    }
}
