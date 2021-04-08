using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace WinConfirmDialogNet
{
    public class WinConfirmDialog
    {
        /**
	    * SetSkipOption: Enable 'skip' button in dialog.
	    * @param bCanSkip: If 'TRUE' dialog show skip button, else dialog does not show skip button.
	    */
        public void SetSkipOption(bool bCanSkip)
        {
            if (bCanSkip)
            {
                this._dwConfirmDialogFlags |= 2;
            }
            else
            {
                this._dwConfirmDialogFlags &= ~2;
            }
        }

        /**
        * SetApplyAllOption: Enable 'Apply to all' checkbox in dialog.
        * @param bCanSkip: If 'TRUE' dialog show apply all checkbox, else dialog does not show checkbox.
        */
        public void SetApplyAllOption(bool bCanApplyAll)
        {
            if (bCanApplyAll)
            {
                this._dwConfirmDialogFlags |= 1;
            }
            else
            {
                this._dwConfirmDialogFlags &= ~1;
            }
        }

        /**
        * SetAllowRenameToAltName: Enable 'Move(Copy),But keep both files' options in dialog. (using item's alternative name)
        * @param bCanApplyAll: If 'TRUE' dialog show keep both files options, else dialog does not show keep both files options.
        */
        public void SetAllowRenameToAltName(bool bCanUseAltName)
        {
            if (bCanUseAltName)
            {
                this._dwConfirmDialogFlags |= 32;
            }
            else
            {
                this._dwConfirmDialogFlags &= ~32;
            }
        }

        /**
        * ShowDialog: Show Conflict Dialog.
        * @param hwnd: A handle to the window that owns the dialog box.
        * @param item: Conflict item's info.
        * @param dwRemainCnt: Remain items count.
        */
        public void ShowDialog(IntPtr hwnd, WinConfirmDialogItem item, int dwRemainCnt)
        {
            ShowDialogAdv(hwnd, item, dwRemainCnt);
        }

        /**
        * ShowDialogAdv: Show Conflict Dialog. for custom ISyncMgrConflictItems implements
        * @param hwnd: A handle to the window that owns the dialog box.
        * @param item: ISyncMgrConflictItems interface.
        * @param dwRemainCnt: Remain items count.
        */
        public void ShowDialogAdv(IntPtr hwnd, ISyncMgrConflictItems item, int dwRemainCnt)
        {
            int hr = 0;
            IntPtr pResolutionItems = IntPtr.Zero;
            IConflictResolveItems resolutionItems = null;

            CONFIRM_CONFLICT_PARAMS conflictParams = new CONFIRM_CONFLICT_PARAMS() { hwndOwner = hwnd, dwFlags = this._dwConfirmDialogFlags, dwOperation = this._dwOperationType, cRemaining = dwRemainCnt };
            CONFIRM_CONFLICT_RESULT conflictResults = new CONFIRM_CONFLICT_RESULT();
            CONFIRM_CONFLICT_RESULT_INFO resultItem = new CONFIRM_CONFLICT_RESULT_INFO();

            if (!this._bInitialized)
            {
                return;
            }

            if (dwRemainCnt == 0)
            {
                conflictParams.dwFlags &= ~1;
            }

            this._dwResults = 0;

            hr = this._ConfirmDialog.ConfirmConflict(ref conflictParams, item, ref conflictResults, ref pResolutionItems);

            if (conflictResults.resultCode == S_OK)
            {
                if (pResolutionItems != IntPtr.Zero)
                {
                    resolutionItems = (IConflictResolveItems)Marshal.GetObjectForIUnknown(pResolutionItems);
                    if (resolutionItems != null)
                    {
                        hr = resolutionItems.GetItem(0, ref resultItem);
                        if (hr == S_OK)
                        {
                            if (resultItem.pszNewName != IntPtr.Zero)
                            {
                                this._dwResults = WINCONFIRMDIALOGRESULT.COPYWITHALTNAME;
                                //CoTaskMemFree(resultItem.pszNewName);
                                Marshal.FreeCoTaskMem(resultItem.pszNewName);
                            }
                            else
                            {
                                if (resultItem.iItemIndex == 0)
                                {
                                    this._dwResults = WINCONFIRMDIALOGRESULT.OVERWRITE;
                                }
                                else
                                {
                                    this._dwResults = WINCONFIRMDIALOGRESULT.SKIPCURRENT;
                                }
                            }
                        }
                        Marshal.ReleaseComObject(resolutionItems);
                    }
                    Marshal.Release(pResolutionItems);
                }
            }
            else if (conflictResults.resultCode == 0x80000000)
            {
                this._dwResults = (WINCONFIRMDIALOGRESULT.CANCELALL | WINCONFIRMDIALOGRESULT.APPLYALL);
            }
            else if (conflictResults.resultCode == 0x80000001)
            {
                this._dwResults = WINCONFIRMDIALOGRESULT.SKIPCURRENT;
            }

            if (conflictResults.fApplyToAll > 0)
            {
                this._dwResults |= WINCONFIRMDIALOGRESULT.APPLYALL;
            }
        }

        /**
        * GetResult: Get Conflict Dialog Results. Return 'WINCONFIRMDIALOGRESULT' enum value.
        */
        public WINCONFIRMDIALOGRESULT GetResult()
        {
            return (WINCONFIRMDIALOGRESULT)this._dwResults;
        }

        /**
        * Initialize: Create and initialize WinConfirmDialog instance.
        * @param bCanApplyAll: Enable 'Apply to all' checkbox in dialog.
        * @param bCanSkip: Enable 'skip' button in dialog.
        * @param dialogType: Set Dialog Operation type. value must in 'WINCONFIRMDIALOGOPERATIONTYPE' enum
        */
        public static WinConfirmDialog Initialize(bool bCanApplyAll, bool bCanSkip, WINCONFIRMDIALOGOPERATIONTYPE dialogType)
        {
            int hr = S_OK;
            WinConfirmDialog confirmDialog = new WinConfirmDialog(bCanApplyAll, bCanSkip, dialogType);

            hr = confirmDialog._Initialize();
            if(hr != 0)
            {
                confirmDialog = null;
            }

            return confirmDialog;
        }

        private WinConfirmDialog(bool bCanApplyAll,bool bCanSkip, WINCONFIRMDIALOGOPERATIONTYPE dialogType)
        {
            this._dwResults = WINCONFIRMDIALOGRESULT.NONE;
            this._dwOperationType = (int)dialogType;

            this._dwConfirmDialogFlags = 4;
            SetSkipOption(bCanSkip);
            SetApplyAllOption(bCanApplyAll);
        }

        ~WinConfirmDialog()
        {
            if(_ConfirmDialog != null)
            {
                Marshal.ReleaseComObject(_ConfirmDialog);
            }
            if(_pConfirmDialog != IntPtr.Zero)
            {
                Marshal.Release(_pConfirmDialog);
            }
        }

        private int _Initialize()
        {
            int re = 1;

            re = CoCreateInstance(ref CLSID_TransferConfirmationUI, IntPtr.Zero, 1,ref IID_ITransferConfirmation, out this._pConfirmDialog);

            if (re == E_NOINTERFACE)
            {
                //windows vista , 7
                re = CoCreateInstance(ref CLSID_TransferConfirmationUI, IntPtr.Zero, 1,ref IID_IUnknown, out this._pConfirmDialog);
            }

            if (re == S_OK)
            {
                this._ConfirmDialog = (ITransferConfirmation)Marshal.GetObjectForIUnknown(this._pConfirmDialog);
                if (_ConfirmDialog != null)
                {
                    _bInitialized = true;
                }
            }

            return re;
        }

        private IntPtr _pConfirmDialog = IntPtr.Zero;
        private ITransferConfirmation _ConfirmDialog = null;

        private int _dwOperationType;
        private WINCONFIRMDIALOGRESULT _dwResults;
        private int _dwConfirmDialogFlags;
        private bool _bInitialized;

        //----------WinAPI ------------

        private const int S_OK = 0;
        private const int E_NOINTERFACE = -2147467262;

        private static Guid CLSID_TransferConfirmationUI = new Guid("6b831e4f-a50d-45fc-842f-16ce27595359");
        private static Guid IID_ITransferConfirmation = new Guid("14cc750c-7b0b-43dc-910e-b687f84e7c3b");
        private static Guid IID_IUnknown = new Guid("00000000-0000-0000-c000-000000000046");

        [DllImport("ole32.Dll")]
        private static extern int CoCreateInstance(ref Guid clsid, IntPtr inner, uint context, ref Guid uuid, out IntPtr ppv);


    }
}
