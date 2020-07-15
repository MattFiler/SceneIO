namespace MaterialEditor
{
    partial class Editor
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
            this.saveParameter = new System.Windows.Forms.Button();
            this.materialParameters = new System.Windows.Forms.ListBox();
            this.parameterIsBound = new System.Windows.Forms.CheckBox();
            this.materialName = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.parameterName = new System.Windows.Forms.TextBox();
            this.materialType = new System.Windows.Forms.ComboBox();
            this.label3 = new System.Windows.Forms.Label();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.editParam = new System.Windows.Forms.Button();
            this.createParam = new System.Windows.Forms.Button();
            this.deleteParam = new System.Windows.Forms.Button();
            this.parameterEditWindow = new System.Windows.Forms.GroupBox();
            this.optionsListLabel = new System.Windows.Forms.Label();
            this.parameterOptionsList = new System.Windows.Forms.TextBox();
            this.parameterType = new System.Windows.Forms.ComboBox();
            this.label5 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.saveMaterial = new System.Windows.Forms.Button();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.pixelShaderCode = new System.Windows.Forms.RichTextBox();
            this.showOtherVars = new System.Windows.Forms.Button();
            this.groupBox1.SuspendLayout();
            this.parameterEditWindow.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.SuspendLayout();
            // 
            // saveParameter
            // 
            this.saveParameter.Location = new System.Drawing.Point(147, 141);
            this.saveParameter.Name = "saveParameter";
            this.saveParameter.Size = new System.Drawing.Size(117, 28);
            this.saveParameter.TabIndex = 0;
            this.saveParameter.Text = "Save";
            this.saveParameter.UseVisualStyleBackColor = true;
            this.saveParameter.Click += new System.EventHandler(this.saveParameter_Click);
            // 
            // materialParameters
            // 
            this.materialParameters.FormattingEnabled = true;
            this.materialParameters.Location = new System.Drawing.Point(9, 115);
            this.materialParameters.Name = "materialParameters";
            this.materialParameters.Size = new System.Drawing.Size(255, 186);
            this.materialParameters.TabIndex = 1;
            // 
            // parameterIsBound
            // 
            this.parameterIsBound.AutoSize = true;
            this.parameterIsBound.Location = new System.Drawing.Point(9, 102);
            this.parameterIsBound.Name = "parameterIsBound";
            this.parameterIsBound.Size = new System.Drawing.Size(121, 17);
            this.parameterIsBound.TabIndex = 2;
            this.parameterIsBound.Text = "Is Bound To Shader";
            this.parameterIsBound.UseVisualStyleBackColor = true;
            // 
            // materialName
            // 
            this.materialName.Location = new System.Drawing.Point(9, 36);
            this.materialName.Name = "materialName";
            this.materialName.Size = new System.Drawing.Size(255, 20);
            this.materialName.TabIndex = 3;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(6, 20);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(35, 13);
            this.label1.TabIndex = 4;
            this.label1.Text = "Name";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(6, 59);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(31, 13);
            this.label2.TabIndex = 6;
            this.label2.Text = "Type";
            // 
            // parameterName
            // 
            this.parameterName.Location = new System.Drawing.Point(9, 36);
            this.parameterName.Name = "parameterName";
            this.parameterName.Size = new System.Drawing.Size(255, 20);
            this.parameterName.TabIndex = 7;
            // 
            // materialType
            // 
            this.materialType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.materialType.FormattingEnabled = true;
            this.materialType.Location = new System.Drawing.Point(9, 75);
            this.materialType.Name = "materialType";
            this.materialType.Size = new System.Drawing.Size(255, 21);
            this.materialType.TabIndex = 8;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(6, 99);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(60, 13);
            this.label3.TabIndex = 9;
            this.label3.Text = "Parameters";
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.editParam);
            this.groupBox1.Controls.Add(this.createParam);
            this.groupBox1.Controls.Add(this.deleteParam);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Controls.Add(this.label3);
            this.groupBox1.Controls.Add(this.materialParameters);
            this.groupBox1.Controls.Add(this.materialType);
            this.groupBox1.Controls.Add(this.materialName);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Location = new System.Drawing.Point(12, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(274, 346);
            this.groupBox1.TabIndex = 10;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Material Core";
            // 
            // editParam
            // 
            this.editParam.Location = new System.Drawing.Point(96, 307);
            this.editParam.Name = "editParam";
            this.editParam.Size = new System.Drawing.Size(81, 28);
            this.editParam.TabIndex = 18;
            this.editParam.Text = "Edit";
            this.editParam.UseVisualStyleBackColor = true;
            this.editParam.Click += new System.EventHandler(this.editParam_Click);
            // 
            // createParam
            // 
            this.createParam.Location = new System.Drawing.Point(183, 307);
            this.createParam.Name = "createParam";
            this.createParam.Size = new System.Drawing.Size(81, 28);
            this.createParam.TabIndex = 17;
            this.createParam.Text = "New";
            this.createParam.UseVisualStyleBackColor = true;
            this.createParam.Click += new System.EventHandler(this.createParam_Click);
            // 
            // deleteParam
            // 
            this.deleteParam.Location = new System.Drawing.Point(9, 307);
            this.deleteParam.Name = "deleteParam";
            this.deleteParam.Size = new System.Drawing.Size(81, 28);
            this.deleteParam.TabIndex = 16;
            this.deleteParam.Text = "Delete";
            this.deleteParam.UseVisualStyleBackColor = true;
            this.deleteParam.Click += new System.EventHandler(this.deleteParam_Click);
            // 
            // parameterEditWindow
            // 
            this.parameterEditWindow.Controls.Add(this.optionsListLabel);
            this.parameterEditWindow.Controls.Add(this.parameterOptionsList);
            this.parameterEditWindow.Controls.Add(this.saveParameter);
            this.parameterEditWindow.Controls.Add(this.parameterType);
            this.parameterEditWindow.Controls.Add(this.label5);
            this.parameterEditWindow.Controls.Add(this.parameterIsBound);
            this.parameterEditWindow.Controls.Add(this.label4);
            this.parameterEditWindow.Controls.Add(this.parameterName);
            this.parameterEditWindow.Location = new System.Drawing.Point(12, 364);
            this.parameterEditWindow.Name = "parameterEditWindow";
            this.parameterEditWindow.Size = new System.Drawing.Size(274, 178);
            this.parameterEditWindow.TabIndex = 11;
            this.parameterEditWindow.TabStop = false;
            this.parameterEditWindow.Text = "Parameter";
            this.parameterEditWindow.Visible = false;
            // 
            // optionsListLabel
            // 
            this.optionsListLabel.AutoSize = true;
            this.optionsListLabel.Location = new System.Drawing.Point(6, 99);
            this.optionsListLabel.Name = "optionsListLabel";
            this.optionsListLabel.Size = new System.Drawing.Size(211, 13);
            this.optionsListLabel.TabIndex = 16;
            this.optionsListLabel.Text = "Options (split options by comma - no space)";
            // 
            // parameterOptionsList
            // 
            this.parameterOptionsList.CharacterCasing = System.Windows.Forms.CharacterCasing.Lower;
            this.parameterOptionsList.Location = new System.Drawing.Point(9, 115);
            this.parameterOptionsList.Name = "parameterOptionsList";
            this.parameterOptionsList.Size = new System.Drawing.Size(255, 20);
            this.parameterOptionsList.TabIndex = 15;
            // 
            // parameterType
            // 
            this.parameterType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.parameterType.FormattingEnabled = true;
            this.parameterType.Location = new System.Drawing.Point(9, 75);
            this.parameterType.Name = "parameterType";
            this.parameterType.Size = new System.Drawing.Size(255, 21);
            this.parameterType.TabIndex = 12;
            this.parameterType.SelectedIndexChanged += new System.EventHandler(this.parameterType_SelectedIndexChanged);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(6, 59);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(31, 13);
            this.label5.TabIndex = 11;
            this.label5.Text = "Type";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(6, 20);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(35, 13);
            this.label4.TabIndex = 10;
            this.label4.Text = "Name";
            // 
            // saveMaterial
            // 
            this.saveMaterial.Location = new System.Drawing.Point(1006, 498);
            this.saveMaterial.Name = "saveMaterial";
            this.saveMaterial.Size = new System.Drawing.Size(149, 44);
            this.saveMaterial.TabIndex = 12;
            this.saveMaterial.Text = "Save";
            this.saveMaterial.UseVisualStyleBackColor = true;
            this.saveMaterial.Click += new System.EventHandler(this.saveMaterial_Click);
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.showOtherVars);
            this.groupBox3.Controls.Add(this.pixelShaderCode);
            this.groupBox3.Location = new System.Drawing.Point(292, 12);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(863, 480);
            this.groupBox3.TabIndex = 13;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Pixel Shader Code";
            // 
            // pixelShaderCode
            // 
            this.pixelShaderCode.DetectUrls = false;
            this.pixelShaderCode.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.pixelShaderCode.Location = new System.Drawing.Point(6, 20);
            this.pixelShaderCode.Name = "pixelShaderCode";
            this.pixelShaderCode.Size = new System.Drawing.Size(851, 428);
            this.pixelShaderCode.TabIndex = 0;
            this.pixelShaderCode.Text = "";
            // 
            // showOtherVars
            // 
            this.showOtherVars.Location = new System.Drawing.Point(6, 451);
            this.showOtherVars.Name = "showOtherVars";
            this.showOtherVars.Size = new System.Drawing.Size(188, 23);
            this.showOtherVars.TabIndex = 1;
            this.showOtherVars.Text = "Other Shader Variables";
            this.showOtherVars.UseVisualStyleBackColor = true;
            this.showOtherVars.Click += new System.EventHandler(this.showOtherVars_Click);
            // 
            // Editor
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1167, 552);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.saveMaterial);
            this.Controls.Add(this.parameterEditWindow);
            this.Controls.Add(this.groupBox1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.MaximizeBox = false;
            this.Name = "Editor";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Material Editor";
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.parameterEditWindow.ResumeLayout(false);
            this.parameterEditWindow.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button saveParameter;
        private System.Windows.Forms.ListBox materialParameters;
        private System.Windows.Forms.CheckBox parameterIsBound;
        private System.Windows.Forms.TextBox materialName;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox parameterName;
        private System.Windows.Forms.ComboBox materialType;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button editParam;
        private System.Windows.Forms.Button createParam;
        private System.Windows.Forms.Button deleteParam;
        private System.Windows.Forms.GroupBox parameterEditWindow;
        private System.Windows.Forms.ComboBox parameterType;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Button saveMaterial;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.RichTextBox pixelShaderCode;
        private System.Windows.Forms.Label optionsListLabel;
        private System.Windows.Forms.TextBox parameterOptionsList;
        private System.Windows.Forms.Button showOtherVars;
    }
}