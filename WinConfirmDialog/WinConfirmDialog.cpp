#include "pch.h"
#include "WinConfirmDialog.h"

//Undocumented GUID
const IID IID_ITransferConfirmation = { 0x14cc750c,0x7b0b,0x43dc,{0x91,0x0e,0xb6,0x87,0xf8,0x4e,0x7c,0x3b} };
const GUID CLSID_TransferConfirmationUI = { 0x6b831e4f,0xa50d,0x45fc,{0x84,0x2f,0x16,0xce,0x27,0x59,0x53,0x59} };

//WinConfirmDialogItem

WinConfirmDialogItem::WinConfirmDialogItem() {
	this->_params = NULL;
	this->_dwItemCnt = 0;
	this->_dwRefCount = 1;
}
WinConfirmDialogItem::~WinConfirmDialogItem() {
	if (this->_params) {
		for (DWORD i = 0; i < this->_dwItemCnt; i++) {
			free(this->_params[i]);
		}
		free(this->_params);
	}
}

HRESULT STDMETHODCALLTYPE WinConfirmDialogItem::QueryInterface(REFIID riid, void** ppvOut) {
	HRESULT hr = E_NOINTERFACE;

	if (riid == IID_IUnknown || riid == IID_ITransferConfirmation) {
		*ppvOut = this;
		hr = S_OK;
	}

	return hr;
}
ULONG STDMETHODCALLTYPE WinConfirmDialogItem::AddRef() {
	ULONG refCount = InterlockedIncrement(&this->_dwRefCount);

	return refCount;
}
ULONG STDMETHODCALLTYPE WinConfirmDialogItem::Release() {
	ULONG refCount = InterlockedDecrement(&this->_dwRefCount);

	if (refCount == 0) {
		delete this;
		return 0;
	}

	return refCount;
}

