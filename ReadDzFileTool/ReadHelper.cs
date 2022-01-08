using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace ReadDzFileTool {
    public class ReadHelper {

        [DllImport("ReadDzFileCore.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern bool ReadActDzFile(StringBuilder filePath);

        [DllImport("ReadDzFileCore.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern bool ReadDzFile(StringBuilder filePath);
    }
}
