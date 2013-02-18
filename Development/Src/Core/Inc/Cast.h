#include "CorePCH.h"

template< class T > T* Cast( VObject* Src )
{
	return Src && Src->IsKindOf(T::StaticClass()) ? (T*)Src : NULL;
}

template< class T > const T* ConstCast( const VObject* Src )
{
	return Src && Src->IsKindOf(T::StaticClass()) ? (T*)Src : NULL;
}

template < class T > T* ExactCast( VObject* Src )
{
	return Src && Src->IsExactKindOf(T::StaticClass()) ? (T*)Src : NULL;
}

template < class T > UBOOL IsKindOf( VObject* Src )
{
	return Src && Src->IsKindOf(T::StaticClass());
}

template < class T > UBOOL IsExactKindOf( VObject* Src )
{
	return Src && Src->IsExactKindOf(T::StaticClass());
}