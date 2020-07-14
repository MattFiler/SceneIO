using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace MaterialEditor
{
    public partial class Overview : Form
    {
        JObject comp_json_config;

        public Overview()
        {
            InitializeComponent();
            ReloadList();
        }

        /* Reload GUI List */
        private void ReloadList(object sender, EventArgs e)
        {
            ReloadList();
        }
        private void ReloadList()
        {
            comp_json_config = JObject.Parse(File.ReadAllText("data/material_config.json"));
            materialList.Items.Clear();
            foreach (JObject config_entry in comp_json_config["materials"])
            {
                materialList.Items.Add(config_entry["name"]);
            }
            this.BringToFront();
        }

        /* Create a new material */
        private void newMaterial_Click(object sender, EventArgs e)
        {
            Editor materialEditor = new Editor();
            materialEditor.FormClosed += new FormClosedEventHandler(ReloadList);
            materialEditor.Show();
        }

        /* Edit the selected material */
        private void editMaterial_Click(object sender, EventArgs e)
        {
            if (materialList.SelectedIndex == -1) return;
            Editor materialEditor = new Editor(comp_json_config, materialList.SelectedIndex);
            materialEditor.FormClosed += new FormClosedEventHandler(ReloadList);
            materialEditor.Show();
        }

        /* Delete the selected material */
        private void deleteMaterial_Click(object sender, EventArgs e)
        {
            if (materialList.SelectedIndex == -1) return;
            DialogResult shouldDo = MessageBox.Show("Are you sure you wish to delete this material?", "Confirmation...", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
            if (shouldDo != DialogResult.Yes) return;
            string shaderPath = "data/" + comp_json_config["materials"][materialList.SelectedIndex]["name"].Value<string>() + ".fx";
            if (File.Exists(shaderPath)) File.Delete(shaderPath);
            comp_json_config["materials"][materialList.SelectedIndex].Remove();
            File.WriteAllText("data/material_config.json", comp_json_config.ToString(Formatting.Indented));
            ReloadList();
            MessageBox.Show("Deleted material!", "Deleted.", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }
    }
}
