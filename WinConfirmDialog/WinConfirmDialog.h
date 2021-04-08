#pragma once

#include <ShlObj_core.h>
#include <syncmgr.h>

//Undocumented struct
typedef struct _CONFIRM_CONFLICT_PARAMS {
	HWND hwndOwner;
	DWORD dwFlags;
	DWORD dwOperation;
	DWORD cCurrentItem;
	DWORD cTotalItems;
	DWORD cRemaining;
	DWORD cOtherRemainingResolvableError;
} CONFIRM_CONFLICT_PARAMS ,*PCONFIRM_CONFLICT_PARAMS;

typedef struct _CONFIRM_CONFLICT_RESULT {
	DWORD resultCode;
	DWORD fApplyToAll;
} CONFIRM_CONFLICT_RESULT, *PCONFIRM_CONFLICT_RESULT;



#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("14cc750c-7b0b-43dc-910e-b687f84e7c3b")
ITransferConfirmation : public IUnknown
{
public:
	virtual HRESULT STDMETHODCALLTYPE Confirm() = 0;
	virtual HRESULT STDMETHODCALLTYPE ConfirmConflict(PCONFIRM_CONFLICT_PARAMS pConflictParams, ISyncMgrConflictItems* pConflictItems, PCONFIRM_CONFLICT_RESULT pConflictResults,ISyncMgrConflictResolutionItems** ppConflictItems) = 0;
};

#endif

enum WINCONFIRMDIALOGRESULT {
	NONE = 0,
	OVERWRITE = 1,
	KEEPORIGINAL = 2,
	COPYWITHALTNAME = 4,
	SKIPCURRENT = 8,
	CANCELALL= 16,
	APPLYALL = 32,
};

enum WINCONFIRMDIALOGOPERATIONTYPE {
	FILEMOVE = 1,
	FILECOPY = 2,
	SYNCCONFLICT = 3
};

class WinConfirmDialogItem : ISyncMgrConflictItems {
public:
	~WinConfirmDialogItem();

	//IUnknown
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvOut);
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();

	//ISyncMgrConflictItems
	HRESULT STDMETHODCALLTYPE GetCount(UINT *pCount);
	HRESULT STDMETHODCALLTYPE GetItem(UINT iIndex, CONFIRM_CONFLICT_ITEM *pItemInfo);

	/**
	* InitializeWithFileSystemPath: Initialize ConflictDialogItem using filesystem path.
	* @param sourcePath: source item path.
	* @param sourceAltName: replacement name when 'keep both files' option. can be NULL.
	* @param targetPath: target item path.
	* @param targetAltName: replacement name when 'keep both files' option. can be NULL.
	*/
	BOOL InitializeWithFileSystemPath(LPCWSTR sourcePath,LPCWSTR sourceAltName,LPCWSTR targetPath,LPCWSTR targetAltName);
	
	/**
	* InitializeWithShellItem: Initialize ConflictDialogItem using 'IShellItem2'.
	* @param sourceItem: source item 'IShellItem2'.
	* @param sourceAltName: replacement name when 'keep both files' option. can be NULL.
	* @param targetItem: target item 'IShellItem2'.
	* @param targetAltName: replacement name when 'keep both files' option. can be NULL.
	*/
	BOOL InitializeWithShellItem(IShellItem2* sourceItem,LPCWSTR sourceAltName,IShellItem2* targetItem,LPCWSTR targetAltName);
	
	/**
	* InitializeWithFileSystemPath: Initialize ConflictDialogItem using 'CONFIRM_CONFLICT_ITEM' (for advanced).
	* @param sourceItem: source CONFIRM_CONFLICT_ITEM.
	* @param targetItem: target CONFIRM_CONFLICT_ITEM.
	*/
	BOOL InitializeWithCustomConflictInfo(CONFIRM_CONFLICT_ITEM* sourceItem, CONFIRM_CONFLICT_ITEM* targetItem);

	/**
	* CreateInstance: Create ConfirmDialogItem.
	*/
	static WinConfirmDialogItem* CreateInstance();

private:
	WinConfirmDialogItem();

	void _Reset();

	CONFIRM_CONFLICT_ITEM* _CreateConflictItemUsingSHItem(IShellItem2* shItem, LPCWSTR altName);

	CONFIRM_CONFLICT_ITEM** _params;
	DWORD _dwItemCnt;

	DWORD _dwRefCount;
};


class WinConfirmDialog {

public:
	~WinConfirmDialog();

	/**
	* SetSkipOption: Enable 'skip' button in dialog.
	* @param bCanSkip: If 'TRUE' dialog show skip button, else dialog does not show skip button.
	*/
	void SetSkipOption(BOOL bCanSkip);

	/**
	* SetApplyAllOption: Enable 'Apply to all' checkbox in dialog.
	* @param bCanSkip: If 'TRUE' dialog show apply all checkbox, else dialog does not show checkbox.
	*/
	void SetApplyAllOption(BOOL bCanApplyAll);

	/**
	* SetAllowRenameToAltName: Enable 'Move(Copy),But keep both files' options in dialog. (using item's alternative name)
	* @param bCanApplyAll: If 'TRUE' dialog show keep both files options, else dialog does not show keep both files options.
	*/
	void SetAllowRenameToAltName(BOOL bUAllowRename);

	/**
	* ShowDialog: Show Conflict Dialog.
	* @param hwnd: A handle to the window that owns the dialog box.
	* @param item: Conflict item's info.
	* @param dwRemainCnt: Remain items count.
	*/
	void ShowDialog(HWND hwnd, WinConfirmDialogItem* item, DWORD dwRemainCnt);
	/**
	* ShowDialogAdv: Show Conflict Dialog. for custom ISyncMgrConflictItems implements
	* @param hwnd: A handle to the window that owns the dialog box.
	* @param item: ISyncMgrConflictItems interface.
	* @param dwRemainCnt: Remain items count.
	*/
	void ShowDialogAdv(HWND hwnd, ISyncMgrConflictItems* item,DWORD dwRemainCnt);

	/**
	* GetResult: Get Conflict Dialog Results. Return 'WINCONFIRMDIALOGRESULT' enum value.
	*/
	WINCONFIRMDIALOGRESULT GetResult();
	/**
	* Initialize: Create and initialize WinConfirmDialog instance.
	* @param bCanApplyAll: Enable 'Apply to all' checkbox in dialog.
	* @param bCanSkip: Enable 'skip' button in dialog.
	* @param dialogType: Set Dialog Operation type. value must in 'WINCONFIRMDIALOGOPERATIONTYPE' enum
	*/
	static WinConfirmDialog* Initialize(BOOL bCanApplyAll,BOOL bCanSkip, WINCONFIRMDIALOGOPERATIONTYPE dialogType);

private:	
	WinConfirmDialog(BOOL bCanApplyAll, BOOL bCanSkip, WINCONFIRMDIALOGOPERATIONTYPE dialogType);

	HRESULT _Initialize();

	ITransferConfirmation* _pConfirmDialog;
	ISyncMgrConflictResolutionItems* _pLastResolutionItems;

	DWORD _dwOperationType;
	DWORD _dwResults;
	DWORD _dwConfirmDialogFlags;
	BOOL _bInitialized;
};