using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace WinConfirmDialogNet
{
    [StructLayout(LayoutKind.Sequential)]
    public struct CONFIRMOP
    {
        public int dwFlags;
        public int dwOperation;
        public int fFolder;
        public int stc1;
        public int stc2;
        public int stc3;
        public int stc4;
        public int cCurrentItem;
        public int cTotalItems;
        public int cRemaining;
        public IntPtr psiItem;
        public IntPtr psiDest;
        public long hrOpError;
        public IntPtr pwszOpText;
        public IntPtr par;
        public IntPtr hwndOwner;
        public int cSameError;
        public int cOtherResolvableError;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct CONFIRM_CONFLICT_PARAMS
    {
        public IntPtr hwndOwner;
        public int dwFlags;
        public int dwOperation;
        public int cCurrentItem;
        public int cTotalItems;
        public int cRemaining;
        public int cOtherRemainingResolvableError;
    };

    [StructLayout(LayoutKind.Sequential)]
    public struct CONFIRM_CONFLICT_RESULT
    {
        public uint resultCode;
        public int fApplyToAll;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct CONFIRM_CONFLICT_ITEM_INT
    {
        public IntPtr pShellItem2;
        public IntPtr pszOriginalName;
        public IntPtr pszAlternateName;
        public IntPtr pszLocationShort;
        public IntPtr pszLocationFull;
        public int nType;
    }

    public struct CONFIRM_CONFLICT_ITEM
    {
        public IntPtr pShellItem2;
        public string pszOriginalName;
        public string pszAlternateName;
        public string pszLocationShort;
        public string pszLocationFull;
        public int nType;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct CONFIRM_CONFLICT_RESULT_INFO
    {
        public IntPtr pszNewName;
        public int iItemIndex;
    }

    [ComImport(), Guid("14cc750c-7b0b-43dc-910e-b687f84e7c3b"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface ITransferConfirmation
    {
        [PreserveSig] int Confirm(ref CONFIRMOP op, ref int response, ref int flags);
        [PreserveSig] int ConfirmConflict(ref CONFIRM_CONFLICT_PARAMS param, ISyncMgrConflictItems pConflictItems, ref CONFIRM_CONFLICT_RESULT results, ref IntPtr pConflictResolutionItems);
    }

    [ComImport(), Guid("9c7ead52-8023-4936-a4db-d2a9a99e436a"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface ISyncMgrConflictItems
    {
        [PreserveSig] int GetCount(ref int pCount);
        [PreserveSig] int GetItem(int iIndex, ref CONFIRM_CONFLICT_ITEM_INT pItemInfo);
    }

    [ComImport(), Guid("458725B9-129D-4135-A998-9CEAFEC27007"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IConflictResolveItems
    {
        [PreserveSig] int GetCount(ref int pCount);
        [PreserveSig] int GetItem(int iIndex, ref CONFIRM_CONFLICT_RESULT_INFO pItemInfo);
    }

    public enum WINCONFIRMDIALOGRESULT
    {
        NONE = 0,
        OVERWRITE = 1,
        KEEPORIGINAL = 2,
        COPYWITHALTNAME = 4,
        SKIPCURRENT = 8,
        CANCELALL = 16,
        APPLYALL = 32,
    };

    public enum WINCONFIRMDIALOGOPERATIONTYPE
    {
        FILEMOVE = 1,
        FILECOPY = 2,
        SYNCCONFLICT = 3
    };
}
