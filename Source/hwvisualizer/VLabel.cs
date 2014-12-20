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
using System.Drawing.Text;

namespace hwvisualizer
{
    public partial class VLabel : Control
    {
        public VLabel() : base()
        {
            _brushFG = new SolidBrush(ForeColor);
            base.CreateControl();
            SetStyle(System.Windows.Forms.ControlStyles.Opaque, true);
            Size = new System.Drawing.Size(30, 120);
            ForeColorChanged += VLabel_ForeColorChanged;
            Resize += VLabel_Resize;
        }

        public bool BottomUp { get; set; }
        public TextRenderingHint RenderingMode
        {
            get { return _renderMode; }
            set { _renderMode = value; Invalidate();  }
        }

        public override string Text
        {
            get { return _text; }
            set { _text = value; Invalidate(); }
        }

        protected override void OnPaint(System.Windows.Forms.PaintEventArgs e)
        {
            base.OnPaint(e);
            e.Graphics.TextRenderingHint = this._renderMode;
            e.Graphics.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.HighQuality;
            
            if (BottomUp)
            {
                e.Graphics.TranslateTransform(0, Size.Height);
                e.Graphics.RotateTransform(270);
            }
            else
            {
                e.Graphics.TranslateTransform(Size.Width, 0);
                e.Graphics.RotateTransform(90);
            }
            e.Graphics.DrawString(_text, Font, _brushFG, 0, 0);
        }

        protected override CreateParams CreateParams
        {
            get
            {
                CreateParams cp = base.CreateParams;
                cp.ExStyle |= 0x20; 
                return cp;
            }
        }

        private void VLabel_ForeColorChanged(object sender, EventArgs e)
        {
            _brushFG.Dispose();
            _brushFG = new SolidBrush(ForeColor);
        }

        void VLabel_Resize(object sender, EventArgs e)
        {
            Invalidate();
        }

        private string _text;
        private TextRenderingHint _renderMode = System.Drawing.Text.TextRenderingHint.SystemDefault;
        private SolidBrush _brushFG = null;

    }
}
