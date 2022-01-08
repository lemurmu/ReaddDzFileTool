using FreeImageApiTest;
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

namespace ReadDzFileTool {
    public partial class FrmMain : Form {
        public FrmMain() {
            InitializeComponent();
            this.StartPosition = FormStartPosition.CenterScreen;
        }

        string fileName = "";
        internal Bitmap Bmp;
        string tgaDir = "";
        TreeNode selectNode;
        private void Button1_Click(object sender, EventArgs e) {
            this.openFileDialog.Filter = "dz文件(*.dz)|*.dz";
            if (this.openFileDialog.ShowDialog() == DialogResult.OK) {
                this.fileName_txt.Text = this.openFileDialog.FileName;
                fileName = this.openFileDialog.FileName;
            }
        }

        private void Button2_Click(object sender, EventArgs e) {
            if (string.IsNullOrEmpty(fileName))
                return;
            StringBuilder sb = new StringBuilder(fileName);
            this.richTextBox1.Clear();
            if (checkBox1.Checked) {
                this.richTextBox1.AppendText($"准备解压-->act:{fileName}\r\n");
                bool flag = ReadHelper.ReadActDzFile(sb);
                if (flag)
                    this.richTextBox1.AppendText($"act已解压到-->{AppDomain.CurrentDomain.BaseDirectory}\\Act文件夹\r\n");
                else
                    this.richTextBox1.AppendText("act解压-->失败!!\r\n");
            }
            else {
                this.richTextBox1.AppendText($"准备解压dz文件-->{fileName}\r\n");
                bool flag = ReadHelper.ReadDzFile(sb);
                if (flag) {
                    this.treeView.Nodes.Clear();
                    this.treeView.Nodes.Add(fileName);
                    this.treeView.Nodes[0].Tag = "Root";
                    try {
                        int dirIndex = fileName.LastIndexOf("\\");
                        int duff = fileName.LastIndexOf(".");
                        string dir = fileName.Substring(0, dirIndex);
                        tgaDir = dir;
                        string id = fileName.Substring(dirIndex + 1, duff - dirIndex - 1);
                        string[] tgaFiles = Directory.GetFiles(dir, "*.tga").Where(t => t.Contains(id)).ToArray();

                        for (int i = 0; i < tgaFiles.Length; i++) {
                            this.richTextBox1.AppendText($"解压生成-->tga图片:{tgaFiles[i]}\r\n");
                            int dirIndex2 = tgaFiles[i].LastIndexOf("\\");
                            string tgaName = tgaFiles[i].Substring(dirIndex2 + 1);
                            TreeNode node = this.treeView.Nodes[0].Nodes.Add(tgaName);
                            node.Tag = tgaName;
                        }

                    }
                    catch (Exception ex) {
                        this.richTextBox1.AppendText($"Error-->:{ex.Message}\r\n");
                    }
                    this.treeView.Nodes[0].ExpandAll();
                }
            }
        }

        private void TreeView_NodeMouseDoubleClick(object sender, TreeNodeMouseClickEventArgs e) {
            if (e.Node == null) return;
            if (e.Button == MouseButtons.Left) {//双击左键
                string tag = e.Node.Tag as string;
                if (tag != "Root") {
                    if (Bmp != null) Bmp.Dispose();
                    Bmp = (Bitmap)FreeImageToBitmap.LoadImageFormFreeImage(tgaDir + "\\" + tag);
                    if (Bmp != null) {
                        this.pictureBox1.Image = Bmp;
                        selectNode = e.Node;
                    }
                }

            }
        }

        private void TreeView_NodeMouseClick(object sender, TreeNodeMouseClickEventArgs e) {
            if (e.Node == null) return;
            if (e.Button == MouseButtons.Right) {
                string tag = e.Node.Tag as string;
                if (tag != "Root") {
                    this.contextMenuStrip.Show(MousePosition);
                    selectNode = e.Node;
                }
            }
        }

        private void 重命名ToolStripMenuItem_Click(object sender, EventArgs e) {
            string oldName = selectNode.Tag as string;
            FrmRename.Instance.SetOldName(oldName);
            FrmRename.Instance.StartPosition = FormStartPosition.CenterScreen;
            DialogResult result = FrmRename.Instance.ShowDialog();
            if (result == DialogResult.OK) {
                string newName = FrmRename.Instance.GetNewName();
                if (string.IsNullOrEmpty(newName))
                    newName = oldName;
                selectNode.Text = newName;
                selectNode.Tag = newName;
                Rename(tgaDir + "\\" + oldName, tgaDir, newName);
            }


        }

        private void Rename(string sourceFile, string destFilePath, string destFileName) {
            FileInfo tempFileInfo = new FileInfo(sourceFile);
            DirectoryInfo dicInfo = new DirectoryInfo(destFilePath);
            FileInfo destFileInfo = new FileInfo(destFilePath + "\\" + destFileName);
            try {
                if (!dicInfo.Exists)
                    dicInfo.Create();
                if (destFileInfo.Exists)
                    destFileInfo.Delete();
                tempFileInfo.MoveTo(destFilePath + "\\" + destFileName);
            }
            catch (Exception ex) {
                this.richTextBox1.AppendText($"Error-->:{ex.Message}\r\n");
            }
        }

        private void 删除ToolStripMenuItem_Click(object sender, EventArgs e) {
            string oldName = selectNode.Tag as string;
            FileInfo destFileInfo = new FileInfo(tgaDir + "\\" + oldName);
            if (destFileInfo.Exists)
                destFileInfo.Delete();
            selectNode.Remove();
        }
    }
}
