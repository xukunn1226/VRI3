/**	@brief : FileSystem.h  
 *	@author : xukun
 *	@data : 08.05.03
 */

#ifndef VRI3_CORE_FILESYSTEM_H_
#define VRI3_CORE_FILESYSTEM_H_

class FArchive;

enum EFileTime
{
	EFT_Create				= 0,
	EFT_LastAccess			= 1,
	EFT_LastWrite			= 2,
};

enum EFileRead
{
	EFR_NoFail				= 0x01,
	EFR_Slient				= 0x02,
};

enum EFileWrite
{
	EFW_NoFail				= 0x01,
	EFW_NoReplaceExisting	= 0x02,
	EFW_EvenIfReadOnly		= 0x04,
	EFW_Append				= 0x08,
	EFW_AllowRead			= 0x10,
};

struct FFileHandle
{
	FFileHandle()
		: _Handle(PTRINT(INDEX_NONE)),
		  _Info(PTRINT(INDEX_NONE))
	{}

	FFileHandle(PTRINT InHandle, PTRINT InInfo)
		: _Handle(InHandle), _Info(InInfo)
	{}

	UBOOL IsValid() { return _Handle != PTRINT(INDEX_NONE); }

	void Invalidate()
	{
		_Handle = PTRINT(INDEX_NONE);
		_Info = PTRINT(INDEX_NONE);
	}

	PTRINT	_Handle;
	PTRINT	_Info;
};

enum EFileOpenFlags
{
	EIO_Read				= 0x01,
	EIO_Write				= 0x02,
	EIO_ReadWrite			= EIO_Read | EIO_Write,
	EIO_Append				= 0x04,
};

enum EFileSeekFlags
{
	EIO_Seek_Begin,
	EIO_Seek_Current,
	EIO_Seek_End,
};

class FFileManager
{
public:
	struct Timestamp
	{
		INT		Year;
		INT		Month;
		INT		Day;
		INT		Hour;
		INT		Minute;
		INT		Second;
		INT		DayOfWeek;
		INT		DayOfYear;

		INT GetJulian() const;
		INT GetSecondOfDay() const;
		UBOOL operator<	 (const Timestamp& Other) const;
		UBOOL operator>  (const Timestamp& Other) const;
		UBOOL operator<= (const Timestamp& Other) const;
		UBOOL operator>= (const Timestamp& Other) const;
		UBOOL operator== (const Timestamp& Other) const;
		UBOOL operator!= (const Timestamp& Other) const;
	};

public:
	FFileManager() {}

	virtual FArchive* CreateFileReader( const TCHAR* Filename, DWORD ReadFlags = 0, FOutputDevice* InError = GNullOut) = 0;
	virtual FArchive* CreateFileWriter( const TCHAR* Filename, DWORD WriteFlags = 0, FOutputDevice* InError = GNullOut) = 0;

	virtual UBOOL IsReadOnly(const TCHAR* Filename) = 0;
	virtual UBOOL Delete( const TCHAR* Filename, UBOOL bEvenIfReadOnly = 0 ) = 0;
	virtual UBOOL Move( const TCHAR* Dest, const TCHAR* Src, UBOOL bReplace, UBOOL bEvenIfReadOnly, UBOOL bAttributes ) = 0;
	virtual UBOOL Copy( const TCHAR* Dest, const TCHAR* Src, UBOOL bReplaceExisting = TRUE, UBOOL bEvenIfReadOnly = FALSE ) = 0;

	virtual UBOOL TouchFile(const TCHAR* Filename) = 0;

	virtual INT FindAvailableFilename(const TCHAR* Base, const TCHAR* Extension, FString& OutFilename, INT StartVal = -1) = 0;

