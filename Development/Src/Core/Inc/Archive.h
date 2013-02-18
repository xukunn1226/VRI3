/**	@brief : Archive.h  
 *	@author : xukun
 *	@data : 08.05.18
 */

#ifndef VRI3_CORE_ARCHIVE_H_
#define VRI3_CORE_ARCHIVE_H_

class FArchive
{
public:
	FArchive(): _bLoading(TRUE), _bError(FALSE) {}
	virtual ~FArchive() {}

	virtual void Serialize( void* Value, INT Length ) {}
	
	virtual FArchive& operator<< (class VObject*& pkObject)
	{
		return *this;
	}

	virtual FString GetArchiveName() const
	{
		return FString(TEXT("FArchive"));
	}

	virtual INT Tell()
	{
		return INDEX_NONE;
	}

	virtual INT TotalSize()
	{
		return INDEX_NONE;
	}

	virtual UBOOL AtEnd()
	{
		INT Pos = Tell();
		return Pos != INDEX_NONE && Pos >= TotalSize();
	}

	virtual void Seek(INT Pos) {}
	virtual void Flush() {}
	virtual UBOOL Close() { return !_bError; }
	virtual UBOOL GetError() { return _bError; }

	friend FArchive& operator<< (FArchive& Ar, ANSICHAR& C)
	{
		Ar.Serialize(&C, 1);
		return Ar;
	}

	friend FArchive& operator<< (FArchive& Ar, BYTE& B)
	{
		Ar.Serialize(&B, 1);
		return Ar;
	}

	friend FArchive& operator<< (FArchive& Ar, SBYTE& B)
	{
		Ar.Serialize(&B, 1);
		return Ar;
	}

	friend FArchive& operator<< (FArchive& Ar, WORD& W)
	{
		Ar.Serialize(&W, sizeof(W));
		return Ar;
	}

	friend FArchive& operator<< (FArchive& Ar, SWORD& S)
	{
		Ar.Serialize(&S, sizeof(S));
		return Ar;
	}

	friend FArchive& operator<< (FArchive& Ar, DWORD& D)
	{
		Ar.Serialize(&D, sizeof(D));
		return Ar;
	}

	friend FArchive& operator<< (FArchive& Ar, UINT& U)
	{
		Ar.Serialize(&U, sizeof(U));
		return Ar;
	}

	friend FArchive& operator<< (FArchive& Ar, INT& I)
	{
		Ar.Serialize(&I, sizeof(I));
		return Ar;
	}

	friend FArchive& operator<< (FArchive& Ar, QWORD& Q)
	{
		Ar.Serialize(&Q, sizeof(Q));
		return Ar;
	}

	friend FArchive& operator<< (FArchive& Ar, SQWORD& SQ)
	{
		Ar.Serialize(&SQ, sizeof(SQ));
		return Ar;
	}

	friend FArchive& operator<< (FArchive& Ar, FLOAT& F)
	{
		Ar.Serialize(&F, sizeof(F));
		return Ar;
	}

	friend FArchive& operator<< (FArchive& Ar, DOUBLE& F)
	{
		Ar.Serialize(&F, sizeof(F));
		return Ar;
	}

	friend FArchive& operator<< (FArchive& Ar, TCHAR*& Str);

	friend FArchive& operator<< (FArchive& Ar, FString& Str);

	UBOOL IsLoading() { return _bLoading; }

protected:
	UBOOL	_bLoading;
	UBOOL	_bError;
};


#endif