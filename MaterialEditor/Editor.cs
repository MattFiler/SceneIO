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
using System.Text.RegularExpressions;
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
            parameterType.Items.Add("TEXTURE_FILEPATH");
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
            parameterIsBound.Checked = thisParamData["is_bound"].Value<bool>();
            if (parameterType.SelectedItem.ToString() == "OPTIONS_LIST")
            {
                optionsListLabel.Visible = true;
                parameterOptionsList.Visible = true;
                parameterOptionsList.Text = "";
                JArray optionsVals = (JArray)thisParamData["values"];
                for (int i = 0; i < optionsVals.Count; i++)
                {
                    parameterOptionsList.Text += optionsVals[i] + ",";
                }
                parameterOptionsList.Text = parameterOptionsList.Text.Substring(0, parameterOptionsList.Text.Length - 1);
            }
        }

        /* Delete selected parameter */
        private void deleteParam_Click(object sender, EventArgs e)
        {
            parameterEditWindow.Visible = false;
            if (materialParameters.SelectedIndex == -1) return;
            DialogResult shouldDo = MessageBox.Show("Are you sure you wish to delete this parameter?", "Confirmation...", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
            if (shouldDo != DialogResult.Yes) return;

            config["materials"][config_index]["parameters"][materialParameters.SelectedIndex].Remove();

            RefreshParamList();
            MessageBox.Show("Deleted parameter!", "Deleted.", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }

        /* Save the selected parameter properties */
        private void saveParameter_Click(object sender, EventArgs e)
        {
            if (parameterName.Text == "")
            {
                MessageBox.Show("Parameter requires a name.", "Failed to save.", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            if (parameterName.Text.Contains(" "))
            {
                MessageBox.Show("Parameter name cannot contain spaces.", "Failed to save.", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            if (parameterType.SelectedItem.ToString() == "OPTIONS_LIST" && parameterOptionsList.Text == "")
            {
                MessageBox.Show("Options list requires pre-set options.", "Failed to save.", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            for (int i = 0; i < materialParameters.Items.Count; i++)
            {
                if (materialParameters.Items[i].ToString() == parameterName.Text)
                {
                    MessageBox.Show("Parameter name must be unique in material.", "Failed to save.", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }
            }

            if (creating_param)
            {
                JArray paramArray = (JArray)config["materials"][config_index]["parameters"];
                paramArray.Add(new JObject());
                edit_param_index = paramArray.Count - 1;
            }

            JObject thisParamData = (JObject)config["materials"][config_index]["parameters"][edit_param_index];
            thisParamData["name"] = parameterName.Text;
            thisParamData["type"] = parameterType.SelectedItem.ToString();
            thisParamData["is_bound"] = (parameterIsBound.Visible && parameterIsBound.Checked);

            if (thisParamData["type"].Value<string>() == "OPTIONS_LIST")
            {
                JArray optionsVals = new JArray();
                string[] splitString = parameterOptionsList.Text.Split(',');
                for (int i = 0; i < splitString.Length; i++)
                {
                    optionsVals.Add(splitString[i]);
                }
                thisParamData["values"] = optionsVals;
            }
            else if (thisParamData["values"] != null)
            {
                thisParamData.Remove("values");
            }

            parameterEditWindow.Visible = false;
            RefreshParamList();
            MessageBox.Show("Saved parameter!", "Saved.", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }

        /* Change available options based on type selection */
        private void parameterType_SelectedIndexChanged(object sender, EventArgs e)
        {
            parameterIsBound.Checked = false;
            parameterIsBound.Visible = (parameterType.SelectedItem.ToString() == "RGB" || parameterType.SelectedItem.ToString() == "TEXTURE_FILEPATH");
            parameterOptionsList.Text = "";
            parameterOptionsList.Visible = parameterType.SelectedItem.ToString() == "OPTIONS_LIST";
            optionsListLabel.Visible = parameterOptionsList.Visible;
        }

        /* Save the config back out if valid */
        private void saveMaterial_Click(object sender, EventArgs e)
        {
            if (materialParameters.Items.Count == 0)
            {
                MessageBox.Show("Material must have at least one parameter.", "Failed to save.", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            bool hasBindable = false;
            for (int i = 0; i < materialParameters.Items.Count; i++)
            {
                hasBindable = config["materials"][config_index]["parameters"][i]["is_bound"].Value<bool>();
                break;
            }
            if (!hasBindable)
            {
                MessageBox.Show("Material must have at least one bound parameter.", "Failed to save.", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            if (materialName.Text == "")
            {
                MessageBox.Show("Material must have a name.", "Failed to save.", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            if (pixelShaderCode.Text == "")
            {
                MessageBox.Show("Material must have pixel shader code.", "Failed to save.", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            JObject thisMatData = (JObject)config["materials"][config_index];
            thisMatData["name"] = materialName.Text;
            thisMatData["type"] = materialType.SelectedItem.ToString();
            thisMatData["pixel_shader"]["code"] = pixelShaderCode.Text;

            if (!CreateShader(thisMatData))
            {
                MessageBox.Show("Couldn't generate shader.\nPlease check pixel shader code.", "Failed to save.", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            File.WriteAllText("data/materials/material_config.json", config.ToString(Formatting.Indented));
            MessageBox.Show("Material saved!", "Saved.", MessageBoxButtons.OK, MessageBoxIcon.Information);
            this.Close();
        }

        /* Create the shader for this material in-editor */
        private bool CreateShader(JObject thisMatData)
        {
            string shaderPath = "data/materials/" + config["materials"][config_index]["name"].Value<string>() + ".fx";

            string baseShader = Properties.Resources.shader_template.ToString();
            List<string> baseShaderList = new List<string>(Regex.Split(baseShader, Environment.NewLine));
            List<string> finalShaderList = new List<string>();

            JArray allParams = (JArray)thisMatData["parameters"];
            int texBuffCount = 0;
            int cBuffCount = 1; //We have one base constant buffer

            for (int i = 0; i < baseShaderList.Count; i++)
            {
                if (baseShaderList[i] == "%CUSTOM_TEXTURES%")
                {
                    for (int x = 0; x < allParams.Count; x++) {
                        if (allParams[x]["type"].Value<string>() == "TEXTURE_FILEPATH")
                        {
                            finalShaderList.Add("Texture2D " + allParams[x]["name"].Value<string>() + " : register(t" + texBuffCount + ");");
                            texBuffCount++;
                        }
                    }
                    continue;
                }
                if (baseShaderList[i] == "%CUSTOM_CBUFFERS%")
                {
                    for (int x = 0; x < allParams.Count; x++)
                    {
                        if (allParams[x]["type"].Value<string>() == "RGB")
                        {
                            finalShaderList.Add("cbuffer ConstantBuffer" + cBuffCount + " : register(b" + cBuffCount + ") {");
                            finalShaderList.Add("    float4 " + allParams[x]["name"].Value<string>() + ";");
                            finalShaderList.Add("}");
                            cBuffCount++;
                        }
                    }
                    continue;
                }
                if (baseShaderList[i] == "%CUSTOM_PIXEL_SHADER%")
                {
                    List<string> pixelCode = new List<string>(Regex.Split(thisMatData["pixel_shader"]["code"].Value<string>(), Environment.NewLine));
                    for (int x = 0; x < pixelCode.Count; x++)
                    {
                        finalShaderList.Add("\t" + pixelCode[x]);
                    }
                    continue;
                }
                finalShaderList.Add(baseShaderList[i]);
            }

            if (File.Exists(shaderPath)) File.Delete(shaderPath);
            File.WriteAllLines(shaderPath, finalShaderList);
            return true;
        }
    }
}
