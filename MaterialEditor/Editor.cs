using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace MaterialEditor
{
    public partial class Editor : Form
    {
        JObject config;
        int config_index;
        int edit_param_index;
        bool creating_param;

        public Editor(JObject configObject = null, int materialIndex = 0)
        {
            config = configObject;
            config_index = materialIndex;

            InitializeComponent();

            /* POPULATE MATERIAL TYPES (should match MaterialSurfaceTypes enum in DynamicMaterial.h) */
            materialType.Items.Add("SURFACE");
            materialType.Items.Add("VOLUME");
            materialType.Items.Add("ENVIRONMENT");

            /* POPULATE PARAMETER TYPES (should match DataTypes enum in DataTypes.h) */
            parameterType.Items.Add("RGB");
            parameterType.Items.Add("STRING");
            parameterType.Items.Add("FLOAT");
            parameterType.Items.Add("INTEGER");
            parameterType.Items.Add("UNSIGNED_INTEGER");
            parameterType.Items.Add("BOOLEAN");
            parameterType.Items.Add("FLOAT_ARRAY");
            parameterType.Items.Add("OPTIONS_LIST");

            if (configObject == null) return;

            JObject thisMatData = (JObject)config["materials"][config_index];
            materialName.Text = thisMatData["name"].Value<string>();
            materialName.ReadOnly = true;
            materialType.SelectedItem = thisMatData["type"].Value<string>().ToUpper();
            pixelShaderCode.Text = thisMatData["pixel_shader"]["code"].Value<string>();

            RefreshParamList();
        }
        private void RefreshParamList()
        {
            materialParameters.Items.Clear();
            foreach (JObject param_entry in config["materials"][config_index]["parameters"])
            {
                materialParameters.Items.Add(param_entry["name"]);
            }
        }

        /* Open creation for selected parameter */
        private void createParam_Click(object sender, EventArgs e)
        {
            creating_param = true;
            parameterEditWindow.Visible = true;

            parameterName.Text = "";
            parameterType.SelectedIndex = 0;
            parameterIsBound.Checked = false;
        }

        /* Open edit for selected parameter */
        private void editParam_Click(object sender, EventArgs e)
        {
            creating_param = false;
            parameterEditWindow.Visible = false;
            if (materialParameters.SelectedIndex == -1) return;
            parameterEditWindow.Visible = true;

            edit_param_index = materialParameters.SelectedIndex;

            JObject thisParamData = (JObject)config["materials"][config_index]["parameters"][edit_param_index];
            parameterName.Text = thisParamData["name"].Value<string>();
            parameterType.SelectedItem = thisParamData["type"].Value<string>().ToUpper();
            parameterIsBound.Checked = (thisParamData["binding"] != null);
        }

        /* Delete selected parameter */
        private void deleteParam_Click(object sender, EventArgs e)
        {
            parameterEditWindow.Visible = false;
            if (materialParameters.SelectedIndex == -1) return;
            DialogResult shouldDo = MessageBox.Show("Are you sure you wish to delete this parameter?", "Confirmation...", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
            if (shouldDo != DialogResult.Yes) return;

            config["materials"][config_index]["parameters"][edit_param_index].Remove();

            RefreshParamList();
        }

        /* Save the selected parameter properties */
        private void saveParameter_Click(object sender, EventArgs e)
        {
            if (parameterName.Text == "") return;

            if (creating_param)
            {
                JArray paramArray = (JArray)config["materials"][config_index]["parameters"];
                paramArray.Add(new JObject());
                edit_param_index = paramArray.Count - 1;
            }

            JObject thisParamData = (JObject)config["materials"][config_index]["parameters"][edit_param_index];
            thisParamData["name"] = parameterName.Text;
            thisParamData["type"] = parameterType.SelectedItem.ToString();
            if (parameterIsBound.Checked)
            {
                thisParamData["binding"] = "tbd";
            }
            else
            {
                thisParamData["binding"] = null;
            }

            parameterEditWindow.Visible = false;
            RefreshParamList();
        }

        /* Save the config back out if valid */
        private void saveMaterial_Click(object sender, EventArgs e)
        {
            //todo
        }
    }
}