	virtual UBOOL MakeDirectory( const TCHAR* Path, UBOOL Tree = 0 ) = 0;
	virtual UBOOL DeleteDirectory( const TCHAR* Path, UBOOL Tree = 0 ) = 0;
	virtual void  FindFiles( vector<FString>& Results, const TCHAR* Filename, UBOOL IncludeSubDir ) = 0;
	virtual void  FindFiles( vector<FString>& Results, const TCHAR* Filename, UBOOL Files, UBOOL Directories ) = 0;

	virtual DOUBLE GetFileAgeSeconds( const TCHAR* Filename ) = 0;
	virtual DOUBLE GetFileTimestamp( const TCHAR* Filename ) = 0;
	virtual UBOOL GetTimestamp(const TCHAR* Filename, Timestamp& Stamp) = 0;
	virtual UBOOL SetFileTimestamp(const TCHAR* Filename, DOUBLE Timestamp) = 0;

	virtual UBOOL SetDefaultDirectory() = 0;
	virtual FString GetDefaultDirectory() = 0;
	virtual UBOOL SetWorkingDirectory(const TCHAR* WorkingDir) = 0;
	virtual FString GetWorkingDirectory() = 0;

	virtual FString ConvertToAbsolutePath( const TCHAR* Filename ) = 0;

	virtual FFileHandle FileOpen(const TCHAR* Filename, DWORD Flags) = 0;
	virtual void FileClose(FFileHandle FileHandle) = 0;

	virtual INT FileSeek(FFileHandle FileHandle, INT Offset, EFileSeekFlags Flags = EIO_Seek_Begin) = 0;
	virtual INT GetFilePosition(FFileHandle FileHandle) = 0;
	virtual INT FileRead(FFileHandle FileHandle, void* Buffer, INT Size) = 0;
	virtual INT FileWrite(FFileHandle FileHandle, const void* Buffer, INT Size) = 0;
	virtual INT FileTruncate(const TCHAR* Filename, INT FileSize) = 0;
	virtual INT FileSize(const TCHAR* Filename) = 0;

	virtual void FileFlush(FFileHandle FileHandle) = 0;
};


class FFileManagerWindows : public FFileManager
{
public:
	FFileManagerWindows();

	/**
	 *	set the default directory in which execute file reside
	 *	@result				TRUE if successful
	 */
	virtual UBOOL SetDefaultDirectory();

	/**
	 *	return the default directory
	 */
	virtual FString GetDefaultDirectory();

	/**
	 *	set a customized working directory
	 *	@param	WorkingDir	a customized directory
	 *	@result				TRUE if successful
	 */
	virtual UBOOL SetWorkingDirectory(const TCHAR* WorkingDir);

	/**
	 *	return the working directory
	 */
	virtual FString GetWorkingDirectory();

	/**
	 *	return an absolute path
	 */
	virtual FString ConvertToAbsolutePath( const TCHAR* Filename );

	/**
	 *	create an archive to read a file
	 *	@param	Filename		path to file
	 *	@param	ReadFlags		EFileRead
	 *	@param	InError			device of output
	 *	@result					return an archive for reading, or NULL if no exist.
	 */
	virtual FArchive* CreateFileReader(const TCHAR* Filename, DWORD ReadFlags = 0, FOutputDevice* InError = NULL);

	/**
	 *	create an archive to write a file
	 *	@param	Filename		path to file
	 *	@param	WriteFlags		EFileWrite
	 *	@param	InError			device of output
	 *	@result					return an archive for writing, or NULL if occurs an error
	 */
	virtual FArchive* CreateFileWriter(const TCHAR* Filename, DWORD WriteFlags = 0, FOutputDevice* InError = NULL);

	/**
	 *	has the file read-only attributes?
	 *	@param	Filename	path to file
	 *	@result				TRUE meaning the file is read-only, otherwise not read-only
	 */
	virtual UBOOL IsReadOnly(const TCHAR* Filename);

	/**
	 *	delete a file
	 *	@param	Filename			path to file
	 *	@param	bEventIfReadOnly	indicates whether would delete it even which is read only
	 *	@result						TRUE if successful
	 */
	virtual UBOOL Delete( const TCHAR* Filename, UBOOL bEvenIfReadOnly = 0 );

