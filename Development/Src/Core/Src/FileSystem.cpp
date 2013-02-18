#include "CorePCH.h"
#include "FileSystem.h"

#include <sys/types.h>
#include <sys/utime.h>
#include <sys/stat.h>
#include <time.h>
#include <shlobj.h>

INT FFileManager::Timestamp::GetJulian() const
{
	return  Day - 32075L +
		1461L*(Year  + 4800L +  (Month  - 14L)/12L)/4L      +
		367L*(Month - 2L     - ((Month - 14L)/12L)*12L)/12L -
		3L*((Year + 4900L    +  (Month  - 14L)/12L)/100L)/4L;
}

INT FFileManager::Timestamp::GetSecondOfDay() const
{
	return Second + Minute * 60 + Hour * 60 * 60;
}

UBOOL FFileManager::Timestamp::operator < (const FFileManager::Timestamp& Other) const
{
	const INT J = GetJulian();
	const INT OJ = Other.GetJulian();
	if( J < OJ )
	{
		return TRUE;
	}
	else if( J > OJ )
	{
		return FALSE;
	}
	else
	{
		if( GetSecondOfDay() < Other.GetSecondOfDay() )
		{
			return TRUE;
		}
	}
	return FALSE;
}

UBOOL FFileManager::Timestamp::operator > (const FFileManager::Timestamp& Other) const
{
	const INT J = GetJulian();
	const INT OJ = Other.GetJulian();
	if( J > OJ )
	{
		return TRUE;
	}
	else if( J < OJ )
	{
		return FALSE;
	}
	else
	{
		if( GetSecondOfDay() > Other.GetSecondOfDay() )
		{
			return TRUE;
		}
	}
	return FALSE;
}

UBOOL FFileManager::Timestamp::operator <= (const FFileManager::Timestamp& Other) const
{
	if( operator ==(Other) )
	{
		return TRUE;
	}

	return operator<(Other);
}

UBOOL FFileManager::Timestamp::operator >= (const FFileManager::Timestamp& Other) const
{
	if( operator ==(Other) )
	{
		return TRUE;
	}

	return operator>(Other);
}

UBOOL FFileManager::Timestamp::operator == (const FFileManager::Timestamp& Other) const
{
	return ( (Year == Other.Year) &&
			 (Month == Other.Month) &&
			 (Day == Other.Day) &&
			 (Hour == Other.Hour) &&
			 (Minute == Other.Minute) &&
			 (Second == Other.Second) );
}

