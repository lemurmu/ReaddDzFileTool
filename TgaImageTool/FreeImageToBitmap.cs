using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.Drawing;
using System.Drawing.Imaging;
using System.Windows.Forms;

namespace FreeImageApiTest
{
    unsafe class FreeImageToBitmap
    {

        private enum FREE_IMAGE_FORMAT
        {
            FIF_UNKNOWN = -1,
            FIF_BMP = 0,
            FIF_ICO = 1,
            FIF_JPEG = 2,
            FIF_JNG = 3,
            FIF_KOALA = 4,
            FIF_LBM = 5,
            FIF_IFF = FIF_LBM,
            FIF_MNG = 6,
            FIF_PBM = 7,
            FIF_PBMRAW = 8,
            FIF_PCD = 9,
            FIF_PCX = 10,
            FIF_PGM = 11,
            FIF_PGMRAW = 12,
            FIF_PNG = 13,
            FIF_PPM = 14,
            FIF_PPMRAW = 15,
            FIF_RAS = 16,
            FIF_TARGA = 17,
            FIF_TIFF = 18,
            FIF_WBMP = 19,
            FIF_PSD = 20,
            FIF_CUT = 21,
            FIF_XBM = 22,
            FIF_XPM = 23,
            FIF_DDS = 24,
            FIF_GIF = 25,
            FIF_HDR = 26
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct RGBQUAD
        {
            internal byte Blue;
            internal byte Green;
            internal byte Red;
            internal byte Reserved;
        }

        [DllImport("FreeImage.dll", EntryPoint = "_FreeImage_Load@12", SetLastError = true)]
        private static extern IntPtr FreeImage_Load(FREE_IMAGE_FORMAT fif, string FileName, int Flag);

        [DllImport("FreeImage.dll", EntryPoint = "_FreeImage_GetFileType@8", SetLastError = true)]
        private static extern FREE_IMAGE_FORMAT FreeImage_GetFileType(string FileName, int Size);

        [DllImport("FreeImage.dll", EntryPoint = "_FreeImage_GetFIFFromFilename@4", SetLastError = true)]
        private static extern FREE_IMAGE_FORMAT FreeImage_GetFIFFromFilename(string FileName);

        [DllImport("FreeImage.dll", EntryPoint = "_FreeImage_FIFSupportsReading@4", SetLastError = true)]
        private static extern FREE_IMAGE_FORMAT FreeImage_FIFSupportsReading(FREE_IMAGE_FORMAT fif);

        [DllImport("FreeImage.dll", EntryPoint = "_FreeImage_GetWidth@4", SetLastError = true)]
        private static extern int FreeImage_GetWidth(IntPtr Dib);

        [DllImport("FreeImage.dll", EntryPoint = "_FreeImage_GetHeight@4", SetLastError = true)]
        private static extern int FreeImage_GetHeight(IntPtr Dib);

        [DllImport("FreeImage.dll", EntryPoint = "_FreeImage_GetPalette@4", SetLastError = true)]
        private static extern RGBQUAD* FreeImage_GetPalette(IntPtr Dib);

        [DllImport("FreeImage.dll", EntryPoint = "_FreeImage_GetBPP@4", SetLastError = true)]
        private static extern int FreeImage_GetBPP(IntPtr Dib);

        [DllImport("FreeImage.dll", EntryPoint = "_FreeImage_GetDIBSize@4", SetLastError = true)]
        private static extern int FreeImage_GetDIBSize(IntPtr Dib);

        [DllImport("FreeImage.dll", EntryPoint = "_FreeImage_GetColorsUsed@4", SetLastError = true)]
        private static extern int FreeImage_GetColorsUsed(IntPtr Dib);

        [DllImport("FreeImage.dll", EntryPoint = "_FreeImage_GetPitch@4", SetLastError = true)]
        private static extern int FreeImage_GetPitch(IntPtr Dib);

        [DllImport("FreeImage.dll", EntryPoint = "_FreeImage_GetBits@4", SetLastError = true)]
        private static extern IntPtr FreeImage_GetBits(IntPtr Dib);

        [DllImport("FreeImage.dll", EntryPoint = "_FreeImage_Unload@4", SetLastError = true)]
        private static extern int FreeImage_Free(IntPtr Dib);

        [DllImport("FreeImage.dll", EntryPoint = "_FreeImage_FlipVertical@4", SetLastError = true)]
        private static extern int FreeImage_FlipVertical(IntPtr Dib);

        public static Bitmap LoadImageFormFreeImage(string FileName)
        {
            Bitmap Bmp = null;
            FREE_IMAGE_FORMAT fif = FREE_IMAGE_FORMAT.FIF_UNKNOWN; ;
            fif = FreeImage_GetFileType(FileName, 0);
            if (fif == FREE_IMAGE_FORMAT.FIF_UNKNOWN)
            {
                fif = FreeImage_GetFIFFromFilename(FileName);
            }

            if ((fif != FREE_IMAGE_FORMAT.FIF_UNKNOWN) && (FreeImage_FIFSupportsReading(fif) != 0))
            {
                IntPtr Dib = FreeImage_Load(fif, FileName, 0);
                int Bpp = FreeImage_GetBPP(Dib);
                PixelFormat PF;
                int Width, Height, Stride;
                switch (Bpp)
                {
                    case 1:
                        PF = PixelFormat.Format1bppIndexed; break;
                    case 4:
                        PF = PixelFormat.Format4bppIndexed; break;
                    case 8:
                        PF = PixelFormat.Format8bppIndexed; break;
                    case 16:
                        PF = PixelFormat.Format16bppRgb555; break;
                    case 24:
                        PF = PixelFormat.Format24bppRgb; break;
                    case 32:
                        PF = PixelFormat.Format32bppArgb; break;
                    default:
                        FreeImage_Free(Dib);
                        return null;
                }
                Width = FreeImage_GetWidth(Dib);                        //  图像宽度
                Height = FreeImage_GetHeight(Dib);                      //  图像高度
                Stride = FreeImage_GetPitch(Dib);                       //  图像扫描行的大小,必然是4的整数倍

                /**  方案1：存在内存泄露
                    *  FreeImage_FlipVertical(Dib);                        // 因为FreeImage的认为的图像的起点在左下角，不进行翻转则图像的倒过来的
                    *  IntPtr Bits = FreeImage_GetBits(Dib);               // 得到图像数据在内存中的地址
                    *  Bmp = new Bitmap(Width, Height, Stride, PF, Bits);  // 实际上调用的GdipCreateBitmapFromScan0函数从内存创建位图
                **/


                //方案2：
                IntPtr Bits = FreeImage_GetBits(Dib);
                Bmp = new Bitmap(Width, Height, Stride, PF, Bits);
                Bmp.RotateFlip(RotateFlipType.RotateNoneFlipY);         // 调用GDI+自己的旋转函数
                if (Bpp <= 8)
                {
                    ColorPalette Pal = Bmp.Palette;                     //  设置调色板
                    RGBQUAD* GdiPal = FreeImage_GetPalette(Dib);
                    int ClrUsed = FreeImage_GetColorsUsed(Dib);
                    for (int I = 0; I < ClrUsed; I++)
                    {
                        Pal.Entries[I] = Color.FromArgb(255, GdiPal[I].Red, GdiPal[I].Green, GdiPal[I].Blue);
                    }
                    Bmp.Palette = Pal;
                }
                FreeImage_Free(Dib);                                        // 使用方案2则可以立马释放
                return Bmp;
            }
            return null;
        }
    }
}

