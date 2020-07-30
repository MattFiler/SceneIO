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
        JObject thisObject;
        int config_index;
        int edit_param_index;
        bool creating_param;
        Color default_tex_colour = Color.White;

        public Editor(JObject configObject = null, int materialIndex = -1)
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

            if (materialIndex == -1)
            {
                thisObject = new JObject();
                thisObject["parameters"] = new JArray();
                thisObject["pixel_shader"] = new JObject();
                return;
            }

            thisObject = (JObject)((JObject)config["materials"][config_index]).DeepClone();
            materialName.Text = thisObject["name"].Value<string>();
            materialName.ReadOnly = true;
            pixelShaderCode.Text = thisObject["pixel_shader"]["code"].Value<string>();
            materialType.SelectedItem = thisObject["type"].Value<string>().ToUpper();

            RefreshParamList();
        }
        private void RefreshParamList()
        {
            materialParameters.Items.Clear();
            foreach (JObject param_entry in thisObject["parameters"])
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
            default_tex_colour = Color.White;
            creating_param = false;
            parameterEditWindow.Visible = false;
            if (materialParameters.SelectedIndex == -1) return;
            parameterEditWindow.Visible = true;

            edit_param_index = materialParameters.SelectedIndex;

            JObject thisParamData = (JObject)thisObject["parameters"][edit_param_index];
            parameterName.Text = thisParamData["name"].Value<string>();
            parameterType.SelectedItem = thisParamData["type"].Value<string>().ToUpper();
            parameterIsBound.Checked = thisParamData["is_bound"].Value<bool>();
            if (parameterType.SelectedItem.ToString() == "TEXTURE_FILEPATH")
                default_tex_colour = Color.FromArgb(thisParamData["default_colour"][0].Value<int>(), thisParamData["default_colour"][1].Value<int>(), thisParamData["default_colour"][2].Value<int>());
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

            thisObject["parameters"][materialParameters.SelectedIndex].Remove();

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
            if (!Regex.IsMatch(materialName.Text, @"^[a-zA-Z]+$"))
            {
                MessageBox.Show("Parameter name cannot contain numbers or special characters.", "Failed to save.", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            if (parameterType.SelectedItem.ToString() == "OPTIONS_LIST" && parameterOptionsList.Text == "")
            {
                MessageBox.Show("Options list requires pre-set options.", "Failed to save.", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            for (int i = 0; i < materialParameters.Items.Count; i++)
            {
                if (i == edit_param_index) continue;
                if (materialParameters.Items[i].ToString() == parameterName.Text)
                {
                    MessageBox.Show("Parameter name must be unique in material.", "Failed to save.", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }
            }

            if (creating_param)
            {
                JArray paramArray = (JArray)thisObject["parameters"];
                paramArray.Add(new JObject());
                edit_param_index = paramArray.Count - 1;
            }

            JObject thisParamData = (JObject)thisObject["parameters"][edit_param_index];
            thisParamData["name"] = parameterName.Text;
            thisParamData["type"] = parameterType.SelectedItem.ToString();
            thisParamData["is_bound"] = (parameterIsBound.Visible && parameterIsBound.Checked);

            if (thisParamData["default_colour"] != null) thisParamData["default_colour"] = null;
            if (parameterType.SelectedItem.ToString() == "TEXTURE_FILEPATH")
            {
                thisParamData["default_colour"] = new JArray();
                ((JArray)thisParamData["default_colour"]).Add(default_tex_colour.R);
                ((JArray)thisParamData["default_colour"]).Add(default_tex_colour.G);
                ((JArray)thisParamData["default_colour"]).Add(default_tex_colour.B);
            }

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
            texDefaultColour.Visible = parameterType.SelectedItem.ToString() == "TEXTURE_FILEPATH";
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
            if (materialType.SelectedIndex == -1)
            {
                MessageBox.Show("Material requires a type to be selected.", "Failed to save.", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            if (materialType.SelectedItem.ToString() == "ENVIRONMENT" && config_index == -1)
            {
                foreach (JObject mat in (JArray)config["materials"])
                {
                    if (mat["type"].Value<string>() == "ENVIRONMENT")
                    {
                        MessageBox.Show("An environment material definition already exists.\nThere can only be one environment material!", "Failed to save.", MessageBoxButtons.OK, MessageBoxIcon.Error);
                        return;
                    }
                }
            }
            if (materialType.SelectedItem.ToString() == "ENVIRONMENT")
            {
                bool validColourNaming = false;
                foreach (JObject mat in (JArray)thisObject["parameters"])
                {
                    if (mat["name"].Value<string>() == "colour" && mat["type"].Value<string>() == "RGB")
                    {
                        validColourNaming = true;
                        break;
                    }
                }
                if (!validColourNaming)
                {
                    MessageBox.Show("This material must have an RGB parameter named 'colour'!", "Failed to save.", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }
            }
            bool hasBindable = false;
            for (int i = 0; i < materialParameters.Items.Count; i++)
            {
                hasBindable = thisObject["parameters"][i]["is_bound"].Value<bool>();
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
            if (materialName.Text.Contains(" "))
            {
                MessageBox.Show("Material name cannot contain spaces.", "Failed to save.", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            if (!Regex.IsMatch(materialName.Text, @"^[a-zA-Z]+$"))
            {
                MessageBox.Show("Material name cannot contain numbers or special characters.", "Failed to save.", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            if (pixelShaderCode.Enabled && pixelShaderCode.Text == "")
            {
                MessageBox.Show("Material must have pixel shader code.", "Failed to save.", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            thisObject["name"] = materialName.Text;
            thisObject["type"] = materialType.SelectedItem.ToString();
            thisObject["pixel_shader"]["code"] = pixelShaderCode.Text;

            if (!CreateShader())
            {
                MessageBox.Show("Couldn't generate shader.\nPlease check pixel shader code.", "Failed to save.", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            if (config_index == -1) ((JArray)config["materials"]).Add(thisObject);
            else ((JArray)config["materials"])[config_index] = thisObject;

            File.WriteAllText("data/materials/material_config.json", config.ToString(Formatting.Indented));
            MessageBox.Show("Material saved!", "Saved.", MessageBoxButtons.OK, MessageBoxIcon.Information);
            this.Close();
        }

        /* Create the shader for this material in-editor */
        private bool CreateShader()
        {
            string shaderPath = "data/materials/" + thisObject["name"].Value<string>() + ".fx";

            string baseShader = Properties.Resources.shader_template.ToString();
            List<string> baseShaderList = new List<string>(Regex.Split(baseShader, Environment.NewLine));
            List<string> finalShaderList = new List<string>();

            JArray allParams = (JArray)thisObject["parameters"];
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
                    List<string> pixelCode = new List<string>(Regex.Split(thisObject["pixel_shader"]["code"].Value<string>(), Environment.NewLine));
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

        /* Show other variables available in shader code */
        private void showOtherVars_Click(object sender, EventArgs e)
        {
            OtherShaderVariablesWindow win = new OtherShaderVariablesWindow();
            win.Show();
        }

        /* Not all material types take pixel shader code */
        private void materialType_SelectedIndexChanged(object sender, EventArgs e)
        {
            if ((materialType.SelectedItem.ToString() == "ENVIRONMENT")) pixelShaderCode.Text = "";
            pixelShaderCode.Enabled = !(materialType.SelectedItem.ToString() == "ENVIRONMENT");
        }

        /* Default colour selection for TEXTURE_FILEPATH types */
        private void texDefaultColour_Click(object sender, EventArgs e)
        {
            colorDialog1.Color = default_tex_colour;
            if (colorDialog1.ShowDialog() == DialogResult.OK)
            {
                default_tex_colour = colorDialog1.Color;
            }
        }
    }
}
