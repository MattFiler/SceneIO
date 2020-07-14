namespace MaterialEditor
{
    partial class Overview
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
            this.deleteMaterial = new System.Windows.Forms.Button();
            this.editMaterial = new System.Windows.Forms.Button();
            this.newMaterial = new System.Windows.Forms.Button();
            this.materialList = new System.Windows.Forms.ListBox();
            this.SuspendLayout();
            // 
            // deleteMaterial
            // 
            this.deleteMaterial.Location = new System.Drawing.Point(610, 308);
            this.deleteMaterial.Name = "deleteMaterial";
            this.deleteMaterial.Size = new System.Drawing.Size(166, 34);
            this.deleteMaterial.TabIndex = 32;
            this.deleteMaterial.Text = "Delete Selected Material";
            this.deleteMaterial.UseVisualStyleBackColor = true;
            this.deleteMaterial.Click += new System.EventHandler(this.deleteMaterial_Click);
            // 
            // editMaterial
            // 
            this.editMaterial.Location = new System.Drawing.Point(610, 268);
            this.editMaterial.Name = "editMaterial";
            this.editMaterial.Size = new System.Drawing.Size(166, 34);
            this.editMaterial.TabIndex = 31;
            this.editMaterial.Text = "Edit Selected Material";
            this.editMaterial.UseVisualStyleBackColor = true;
            this.editMaterial.Click += new System.EventHandler(this.editMaterial_Click);
            // 
            // newMaterial
            // 
            this.newMaterial.Location = new System.Drawing.Point(610, 228);
            this.newMaterial.Name = "newMaterial";
            this.newMaterial.Size = new System.Drawing.Size(166, 34);
            this.newMaterial.TabIndex = 30;
            this.newMaterial.Text = "New Material";
            this.newMaterial.UseVisualStyleBackColor = true;
            this.newMaterial.Click += new System.EventHandler(this.newMaterial_Click);
            // 
            // materialList
            // 
            this.materialList.FormattingEnabled = true;
            this.materialList.Location = new System.Drawing.Point(12, 12);
            this.materialList.Name = "materialList";
            this.materialList.Size = new System.Drawing.Size(592, 329);
            this.materialList.TabIndex = 29;
            // 
            // Overview
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(787, 352);
            this.Controls.Add(this.deleteMaterial);
            this.Controls.Add(this.editMaterial);
            this.Controls.Add(this.newMaterial);
            this.Controls.Add(this.materialList);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.MaximizeBox = false;
            this.Name = "Overview";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Material Overview";
            this.ResumeLayout(false);

        }

        #endregion
        private System.Windows.Forms.Button deleteMaterial;
        private System.Windows.Forms.Button editMaterial;
        private System.Windows.Forms.Button newMaterial;
        private System.Windows.Forms.ListBox materialList;
    }
}

