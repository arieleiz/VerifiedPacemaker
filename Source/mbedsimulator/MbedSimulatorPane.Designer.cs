namespace mbedsimulator
{
    partial class MbedSimulatorPane
    {
        /// <summary> 
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.lcdDisp = new LCDLabel.LcdLabel();
            this.rtSerial = new mbedsimulator.SerialConsole();
            this.label1 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.pnlLCD = new System.Windows.Forms.Panel();
            this.btnStart = new System.Windows.Forms.Button();
            this.btnStop = new System.Windows.Forms.Button();
            this.lblState = new System.Windows.Forms.Label();
            this.tbState = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.tbName = new System.Windows.Forms.TextBox();
            this.flowButtons = new System.Windows.Forms.FlowLayoutPanel();
            this.pnlVis = new System.Windows.Forms.Panel();
            this.tmrSerial = new System.Windows.Forms.Timer(this.components);
            this.btnFuzz = new System.Windows.Forms.Button();
            this.btnStopFuzz = new System.Windows.Forms.Button();
            this.pnlLCD.SuspendLayout();
            this.SuspendLayout();
            // 
            // lcdDisp
            // 
            this.lcdDisp.BackGround = System.Drawing.Color.LightGreen;
            this.lcdDisp.BorderColor = System.Drawing.Color.Black;
            this.lcdDisp.BorderSpace = 3;
            this.lcdDisp.CharSpacing = 3;
            this.lcdDisp.DotMatrix = LCDLabel.DotMatrix.mat9x12;
            this.lcdDisp.LineSpacing = 2;
            this.lcdDisp.Location = new System.Drawing.Point(0, 0);
            this.lcdDisp.Name = "lcdDisp";
            this.lcdDisp.NumberOfCharacters = 16;
            this.lcdDisp.PixelHeight = 2;
            this.lcdDisp.PixelOff = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(170)))), ((int)(((byte)(170)))), ((int)(((byte)(170)))));
            this.lcdDisp.PixelOn = System.Drawing.Color.Black;
            this.lcdDisp.PixelShape = LCDLabel.PixelShape.Square;
            this.lcdDisp.PixelSize = LCDLabel.PixelSize.pix2x2;
            this.lcdDisp.PixelSpacing = 1;
            this.lcdDisp.PixelWidth = 2;
            this.lcdDisp.Size = new System.Drawing.Size(469, 80);
            this.lcdDisp.TabIndex = 0;
            this.lcdDisp.TextLines = 2;
            // 
            // rtSerial
            // 
            this.rtSerial.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.rtSerial.BackColor = System.Drawing.Color.MidnightBlue;
            this.rtSerial.DetectUrls = false;
            this.rtSerial.Font = new System.Drawing.Font("Consolas", 8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.rtSerial.ForeColor = System.Drawing.Color.White;
            this.rtSerial.Location = new System.Drawing.Point(7, 143);
            this.rtSerial.Name = "rtSerial";
            this.rtSerial.ReadOnly = true;
            this.rtSerial.Size = new System.Drawing.Size(926, 298);
            this.rtSerial.TabIndex = 1;
            this.rtSerial.Text = "";
            this.rtSerial.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.rtSerial_KeyPress);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(4, 124);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(100, 13);
            this.label1.TabIndex = 2;
            this.label1.Text = "Serial Input/Output:";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(4, 39);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(68, 13);
            this.label3.TabIndex = 2;
            this.label3.Text = "LCD Display:";
            // 
            // pnlLCD
            // 
            this.pnlLCD.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.pnlLCD.Controls.Add(this.lcdDisp);
            this.pnlLCD.Location = new System.Drawing.Point(131, 48);
            this.pnlLCD.Name = "pnlLCD";
            this.pnlLCD.Size = new System.Drawing.Size(472, 84);
            this.pnlLCD.TabIndex = 3;
            // 
            // btnStart
            // 
            this.btnStart.Enabled = false;
            this.btnStart.Location = new System.Drawing.Point(159, 6);
            this.btnStart.Name = "btnStart";
            this.btnStart.Size = new System.Drawing.Size(75, 23);
            this.btnStart.TabIndex = 4;
            this.btnStart.Text = "&Start";
            this.btnStart.UseVisualStyleBackColor = true;
            this.btnStart.Click += new System.EventHandler(this.btnStart_Click);
            // 
            // btnStop
            // 
            this.btnStop.Enabled = false;
            this.btnStop.Location = new System.Drawing.Point(240, 6);
            this.btnStop.Name = "btnStop";
            this.btnStop.Size = new System.Drawing.Size(75, 23);
            this.btnStop.TabIndex = 4;
            this.btnStop.Text = "Sto&p";
            this.btnStop.UseVisualStyleBackColor = true;
            this.btnStop.Click += new System.EventHandler(this.btnStop_Click);
            // 
            // lblState
            // 
            this.lblState.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.lblState.AutoSize = true;
            this.lblState.Location = new System.Drawing.Point(792, 11);
            this.lblState.Name = "lblState";
            this.lblState.Size = new System.Drawing.Size(35, 13);
            this.lblState.TabIndex = 5;
            this.lblState.Text = "State:";
            // 
            // tbState
            // 
            this.tbState.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.tbState.Location = new System.Drawing.Point(833, 7);
            this.tbState.Name = "tbState";
            this.tbState.ReadOnly = true;
            this.tbState.Size = new System.Drawing.Size(100, 20);
            this.tbState.TabIndex = 6;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(3, 11);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(38, 13);
            this.label2.TabIndex = 5;
            this.label2.Text = "Name:";
            // 
            // tbName
            // 
            this.tbName.Location = new System.Drawing.Point(44, 7);
            this.tbName.Name = "tbName";
            this.tbName.ReadOnly = true;
            this.tbName.Size = new System.Drawing.Size(100, 20);
            this.tbName.TabIndex = 6;
            this.tbName.Text = "Not loaded";
            // 
            // flowButtons
            // 
            this.flowButtons.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.flowButtons.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.flowButtons.Location = new System.Drawing.Point(7, 447);
            this.flowButtons.Name = "flowButtons";
            this.flowButtons.Size = new System.Drawing.Size(926, 106);
            this.flowButtons.TabIndex = 7;
            // 
            // pnlVis
            // 
            this.pnlVis.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.pnlVis.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.pnlVis.Location = new System.Drawing.Point(8, 559);
            this.pnlVis.Name = "pnlVis";
            this.pnlVis.Size = new System.Drawing.Size(925, 291);
            this.pnlVis.TabIndex = 8;
            // 
            // tmrSerial
            // 
            this.tmrSerial.Interval = 250;
            this.tmrSerial.Tick += new System.EventHandler(this.tmrSerial_Tick);
            // 
            // btnFuzz
            // 
            this.btnFuzz.Enabled = false;
            this.btnFuzz.Location = new System.Drawing.Point(547, 6);
            this.btnFuzz.Name = "btnFuzz";
            this.btnFuzz.Size = new System.Drawing.Size(108, 23);
            this.btnFuzz.TabIndex = 9;
            this.btnFuzz.Text = "Fuzz Inputs";
            this.btnFuzz.UseVisualStyleBackColor = true;
            this.btnFuzz.Click += new System.EventHandler(this.btnFuzz_Click);
            // 
            // btnStopFuzz
            // 
            this.btnStopFuzz.Enabled = false;
            this.btnStopFuzz.Location = new System.Drawing.Point(661, 5);
            this.btnStopFuzz.Name = "btnStopFuzz";
            this.btnStopFuzz.Size = new System.Drawing.Size(108, 23);
            this.btnStopFuzz.TabIndex = 9;
            this.btnStopFuzz.Text = "Stop Fuzzing";
            this.btnStopFuzz.UseVisualStyleBackColor = true;
            this.btnStopFuzz.Click += new System.EventHandler(this.btnStopFuzz_Click);
            // 
            // MbedSimulatorPane
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.Controls.Add(this.btnStopFuzz);
            this.Controls.Add(this.btnFuzz);
            this.Controls.Add(this.pnlVis);
            this.Controls.Add(this.flowButtons);
            this.Controls.Add(this.tbName);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.tbState);
            this.Controls.Add(this.lblState);
            this.Controls.Add(this.btnStop);
            this.Controls.Add(this.btnStart);
            this.Controls.Add(this.pnlLCD);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.rtSerial);
            this.MinimumSize = new System.Drawing.Size(940, 600);
            this.Name = "MbedSimulatorPane";
            this.Size = new System.Drawing.Size(936, 853);
            this.Load += new System.EventHandler(this.MbedSimulatorPane_Load);
            this.Resize += new System.EventHandler(this.MbedSimulatorPane_Resize);
            this.pnlLCD.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private LCDLabel.LcdLabel lcdDisp;
        private SerialConsole rtSerial;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Panel pnlLCD;
        private System.Windows.Forms.Button btnStart;
        private System.Windows.Forms.Button btnStop;
        private System.Windows.Forms.Label lblState;
        private System.Windows.Forms.TextBox tbState;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox tbName;
        private System.Windows.Forms.FlowLayoutPanel flowButtons;
        private System.Windows.Forms.Panel pnlVis;
        private System.Windows.Forms.Timer tmrSerial;
        private System.Windows.Forms.Button btnFuzz;
        private System.Windows.Forms.Button btnStopFuzz;
    }
}
