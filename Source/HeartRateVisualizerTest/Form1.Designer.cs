namespace HeartRateVisualizerTest
{
    partial class Form1
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
            this.visualizer = new HeartRateVisualizer.Visualizer();
            this.SuspendLayout();
            // 
            // visualizer
            // 
            this.visualizer.AutoSize = true;
            this.visualizer.Location = new System.Drawing.Point(51, 43);
            this.visualizer.Name = "visualizer";
            this.visualizer.Size = new System.Drawing.Size(900, 300);
            this.visualizer.TabIndex = 0;
            // 
            // Form1
            // 
            this.ClientSize = new System.Drawing.Size(1083, 589);
            this.Controls.Add(this.visualizer);
            this.Name = "Form1";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private HeartRateVisualizer.Visualizer visualizer;
    }
}

