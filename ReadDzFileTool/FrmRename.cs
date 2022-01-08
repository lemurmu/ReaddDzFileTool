using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ReadDzFileTool {
    public partial class FrmRename : Form {
        public FrmRename() {
            InitializeComponent();
            this.textBox1.ReadOnly = true;
        }

        public static FrmRename Instance = new FrmRename();
        public string GetNewName() {
            return this.textBox2.Text;
        }

        public void SetOldName(string name) {
            this.textBox1.Text = name;
            this.textBox2.Text = name;
        }
    }
}