	/**
	 *	move a file from 'Src' to 'Dest'
	 *	@param	Dest				the destination to move
	 *	@param	Src					the source from move
	 *	@param	bReplace			no work
	 *	@param	bEventIfReadOnly	no work
	 *	@param	bAttributes			no work
	 *	@result						TRUE if successful
	 */
	virtual UBOOL Move( const TCHAR* Dest, const TCHAR* Src, UBOOL bReplace, UBOOL bEvenIfReadOnly, UBOOL bAttributes );

	/**
	 *	copy a file from 'Src' to 'Dest'
	 *	@param	Dest				the destination to copy
	 *	@param	Src					the source to copy
	 *	@param	bReplaceExisting	indicates whether would replace it even which is existing
	 *	@param	bEvenIfReadOnly		indicates whether would copy to 'Dest' even which is read-only
	 *	@result						TRUE if successful
	 */
	virtual UBOOL Copy( const TCHAR* Dest, const TCHAR* Src, UBOOL bReplaceExisting = TRUE, UBOOL bEvenIfReadOnly = FALSE );

	/**
	 *	update the modification time of the file on disk
	 *	@param	Filename	path of the file to touch
	 *	@result				TRUE if successful
	 */
	virtual UBOOL TouchFile(const TCHAR* Filename);

	/**
	 *	find any files comply with Base####.Extension where #### is a 4-digit number in [0000-9999]
	 *	@param	Base			Filename base, optionally including a path
	 *	@param	Extension		File extension
	 *	@param	OutFilename		[out]
	 *	@result					the index of the created file, or -1 if no free file with index (StartVal, 9999] was found.
	 **/
	virtual INT FindAvailableFilename(const TCHAR* Base, const TCHAR* Extension, FString& OutFilename, INT StartVal = -1);

	/**
	 *	create the directory according to the path, optionally whether create intermediate directory
	 *	@param	Path		path to create
	 *	@param	Tree		whether create intermediate directory
	 *	@result				TRUE if successful
	 */
	virtual UBOOL MakeDirectory( const TCHAR* Path, UBOOL Tree = 0 );
	virtual UBOOL DeleteDirectory( const TCHAR* Path, UBOOL Tree = 0 );

	/**
	 *	find any files from a directory
	 *	@param	Results			[out] files found
	 *	@param	Filename		indicate a directory
	 *	@param	IncludeSubDir	whether find subdirectory or not
	 */
	virtual void  FindFiles( vector<FString>& Results, const TCHAR* Filename, UBOOL IncludeSubDir );
	virtual void  FindFiles( vector<FString>& Results, const TCHAR* Filename, UBOOL Files, UBOOL Directories );

	/**
	 *	finds the difference between two times
	 *	@param	Filename		the file to be compared
	 *	@result					the elapsed time in seconds from the last modification of Filename to current time
	 *							return 0, indicating an error
	 */
	virtual DOUBLE GetFileAgeSeconds( const TCHAR* Filename );

	/**
	 *	get the last modification of filename
	 *	@param	Filename		path of the file
	 *	@result					the last modification of Filename, return -1 if occurs an error.
	 */
	virtual DOUBLE GetFileTimestamp( const TCHAR* Filename );

	/**
	 *	get detailed time stamp of a file
	 *	@param	Filename		path of the file
	 *	@param	Stamp			[out]detailed time stamp
	 *	@result					TRUE if obtains, otherwise FALSE
	 */
	virtual UBOOL GetTimestamp(const TCHAR* Filename, Timestamp& Stamp);

	/**
	 *	set timestamp to filename
	 *	@param	Filename		path of the file
	 *	@param	Timestamp		time stamp to a file
	 *	@result					TRUE if successful
	 */
	virtual UBOOL SetFileTimestamp(const TCHAR* Filename, DOUBLE Timestamp);

