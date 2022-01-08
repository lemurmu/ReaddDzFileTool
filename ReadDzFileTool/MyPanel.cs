using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
namespace FreeImageApiTest
{
    public class FlickFreePanel : Panel
    {
        public FlickFreePanel()
        {
            SetStyle(ControlStyles.UserPaint, true);
            SetStyle(ControlStyles.AllPaintingInWmPaint, true); // 禁止擦除背景.
            SetStyle(ControlStyles.DoubleBuffer, true); //双缓冲
        }
    }
}
