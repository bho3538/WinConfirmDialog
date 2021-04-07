using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace WinConfirmDialogNet
{
    public class WinConfirmDialogItem : ISyncMgrConflictItems
    {
        public int GetCount(ref int pCount)
        {
            pCount = this._items.Count;
            return 0;
        }

        public int GetItem(int iIndex, ref CONFIRM_CONFLICT_ITEM_INT pItemInfo)
        {
            if (iIndex >= this._items.Count)
            {
                return -2147467259; //E_FAIL
            }

            CONFIRM_CONFLICT_ITEM item = this._items[iIndex];

            pItemInfo.nType = item.nType;
            pItemInfo.pShellItem2 = item.pShellItem2;
            Marshal.AddRef(pItemInfo.pShellItem2);

            if (!string.IsNullOrEmpty(item.pszAlternateName))
            {
                pItemInfo.pszAlternateName = Marshal.StringToCoTaskMemUni(item.pszAlternateName);
            }
            else
            {
                pItemInfo.pszAlternateName = IntPtr.Zero;
            }

            if (!string.IsNullOrEmpty(item.pszLocationFull))
            {
                pItemInfo.pszLocationFull = Marshal.StringToCoTaskMemUni(item.pszLocationFull);
            }
            else
            {
                pItemInfo.pszLocationFull = IntPtr.Zero;
            }

            if (!string.IsNullOrEmpty(item.pszLocationShort))
            {
                pItemInfo.pszLocationShort = Marshal.StringToCoTaskMemUni(item.pszLocationShort);
            }
            else
            {
                pItemInfo.pszLocationShort = IntPtr.Zero;
            }

            if (!string.IsNullOrEmpty(item.pszOriginalName))
            {
                pItemInfo.pszOriginalName = Marshal.StringToCoTaskMemUni(item.pszOriginalName);
            }
            else
            {
                pItemInfo.pszOriginalName = IntPtr.Zero;
            }

            return 0;
        }

        /**
	    * InitializeWithFileSystemPath: Initialize ConflictDialogItem using filesystem path.
	    * @param sourcePath: source item path.
	    * @param sourceAltName: replacement name when 'keep both files' option. can be null.
	    * @param targetPath: target item path.
	    * @param targetAltName: replacement name when 'keep both files' option. can be null.
	    */
        public bool InitializeWithFileSystemPath(string sourcePath, string sourceAltName, string targetPath, string targetAltName)
        {
            bool re = false;
            IntPtr pSourceItem = IntPtr.Zero;
            IntPtr pTargetItem = IntPtr.Zero;

            SHCreateItemFromParsingName(sourcePath, IntPtr.Zero, ref IID_IShellItem2, ref pSourceItem);

            if(pSourceItem == IntPtr.Zero)
            {
                goto escapeArea;
            }

            SHCreateItemFromParsingName(targetPath, IntPtr.Zero, ref IID_IShellItem2, ref pTargetItem);

            if(pTargetItem == IntPtr.Zero)
            {
                goto escapeArea;
            }

            _Reset();

            this._items.Add(_CreateConflictItemUsingSHItem(pSourceItem, sourceAltName));
            this._items.Add(_CreateConflictItemUsingSHItem(pTargetItem, targetAltName));

            re = true;
        escapeArea:
            if (!re)
            {
                if (pSourceItem != IntPtr.Zero)
                {
                    Marshal.Release(pSourceItem);
                }

                if (pTargetItem != IntPtr.Zero)
                {
                    Marshal.Release(pTargetItem);
                }
            }

            return re;
        }

        /**
        * InitializeWithShellItem: Initialize ConflictDialogItem using 'IShellItem2'.
        * @param pSourceItem: source item 'IShellItem2'.
        * @param sourceAltName: replacement name when 'keep both files' option. can be null.
        * @param pTargetItem: target item 'IShellItem2'.
        * @param targetAltName: replacement name when 'keep both files' option. can be null.
        */
        public bool InitializeWithShellItem(IntPtr pSourceItem, string sourceAltName, IntPtr pTargetItem, string targetAltName)
        {
            if(pSourceItem == IntPtr.Zero || pTargetItem == IntPtr.Zero)
            {
                return false;
            }

            _Reset();

            Marshal.AddRef(pSourceItem);
            Marshal.AddRef(pTargetItem);

            this._items.Add(_CreateConflictItemUsingSHItem(pSourceItem, sourceAltName));
            this._items.Add(_CreateConflictItemUsingSHItem(pTargetItem, targetAltName));

            return true;
        }

        /**
        * InitializeWithFileSystemPath: Initialize ConflictDialogItem using 'CONFIRM_CONFLICT_ITEM' (for advanced).
        * @param sourceItem: source CONFIRM_CONFLICT_ITEM.
        * @param targetItem: target CONFIRM_CONFLICT_ITEM.
        */
        public bool InitializeWithCustomConflictInfo(ref CONFIRM_CONFLICT_ITEM sourceItem,ref CONFIRM_CONFLICT_ITEM targetItem)
        {
            _Reset();

            this._items.Add(sourceItem);
            this._items.Add(targetItem);

            return true;
        }

        private List<CONFIRM_CONFLICT_ITEM> _items = new List<CONFIRM_CONFLICT_ITEM>();

        private CONFIRM_CONFLICT_ITEM _CreateConflictItemUsingSHItem(IntPtr shItem, string altName)
        {
            CONFIRM_CONFLICT_ITEM item = new CONFIRM_CONFLICT_ITEM();

            item.pShellItem2 = shItem;
            item.pszAlternateName = altName;

            return item;
        }

        private void _Reset()
        {
            this._items.Clear();
        }

        ~WinConfirmDialogItem()
        {
            if(this._items != null)
            {
                for(int i = 0; i < this._items.Count; i++)
                {
                    Marshal.Release(this._items[i].pShellItem2);
                }

                this._items.Clear();
            }
        }

        private static Guid IID_IShellItem2 = new Guid("7e9fb0d3-919f-4307-ab2e-9b1860310c93");

        [DllImport("shell32.dll")]
        private static extern int SHCreateItemFromParsingName([MarshalAs(UnmanagedType.LPWStr)]string itemName, IntPtr p, ref Guid riid, ref IntPtr ppvOut);
    }
}
