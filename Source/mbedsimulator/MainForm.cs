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
using mbedsimulatorctr;
using mbedsimulatortypes;

namespace mbedsimulator
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            resizePanes();
        }

        private void MainForm_Resize(object sender, EventArgs e)
        {
            resizePanes();
        }

        private void resizePanes()
        {
            mbed1.Width = mbed2.Width = (ClientRectangle.Width - 12)/2;
            mbed1.Left = 1;
            mbed2.Left = ClientRectangle.Width - mbed1.Width - 1;
            mbed1.Height = mbed2.Height = ClientRectangle.Height - mbed1.Top;
        }

        private void btnLoad_Click(object sender, EventArgs e)
        {
            using(OpenFileDialog ofd = new OpenFileDialog() { Filter = "Configuration files (*.xml)|*.xml"  })
            {
                if (ofd.ShowDialog() != DialogResult.OK)
                    return;

                DataLoader dl = new DataLoader();
                if(dl.parseXml(ofd.FileName))
                {
                    loadData(Path.GetFileNameWithoutExtension(ofd.FileName), dl.Model);
                }

            }
        }

        private void loadData(string logbase, DataModel dm)
        {
            if(dm.Infos.Length > 2)
            {
                MessageBox.Show("Only two MBEDs supported by UI currently");
                return;
            }

            if (dm.Infos.Length == 0)
            {
                MessageBox.Show("At least one MBED required");
                return;
            }

            mbed1.init(createModel(dm.Infos[0], mbed1));
            mbed1.LogFilePath = String.Format("{0}_1.log.txt", logbase);
            if (dm.Infos.Length == 2)
            {
                mbed2.init(createModel(dm.Infos[1], mbed2));
                mbed2.LogFilePath = String.Format("{0}_2.log.txt", logbase);
            }
        }

        private mbedsimulatorctr.MbedEmulator createModel(MbedProgramInfo info, IMbedEmulatorCB cb)
        {
            mbedsimulatorctr.MbedEmulator emu = new MbedEmulator(info, cb);
            return emu;
        }

        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            mbed1.stop();
            mbed2.stop();
        }
    }
}
