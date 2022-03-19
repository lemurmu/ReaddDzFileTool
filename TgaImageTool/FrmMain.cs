using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace TgaImageTool
{
    public partial class FrmMain : Form
    {
        public FrmMain()
        {
            InitializeComponent();
        }

        //定义变量
        private string folderDirPath;                            //图片文件夹地址
        private string picDirPath = null;                        //图片路径
        private List<string> imagePathList = new List<string>(); //获取列表图片路径
        private int index;                                       //获取选中列表图片序号

        public static Bitmap rotateImage(Bitmap b, float angle)
        {
            //create a new empty bitmap to hold rotated image
            Bitmap returnBitmap = new Bitmap(b.Width, b.Height);
            //make a graphics object from the empty bitmap
            Graphics g = Graphics.FromImage(returnBitmap);
            g.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.NearestNeighbor;
            //move rotation point to center of image
            g.TranslateTransform((float)b.Width / 2, (float)b.Height / 2);
            //rotate
            g.RotateTransform(angle);
            //move image back
            g.TranslateTransform(-(float)b.Width / 2, -(float)b.Height / 2);
            //draw passed in image onto graphics object
            g.DrawImage(b, new Point(0, 0));
            return returnBitmap;
        }

        #region 图片旋转函数
        /// <summary>
        /// 以逆时针为方向对图像进行旋转
        /// </summary>
        /// <param name="b">位图流</param>
        /// <param name="angle">旋转角度[0,360](前台给的)</param>
        /// <returns></returns>
        public static Bitmap Rotate(Bitmap b, int angle)
        {
            angle = angle % 360;
            //弧度转换
            double radian = angle * Math.PI / 180.0;
            double cos = Math.Cos(radian);
            double sin = Math.Sin(radian);
            //原图的宽和高
            int w = b.Width;
            int h = b.Height;
            int W = (int)(Math.Max(Math.Abs(w * cos - h * sin), Math.Abs(w * cos + h * sin)));
            int H = (int)(Math.Max(Math.Abs(w * sin - h * cos), Math.Abs(w * sin + h * cos)));
            //目标位图
            Bitmap dsImage = new Bitmap(W, H);
            System.Drawing.Graphics g = System.Drawing.Graphics.FromImage(dsImage);
            g.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.Bilinear;
            g.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.HighQuality;
            //计算偏移量
            Point Offset = new Point((W - w) / 2, (H - h) / 2);
            //构造图像显示区域：让图像的中心与窗口的中心点一致
            Rectangle rect = new Rectangle(Offset.X, Offset.Y, w, h);
            Point center = new Point(rect.X + rect.Width / 2, rect.Y + rect.Height / 2);
            g.TranslateTransform(center.X, center.Y);
            g.RotateTransform(360 - angle);
            //恢复图像在水平和垂直方向的平移
            g.TranslateTransform(-center.X, -center.Y);
            g.DrawImage(b, rect);
            //重至绘图的所有变换
            g.ResetTransform();
            g.Save();
            g.Dispose();
            //dsImage.Save("yuancd.jpg", System.Drawing.Imaging.ImageFormat.Jpeg);
            return dsImage;
        }
        #endregion 图片旋转函数
        private static System.Drawing.Image resizeImage(System.Drawing.Image imgToResize, Size size)
        {
            //获取图片宽度
            int sourceWidth = imgToResize.Width;
            //获取图片高度
            int sourceHeight = imgToResize.Height;

            float nPercent = 0;
            float nPercentW = 0;
            float nPercentH = 0;
            //计算宽度的缩放比例
            nPercentW = ((float)size.Width / (float)sourceWidth);
            //计算高度的缩放比例
            nPercentH = ((float)size.Height / (float)sourceHeight);

            if (nPercentH < nPercentW)
                nPercent = nPercentH;
            else
                nPercent = nPercentW;
            //期望的宽度
            int destWidth = (int)(sourceWidth * nPercent);
            //期望的高度
            int destHeight = (int)(sourceHeight * nPercent);

            Bitmap b = new Bitmap(destWidth, destHeight);
            Graphics g = Graphics.FromImage((System.Drawing.Image)b);
            g.InterpolationMode = InterpolationMode.HighQualityBicubic;
            //绘制图像
            g.DrawImage(imgToResize, 0, 0, destWidth, destHeight);
            g.Dispose();
            return (System.Drawing.Image)b;
        }
        /// <SUMMARY>
        /// 图片无损缩放 自定义函数生成缩略图
        /// </SUMMARY>
        /// <PARAM name="sourceFile">图片源路径</PARAM>
        /// <PARAM name="destFile">缩放后图片输出路径</PARAM>
        /// <PARAM name="destHeight">缩放后图片高度</PARAM>
        /// <PARAM name="destWidth">缩放后图片宽度</PARAM>
        /// <RETURNS></RETURNS>
        public static bool GetThumbnail(string sourceFile, string destFile, int destHeight, int destWidth)
        {
            System.Drawing.Image imgSource = System.Drawing.Image.FromFile(sourceFile);
            System.Drawing.Imaging.ImageFormat thisFormat = imgSource.RawFormat;
            int sW = 0, sH = 0;

            // 按比例缩放
            int sWidth = imgSource.Width;
            int sHeight = imgSource.Height;

            if (sHeight > destHeight || sWidth > destWidth)
            {
                if ((sWidth * destHeight) > (sHeight * destWidth))
                {
                    sW = destWidth;
                    sH = (destWidth * sHeight) / sWidth;
                }
                else
                {
                    sH = destHeight;
                    sW = (sWidth * destHeight) / sHeight;
                }
            }
            else
            {
                sW = sWidth;
                sH = sHeight;
            }

            //新建一个bmp图片  
            Bitmap outBmp = new Bitmap(destWidth, destHeight);
            //新建一个画板
            Graphics g = Graphics.FromImage(outBmp);
            //清空画布并以透明背景色填充 Color.Black黑色填充
            g.Clear(System.Drawing.Color.Transparent);
            //设置画布的描绘质量
            g.CompositingQuality = CompositingQuality.HighQuality;
            //设置高质量，低速度呈现平滑程度
            g.SmoothingMode = SmoothingMode.HighQuality;
            //设置高质量插值法
            g.InterpolationMode = InterpolationMode.HighQualityBicubic;
            //在指定位置并且按指定大小绘制原图片的指定部分
            g.DrawImage(imgSource, new Rectangle((destWidth - sW) / 2, (destHeight - sH) / 2, sW, sH), 0, 0, imgSource.Width, imgSource.Height, GraphicsUnit.Pixel);
            g.Dispose();

            //以下代码为保存图片时 设置压缩质量
            EncoderParameters encoderParams = new EncoderParameters();
            long[] quality = new long[1];
            quality[0] = 100;
            EncoderParameter encoderParam = new EncoderParameter(System.Drawing.Imaging.Encoder.Quality, quality);
            encoderParams.Param[0] = encoderParam;

            try
            {
                //获得包含有关内置图像编码解码器的信息的ImageCodecInfo对象
                ImageCodecInfo[] arrayICI = ImageCodecInfo.GetImageEncoders();
                ImageCodecInfo jpegICI = null;
                for (int x = 0; x < arrayICI.Length; x++)
                {
                    if (arrayICI[x].FormatDescription.Equals("JPEG"))
                    {
                        jpegICI = arrayICI[x]; //设置JPEG编码
                        break;
                    }
                }
                //保存为JPG格式图片
                if (jpegICI != null)
                {
                    outBmp.Save(destFile, jpegICI, encoderParams);
                }
                else
                {
                    outBmp.Save(destFile, thisFormat);
                }

                return true;
            }
            catch (Exception e)
            {
                throw e;
            }
            finally
            {
                imgSource.Dispose();
                outBmp.Dispose();
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            Image image = Image.FromFile("test.jpg");
            image = resizeImage(image, new Size { Height = 400, Width = 200 });
            Bitmap returnBitmap = new Bitmap(image);
            pictureBox1.Image = Rotate(returnBitmap, 180);
        }

        private void button9_Click(object sender, EventArgs e)
        {
            try
            {
                //打开选择文件夹对话框
                FolderBrowserDialog folderBrowserDialog = new FolderBrowserDialog();
                DialogResult result = folderBrowserDialog.ShowDialog();
                if (result == DialogResult.OK)
                {
                    //获取用户选择的文件夹路径
                    this.folderDirPath = folderBrowserDialog.SelectedPath;
                    //调用自定义函数显示图片列表至ListView控件
                    ShowPicture();
                }
                else if (result == DialogResult.Cancel)
                {
                    MessageBox.Show("取消显示图片列表");
                }
            }
            catch (Exception msg)
            {
                //报错提示 未将对象引用设置到对象的实例
                throw msg;
            }
        }

        //显示图片列表至ListView控件
        private void ShowPicture()
        {
            //提供一种方法测试运行时间 开始计算
            //参考资料:http://www.cnblogs.com/newstart/archive/2012/09/21/2696884.html
            Stopwatch sw = new Stopwatch();
            sw.Start();

            //获取目录与子目录
            DirectoryInfo dir = new DirectoryInfo(folderDirPath);
            //获取当前目录JPG文件列表 GetFiles获取指定目录中文件的名称(包括其路径)
            FileInfo[] fileInfo = dir.GetFiles("*.png");
            //防止图片失真
            this.imageList1.ColorDepth = ColorDepth.Depth32Bit;
            for (int i = 0; i < fileInfo.Length; i++)
            {
                //获取文件完整目录
                picDirPath = fileInfo[i].FullName;
                //记录图片源路径 双击显示图片时使用
                imagePathList.Add(picDirPath);
                //图片加载到ImageList控件和imageList图片列表
                this.imageList1.Images.Add(Image.FromFile(picDirPath));
            }

            //显示文件列表
            this.listView1.Items.Clear();
            this.listView1.LargeImageList = this.imageList1;
            this.listView1.View = View.LargeIcon;        //大图标显示
                                                         //imageList1.ImageSize = new Size(40, 40);   //不能设置ImageList的图像大小 属性处更改

            //开始绑定
            this.listView1.BeginUpdate();
            //增加图片至ListView控件中
            for (int i = 0; i < imageList1.Images.Count; i++)
            {
                ListViewItem lvi = new ListViewItem();
                lvi.ImageIndex = i;
                lvi.Text = "pic" + i;
                this.listView1.Items.Add(lvi);
            }
            this.listView1.EndUpdate();

            //显示打开图片列表所需时间
            sw.Stop();
            long secords = sw.ElapsedMilliseconds; //毫秒单位
            // label1.Text += '\n' + (Convert.ToDouble(secords) / 1000).ToString();  //转换为秒
        }

    }
}
