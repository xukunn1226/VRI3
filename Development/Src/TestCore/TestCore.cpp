#include "TestCorePCH.h"
#include "Core.h"
//#include "LaunchPCH.h"

#include <stdio.h>
#include <tchar.h>
//
HINSTANCE hInstance = NULL;
HWND GGameWindow = NULL;
INT		GGameIcon = 131/*IDI_ICON_GameDemo*/;
UBOOL GGameWindowUsingStartupWindowProc = FALSE;

//FString OutputSystemTime()
//{
//	SYSTEMTIME sm;
//	GetSystemTime(&sm);
//
//	TCHAR Dest[MAX_SPRINTF] = TEXT("");
//	appSprintf(Dest, TEXT("%d:%d:%d"), sm.wMinute, sm.wSecond, sm.wMilliseconds);
//	return FString(&Dest[0]);
//}

//void TestFileManager()
//{
//	DOUBLE StartTime = appSeconds();
//	debugf(TEXT("Begin reading....%s"), OutputSystemTime().c_str());
//
//	BYTE Buffer[1024];
//	FArchive* Ar = GFileManager->CreateFileReader(TEXT("..\\SM4.upk"));
//	for(int i = 0; i < 102400; ++i)
//		Ar->Serialize(&Buffer[0], 1024);
//	INT Pos = Ar->Tell();
//	debugf(TEXT("End reading....%s"), OutputSystemTime().c_str());
//	Ar->Close();
//	delete Ar;
//
//	//BYTE buf[1024*1004];
//	void* buf = malloc(1024*102400);
//	debugf(TEXT("Begin reading....%s"), OutputSystemTime().c_str());
//	FFileHandle handle = GFileManager->FileOpen(TEXT("..\\SM4.upk"), EIO_Read);
//	GFileManager->FileRead(handle, buf, 1024*102400);
//	INT Pos1 = GFileManager->GetFilePosition(handle);
//	debugf(TEXT("Begin reading....%s"), OutputSystemTime().c_str());
//
//	int ii = 0;
//}

//extern FEngineLoop GEngineLoop;


class Base
{
public:
	Base(const char* strMsg) { strcpy_s(&Buffer[0], 20, strMsg); }
	~Base()
	{
		int ii = 0;
		OutputDebugString(TEXT("zenme keneng a "));
	}

	 char* buf() { return &Buffer[0]; }
private:
	char Buffer[20];
};

char* foo()
{
	char* strANSI;	// = TCHAR_TO_ANSI(TEXT("ABSCC"));
	Base B(("AFDF"));
	strANSI = B.buf();
	return strANSI;
}

