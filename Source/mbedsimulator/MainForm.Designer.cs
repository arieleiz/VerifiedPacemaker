namespace mbedsimulator
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
            this.mbed1 = new mbedsimulator.MbedSimulatorPane();
            this.mbed2 = new mbedsimulator.MbedSimulatorPane();
            this.btnLoad = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.tbModel = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // mbed1
            // 
            this.mbed1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.mbed1.Location = new System.Drawing.Point(1, 50);
            this.mbed1.MinimumSize = new System.Drawing.Size(750, 600);
            this.mbed1.Name = "mbed1";
            this.mbed1.Size = new System.Drawing.Size(750, 600);
            this.mbed1.TabIndex = 0;
            // 
            // mbed2
            // 
            this.mbed2.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.mbed2.Location = new System.Drawing.Point(757, 50);
            this.mbed2.MinimumSize = new System.Drawing.Size(750, 600);
            this.mbed2.Name = "mbed2";
            this.mbed2.Size = new System.Drawing.Size(750, 600);
            this.mbed2.TabIndex = 0;
            // 
            // btnLoad
            // 
            this.btnLoad.Location = new System.Drawing.Point(449, 10);
            this.btnLoad.Name = "btnLoad";
            this.btnLoad.Size = new System.Drawing.Size(75, 23);
            this.btnLoad.TabIndex = 1;
            this.btnLoad.Text = "&Load ...";
            this.btnLoad.UseVisualStyleBackColor = true;
            this.btnLoad.Click += new System.EventHandler(this.btnLoad_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(13, 15);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(63, 13);
            this.label1.TabIndex = 2;
            this.label1.Text = "Test Model:";
            // 
            // tbModel
            // 
            this.tbModel.Location = new System.Drawing.Point(82, 12);
            this.tbModel.Name = "tbModel";
            this.tbModel.ReadOnly = true;
            this.tbModel.Size = new System.Drawing.Size(361, 20);
            this.tbModel.TabIndex = 3;
            this.tbModel.TabStop = false;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1297, 578);
            this.Controls.Add(this.tbModel);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.btnLoad);
            this.Controls.Add(this.mbed2);
            this.Controls.Add(this.mbed1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Name = "MainForm";
            this.Text = "MBED Simulator";
            this.WindowState = System.Windows.Forms.FormWindowState.Maximized;
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainForm_FormClosing);
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.Resize += new System.EventHandler(this.MainForm_Resize);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private MbedSimulatorPane mbed1;
        private MbedSimulatorPane mbed2;
        private System.Windows.Forms.Button btnLoad;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox tbModel;
    }
}

