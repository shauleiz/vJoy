using System;
using System.Collections.Generic;
using System.Windows.Forms;

namespace WrapperTest
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            TesterForm top = new TesterForm();
            new FfbInterface(top);
            Application.Run(top);
        }
    }
}
