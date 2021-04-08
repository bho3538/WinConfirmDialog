# WinConfirmDialog

Windows file conflict confirm dialog for any programs.

## How to use (or see sample project)
```c++
#include <WinConfirmDialog.h>

int test(){
	//Create and initialize Conflict confirm dialog.
	ConfirmDialog* confirmDialog = WinConfirmDialog::Initialize(TRUE, TRUE, WINCONFIRMDIALOGOPERATIONTYPE::FILECOPY);
	if (!confirmDialog) {
		printf("Error to initialize WinConfirmDialog\n");
		return -1;
	}
	
	//Create conflict item information.
	WinConfirmDialogItem* item = WinConfirmDialogItem::CreateInstance();
	
	//Initialize conflict item information using file path.
	item->InitializeWithFileSystemPath(L"c:\\Windows\\notepad.exe", L"notepad(2).exe", L"c:\\Windows\\system32\\notepad.exe",NULL);
	
	//Show Dialog.
	confirmDialog->ShowDialog(GetForgroundWindow(), item, 0);		
	//Get Dialog result. (user selection)
	WINCONFIRMDIALOGRESULT results = confirmDialog->GetResult();
	
	//cleanup
	item->Release();
	delete confirmDialog;
	return 0;

}

```
# API
## ** WinConfirmDialog class
### WinConfirmDialog::Initialize(BOOL bCanApplyAll, BOOL bCanSkip, WINCONFIRMDIALOGOPERATIONTYPE dialogType); -static.
Create and initialize WinConfirmDialog instance.\
	* bCanApplyAll: Enable 'Apply to all' checkbox in dialog.\
	* bCanSkip: Enable 'skip' button in dialog.\
	* dialogType: Set Dialog Operation type. value must in 'WINCONFIRMDIALOGOPERATIONTYPE' enum

### void SetSkipOption(BOOL bCanSkip); 
 Enable 'skip' button in dialog.\
	* bCanSkip: If 'TRUE' dialog show skip button, else dialog does not show skip button.

### void SetApplyAllOption(BOOL bCanApplyAll);
Enable 'Apply to all' checkbox in dialog.\
	* bCanSkip: If 'TRUE' dialog show apply all checkbox, else dialog does not show checkbox.
	
### void SetAllowRenameToAltName(BOOL bUAllowRename);
Enable 'Move(Copy),But keep both files' options in dialog. (using item's alternative name)\
	* bUAllowRename:  If 'TRUE' dialog show keep both files options, else dialog does not show keep both files options.

### void ShowDialog(HWND hwnd, WinConfirmDialogItem* item, DWORD dwRemainCnt);
Show Conflict Dialog.\
	* hwnd: A handle to the window that owns the dialog box.\
	* item: Conflict item's info.\
	* dwRemainCnt: Remain items count.
### void ShowDialogAdv(HWND hwnd, ISyncMgrConflictItems* item,DWORD dwRemainCnt);
Show Conflict Dialog. for custom ISyncMgrConflictItems implements\
	* hwnd: A handle to the window that owns the dialog box.\
	* item: ISyncMgrConflictItems interface.\
	* dwRemainCnt: Remain items count.
### WINCONFIRMDIALOGRESULT GetResult();
Get Conflict Dialog Results. 

## ** WinConfirmDialogItem class
Implements ISyncMgrConflictItems

### WinConfirmDialogItem* CreateInstance(); -static
Create ConfirmDialogItem.

### BOOL InitializeWithFileSystemPath(LPCWSTR sourcePath,LPCWSTR sourceAltName,LPCWSTR targetPath,LPCWSTR targetAltName);
Initialize ConflictDialogItem using filesystem path.\
	* sourcePath: source item path.\
	* sourceAltName: replacement name when 'keep both files' option. can be NULL.\
	* targetPath: target item path.\
	* targetAltName: replacement name when 'keep both files' option. can be NULL.

### BOOL InitializeWithShellItem(IShellItem2* sourceItem,LPCWSTR sourceAltName,IShellItem2* targetItem,LPCWSTR targetAltName);
InitializeWithShellItem: Initialize ConflictDialogItem using 'IShellItem2'.\
	* sourceItem: source item 'IShellItem2'.\
	* sourceAltName: replacement name when 'keep both files' option. can be NULL.\
	* targetItem: target item 'IShellItem2'.\
	* targetAltName: replacement name when 'keep both files' option. can be NULL.

### BOOL InitializeWithCustomConflictInfo(CONFIRM_CONFLICT_ITEM* sourceItem, CONFIRM_CONFLICT_ITEM* targetItem);
InitializeWithFileSystemPath: Initialize ConflictDialogItem using 'CONFIRM_CONFLICT_ITEM' (for advanced).\
	* sourceItem: source 'CONFIRM_CONFLICT_ITEM' structure.\
	* targetItem: target 'CONFIRM_CONFLICT_ITEM' structure.




## ** Common
### WINCONFIRMDIALOGOPERATIONTYPE enum
enum WINCONFIRMDIALOGOPERATIONTYPE { \
	FILEMOVE = 1, // conflict dialog in file move operation.\
	FILECOPY = 2, // conflict dialog in file copy operation.\
	SYNCCONFLICT = 3 // conflict dialog in file sync operation.\
};

### WINCONFIRMDIALOGRESULT enum
enum WINCONFIRMDIALOGRESULT {\
	NONE = 0, //None\
	OVERWRITE = 1, //User select overwrite original file\
	KEEPORIGINAL = 2, //User select keep original file\
	COPYWITHALTNAME = 4, //User select keep original file\
	SKIPCURRENT = 8, // User select skip this item.\
	CANCELALL= 16, //User cancel all tasks\
	APPLYALL = 32, //User select apply all check box. (can combine other flags)\
};
