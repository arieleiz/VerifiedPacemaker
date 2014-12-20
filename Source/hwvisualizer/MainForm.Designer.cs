namespace hwvisualizer
{
    partial class MainForm
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

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.serial1 = new System.IO.Ports.SerialPort(this.components);
            this.serial2 = new System.IO.Ports.SerialPort(this.components);
            this.lblWaitPM = new System.Windows.Forms.Label();
            this.lblWaitHM = new System.Windows.Forms.Label();
            this.btnRefresh = new System.Windows.Forms.Button();
            this.output2 = new hwvisualizer.OutputDisplay();
            this.output1 = new hwvisualizer.OutputDisplay();
            this.vLabel1 = new hwvisualizer.VLabel();
            this.label1 = new hwvisualizer.VLabel();
            this.SuspendLayout();
            // 
            // serial1
            // 
            this.serial1.ErrorReceived += new System.IO.Ports.SerialErrorReceivedEventHandler(this.serial_ErrorReceived);
            this.serial1.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(this.serial_DataReceived);
            // 
            // serial2
            // 
            this.serial2.ErrorReceived += new System.IO.Ports.SerialErrorReceivedEventHandler(this.serial_ErrorReceived);
            this.serial2.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(this.serial_DataReceived);
            // 
            // lblWaitPM
            // 
            this.lblWaitPM.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.lblWaitPM.Font = new System.Drawing.Font("Tahoma", 24F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblWaitPM.Location = new System.Drawing.Point(81, 9);
            this.lblWaitPM.Name = "lblWaitPM";
            this.lblWaitPM.Size = new System.Drawing.Size(900, 344);
            this.lblWaitPM.TabIndex = 5;
            this.lblWaitPM.Text = "Waiting for PaceMaker to initialize ...";
            this.lblWaitPM.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // lblWaitHM
            // 
            this.lblWaitHM.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.lblWaitHM.Font = new System.Drawing.Font("Tahoma", 24F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblWaitHM.Location = new System.Drawing.Point(81, 359);
            this.lblWaitHM.Name = "lblWaitHM";
            this.lblWaitHM.Size = new System.Drawing.Size(900, 344);
            this.lblWaitHM.TabIndex = 5;
            this.lblWaitHM.Text = "Waiting for Heart to initialize ...";
            this.lblWaitHM.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // btnRefresh
            // 
            this.btnRefresh.Image = global::hwvisualizer.Properties.Resources._1417827044_sync_01_32;
            this.btnRefresh.Location = new System.Drawing.Point(946, 1);
            this.btnRefresh.Name = "btnRefresh";
            this.btnRefresh.Size = new System.Drawing.Size(46, 40);
            this.btnRefresh.TabIndex = 6;
            this.btnRefresh.UseVisualStyleBackColor = true;
            this.btnRefresh.Click += new System.EventHandler(this.btnRefresh_Click);
            // 
            // output2
            // 
            this.output2.Location = new System.Drawing.Point(82, 359);
            this.output2.Name = "output2";
            this.output2.Size = new System.Drawing.Size(899, 344);
            this.output2.TabIndex = 8;
            // 
            // output1
            // 
            this.output1.Location = new System.Drawing.Point(81, 9);
            this.output1.Name = "output1";
            this.output1.Size = new System.Drawing.Size(900, 344);
            this.output1.TabIndex = 7;
            this.output1.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.output1_KeyPress);
            // 
            // vLabel1
            // 
            this.vLabel1.BottomUp = false;
            this.vLabel1.Font = new System.Drawing.Font("Tahoma", 20F);
            this.vLabel1.Location = new System.Drawing.Point(39, 359);
            this.vLabel1.Name = "vLabel1";
            this.vLabel1.RenderingMode = System.Drawing.Text.TextRenderingHint.SystemDefault;
            this.vLabel1.Size = new System.Drawing.Size(27, 164);
            this.vLabel1.TabIndex = 0;
            this.vLabel1.Text = "Heart";
            // 
            // label1
            // 
            this.label1.BottomUp = false;
            this.label1.Font = new System.Drawing.Font("Tahoma", 20.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(39, 12);
            this.label1.Name = "label1";
            this.label1.RenderingMode = System.Drawing.Text.TextRenderingHint.SystemDefault;
            this.label1.Size = new System.Drawing.Size(27, 164);
            this.label1.TabIndex = 0;
            this.label1.Text = "Pacemaker";
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(993, 747);
            this.Controls.Add(this.btnRefresh);
            this.Controls.Add(this.output2);
            this.Controls.Add(this.output1);
            this.Controls.Add(this.vLabel1);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.lblWaitPM);
            this.Controls.Add(this.lblWaitHM);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "MainForm";
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
            this.Text = "CIS541 Project Visualizer";
            this.ResumeLayout(false);

        }

        #endregion

        private System.IO.Ports.SerialPort serial1;
        private System.IO.Ports.SerialPort serial2;
        private VLabel label1;
        private VLabel vLabel1;
        private System.Windows.Forms.Label lblWaitPM;
        private System.Windows.Forms.Label lblWaitHM;
        private System.Windows.Forms.Button btnRefresh;
        private OutputDisplay output1;
        private OutputDisplay output2;
    }
}