HRESULT STDMETHODCALLTYPE WinConfirmDialogItem::GetCount(UINT *pCount) {
	*pCount = this->_dwItemCnt;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE WinConfirmDialogItem::GetItem(UINT iIndex, CONFIRM_CONFLICT_ITEM *pItemInfo) {
	DWORD len = 0;
	CONFIRM_CONFLICT_ITEM* itemInfo = NULL;
	if (iIndex >= this->_dwItemCnt) {
		return E_FAIL;
	}

	itemInfo = this->_params[iIndex];
	if (!itemInfo) {
		return E_FAIL;
	}

	ZeroMemory(pItemInfo, sizeof(CONFIRM_CONFLICT_ITEM));
	pItemInfo->pShellItem = itemInfo->pShellItem;
	pItemInfo->pShellItem->AddRef();

	if (itemInfo->pszOriginalName) {
		len = (DWORD)wcslen(itemInfo->pszOriginalName) + 1;
		pItemInfo->pszOriginalName = (LPWSTR)CoTaskMemAlloc(len);
		wcscpy_s(pItemInfo->pszOriginalName, len, itemInfo->pszOriginalName);
	}
	
	if (itemInfo->pszAlternateName) {
		len = (DWORD)wcslen(itemInfo->pszAlternateName) + 1;
		pItemInfo->pszAlternateName = (LPWSTR)CoTaskMemAlloc(len);
		wcscpy_s(pItemInfo->pszAlternateName, len, itemInfo->pszAlternateName);
	}

	if (itemInfo->pszLocationShort) {
		len = (DWORD)wcslen(itemInfo->pszLocationShort) + 1;
		pItemInfo->pszLocationShort = (LPWSTR)CoTaskMemAlloc(len);
		wcscpy_s(pItemInfo->pszLocationShort, len, itemInfo->pszLocationShort);
	}

	if (itemInfo->pszLocationFull) {
		len = (DWORD)wcslen(itemInfo->pszLocationFull) + 1;
		pItemInfo->pszLocationFull = (LPWSTR)CoTaskMemAlloc(len);
		wcscpy_s(pItemInfo->pszLocationFull, len, itemInfo->pszLocationFull);
	}


	return S_OK;
}

BOOL WinConfirmDialogItem::InitializeWithFileSystemPath(LPCWSTR sourcePath,LPCWSTR sourceAltName, LPCWSTR targetPath,LPCWSTR targetAltName) {
	BOOL re = FALSE;
	IShellItem2* item1 = NULL;
	IShellItem2* item2 = NULL;

	if (!sourcePath || !targetPath) {
		goto escapeArea;
	}

	SHCreateItemFromParsingName(sourcePath, NULL, IID_IShellItem2, (PVOID*)&item1);
	if (!item1) {
		goto escapeArea;
	}

	SHCreateItemFromParsingName(targetPath, NULL, IID_IShellItem2, (PVOID*)&item2);
	if (!item2) {
		goto escapeArea;
	}

	if (!this->_params) {
		this->_params = (CONFIRM_CONFLICT_ITEM**)malloc(sizeof(CONFIRM_CONFLICT_ITEM*) * 2);
		if (!this->_params) {
			goto escapeArea;
		}
		ZeroMemory(this->_params, sizeof(CONFIRM_CONFLICT_ITEM*) * 2);
	}
	else {
		_Reset();
	}
	
	//set source item
	this->_params[0] = _CreateConflictItemUsingSHItem(item1, sourceAltName);

	//set target item
	this->_params[1] = _CreateConflictItemUsingSHItem(item2, targetAltName);

	this->_dwItemCnt = 2;
	re = TRUE;

escapeArea:

	if (!re) {
		if (item1) {
			item1->Release();
		}
		if (item2) {
			item2->Release();
		}
	}

	return re;
}



BOOL WinConfirmDialogItem::InitializeWithShellItem(IShellItem2* sourceItem, LPCWSTR sourceAltName, IShellItem2* targetItem, LPCWSTR targetAltName) {
	BOOL re = FALSE;

	if (!sourceItem || !targetItem) {
		goto escapeArea;
	}

	if (!this->_params) {
		this->_params = (CONFIRM_CONFLICT_ITEM**)malloc(sizeof(CONFIRM_CONFLICT_ITEM*) * 2);
		if (!this->_params) {
			goto escapeArea;
		}
		ZeroMemory(this->_params, sizeof(CONFIRM_CONFLICT_ITEM*) * 2);
	}
	else {
		_Reset();
	}

	//set source item
	this->_params[0] = _CreateConflictItemUsingSHItem(sourceItem, sourceAltName);

	//set target item
	this->_params[1] = _CreateConflictItemUsingSHItem(targetItem, targetAltName);

	this->_dwItemCnt = 2;

	re = TRUE;
escapeArea:

	return re;
}

BOOL WinConfirmDialogItem::InitializeWithCustomConflictInfo(CONFIRM_CONFLICT_ITEM* sourceItem, CONFIRM_CONFLICT_ITEM* targetItem) {
	BOOL re = FALSE;

	if (!sourceItem || !targetItem) {
		goto escapeArea;
	}

	if (!this->_params) {
		this->_params = (CONFIRM_CONFLICT_ITEM**)malloc(sizeof(CONFIRM_CONFLICT_ITEM*) * 2);
		if (!this->_params) {
			goto escapeArea;
		}
		ZeroMemory(this->_params, sizeof(CONFIRM_CONFLICT_ITEM*) * 2);
	}
	else {
		_Reset();
	}

	this->_params[0] = sourceItem;
	this->_params[1] = targetItem;

	re = TRUE;

escapeArea:

	return re;
}

void WinConfirmDialogItem::_Reset() {
	for (DWORD i = 0; i < this->_dwItemCnt; i++) {
		CONFIRM_CONFLICT_ITEM* item = this->_params[i];
		if (!item) {
			continue;
		}
		if (item->pShellItem) {
			item->pShellItem->Release();
		}

		CoTaskMemFree(item->pszAlternateName);
		CoTaskMemFree(item->pszLocationFull);
		CoTaskMemFree(item->pszLocationShort);
		CoTaskMemFree(item->pszOriginalName);

		this->_params[i] = NULL;
	}
	this->_dwItemCnt = 0;
}

WinConfirmDialogItem* WinConfirmDialogItem::CreateInstance() {
	return new WinConfirmDialogItem();
}

CONFIRM_CONFLICT_ITEM* WinConfirmDialogItem::_CreateConflictItemUsingSHItem(IShellItem2* shItem, LPCWSTR altName) {
	CONFIRM_CONFLICT_ITEM* conflictItem = (CONFIRM_CONFLICT_ITEM*)malloc(sizeof(CONFIRM_CONFLICT_ITEM));

	if (conflictItem) {
		ZeroMemory(conflictItem, sizeof(CONFIRM_CONFLICT_ITEM));
		conflictItem->pShellItem = shItem;
		
		if (altName) {
			DWORD len = (DWORD)wcslen(altName) + 1;
			conflictItem->pszAlternateName = (LPWSTR)CoTaskMemAlloc(len * sizeof(WCHAR));
			wcscpy_s(conflictItem->pszAlternateName, len, altName);
		}
	}


	return conflictItem;
}


//WinConfirmDialog

WinConfirmDialog* WinConfirmDialog::Initialize(BOOL bCanApplyAll, BOOL bCanSkip, WINCONFIRMDIALOGOPERATIONTYPE dialogType) {
	HRESULT hr = S_OK;
	WinConfirmDialog* confirmDialog = new WinConfirmDialog(bCanApplyAll, bCanSkip, dialogType);

	if (confirmDialog) {
		hr = confirmDialog->_Initialize();
		if (hr != S_OK) {
			delete confirmDialog;
			confirmDialog = NULL;
		}
	}

	return confirmDialog;
}

WinConfirmDialog::WinConfirmDialog(BOOL bCanApplyAll, BOOL bCanSkip, WINCONFIRMDIALOGOPERATIONTYPE dialogType) {
	this->_pConfirmDialog = NULL;
	this->_pLastResolutionItems = NULL;
	this->_dwResults = WINCONFIRMDIALOGRESULT::NONE;
	this->_dwOperationType = dialogType;

	this->_dwConfirmDialogFlags = 4;
	SetSkipOption(bCanSkip);
	SetApplyAllOption(bCanApplyAll);
}

WinConfirmDialog::~WinConfirmDialog() {
	if (this->_pConfirmDialog) {
		this->_pConfirmDialog->Release();
	}

}

HRESULT WinConfirmDialog::_Initialize() {
	HRESULT re = E_FAIL;

	re = CoCreateInstance(CLSID_TransferConfirmationUI, NULL, 1, IID_ITransferConfirmation, (PVOID*)&this->_pConfirmDialog);

	if (re == E_NOINTERFACE) {
		//windows vista , 7
		re = CoCreateInstance(CLSID_TransferConfirmationUI, NULL, 1, IID_IUnknown, (PVOID*)&this->_pConfirmDialog);
	}

	if (re == S_OK) {
		_bInitialized = TRUE;
	}

	return re;
}

void WinConfirmDialog::ShowDialogAdv(HWND hwnd, ISyncMgrConflictItems* item,DWORD dwRemainCnt) {
	HRESULT hr = 0;
	CONFIRM_CONFLICT_PARAMS conflictParams = { hwnd,this->_dwConfirmDialogFlags,this->_dwOperationType,0,0,dwRemainCnt,0 };
	CONFIRM_CONFLICT_RESULT conflictResults = { 0, };

	ISyncMgrConflictResolutionItems* pResolutionItems = NULL;

	if (!_bInitialized) {
		return;
	}

	if (dwRemainCnt == 0) {
		conflictParams.dwFlags &= ~1;
	}

	this->_dwResults = 0;

	hr = this->_pConfirmDialog->ConfirmConflict(&conflictParams, item,&conflictResults,&pResolutionItems);

	if (conflictResults.resultCode == S_OK) {
		if (pResolutionItems) {
			CONFIRM_CONFLICT_RESULT_INFO resultItem = { 0, };
			hr = pResolutionItems->GetItem(0, &resultItem);
			if (hr == S_OK) {
				if (resultItem.pszNewName) {
					this->_dwResults = WINCONFIRMDIALOGRESULT::COPYWITHALTNAME;
					CoTaskMemFree(resultItem.pszNewName);
				}
				else {
					if (resultItem.iItemIndex == 0) {
						this->_dwResults = WINCONFIRMDIALOGRESULT::OVERWRITE;
					}
					else {
						this->_dwResults = WINCONFIRMDIALOGRESULT::SKIPCURRENT;
					}
				}
			}
		}
	}
	else if (conflictResults.resultCode == 0x80000000) {
		this->_dwResults = (WINCONFIRMDIALOGRESULT::CANCELALL | WINCONFIRMDIALOGRESULT::APPLYALL);
	}
	else if (conflictResults.resultCode == 0x80000001) {
		this->_dwResults = WINCONFIRMDIALOGRESULT::SKIPCURRENT;
	}

	if (conflictResults.fApplyToAll) {
		this->_dwResults |= WINCONFIRMDIALOGRESULT::APPLYALL;
	}

	if (pResolutionItems) {
		pResolutionItems->Release();
	}
}

void WinConfirmDialog::ShowDialog(HWND hwnd, WinConfirmDialogItem* item,DWORD dwRemainCnt) {
	ISyncMgrConflictItems* items = NULL;

	if (!item) {
		return;
	}
	//do not release it (will release inside ConfirmConflict)
	item->QueryInterface(IID_ITransferConfirmation, (PVOID*)&items);

	if (!items) {
		return;
	}

	ShowDialogAdv(hwnd, items,dwRemainCnt);
}

void WinConfirmDialog::SetSkipOption(BOOL bCanSkip) {
	if (bCanSkip) {
		this->_dwConfirmDialogFlags |= 2;
	}
	else {
		this->_dwConfirmDialogFlags &= ~2;
	}
}
void WinConfirmDialog::SetApplyAllOption(BOOL bCanApplyAll) {
	if (bCanApplyAll) {
		this->_dwConfirmDialogFlags |= 1;
	}
	else {
		this->_dwConfirmDialogFlags &= ~1;
	}
}

void WinConfirmDialog::SetAllowRenameToAltName(BOOL bUseAllowRename) {
	if (bUseAllowRename) {
		this->_dwConfirmDialogFlags |= 32;
	}
	else {
		this->_dwConfirmDialogFlags &= ~32;
	}
}

WINCONFIRMDIALOGRESULT WinConfirmDialog::GetResult() {
	return (WINCONFIRMDIALOGRESULT)this->_dwResults;
}