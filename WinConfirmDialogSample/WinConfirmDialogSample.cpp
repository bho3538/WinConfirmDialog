#include "..\WinConfirmDialog\WinConfirmDialog.h"

#if _DEBUG
#if _WIN64
#pragma comment(lib,"..\\x64\\Debug\\WinConfirmDialog.lib")
#else
#pragma comment(lib,"..\\Debug\\WinConfirmDialog.lib")
#endif
#else
#if _WIN64
#pragma comment(lib,"..\\x64\\Release\\WinConfirmDialog.lib")
#else
#pragma comment(lib,"..\\Release\\WinConfirmDialog.lib")
#endif
#endif

#include <vector>

int main()
{
	//#error change file path in your pc. and remove this line.
	std::vector<std::wstring> sourceFileList = { L"C:\\Windows\\system32\\notepad.exe",L"C:\\Users\\bho35\\Pictures\\capture2.png" };
	std::vector<std::wstring> targetFileList = { L"c:\\Windows\\notepad.exe",L"c:\\Windows\\explorer.exe" };

	CoInitialize(0);

	HWND hwnd = GetForegroundWindow();

	printf("WinConfirmDialog sample program\n");

	WinConfirmDialog* confirmDialog = WinConfirmDialog::Initialize(TRUE, TRUE, WINCONFIRMDIALOGOPERATIONTYPE::FILECOPY);

	if (!confirmDialog) {
		printf("Error to initialize WinConfirmDialog\n");
		return -1;
	}
	//enable 'keep both files' options. (use item alternative name)
	confirmDialog->SetAllowRenameToAltName(TRUE);

	WinConfirmDialogItem* item = WinConfirmDialogItem::CreateInstance();

	int sourceCnt = sourceFileList.size();

	WINCONFIRMDIALOGRESULT results = WINCONFIRMDIALOGRESULT::NONE;
	for (int i = 0; i < sourceCnt; i++) {
		item->InitializeWithFileSystemPath(sourceFileList[i].c_str(), L"aa", targetFileList[i].c_str(),NULL);

		if (!(results & WINCONFIRMDIALOGRESULT::APPLYALL)) {
			confirmDialog->ShowDialog(hwnd, item, sourceCnt - (i + 1));
			results = confirmDialog->GetResult();
		}


		if (results & WINCONFIRMDIALOGRESULT::OVERWRITE) {
			printf("user select overwrite");
		}
		else if (results & WINCONFIRMDIALOGRESULT::KEEPORIGINAL || results & WINCONFIRMDIALOGRESULT::SKIPCURRENT) {
			printf("user select keep original or skip");
		}
		else if (results & WINCONFIRMDIALOGRESULT::COPYWITHALTNAME) {
			printf("user select using new name");
		}
		else if (results & WINCONFIRMDIALOGRESULT::CANCELALL) {
			printf("user cancel all tasks");
			break;
		}

		if (results & WINCONFIRMDIALOGRESULT::APPLYALL) {
			printf("user select apply all with current select option");
		}
	}

	delete confirmDialog;

	item->Release();


	return 0;
}

