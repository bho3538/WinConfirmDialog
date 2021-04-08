using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using WinConfirmDialogNet;

namespace WinConfirmDialogNetSample
{
    class Program
    {

        [STAThread]
        static void Main(string[] args)
        {
            IntPtr hwnd = IntPtr.Zero; //or GetForegroundWindow()

            string[] sourceFileList = new string[2] { "C:\\Windows\\system32\\notepad.exe", "C:\\Users\\bho35\\Pictures\\capture2.png" };
            string[] targetFileList = new string[2] { "c:\\Windows\\notepad.exe", "c:\\Windows\\explorer.exe" };

            WinConfirmDialog confirmDialog = WinConfirmDialog.Initialize(true, true, WINCONFIRMDIALOGOPERATIONTYPE.FILECOPY);
            if(confirmDialog == null)
            {
                Console.WriteLine("Error to initialize WinConfirmDialog");
                return;
            }
            //enable 'keep both files' options. (use item alternative name)
            confirmDialog.SetAllowRenameToAltName(true);

            WinConfirmDialogItem item = new WinConfirmDialogItem();

            int sourceCnt = sourceFileList.Length;

            WINCONFIRMDIALOGRESULT results = WINCONFIRMDIALOGRESULT.NONE;
            for (int i = 0; i < sourceCnt; i++)
            {
                item.InitializeWithFileSystemPath(sourceFileList[i], "aa", targetFileList[i], null);

                if ((results & WINCONFIRMDIALOGRESULT.APPLYALL) != WINCONFIRMDIALOGRESULT.APPLYALL)
                {
                    confirmDialog.ShowDialog(hwnd, item, sourceCnt - (i + 1));
                    results = confirmDialog.GetResult();
                }


                if ((results & WINCONFIRMDIALOGRESULT.OVERWRITE) == WINCONFIRMDIALOGRESULT.OVERWRITE)
                {
                    Console.WriteLine("user select overwrite");
                }
                else if ((results & WINCONFIRMDIALOGRESULT.KEEPORIGINAL) == WINCONFIRMDIALOGRESULT.KEEPORIGINAL 
                    || (results & WINCONFIRMDIALOGRESULT.SKIPCURRENT) == WINCONFIRMDIALOGRESULT.SKIPCURRENT)
                {
                    Console.WriteLine("user select keep original or skip");
                }
                else if ((results & WINCONFIRMDIALOGRESULT.COPYWITHALTNAME) == WINCONFIRMDIALOGRESULT.COPYWITHALTNAME)
                {
                    Console.WriteLine("user select using new name");
                }
                else if ((results & WINCONFIRMDIALOGRESULT.CANCELALL) == WINCONFIRMDIALOGRESULT.CANCELALL)
                {
                    Console.WriteLine("user cancel all tasks");
                    break;
                }

                if ((results & WINCONFIRMDIALOGRESULT.APPLYALL) == WINCONFIRMDIALOGRESULT.APPLYALL)
                {
                    Console.WriteLine("user select apply all with current select option");
                }
            }


            Console.ReadLine();
        }
    }
}
