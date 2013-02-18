#include "CorePCH.h"

///////////////////////////FArchiveFileReader Implementation/////////////////////////////
FArchiveFileReader::FArchiveFileReader(HANDLE InHandle, const TCHAR *InFilename, INT InSize, FOutputDevice* InError)
: _Handle(InHandle), 
  _Filename(InFilename), 
  _Size(InSize), 
  _Pos(0), 
  _BufferBase(0), 
  _BufferCount(0),
  _Error(InError)
{
	_bLoading = TRUE;
}

FArchiveFileReader::~FArchiveFileReader()
{
	if(_Handle)
	{
		Close();
	}
}

void FArchiveFileReader::Seek(INT Pos)
{
	check(Pos >= 0 && Pos <= _Size);
	if( SetFilePointer(_Handle, Pos, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER )
	{
		_bError = TRUE;
		_Error->Logf(TEXT("SetFilePointer Failed %i%i : %i"), Pos, _Size, _Pos);
	}
	_Pos = Pos;
	_BufferBase = Pos;
	_BufferCount = 0;		// meaning clear the buffer
}

INT FArchiveFileReader::Tell()
{
	return _Pos;
}

INT FArchiveFileReader::TotalSize()
{
	return _Size;
}

UBOOL FArchiveFileReader::Close()
{
	if( _Handle )
	{
		CloseHandle(_Handle);
	}
	_Handle = NULL;
	_BufferBase = 0;
	_BufferCount = 0;
	return !_bError;
}

void FArchiveFileReader::Serialize(void *Value, INT Length)
{
	while( Length > 0 )
	{
		INT Copy = Min( Length, _BufferBase + _BufferCount - _Pos );
		if( Copy <= 0 )
		{
			if( Length >= ARRAY_COUNT(_Buffer) )	// 读取字段大于BUFFER则直接从DEVICE获取
			{
				INT Count=0;
				// Read data from device via Win32 ReadFile API.
				{
					ReadFile( _Handle, Value, Length, (DWORD*)&Count, NULL );
				}

				if( Count != Length )
				{
					const TCHAR* Msg = appGetSystemErrorMessage();
					_bError = TRUE;
					_Error->Logf( TEXT("ReadFile failed: Count=%i Length=%i for file %s"), 
						Count, Length, _Filename );
				}
				_Pos += Length;
				_BufferBase += Length;
				return;
			}

			InternalPrecache( _Pos, MAXINT );
			Copy = Min( Length, _BufferBase + _BufferCount - _Pos );
			if( Copy <= 0 )
			{
				_bError = TRUE;
				_Error->Logf( TEXT("ReadFile beyond EOF %i+%i/%i for file %s"), 
					_Pos, Length, _Size, _Filename );
			}

			if( _bError )
			{
				return;
			}
		}

		appMemcpy( Value, _Buffer + _Pos - _BufferBase, Copy );
		_Pos    += Copy;
		Length    -= Copy;
		Value     = (BYTE*)Value + Copy;
	}
}

UBOOL FArchiveFileReader::InternalPrecache(INT PrecacheOffset, INT PrecacheSize)
{
	// Only precache at current position and avoid work if precaching same offset twice.
	if( _Pos == PrecacheOffset && (!_BufferBase || !_BufferCount || _BufferBase != _Pos) )
	{
		_BufferBase = _Pos;
		_BufferCount = Min( Min( PrecacheSize, (INT)(ARRAY_COUNT(_Buffer) - (_Pos&(ARRAY_COUNT(_Buffer)-1))) ), _Size-_Pos );
		_BufferCount = Max(_BufferCount,0); // clamp to 0
		INT Count=0;

		// Read data from device via Win32 ReadFile API.
		{
			ReadFile( _Handle, _Buffer, _BufferCount, (DWORD*)&Count, NULL );
		}

		if( Count != _BufferCount )
		{
			_bError = TRUE;
		}
	}
	return TRUE;
}

///////////////////////////FArchiveFileWriter Implementation/////////////////////////////
FArchiveFileWriter::FArchiveFileWriter(HANDLE InHandle, const TCHAR *InFilename, INT Pos, FOutputDevice* InError)
: _Handle(InHandle), 
  _Filename(InFilename), 
  _Pos(Pos),
  _BufferCount(0),
  _Error(InError)
{
	_bLoading = FALSE;
}

FArchiveFileWriter::~FArchiveFileWriter()
{
	if( _Handle )
	{
		Close();
	}
}

void FArchiveFileWriter::Seek(INT Pos)
{
	Flush();
	if( SetFilePointer(_Handle, Pos, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER )
	{
		_bError = TRUE;
		_Error->Logf(TEXT("SeekFailed: %s"), _Filename);
	}
	_Pos = Pos;
}

INT FArchiveFileWriter::Tell()
{
	return _Pos;
}

INT FArchiveFileWriter::TotalSize()
{
	// make sure that all data is writen before looking at file size
	Flush();

	LARGE_INTEGER FileSize;
	FileSize.QuadPart = -1;

	if( GetFileSizeEx(_Handle, &FileSize) == FALSE )
	{
		_bError = TRUE;
		_Error->Logf(TEXT("WriteFailed: %s"), _Filename);
	}

	return FileSize.QuadPart;
}

UBOOL FArchiveFileWriter::Close()
{
	Flush();

	if( _Handle && !CloseHandle(_Handle) )
	{
		_bError = TRUE;
	}
	_Handle = NULL;
	return !_bError;
}

void FArchiveFileWriter::Serialize(void* Value, INT Length)
{
	_Pos += Length;
	INT Copy;
	
	while( Length > ( Copy = ARRAY_COUNT(_Buffer) - _BufferCount ) )
	{
		appMemcpy(_Buffer + _BufferCount, Value, Copy);
		_BufferCount += Copy;
		Length -= Copy;
		Value = (BYTE*)Value + Copy;
		Flush();
	}

	if( Length > 0 )
	{
		appMemcpy(_Buffer + _BufferCount, Value, Length);
		_BufferCount += Length;
	}
}

void FArchiveFileWriter::Flush()
{
	if( _BufferCount )
	{
		INT Result = 0;
		if( !WriteFile(_Handle, _Buffer, _BufferCount, (DWORD*)&Result, 0) )
		{
			_bError = TRUE;
			_Error->Logf(TEXT("WriteFailed: %s"), _Filename);
		}
	}
	_BufferCount = 0;
}