namespace hwvisualizer
{
    partial class OutputDisplay
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
            this.visualizer1 = new HeartRateVisualizer.Visualizer();
            this.serialLog = new hwvisualizer.SerialConsole();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.SuspendLayout();
            // 
            // visualizer1
            // 
            this.visualizer1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.visualizer1.Location = new System.Drawing.Point(3, 112);
            this.visualizer1.Name = "visualizer1";
            this.visualizer1.Size = new System.Drawing.Size(900, 222);
            this.visualizer1.TabIndex = 5;
            // 
            // serialLog
            // 
            this.serialLog.BackColor = System.Drawing.Color.MidnightBlue;
            this.serialLog.DetectUrls = false;
            this.serialLog.Font = new System.Drawing.Font("Consolas", 8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.serialLog.ForeColor = System.Drawing.Color.White;
            this.serialLog.Location = new System.Drawing.Point(0, 0);
            this.serialLog.Name = "serialLog";
            this.serialLog.ReadOnly = true;
            this.serialLog.Size = new System.Drawing.Size(722, 106);
            this.serialLog.TabIndex = 6;
            this.serialLog.Text = "";
            this.serialLog.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.serialLog_KeyPress);
            // 
            // timer1
            // 
            this.timer1.Enabled = true;
            this.timer1.Interval = 80;
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // OutputDisplay
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.visualizer1);
            this.Controls.Add(this.serialLog);
            this.Name = "OutputDisplay";
            this.Size = new System.Drawing.Size(900, 339);
            this.ResumeLayout(false);

        }

        #endregion

        private HeartRateVisualizer.Visualizer visualizer1;
        private SerialConsole serialLog;
        private System.Windows.Forms.Timer timer1;
    }
}