int _tmain(int argc, _TCHAR* argv[])
//INT WINAPI WinMain( HINSTANCE hInInstance, HINSTANCE hPrevInstance, char*, INT nCmdShow )
{
	//GEngineLoop;

	char* strANSI = foo();
	char buf[20];
	strcpy_s(&buf[0], 20, strANSI);
	int ii = 0;
	//const TCHAR* strCmdLine = GetCommandLine();
	//appInit(strCmdLine, &Log, &LogConsole, &FileManager);

	//FLinearColor C(1,2,3,4);
	//DWORD D = C.DWColor();
	//INT n = 2 ^3;
	//n = 11 ^ 6;
	//n = 0x5555 ^ 0xffff;
	//n = 2 | 3;
	//n = 11 | 6;

	//FGuid G(11,22,33,44);

	//INT T = 23 >> 2;
	//T = (-25 >> 2) & 255;
	//const TCHAR* Dir = appGameConfigDir();
	//TestFileManager();
	//debugf(TEXT("hello world, %d"), 11);
	//debugf(NAME_Log, TEXT("log"));
	//debugf(NAME_Init, TEXT("Init"));
	//debugf(NAME_Warning, TEXT("Color"));
	//debugf(NAME_Init, TEXT("Init12123"));
	//debugf(NAME_Error, TEXT("SFSFSDF"));
	//for(int i = 0; i < 99; ++i)
	//	debugf(NAME_Init, TEXT("Init23425"));
	//::MessageBox(NULL, v.ToString().c_str(), TEXT("23"), MB_OK);
	//::MessageBox(NULL, v.ToString().c_str(), TEXT("23"), MB_OK);

	//INT nRef = 1024;
	//INT nPos = 1036;
	//INT nTest = nPos & (nRef - 1);

	//INT nInt = 2;
	//FLOAT fFloat = 3.4f;
	//BYTE b = 'b';
	//WORD wd = 11;
	//DWORD dd = 12345;
	//const TCHAR* tchar= TEXT("hello world");

	//vector<BYTE> buf(200);
	//appLoadFileToBuffer(buf, TEXT(".\\ArchiveTest"), GFileManager);
	//FArchive* aw = FileManager.CreateFileWriter(TEXT(".\\ArchiveTest"), EFW_EvenIfReadOnly, GLog);
	//aw->Serialize(&nInt, 4);
	//aw->Serialize(&fFloat, 4);
	//aw->Serialize(&b, 1);
	//aw->Serialize(&wd, sizeof(WORD));
	//aw->Serialize(&dd, sizeof(DWORD));
	//aw->Serialize((void*)tchar, appStrlen(tchar) * sizeof(TCHAR));
	//aw->Flush();
	//delete aw;

	//INT arInt;
	//FLOAT arFloat;
	//BYTE arB;
	//WORD arW;
	//DWORD arD;
	//TCHAR arTchar[256]  = TEXT("");

	//FArchive* ar = FileManager.CreateFileReader(TEXT(".\\ArchiveTest"), 0, GLog);
	//ar->Serialize(&arInt, 4);
	//ar->Serialize(&arFloat, 4);
	//ar->Serialize(&arB, 1);
	//ar->Serialize(&arW, sizeof(WORD));
	//ar->Serialize(&arD, sizeof(DWORD));
	//ar->Serialize(&arTchar, 22);

	//const TCHAR* tchar = TEXT("Ðìçûhello world");
	//FArchive* aw = FileManager.CreateFileWriter(TEXT(".\\ArchiveTest"), 0, 0);
	//aw->Serialize((void*)tchar, appStrlen(tchar) * sizeof(TCHAR));
	//aw->Flush();
	//delete aw;

	//TCHAR arTchar[256]  = TEXT("");

	//FArchive* ar = FileManager.CreateFileReader(TEXT(".\\ArchiveTest"), 0, 0);
	//ar->Serialize(&arTchar, 26);

	//FString Result;
	//appLoadFileToString(Result, TEXT(".\\ArchiveTest"), GFileManager);
	//appSaveStringToFile(Result, TEXT(".\\Test"));
	//appLoadFileToString(Result, TEXT(".\\Test"), GFileManager);
	//appSaveStringToFile(Result, TEXT(".\\Test3"));

	//FString Result1;
	//appLoadFileToString(Result1, TEXT(".\\ANSI_Test"), GFileManager);
	//appSaveStringToFile(Result1, TEXT(".\\Test1"), TRUE);

	//FString Result2;
	//appLoadFileToString(Result2, TEXT(".\\UNICODE_Test"), GFileManager);
	//appSaveStringToFile(Result2, TEXT(".\\Test2"));

	//UBOOL bRet = GFileManager->IsReadOnly(TEXT("111.txt"));
	//bRet = GFileManager->Delete(TEXT("111.txt"), TRUE);
	//
	//bRet = GFileManager->Move(TEXT("..\\Move.txt"), TEXT("2sd"), 0, 0, 0);
	//bRet = GFileManager->Copy(TEXT("e:\\qwe"), TEXT("ANSI_Test"), 1, 1);
	//bRet = GFileManager->TouchFile(TEXT("e:\\qwe"));
	//bRet = GFileManager->MakeDirectory(TEXT("..\\A\\B\\C\\D.txt"), 1);
	//bRet = GFileManager->DeleteDirectory(TEXT("..\\a\\b"), 1);
	//vector<FString> Result;
	//GFileManager->FindFiles(Result, TEXT("..\\a"), FALSE);

	//DOUBLE t = GFileManager->GetFileAgeSeconds(TEXT("..\\123"));
	//t = GFileManager->GetFileTimestamp(TEXT("..\\123"));
	//FFileManager::Timestamp Stamp;
	//GFileManager->GetTimestamp(TEXT("..\\123"), Stamp);

	//FFileHandle h = GFileManager->FileOpen(TEXT("..\\Test3"), EIO_Read);
	//TCHAR bb[128] = TEXT("");
	//GFileManager->FileRead(h, &bb[0], 128);
	//GFileManager->FileClose(h);

	//FQuat q = FQuat::GenerateRandomQuat();
	//FVector V(1, 2,3);
	//FVector R1 = q.RotateVector(V);

	//FQuat R2 = q * FQuat(1,2,3,0) * FQuat(-q.x, -q.y, -q.z, q.w);

	//FRotator R(5786, 11557, 24362);
	//FMatrix Mat = FRotationMatrix(R);
	//FRotator R1 = Mat.Rotator();

	//FVector V(1,-2,-3);
	//V.Normalize();
	//FRotator R = V.Rotation();
	//V = R.Vector();
	//FQuat q = R.Quaternion();
	//FVector V1 = q.RotateVector(FVector(1,0,0));

	//FRotationMatrix rotm(R);
	//FVector V2 = rotm.TransformVector(FVector(1,0,0));

	//FMatrix Mat;
	//q.ToRotationMatrix(Mat);
	//FQuat q1;
	//q1.FromRotationMatrix(Mat);

	return 0;
}













