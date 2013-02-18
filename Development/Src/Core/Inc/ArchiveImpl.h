#ifndef VRI3_CORE_ARCHIVEIMPL_H_
#define VRI3_CORE_ARCHIVEIMPL_H_


class FArchiveFileReader : public FArchive
{
public:
	FArchiveFileReader(HANDLE InHandle, const TCHAR* InFilename, INT InSize, FOutputDevice* InError);
	~FArchiveFileReader();

	virtual void Serialize(void* Value, INT Length);

	virtual void Seek(INT Pos);
	virtual INT Tell();
	virtual INT TotalSize();
	virtual UBOOL Close();
	
protected:
	UBOOL InternalPrecache(INT PrecacheOffset, INT PrecacheSize);

protected:
	HANDLE			_Handle;
	FString			_Filename;
	INT				_Size;
	INT				_Pos;
	FOutputDevice*	_Error;

	// variable for precache
	INT				_BufferBase;				// 缓存的起始位置
	INT				_BufferCount;				// 
	BYTE			_Buffer[1024];				// buffer大小必须为2的幂
};

class FArchiveFileWriter : public FArchive
{
public:
	FArchiveFileWriter(HANDLE InHandle, const TCHAR* InFilename, INT Pos, FOutputDevice* InError);
	~FArchiveFileWriter();

	virtual void Serialize(void* Value, INT Length);

	virtual void Seek(INT Pos);
	virtual INT Tell();
	virtual INT TotalSize();
	virtual UBOOL Close();
	virtual void Flush();

protected:
	HANDLE			_Handle;
	FString			_Filename;
	INT				_Pos;
	FOutputDevice*	_Error;

	// variable for precache
	INT				_BufferCount;				// usage size of buffer
	BYTE			_Buffer[4096];
};

#endif