UBOOL FFileManager::Timestamp::operator != (const FFileManager::Timestamp& Other) const
{
	return !operator==(Other);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FFileManagerWindows::FFileManagerWindows()
: FFileManager()
{
	SetDefaultDirectory();
}

UBOOL FFileManagerWindows::SetDefaultDirectory()
{
	m_strRootDir = appBaseDir();
	return SetCurrentDirectory(m_strRootDir.c_str()) != 0;
}

FString FFileManagerWindows::GetDefaultDirectory()
{
	return appBaseDir();
}

UBOOL FFileManagerWindows::SetWorkingDirectory(const TCHAR* WorkingDir)
{
	m_strRootDir.empty();
	if( appIsAbsolutePath(WorkingDir) )
	{
		m_strRootDir = WorkingDir;
	}
	else if( appIsRelativePath(WorkingDir) )
	{
		FString TempPath(appBaseDir());
		TempPath = TempPath + TEXT("\\") + WorkingDir;
		m_strRootDir = TempPath;
	}
	else
	{
		return FALSE;
	}

	return SetCurrentDirectory(m_strRootDir.c_str()) != 0;
}

FString FFileManagerWindows::GetWorkingDirectory()
{
	return m_strRootDir;
}

FString FFileManagerWindows::ConvertToAbsolutePath(const TCHAR *Filename)
{
	FString AbsolutePath = Filename;
	if( FStringUtil::StartWith(AbsolutePath, TEXT("..")) ||
		FStringUtil::StartWith(AbsolutePath, TEXT(".")) )
	{
		AbsolutePath = GetWorkingDirectory() + AbsolutePath;
	}
	return AbsolutePath;
}

FArchive* FFileManagerWindows::CreateFileReader(const TCHAR* Filename, DWORD ReadFlags, FOutputDevice* InError)
{
	DWORD  Access    = GENERIC_READ;
	DWORD  WinFlags  = FILE_SHARE_READ;
	DWORD  Create    = OPEN_EXISTING;
	HANDLE Handle = CreateFile(Filename, Access, WinFlags, NULL, Create, FILE_ATTRIBUTE_NORMAL, NULL);
	if( Handle == INVALID_HANDLE_VALUE )
	{
		if( ReadFlags & EFR_NoFail )
		{
			//appErrorf(TEXT("Failed to read file: %s"), Filename);
		}
		return NULL;
	}

	FArchive* Ar = new FArchiveFileReader(Handle, Filename, GetFileSize(Handle, NULL), InError);

	return Ar;
}

FArchive* FFileManagerWindows::CreateFileWriter(const TCHAR* Filename, DWORD WriteFlags, FOutputDevice* InError)
{
	FString strTemp(Filename);
	FString strName, strPath;
	FStringUtil::SplitFilename(strTemp, strName, strPath);

	MakeDirectory(strPath.c_str(), TRUE);

	if( FileSize(Filename) >= 0 && (WriteFlags & EFW_EvenIfReadOnly) )
	{
		SetFileAttributes(Filename, 0);
	}
	DWORD  Access    = GENERIC_WRITE;
	DWORD  WinFlags  = (WriteFlags & EFW_AllowRead) ? FILE_SHARE_READ : 0;
	DWORD  Create    = (WriteFlags & EFW_Append) ? OPEN_ALWAYS : (WriteFlags & EFW_NoReplaceExisting) ? CREATE_NEW : CREATE_ALWAYS;
	HANDLE Handle    = CreateFile( Filename, Access, WinFlags, NULL, Create, FILE_ATTRIBUTE_NORMAL, NULL );
	INT    Pos       = 0;
	if( Handle == INVALID_HANDLE_VALUE )
	{
		if( WriteFlags & EFW_NoFail )
		{
			//const DWORD LastError = GetLastError();
			const TCHAR* ErrMsg = appGetSystemErrorMessage();
			appErrorf(TEXT("Failed to create file: %s, GetLastError %s"), Filename, ErrMsg);
		}
		return 0;
	}
	if( WriteFlags & EFW_Append )
	{
		Pos = SetFilePointer(Handle, 0, 0, FILE_END);
	}

	FArchive* Ar = new FArchiveFileWriter(Handle, Filename, Pos, InError);

	return Ar;
}

INT FFileManagerWindows::FindAvailableFilename(const TCHAR* Base, const TCHAR* Extension, FString& OutFilename, INT StartVal)
{
	check(Base && Extension);

	FString strFullPath(Base);
	const INT nIndexMarker = strFullPath.length();
	strFullPath += TEXT("0000.");
	strFullPath += Extension;

	for(INT i = StartVal + 1; i < 10000; ++i)
	{
		strFullPath[nIndexMarker	] = i / 1000	+ '0';
		strFullPath[nIndexMarker + 1] = (i/100) % 10 + '0';
		strFullPath[nIndexMarker + 2] = (i/10) % 10 +'0';
		strFullPath[nIndexMarker + 3] = i % 10 + '0';

		if( GFileManager->FileSize(strFullPath.c_str()) == -1 )
		{
			OutFilename = strFullPath;
			return i;
		}
	}
	return -1;
}

UBOOL FFileManagerWindows::MakeDirectory(const TCHAR *Path, UBOOL Tree)
{
	DWORD Ret;
	if( Tree )
	{
		Ret = SHCreateDirectoryEx(NULL, ConvertToAbsolutePath(Path).c_str(), NULL);
		if( Ret != ERROR_SUCCESS && Ret != ERROR_ALREADY_EXISTS )
		{
			const TCHAR* ErrMsg = appGetSystemErrorMessage();
			debugf(TEXT("Failed to make directory(%s) with error code: %s"), Path, ErrMsg);
			return FALSE;
		}
		return TRUE;
	}

	if( CreateDirectory(ConvertToAbsolutePath(Path).c_str(), NULL) == 0 && GetLastError() != ERROR_ALREADY_EXISTS )
	{
		const TCHAR* ErrMsg = appGetSystemErrorMessage();
		debugf(TEXT("Failed to make directory(%s) with error code: %s"), Path, ErrMsg);
		return FALSE;
	}

	return TRUE;
}

UBOOL FFileManagerWindows::InternalMakeDirectory(const TCHAR* Path, UBOOL Tree)
{
	check(Tree);

	INT CreateCount = 0;
	TCHAR Separator[256] = TEXT("\\");
	TCHAR Full[256] = TEXT("");
	TCHAR* Ptr = Full;
	for( ; ; *Ptr++ = *Path++ )
	{
		TCHAR c = *Path;
		if( c == Separator[0] || *Path == 0 )
		{
			*Ptr = 0;
			if( Ptr != Full && !IsDrive(Full) )
			{
				if( !MakeDirectory(Full, 0) )
				{
					return FALSE;
				}
				++CreateCount;
			}
		}

		if( *Path == 0 )
			break;
	}
	return CreateCount != 0;
}

UBOOL FFileManagerWindows::DeleteDirectory(const TCHAR *Path, UBOOL Tree)
{
	if( Tree )
	{
		return InternalDeleteDirectory(ConvertToAbsolutePath(Path).c_str());
	}

	// delete an existing empty directory
	return RemoveDirectory(ConvertToAbsolutePath(Path).c_str()) != 0 || GetLastError() == ERROR_FILE_NOT_FOUND;
}

UBOOL FFileManagerWindows::InternalDeleteDirectory(const TCHAR* Path)
{
	if( !appStrlen(Path) )
	{
		return FALSE;
	}

	TCHAR TempPath[1024];
	appStrcpy(TempPath, Path);
	if( Path[appStrlen(Path) - 1] != '\\' )
	{
		appStrcat(TempPath, 1024, TEXT("\\"));
	}

	vector<FString>	List;
	FindFiles(List, TempPath, TRUE, FALSE);
	for( size_t i = 0; i < List.size(); ++i )
	{
		if( !Delete( (TempPath + List.at(i)).c_str(), TRUE) )
		{
			return FALSE;
		}
	}

	List.clear();
	FindFiles(List, TempPath, FALSE, TRUE);
	for( size_t i = 0; i < List.size(); ++i )
	{
		if( !DeleteDirectory( (TempPath + List.at(i)).c_str(), TRUE) )
		{
			return FALSE;
		}
	}

	return DeleteDirectory(TempPath, FALSE);
}

void FFileManagerWindows::FindFiles(vector<FString>& Results, const TCHAR* Filename, UBOOL IncludeSubDir )
{
	HANDLE Handle = NULL;

	TCHAR FullFilename[1024];
	appStrcpy( FullFilename, 1024, Filename);

	if( Filename[appStrlen(FullFilename) - 1] != '\\' )
	{
		appStrcat(FullFilename, 1024, TEXT("\\"));
	}
	appStrcat(FullFilename, 1024, TEXT("*.*"));

	WIN32_FIND_DATA Data;
	Handle = FindFirstFile(FullFilename, &Data);

	if( !IncludeSubDir )
	{
		if( Handle != INVALID_HANDLE_VALUE )
		{
			do
			{
				if( appStricmp(Data.cFileName, TEXT("."))
					&& appStricmp(Data.cFileName, TEXT(".."))
					&& !( Data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
				{
					Results.push_back(Data.cFileName);
				}
			}
			while( FindNextFile(Handle, &Data) );
		}
	}
	else
	{
		if( Handle != INVALID_HANDLE_VALUE )
		{
			do
			{
				if( appStricmp(Data.cFileName, TEXT(".")) && appStricmp(Data.cFileName, TEXT(".."))
					&& (Data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
				{
					const FString temPath = FStringUtil::Sprintf(TEXT("%s\\%s"), Filename, Data.cFileName);
					FindFiles(Results, temPath.c_str(), IncludeSubDir);
				}
				else if( appStricmp(Data.cFileName, TEXT(".")) && appStricmp(Data.cFileName, TEXT("..")) )
				{
					Results.push_back(Data.cFileName);
				}
			}
			while( FindNextFile(Handle, &Data) );
		}
	}

	if( Handle != INVALID_HANDLE_VALUE )
	{
		FindClose(Handle);
	}
}

void FFileManagerWindows::FindFiles(std::vector<FString> &Results, const TCHAR *Filename, UBOOL Files, UBOOL Directories)
{
	HANDLE Handle = NULL;

	TCHAR FullFilename[1024];
	appStrcpy( FullFilename, 1024, Filename);

	if( Filename[appStrlen(FullFilename) - 1] != '\\' )
	{
		appStrcat(FullFilename, 1024, TEXT("\\"));
	}
	appStrcat(FullFilename, 1024, TEXT("*.*"));

	WIN32_FIND_DATA Data;
	Handle = FindFirstFile(FullFilename, &Data);

	if( Handle != INVALID_HANDLE_VALUE )
	{
		do
		{
			if( appStricmp(Data.cFileName, TEXT("."))
				&& appStricmp(Data.cFileName, TEXT(".."))
				&& ( ( Data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) ? Directories : Files ) )
			{
				Results.push_back(Data.cFileName);
			}
		}
		while( FindNextFile(Handle, &Data) );
	}

	if( Handle != INVALID_HANDLE_VALUE )
	{
		FindClose(Handle);
	}
}

UBOOL FFileManagerWindows::IsReadOnly(const TCHAR* Filename)
{
	return InternalIsReadOnly( ConvertToAbsolutePath(Filename).c_str() );
}

UBOOL FFileManagerWindows::InternalIsReadOnly(const TCHAR* Filename)
{
	DWORD rc;

	if( FileSize(Filename) < 0 )
	{
		return FALSE;
	}

	rc = GetFileAttributes(Filename);
	if( rc != INVALID_FILE_ATTRIBUTES )
	{
		return ( (rc & FILE_ATTRIBUTE_READONLY) != 0 );
	}

	return FALSE;
}

UBOOL FFileManagerWindows::Delete(const TCHAR *Filename, UBOOL bEvenIfReadOnly)
{
	return InternalDelete(ConvertToAbsolutePath(Filename).c_str(), bEvenIfReadOnly);
}

UBOOL FFileManagerWindows::InternalDelete(const TCHAR* Filename, UBOOL bEvenIfReadOnly)
{
	if( bEvenIfReadOnly )
	{
		SetFileAttributes(Filename, FILE_ATTRIBUTE_NORMAL);
	}

	UBOOL Result = DeleteFile(Filename);
	if( !Result )
	{
		const TCHAR* Msg = appGetSystemErrorMessage();
		debugf(NAME_Error, TEXT("Error deleting file '%s' (GetLastError: %s)"), Filename, Msg);
	}

	return Result;
}

UBOOL FFileManagerWindows::Move(const TCHAR* Dest, const TCHAR* Src, UBOOL bReplace, UBOOL bEvenIfReadOnly, UBOOL bAttributes)
{
	return InternalMove(ConvertToAbsolutePath(Dest).c_str(), ConvertToAbsolutePath(Src).c_str(), bReplace, bEvenIfReadOnly, bAttributes);
}

UBOOL FFileManagerWindows::InternalMove(const TCHAR* Dest, const TCHAR* Src, UBOOL bReplace, UBOOL bEvenIfReadOnly, UBOOL bAttributes)
{
	FString strFilename, strPath;
	FStringUtil::SplitFilename(Dest, strFilename, strPath);
	MakeDirectory(strPath.c_str(), TRUE);

	UBOOL Result = MoveFileEx(Src, Dest, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
	if( !Result )
	{
		const TCHAR* ErrMsg = appGetSystemErrorMessage();
		debugf(NAME_Warning, TEXT("MoveFileEx unable to move '%s' to '%s', retry... (LastError: %s)"), Src, Dest, ErrMsg);

		Sleep(500);

		Result = MoveFileEx(Src, Dest, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
		if( !Result )
		{
			const TCHAR* ErrMsg = appGetSystemErrorMessage();
			debugf(NAME_Warning, TEXT("Error moving file '%s' to '%s' (LastError: %s)"), Src, Dest, ErrMsg);
		}
		else
		{
			debugf(NAME_Warning, TEXT("MoveFileEx recovered during retry."));
		}
	}
	return Result;
}

UBOOL FFileManagerWindows::Copy(const TCHAR* Dest, const TCHAR* Src, UBOOL bReplaceExisting, UBOOL bEvenIfReadOnly)
{
	return InternalCopy(ConvertToAbsolutePath(Dest).c_str(), ConvertToAbsolutePath(Src).c_str(), bReplaceExisting, bEvenIfReadOnly);
}

UBOOL FFileManagerWindows::InternalCopy(const TCHAR* Dest, const TCHAR* Src, UBOOL bReplaceExisting, UBOOL bEvenIfReadOnly)
{
	if( bEvenIfReadOnly )
	{
		SetFileAttributes(Dest, 0);
	}

	FString strFilename, strPath;
	FStringUtil::SplitFilename(Dest, strFilename, strPath);
	MakeDirectory(strPath.c_str(), TRUE);

	UBOOL Result;
	Result = (CopyFile(Src, Dest, !bReplaceExisting) != 0);
	if( !Result )
	{
		const TCHAR* ErrMsg = appGetSystemErrorMessage();
		debugf(NAME_Warning, TEXT("Failed to copy file '%s' to '%s' (LastError: %s)"), Src, Dest, ErrMsg);
	}
	else
	{
		SetFileAttributes(Dest, 0);
	}

	return Result;
}

UBOOL FFileManagerWindows::IsDrive(const TCHAR* Path)
{
	if( appStricmp(Path, TEXT("")) == 0 )
		return TRUE;
	else if( appIsAlpha(Path[0]) && Path[1] == ':' && Path[2] == 0 )
		return TRUE;
	else if( appStricmp(Path, TEXT("\\")) == 0 )
		return TRUE;
	else if( appStricmp(Path, TEXT("\\\\")) == 0 )
		return TRUE;

	return FALSE;
}

/*
 * Update the modification time of the file
 */
UBOOL FFileManagerWindows::TouchFile(const TCHAR *Filename)
{
	time_t Now;
	time(&Now);

	_utimbuf Time;
	Time.modtime = Now;		// modification time
	Time.actime = Now;		// access time

	return _tutime(Filename, &Time) == 0;
}

DOUBLE FFileManagerWindows::GetFileTimestamp(const TCHAR* Filename)
{
	return InternalGetFileTimestamp(ConvertToAbsolutePath(Filename).c_str());
}

DOUBLE FFileManagerWindows::InternalGetFileTimestamp(const TCHAR* Filename)
{
	struct _stat FileInfo;
	if( _tstat(Filename, &FileInfo) == 0 )
	{
		time_t FileTime;
		FileTime = FileInfo.st_mtime;		// Time of last modification of file
		return FileTime;
	}
	return -1;
}

DOUBLE FFileManagerWindows::GetFileAgeSeconds(const TCHAR* Filename)
{
	return InternalGetFileAgeSeconds(ConvertToAbsolutePath(Filename).c_str());
}

DOUBLE FFileManagerWindows::InternalGetFileAgeSeconds(const TCHAR* Filename)
{
	struct _stat FileInfo;
	if( _tstat(Filename, &FileInfo) == 0 )
	{
		time_t CurrentTime, FileTime;
		FileTime = FileInfo.st_mtime;
		time(&CurrentTime);
		return difftime(CurrentTime, FileTime);
	}
	return -1;
}

UBOOL FFileManagerWindows::GetTimestamp(const TCHAR* Filename, Timestamp& Stamp)
{
	return InternalGetTimestamp(ConvertToAbsolutePath(Filename).c_str(), Stamp);
}

UBOOL FFileManagerWindows::InternalGetTimestamp(const TCHAR* Filename, Timestamp& Stamp)
{
	appMemzero(&Stamp, sizeof(Timestamp));
	struct _stat FileInfo;
	if( _tstat(Filename, &FileInfo) == 0 )
	{
		time_t FileTime;
		FileTime = FileInfo.st_ctime;		// Time of creation of file

		tm Time;
		gmtime_s(&Time, &FileTime);

		Stamp.Day		= Time.tm_mday;
		Stamp.Month		= Time.tm_mon;
		Stamp.DayOfWeek = Time.tm_wday;
		Stamp.DayOfYear = Time.tm_yday;
		Stamp.Hour		= Time.tm_hour;
		Stamp.Minute	= Time.tm_min;
		Stamp.Second	= Time.tm_sec;
		Stamp.Year		= Time.tm_year + 1900;
		return TRUE;
	}

	return FALSE;
}

UBOOL FFileManagerWindows::SetFileTimestamp(const TCHAR *Filename, DOUBLE Timestamp)
{
	return InternalSetFileTimestamp(ConvertToAbsolutePath(Filename).c_str(), Timestamp);
}

UBOOL FFileManagerWindows::InternalSetFileTimestamp(const TCHAR* Filename, DOUBLE Timestamp)
{
	_utimbuf Time;
	Time.modtime = Timestamp;
	Time.actime = Timestamp;

	INT Ret = _tutime(Filename, &Time);
	return Ret == 0;
}

FFileHandle FFileManagerWindows::FileOpen(const TCHAR *Filename, DWORD Flags)
{
	return InternalFileOpen(ConvertToAbsolutePath(Filename).c_str(), Flags);
}

FFileHandle FFileManagerWindows::InternalFileOpen(const TCHAR *Filename, DWORD Flags)
{
	DWORD AccessFlags = (Flags & EIO_Read) ? GENERIC_READ : 0;
	DWORD CreateFlags = 0;
	if( Flags & EIO_Write )
	{
		AccessFlags |= GENERIC_WRITE;
		CreateFlags = (Flags & EIO_Append) ? OPEN_ALWAYS : CREATE_NEW;
	}
	else
	{
		CreateFlags = OPEN_EXISTING;
	}

	HANDLE FileHandle = CreateFile(Filename, AccessFlags, FILE_SHARE_READ, NULL, CreateFlags, FILE_ATTRIBUTE_NORMAL, NULL);
	if( FileHandle == INVALID_HANDLE_VALUE )
	{
		FileHandle = (HANDLE)INDEX_NONE;
	}
	else if( Flags & EIO_Append )
	{
		SetFilePointer(FileHandle, 0, NULL, FILE_END);
	}
	FFileHandle Handle(PTRINT(FileHandle), 0);
	return Handle;
}

void FFileManagerWindows::FileClose(FFileHandle FileHandle)
{
	if( FileHandle.IsValid() )
	{
		CloseHandle((HANDLE)FileHandle._Handle);
	}
}

INT FFileManagerWindows::FileSeek(FFileHandle FileHandle, INT Offset, EFileSeekFlags Base)
{
	if( FileHandle.IsValid() )
	{
		DWORD SeekFlags;
		switch(Base)
		{
		case EIO_Seek_End:
			SeekFlags = FILE_END;
			break;
		case EIO_Seek_Current:
			SeekFlags = FILE_CURRENT;
			break;
		case EIO_Seek_Begin:
		default:
			SeekFlags = FILE_BEGIN;
		}
		DWORD NewPosition = SetFilePointer((HANDLE)FileHandle._Handle, Offset, NULL, SeekFlags);
		if( NewPosition != INVALID_SET_FILE_POINTER )
		{
			return NewPosition;
		}
	}
	return INDEX_NONE;
}

INT FFileManagerWindows::GetFilePosition(FFileHandle FileHandle)
{
	if( FileHandle.IsValid() )
	{
		DWORD NewPosition = SetFilePointer((HANDLE)FileHandle._Handle, 0, NULL, FILE_CURRENT);
		if( NewPosition != INVALID_SET_FILE_POINTER )
		{
			return NewPosition;
		}
	}
	return INDEX_NONE;
}

INT FFileManagerWindows::FileRead(FFileHandle FileHandle, void *Buffer, INT Size)
{
	if( FileHandle.IsValid() )
	{
		DWORD ReadBytes;
		UBOOL bSuccess = ReadFile((HANDLE)FileHandle._Handle, Buffer, Size, &ReadBytes, NULL);
		if( bSuccess )
		{
			return ReadBytes;
		}
	}
	return INDEX_NONE;
}

INT FFileManagerWindows::FileWrite(FFileHandle FileHandle, const void *Buffer, INT Size)
{
	if( FileHandle.IsValid() )
	{
		DWORD WriteBytes;
		UBOOL bSuccess = WriteFile((HANDLE)FileHandle._Handle, Buffer, Size, &WriteBytes, NULL);
		if( bSuccess )
		{
			return WriteBytes;
		}
	}
	return INDEX_NONE;
}

INT FFileManagerWindows::FileTruncate(const TCHAR *Filename, INT FileSize)
{
	FString strFilename = ConvertToAbsolutePath(Filename);

	INT NewFileSize = INDEX_NONE;
	HANDLE FileHandle = CreateFile(strFilename.c_str(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	UBOOL bSuccess = (FileHandle != INVALID_HANDLE_VALUE);
	if( bSuccess )
	{
		DWORD CurrentFileSize = GetFileSize(FileHandle, NULL);
		NewFileSize = INT(CurrentFileSize);
		UBOOL bSuccess = (CurrentFileSize != INVALID_FILE_SIZE);
		if( CurrentFileSize > (DWORD)FileSize && bSuccess )
		{
			DWORD NewPosition = SetFilePointer(FileHandle, FileSize, NULL, FILE_BEGIN);
			bSuccess = (NewPosition == DWORD(FileSize) && SetEndOfFile(FileHandle));
			NewFileSize = FileSize;
		}
		CloseHandle(FileHandle);
	}
	return bSuccess ? NewFileSize : INDEX_NONE;
}

INT FFileManagerWindows::FileSize(const TCHAR* Filename)
{
	FString strFilename = ConvertToAbsolutePath(Filename);

	HANDLE FileHandle = CreateFile(strFilename.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if( FileHandle == INVALID_HANDLE_VALUE )
	{
		return INDEX_NONE;
	}
	DWORD Result = GetFileSize(FileHandle, NULL);
	CloseHandle(FileHandle);
	return Result;
}

void FFileManagerWindows::FileFlush(FFileHandle FileHandle)
{
	if( FileHandle.IsValid() )
	{
		FlushFileBuffers((HANDLE)FileHandle._Handle);
	}
}