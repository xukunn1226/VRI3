#ifndef __VPACKAGE_H__
#define __VPACKAGE_H__

class VPackage/* : public VObject*/
{
	//DECLARE_CLASS(VPackage, VObject);

public:
	VPackage();

	void SetDirtyFlag(UBOOL bDirty)
	{
		m_bDirty = bDirty;
	}

	UBOOL IsDirty() const
	{
		return m_bDirty;
	}

	UBOOL IsFullyLoaded() const
	{
		return m_bHasBeenFullyLoaded;
	}

	INT GetFileSize()
	{
		return m_iFileSize;
	}

public:
	UBOOL			m_bDirty;
	INT				m_iFileSize;
	UBOOL			m_bHasBeenFullyLoaded;
};

#endif