	/** 
	 *	Open a file, if opened for write, it will create any necessary directory structure.
	 *
	 *	@param	Filename		path of the file
	 *	@param	Flags			a combination of EFileOpenFlags
	 *	@return					File handle
	 **/
	virtual FFileHandle FileOpen(const TCHAR* Filename, DWORD Flags);

	/**
	 *	close a file
	 *	@param	FileHandle		the handle of a file to be closed
	 */
	virtual void FileClose(FFileHandle FileHandle);

	/**
	 *	set the current position in a file
	 *	@param	FileHandle		file handle
	 *	@param	Offset			new file offset in bytes from 'Base'
	 *	@param	Base			a EFileSeekFlags flag, specifying the seek operation.
	 *	@result					the new position if successful, otherwise return -1.
	 */
	virtual INT FileSeek(FFileHandle FileHandle, INT Offset, EFileSeekFlags Base = EIO_Seek_Begin);

	/**
	 *	obtains the current position in a file
	 *	@param	FileHandle		file handle
	 *	@result					current file position from the beginning of the file, or return -1 on error
	 */
	virtual INT GetFilePosition(FFileHandle FileHandle);

	/**
	 *	read a number of bytes from file
	 *	@param	FileHandle		file handle
	 *	@param	Buffer			buffer to store the read data
	 *	@param	Size			number of bytes to read
	 *	@result					actual number of bytes read into 'Buffer', or -1 on error
	 */
	virtual INT FileRead(FFileHandle FileHandle, void* Buffer, INT Size);

	/**
	 *	write a number of bytes to file
	 *	@param	FileHandle		file handle
	 *	@param	Buffer			buffer that contains the data to write
	 *	@param	Size			number of bytes to write
	 *	@result					actual number of bytes written to file, or -1 on error
	 */
	virtual INT FileWrite(FFileHandle FileHandle, const void* Buffer, INT Size);

	/**
	 *	truncate an existing file, discarding data at the end to make it smaller
	 *	@param	Filename		path to file
	 *	@param	FileSize		new file size to truncate to. If this is larger than current file size, the function won't do anything.
	 *	@result					new file size, or -1 if the file didn't exist.
	 */
	virtual INT FileTruncate(const TCHAR* Filename, INT FileSize);

	/**
	 *	return the size of file
	 *	@param	Filename		path to file
	 *	@result					file size in bytes or -1 if the file didn't exist
	 */
	virtual INT FileSize(const TCHAR* Filename);

	/**
	 *	flushes the file to make sure any buffered data has been fully written to disk.
	 *	@param	FileHandle		file handle
	 */
	virtual void FileFlush(FFileHandle FileHandle);

protected:
	UBOOL IsDrive(const TCHAR* Path);
	UBOOL InternalIsReadOnly(const TCHAR* Filename);
	UBOOL InternalDelete(const TCHAR* Filename, UBOOL bEvenIfReadOnly);
	UBOOL InternalMove(const TCHAR* Dest, const TCHAR* Src, UBOOL bReplace, UBOOL bEvenIfReadOnly, UBOOL bAttributes);
	UBOOL InternalCopy(const TCHAR* Dest, const TCHAR* Src, UBOOL bReplaceExisting, UBOOL bEvenIfReadOnly);
	UBOOL InternalMakeDirectory( const TCHAR* Path, UBOOL Tree );
	UBOOL InternalDeleteDirectory( const TCHAR* Path );
	UBOOL InternalGetTimestamp(const TCHAR* Filename, Timestamp& Stamp);
	DOUBLE InternalGetFileTimestamp(const TCHAR* Filename);
	DOUBLE InternalGetFileAgeSeconds(const TCHAR* Filename);
	UBOOL InternalSetFileTimestamp(const TCHAR* Filename, DOUBLE Timestamp);
	FFileHandle InternalFileOpen(const TCHAR* Filename, DWORD Flags);
protected:
	FString		m_strRootDir;
};


#endif