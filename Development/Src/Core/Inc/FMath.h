#ifndef __MATH_H__
#define __MATH_H__

#include <time.h>

class FVector2;
class FVector;
class FVector4;
class FRotator;
class FQuat;
class FPlane;
class FMatrix;

// Constants.
#undef  PI
#define PI 							(3.1415926535897932)
#define SMALL_NUMBER				(1.e-8)
#define KINDA_SMALL_NUMBER			(1.e-4)
#define BIG_NUMBER					(3.4e+38f)
#define EULERS_NUMBER				(2.71828182845904523536)
#define RADIAN_PER_DEGREE			( 0.017453292519943 )		//( PI / 180 )
#define DEGREE_PER_RADIAN			( 57.29577951308232 )		//( 180 / PI )
#define DEGREE_TO_RADIAN(DEGREE)	( DEGREE * RADIAN_PER_DEGREE )
#define RADIAN_TO_DEGREE(RADIAN)	( RADIAN * DEGREE_PER_RADIAN )

#define INV_PI						(0.31830988618)
#define HALF_PI						(1.57079632679)

#define DELTA						(0.00001f)


#define THRESH_VECTORS_ARE_PARALLEL		(0.02f)

enum EAxis
{
	AXIS_None		= 0,
	AXIS_X			= 1,
	AXIS_Y			= 2,
	AXIS_Z			= 4,
	AXIS_XY			= AXIS_X | AXIS_Y,
	AXIS_XZ			= AXIS_X | AXIS_Z,
	AXIS_YZ			= AXIS_Y | AXIS_Z,
	AXIS_XYZ		= AXIS_X | AXIS_Y | AXIS_Z
};

#include "Color.h"

/*-----------------------------------------------------------------------------
	FVector2.
-----------------------------------------------------------------------------*/
class FVector2
{
public:
	FLOAT x, y;
	
public:
	FVector2() : x(0.f), y(0.f) {}
	FVector2(FLOAT InX, FLOAT InY)
		: x(InX), y(InY)
	{
	}

	FVector2& operator= (const FVector2& V)
	{
		x = V.x;
		y = V.y;
		return *this;
	}

	FVector2 operator+ (const FVector2& V) const
	{
		return FVector2(x + V.x, y + V.y);
	}

	FVector2 operator- (const FVector2& V) const
	{
		return FVector2(x - V.x, y - V.y);
	}

	FVector2 operator* (FLOAT Scale) const
	{
		return FVector2(x * Scale, y * Scale);
	}

	friend FVector2 operator* (FLOAT Scale, const FVector2& V)
	{
		return FVector2( Scale * V.x, Scale * V.y );
	}

	FVector2 operator/ (FLOAT Scale) const
	{
		FLOAT InvScale = 1.f / Scale;
		return FVector2(x * InvScale, y * InvScale);
	}

	FVector2 operator* (const FVector2& V) const
	{
		return FVector2(x * V.x, y * V.y);
	}

	UBOOL operator== (const FVector2& V) const
	{
		return x == V.x && y == V.y;
	}

	UBOOL operator!= (const FVector2& V) const
	{
		return x != V.x || y != V.y;
	}

	UBOOL operator< (const FVector2& V) const
	{
		return x < V.x && y < V.y;
	}

	UBOOL operator<= (const FVector2& V) const
	{
		return x <= V.x && y <= V.y;
	}

	UBOOL operator> (const FVector2& V) const
	{
		return x > V.x && y > V.y;
	}

	UBOOL operator>= (const FVector2& V) const
	{
		return x >= V.x && y >= V.y;
	}

	UBOOL Equals(const FVector2& V, FLOAT Tolerance = KINDA_SMALL_NUMBER) const
	{
		return Abs(x - V.x) < Tolerance && Abs(y - V.y) < Tolerance;
	}

	FVector2 operator- () const
	{
		return FVector2(-x, -y);
	}

	FVector2& operator+= (const FVector2& V)
	{
		x += V.x;
		y += V.y;
		return *this;
	}

	FVector2& operator-= (const FVector2& V)
	{
		x -= V.x;
		y -= V.y;
		return *this;
	}

	FVector2& operator*= (const FVector2& V)
	{
		x *= V.x;
		y *= V.y;
		return *this;
	}

	FVector2& operator*= (FLOAT Scale)
	{
		x *= Scale;
		y *= Scale;
		return *this;
	}

	FVector2& operator/= (FLOAT Scale)
	{
		const FLOAT InvScale = 1.f / Scale;
		x *= InvScale;
		y *= InvScale;
		return *this;
	}

	FVector2& operator/= (const FVector2& V)
	{
		x /= V.x;
		y /= V.y;
		return *this;
	}

	FLOAT& operator[] ( INT i )
	{
		check( i > -1 && i < 2 );
		return (&x)[i];
	}

	const FLOAT& operator[] ( INT i ) const
	{
		check( i > -1 && i < 2 );
		return (&x)[i];
	}

	operator const FLOAT*() const { return &x; }
	operator FLOAT*() { return &x; }

	FLOAT GetMax() const
	{
		return Max(x, y);
	}

	FLOAT GetMin() const
	{
		return Min(x, y);
	}

	FLOAT Size() const
	{
		return appSqrt( x * x + y * y );
	}

	FLOAT SizeSquared() const
	{
		return x * x + y * y;
	}

	void Normalize(FLOAT Tolerance = KINDA_SMALL_NUMBER)
	{
		const FLOAT Mag = Size();
		if( Mag < Tolerance )
		{
			x = 0.f;
			y = 0.f;
		}
		else
		{
			const FLOAT InvMag = 1.f / Mag;
			x *= InvMag;
			y *= InvMag;
		}
	}

	FVector2 SafeNormal(FLOAT Tolerance = KINDA_SMALL_NUMBER) const
	{
		const FLOAT Mag = Size();
		if( Mag < Tolerance )
		{
			return FVector2(0.f, 0.f);
		}
		else
		{
			const FLOAT InvMag = 1.f / Mag;
			return FVector2(x * InvMag, y * InvMag);
		}
	}

	FLOAT Distance(const FVector2& V) const
	{
		return (*this - V).Size();
	}

	FLOAT SquaredDistance(const FVector2& V) const
	{
		return (*this - V).SizeSquared();
	}

	FLOAT DotProduct(const FVector2& V) const
	{
		return x * V.x + y * V.y;
	}

	FLOAT CrossProduct(const FVector2& V) const
	{
		return x * V.y - y * V.x;
	}

	UBOOL IsNearlyZero(FLOAT Tolerance = KINDA_SMALL_NUMBER) const
	{
		return Abs(x) < Tolerance && Abs(y) < Tolerance;
	}

	UBOOL IsZero() const
	{
		return x == 0.f && y == 0.f;
	}

	FString ToString() const
	{
		return FStringUtil::Sprintf(TEXT("x = %3.3f  y = %3.3f"), x, y);
	}

	friend FArchive& operator << (FArchive& Ar, FVector2& V)
	{
		return Ar << V.x << V.y;
	}

	static const FVector2	ZERO;
	static const FVector2	UNIT_X;
	static const FVector2	UNIT_Y;
	static const FVector2	NEGATIVE_UNIT_X;
	static const FVector2	NEGATIVE_UNIT_Y;
	static const FVector2	UNIT_SCALE;
};

/*----------------------------------------------------------------------------
								Global Function
------------------------------------------------------------------------------*/
inline FLOAT FSnap(FLOAT Location, FLOAT Grid)
{
	if( Grid == 0.0f )
		return Location;

	return appFloor((Location + 0.5*Grid) / Grid) * Grid;
}

inline DWORD FLOAT_TO_DWORD(FLOAT Float)
{
	return *(DWORD*)&Float;
}

inline UBOOL appIsNearlyEqual(FLOAT A, FLOAT B, FLOAT Tolerance = SMALL_NUMBER)
{
	return Abs<FLOAT>(A - B) < Tolerance;
}

inline UBOOL appIsNearlyEqual(DOUBLE A, DOUBLE B, DOUBLE Tolerance = SMALL_NUMBER)
{
	return Abs<DOUBLE>(A - B) < Tolerance;
}

inline UBOOL appIsNearlyZero(FLOAT A, FLOAT Tolerance = SMALL_NUMBER)
{
	return Abs<FLOAT>(A) < Tolerance;
}

inline UBOOL appIsNearlyZero(DOUBLE A, DOUBLE Tolerance = SMALL_NUMBER)
{
	return Abs<DOUBLE>(A) < Tolerance;
}

/*-----------------------------------------------------------------------------
	FVector.
-----------------------------------------------------------------------------*/
class FVector
{
public:
	FLOAT x, y, z;

public:
	FVector() : x(0.f), y(0.f), z(0.f) {}
	FVector(FLOAT InX, FLOAT InY, FLOAT InZ) : x(InX), y(InY), z(InZ) {}
	FVector(FLOAT InF) : x(InF), y(InF), z(InF) {}
	FVector(const FVector4& V);
	FVector(const FVector& V)
	{
		x = V.x;
		y = V.y;
		z = V.z;
	}

	FVector& operator= (const FVector& V)
	{
		x = V.x;
		y = V.y;
		z = V.z;
		return *this;
	}

	FVector CrossProduct(const FVector& V) const
	{
		return FVector( y * V.z - z * V.y,
						z * V.x - x * V.z,
						x * V.y - y * V.x);
	}

	FLOAT DotProduct(const FVector& V) const
	{
		return x * V.x + y * V.y + z * V.z;
	}

	FLOAT& operator[] ( INT i )
	{
		check( i > -1 && i < 3 );
		return (&x)[i];
	}

	const FLOAT& operator[] ( INT i ) const
	{
		check( i > -1 && i < 3 );
		return (&x)[i];
	}

	operator const FLOAT*() const { return &x; }
	operator FLOAT*() { return &x; }

	FVector operator+ (const FVector& V) const
	{
		return FVector(x + V.x, y + V.y, z + V.z);
	}

	FVector operator- (const FVector& V) const
	{
		return FVector(x - V.x, y - V.y, z - V.z);
	}

	FVector operator* (FLOAT Scale) const
	{
		return FVector(x * Scale, y * Scale, z * Scale);
	}

	friend FVector operator* (FLOAT Scale, const FVector& V)
	{
		return FVector( V.x * Scale, V.y * Scale, V.z * Scale );
	}

	FVector operator* (const FVector& V) const
	{
		return FVector(x * V.x, y * V.y, z * V.z);
	}

	FVector operator/ (FLOAT Scale) const
	{
		const FLOAT InvScale = 1.f / Scale;
		return FVector(x * InvScale, y * InvScale, z * InvScale);
	}

	FVector operator/ (const FVector& V) const
	{
		return FVector(x / V.x, y / V.y, z / V.z);
	}

	UBOOL operator== (const FVector& V) const
	{
		return x == V.x && y == V.y && z == V.z;
	}

	UBOOL operator!= (const FVector& V) const
	{
		return x != V.x || y != V.y || z != V.z;
	}

	UBOOL Equals(const FVector& V, FLOAT Tolerance = KINDA_SMALL_NUMBER) const
	{
		return Abs(x - V.x) < Tolerance && Abs(y - V.y) < Tolerance && Abs(z - V.z) < Tolerance;
	}

	FVector operator- () const
	{
		return FVector(-x, -y, -z);
	}

	FVector& operator+= (const FVector& V)
	{
		x += V.x;
		y += V.y;
		z += V.z;
		return *this;
	}

	FVector& operator-= (const FVector& V)
	{
		x -= V.x;
		y -= V.y;
		z -= V.z;
		return *this;
	}

	FVector& operator*= (FLOAT Scale)
	{
		x *= Scale;
		y *= Scale;
		z *= Scale;
		return *this;
	}

	FVector& operator/= (FLOAT Scale)
	{
		const FLOAT InvScale = 1.f / Scale;
		x *= InvScale;
		y *= InvScale;
		z *= InvScale;
		return *this;
	}

	FVector& operator*= (const FVector& V)
	{
		x *= V.x;
		y *= V.y;
		z *= V.z;
		return *this;
	}

	FVector& operator/= (const FVector& V)
	{
		x /= V.x;
		y /= V.y;
		z /= V.z;
		return *this;
	}

	FLOAT GetMax() const
	{
		return Max( Max(x, y), z );
	}

	FLOAT GetMin() const
	{
		return Min( Min(x, y), z );
	}

	FLOAT Size() const
	{
		return appSqrt( x * x + y * y + z * z );
	}

	FLOAT SizeSquared() const
	{
		return x * x + y * y + z * z;
	}

	UBOOL IsNearlyZero(FLOAT Tolerance = KINDA_SMALL_NUMBER) const
	{
		return Abs(x) < Tolerance && Abs(y) < Tolerance && Abs(z) < Tolerance;
	}

	UBOOL IsZero() const
	{
		return x == 0.f && y == 0.f && z == 0.f;
	}

	UBOOL Normalize(FLOAT Tolerance = SMALL_NUMBER)
	{
		const FLOAT Size2 = SizeSquared();
		if( Size2 == 1.f )
		{
			return TRUE;
		}
		else if( Size2 >= Tolerance )
		{
			const FLOAT Scale = 1.f / appSqrt(Size2);
			x *= Scale;
			y *= Scale;
			z *= Scale;
			return TRUE;
		}
		return FALSE;
	}

	FLOAT Distance(const FVector& V) const
	{
		return (*this - V).Size();
	}

	FLOAT DistanceSquared(const FVector& V) const
	{
		return (*this - V).SizeSquared();
	}

	UBOOL IsUniform(FLOAT Tolerance = KINDA_SMALL_NUMBER) const
	{
		return Abs(x - y) < Tolerance && Abs(y - z) < Tolerance;
	}

	FVector ProjectOnAxis(const FVector& Direction) const
	{
		return (*this).DotProduct(Direction) / Direction.SizeSquared() * Direction;
	}

	FVector ProjectOnPlane(const FVector& Normal) const
	{
		return (*this) - (*this).ProjectOnAxis(Normal);
	}

	// assume "this" shot the plane from far away
	FVector Reflect(const FVector& Normal) const
	{
		return 2.f * ProjectOnPlane(Normal) - (*this);
	}

	FVector SafeNormal(FLOAT Tolerance = SMALL_NUMBER) const
	{
		const FLOAT SquareSum = SizeSquared();

		if( SquareSum == 1.f )
		{
			return *this;
		}
		else if( SquareSum < Tolerance )
		{
			return FVector(0.f);
		}

		const FLOAT Scale = 1.f / appSqrt(SquareSum);
		return FVector(x * Scale, y * Scale, z * Scale);
	}

	FVector GridSnap(const FLOAT Grid) const
	{
		return FVector( FSnap(x, Grid), FSnap(y, Grid), FSnap(z, Grid) );
	}

	FVector RotateAngleAxis(const INT Angle, const FVector& Axis) const;

	/**
	 *	只计算YAW和PITCH
	 */
	FRotator Rotation() const;

	void FindBestAxisVectors(FVector& Axis1, FVector& Axis2) const;

	FString ToString() const
	{
		return FStringUtil::Sprintf(TEXT("x = %3.3f y = %3.3f z = %3.3f"), x, y, z);
	}

	friend FArchive& operator<< (FArchive& Ar, FVector& V)
	{
		return Ar << V.x << V.y << V.z;
	}

	static const FVector	ZERO;
	static const FVector	UNIT_X;
	static const FVector	UNIT_Y;
	static const FVector	UNIT_Z;
	static const FVector	NEGATIVE_UNIT_X;
	static const FVector	NEGATIVE_UNIT_Y;
	static const FVector	NEGATIVE_UNIT_Z;
	static const FVector	UNIT_SCALE;

};

inline FLOAT GetAngleFromVectors(const FVector& V1, const FVector& V2)
{
	FLOAT fDot = V1.DotProduct(V2);
	fDot /= V1.Size();
	fDot /= V2.Size();

	return appAcos(fDot);
}

FVector ClosestPointOnLine(const FVector& LineStart, const FVector& LineEnd, const FVector& Point);

/**
 *	构造一个正交坐标系，XAxis、YAxis方向可能改变，ZAxis则不会
 */
void CreateOrthonormalBasis(FVector& XAxis, FVector& YAxis, FVector& ZAxis);

inline DWORD GetTypeHash(const FVector& V)
{
	return appMemCrc(&V, sizeof(FVector));
}

class FVector4
{
public:
	FLOAT x, y, z, w;

public:
	FVector4(const FVector& InVector, FLOAT InW = 1.f)
		: x(InVector.x), y(InVector.y), z(InVector.z), w(InW)
	{}

	FVector4(FLOAT InX = 0.f, FLOAT InY = 0.f, FLOAT InZ = 0.f, FLOAT InW = 1.f)
		: x(InX), y(InY), z(InZ), w(InW)
	{}

	FLOAT& operator[] ( INT i )
	{
		check( i > -1 && i < 4 );
		return (&x)[i];
	}

	const FLOAT& operator[] ( INT i ) const
	{
		check( i > -1 && i < 4 );
		return (&x)[i];
	}

	void SetValue(FLOAT InX, FLOAT InY, FLOAT InZ, FLOAT InW)
	{
		x = InX;	y = InY;	z = InZ;	w = InW;
	}

	FVector4 operator+ (const FVector4& V) const
	{
		return FVector4(x + V.x, y + V.y, z + V.z, w + V.w);
	}

	FVector4& operator+= (const FVector4& V)
	{
		x += V.x;	y += V.y;	z += V.z;	w += V.w;
		return *this;
	}

	FVector4 operator- (const FVector4& V) const
	{
		return FVector4(x - V.x, y - V.y, z - V.z, w - V.w);
	}

	FVector4 operator* (FLOAT Scale) const
	{
		return FVector4(x * Scale, y * Scale, z * Scale, w * Scale);
	}

	FVector4 operator/ (FLOAT Scale) const
	{
		const FLOAT InvScale = 1.f / Scale;
		return FVector4(x * InvScale, y * InvScale, z * InvScale, w * InvScale);
	}

	FVector4 operator* (const FVector4& V) const
	{
		return FVector4(x * V.x, y * V.y, z * V.z, w * V.w);
	}

	FVector4 operator- () const
	{
		return FVector4(-x, -y, -z, -w);
	}

	FLOAT DotProduct(const FVector4& V) const
	{
		return x * V.x + y * V.y + z * V.z + w * V.w;
	}

	FVector4 SafeNormal(FLOAT Tolerance = SMALL_NUMBER) const
	{
		const FLOAT SquareSum = x * x + y * y + z * z;

		if( SquareSum == 1.f )
		{
			return *this;
		}
		else if( SquareSum < Tolerance )
		{
			return FVector4();
		}
		const FLOAT InvScale = 1.f / appSqrt(SquareSum);
		return FVector4(x * InvScale, y * InvScale, z * InvScale, 0.f);
	}

	FRotator Rotation() const;

	friend FArchive& operator<< (FArchive& Ar, FVector4& V)
	{
		return Ar << V.x << V.y << V.z << V.w;
	}

};

// the formula is stored as Ax + By +Cz = D
// the normal of plane is FVector(A, B, C)
class FPlane : public FVector
{
public:
	FLOAT d;

public:
	FPlane()
		: d(0.f)
	{
	}

	FPlane(const FPlane& P)
		: FVector(P), d(P.d)
	{
	}

	FPlane(FLOAT InX, FLOAT InY, FLOAT InZ, FLOAT InD)
		: FVector(InX, InY, InZ), d(InD)
	{
	}

	FPlane(const FVector& Normal, FLOAT D)
		: FVector(Normal), d(D)
	{
	}

	FPlane(const FVector& Normal, FVector Point)
		: FVector(Normal), d(Point.DotProduct(Normal))
	{
	}

	FPlane& operator= (const FPlane& rhs)
	{
		FVector::operator= (rhs);
		d = rhs.d;
		return *this;
	}

	FPlane(const FVector& P1, const FVector& P2, const FVector& P3)
	{
		FVector Edg1 = P2 - P1;
		FVector Edg2 = P3 - P1;
		FVector Normal = Edg1.CrossProduct(Edg2).SafeNormal();
		
		x = Normal.x;	y = Normal.y;	z = Normal.z;
		d = Normal.DotProduct(P1);
	}

	FLOAT GetDistance(const FVector& P) const
	{
		return x * P.x + y * P.y + z * P.z - d;
	}

	FVector GetNormal() const
	{
		return FVector(x, y, z);
	}

	FPlane Flip() const
	{
		return FPlane(-x, -y, -z, -d);
	}

	// 点到平面的距离	< 0 位于平面正面，>0反之，=0位于平面上
	FLOAT PlaneDot(const FVector& P) const
	{
		return x * P.x + y * P.y + z * P.z - d;
	}

	UBOOL operator== (const FPlane& P) const
	{
		return x == P.x && y == P.y && z == P.z && d == P.d;
	}

	UBOOL operator!= (const FPlane& P) const
	{
		return x != P.x || y != P.y || z != P.z || d != P.d;
	}

	UBOOL Equals(const FPlane& P, FLOAT Tolerance = KINDA_SMALL_NUMBER) const
	{
		return Abs(x - P.x) < Tolerance && Abs(y - P.y) < Tolerance && Abs(z - P.z) < Tolerance && Abs(d - P.d) < Tolerance;
	}

	FLOAT DotProduct(const FPlane& P) const
	{
		return x * P.x + y * P.y + z * P.z + d * P.d;
	}

	FPlane operator+ (const FPlane& P) const
	{
		return FPlane(x + P.x, y + P.y, z + P.z, d + P.d);
	}

	FPlane operator- (const FPlane& P) const
	{
		return FPlane(x - P.x, y - P.y, z - P.z, d - P.d);
	}

	FPlane operator* (FLOAT Scale) const
	{
		return FPlane(x * Scale, y * Scale, z * Scale, d * Scale);
	}

	friend FPlane operator*(FLOAT Scale, const FPlane& P)
	{
		return FPlane(P.x * Scale, P.y * Scale, P.z * Scale, P.d * Scale);
	}

	FPlane operator/ (FLOAT Scale) const
	{
		const FLOAT InvScale = 1.f / Scale;
		return FPlane(x * InvScale, y * InvScale, z * InvScale, d * InvScale);
	}

	FPlane operator* (const FPlane& P) const
	{
		return FPlane(x * P.x, y * P.y, z * P.z, d * P.d);
	}

	FPlane& operator+= (const FPlane& P)
	{
		x += P.x;	y += P.y;	z += P.z;	d += P.d;
		return *this;
	}

	FPlane& operator-= (const FPlane& P)
	{
		x -= P.x;	y -= P.y;	z -= P.z;	d -= P.d;
		return *this;
	}

	FPlane& operator*= (FLOAT Scale)
	{
		x *= Scale;	y *= Scale;	z *= Scale;	d *= Scale;
		return *this;
	}

	FPlane& operator*= (const FPlane& P)
	{
		x *= P.x;	y *= P.y;	z *= P.z;	d *= P.d;
		return *this;
	}

	FPlane& operator/= (FLOAT Scale)
	{
		const FLOAT InvScale = 1.f / Scale;
		x *= InvScale;	y *= InvScale;	z *= InvScale;	d *= InvScale;
		return *this;
	}

	FLOAT Normalize()
	{
		FLOAT Len = (x * x + y * y + z * z);
		if( Len > SMALL_NUMBER )
		{
			const FLOAT InvLen = 1.f / appSqrt(Len);
			x *= InvLen;	y *= InvLen;	z *= InvLen;	
			d *= InvLen;
		}
		else
		{
			x = 0.f;	y = 0.f;	z = 0.f;
			d = 0.f;
		}

		return appSqrt(Len);
	}

	FVector GetNearestPoint(const FVector& P) const
	{
		FLOAT Dist = GetDistance(P);

		return P + (-Dist) * FVector(x, y, z);
	}

	UBOOL GetIntersectPoint(FVector& OutP, const FVector& From, const FVector& To) const
	{
		// p(t) = From + t(To - From);
		FVector Normal(x, y, z);

		FLOAT Dot1 = Normal.DotProduct(To - From);
		if( Dot1 < KINDA_SMALL_NUMBER )
		{
			// parallel
			return FALSE;
		}

		FLOAT Dot2 = Normal.DotProduct(From);

		FLOAT T = ( d - Dot2 ) / Dot1;
		if( T < 0.f || T > 1.0f )
		{
			return FALSE;
		}

		OutP = From + T * (To - From);

		return TRUE;
	}

	// 目前只支持正交矩阵变换
	FPlane TransformPlaneByOrtho(const FMatrix& M) const;
	//FPlane TransformBy(const FMatrix& M) const;

	friend FArchive& operator<< (FArchive& Ar, FPlane& P)
	{
		Ar << (FVector&)P << P.d;
		return Ar;
	}

};

class FBox
{
public:
	FVector		Min;
	FVector		Max;
	UBOOL		IsValid;

public:
	FBox() { Init(); }
	FBox(const FVector& InMin, const FVector& InMax)
		: Min(InMin), Max(InMax), IsValid(TRUE)
	{
	}

	FBox(const FVector* Pts, INT Count);

	static FBox BuildAABB(const FVector& Origin, const FVector& Extent)
	{
		FBox NewBox;
		NewBox += Origin - Extent;
		NewBox += Origin + Extent;
		return NewBox;
	}

	void Init()
	{
		Min = Max = FVector::ZERO;
		IsValid = FALSE;
	}

	FVector& GetExtrema(INT i)
	{
		return (&Min)[i];
	}

	const FVector& GetExtrema(INT i) const
	{
		return (&Min)[i];
	}

	FBox& operator+= (const FVector& rhs)
	{
		if( IsValid )
		{
			Min.x = ::Min(Min.x, rhs.x);
			Min.y = ::Min(Min.y, rhs.y);
			Min.z = ::Min(Min.z, rhs.z);

			Max.x = ::Max(Max.x, rhs.x);
			Max.y = ::Max(Max.y, rhs.y);
			Max.z = ::Max(Max.z, rhs.z);
		}
		else
		{
			Min = Max = rhs;
			IsValid = TRUE;
		}

		return *this;
	}

	FBox operator+ (const FVector& rhs)
	{
		return FBox(*this) += rhs;
	}

	FBox& operator+= (const FBox& rhs)
	{
		if( IsValid && rhs.IsValid )
		{
			Min.x = ::Min(Min.x, rhs.Min.x);
			Min.y = ::Min(Min.y, rhs.Min.y);
			Min.z = ::Min(Min.z, rhs.Min.z);

			Max.x = ::Max(Max.x, rhs.Max.x);
			Max.y = ::Max(Max.y, rhs.Max.y);
			Max.z = ::Max(Max.z, rhs.Max.z);
		}
		else if( rhs.IsValid )
		{
			*this = rhs;
		}
		return *this;
	}

	FBox operator+ (const FBox& rhs) const
	{
		return FBox(*this) += rhs;
	}

	FVector& operator[] (INT i)
	{
		check( i > -1 && i < 2 );
		if( i == 0 )
			return Min;
		else
			return Max;
	}

	FBox TransformBy(const FMatrix& M) const;

	FBox TransformProjectBy(const FMatrix& ProjM) const;

	FBox ExpandBy(FLOAT Ex) const
	{
		return FBox(Min - FVector(Ex, Ex, Ex), Max + FVector(Ex, Ex, Ex));
	}

	FVector GetCenter() const
	{
		return (Min + Max) * 0.5f;
	}

	FVector GetExtent() const
	{
		return (Max - Min) * 0.5f;
	}

	void GetCenterAndExtent(FVector& Center, FVector& Extent) const
	{
		Center = GetCenter();
		Extent = GetExtent();
	}

	UBOOL Intersect(const FBox& Other) const
	{
		if( Min.x > Other.Max.x || Other.Min.x > Max.x )
			return FALSE;

		if( Min.y > Other.Max.y || Other.Min.y > Max.y )
			return FALSE;

		if( Min.z > Other.Max.z || Other.Min.z > Max.z )
			return FALSE;

		return TRUE;
	}

	UBOOL IntersectXY(const FBox& Other) const
	{
		if( Min.x > Other.Max.x || Other.Min.x > Max.x )
			return FALSE;

		if( Min.y > Other.Max.y || Other.Min.y > Max.y )
			return FALSE;

		return TRUE;
	}

	UBOOL IsInside(const FVector& In) const
	{
		return (In.x > Min.x && In.x < Max.x
			&& In.y > Min.y && In.y < Max.y
			&& In.z > Min.z && In.z < Max.z);
	}

	FLOAT GetVolume() const
	{
		return (Max.x - Min.x) * (Max.y - Min.y) * (Max.z - Min.z);
	}

	friend FArchive& operator<< (FArchive& Ar, FBox& B)
	{
		Ar << B.Min << B.Max << B.IsValid;
		return Ar;
	}

};

class FSphere : public FPlane
{
public:
	FSphere()
		: FPlane(0.f, 0.f, 0.f, 0.f)
	{
	}

	FSphere(const FVector& Origin, FLOAT Radius)
		: FPlane(Origin, Radius)
	{
	}

	FSphere(const FVector* Pts, INT Count);

	FSphere TransformBy(const FMatrix& M) const;

	friend FArchive& operator<< (FArchive& Ar, FSphere& S)
	{
		Ar << (FPlane&)S;
		return Ar;
	}
};

class FBoxSphereBounds
{
public:
	FVector		Origin;
	FVector		BoxExtent;
	FLOAT		SphereRadius;

public:
	FBoxSphereBounds() {}
	FBoxSphereBounds(const FVector& InOrigin, const FVector& InBoxExtent, FLOAT InSphereRadius) :
		Origin(InOrigin), BoxExtent(InBoxExtent), SphereRadius(InSphereRadius)
	{}

	FBoxSphereBounds(const FBox& InBox, const FSphere& InSphere)
	{
		InBox.GetCenterAndExtent(Origin, BoxExtent);
		SphereRadius = Min( BoxExtent.Size(), ((FVector)InSphere - Origin).Size() + InSphere.d );
	}

	FBoxSphereBounds(const FBox& InBox)
	{
		InBox.GetCenterAndExtent(Origin, BoxExtent);
		SphereRadius = BoxExtent.Size();
	}

	FBoxSphereBounds(const FVector* Pts, INT Count)
	{
		FBox Box(Pts, Count);
		Box.GetCenterAndExtent(Origin, BoxExtent);

		SphereRadius = 0.f;
		for( INT i = 0; i < Count; ++i )
		{
			SphereRadius = Max(SphereRadius, (Pts[i] - Origin).Size());
		}
	}

	FVector GetBoxExtrema(UBOOL bMin) const
	{
		if( bMin )
			return Origin - BoxExtent;
		else
			return Origin + BoxExtent;
	}

	FBox GetBox() const
	{
		return FBox(Origin - BoxExtent, Origin + BoxExtent);
	}

	FSphere GetSphere() const
	{
		return FSphere(Origin, SphereRadius);
	}

	FBoxSphereBounds TransformBy(const FMatrix& M) const;

	friend FBoxSphereBounds LegacyUnion(const FBoxSphereBounds& A, const FBoxSphereBounds& B)
	{
		FBox BoundingBox;
		BoundingBox += ( A.Origin - A.BoxExtent );
		BoundingBox += ( A.Origin + A.BoxExtent );
		BoundingBox += ( B.Origin - B.BoxExtent );
		BoundingBox += ( B.Origin + B.BoxExtent );

		FBoxSphereBounds Result(BoundingBox);
		Result.SphereRadius = Min(Result.SphereRadius, 
									Max( 
										(A.Origin - Result.Origin).Size() + A.SphereRadius, 
										(B.Origin - Result.Origin).Size() + B.SphereRadius
										)
								  );

		return Result;
	}

	FBoxSphereBounds operator+= (const FBoxSphereBounds& B) const
	{
		FBox BoundingBox;
		BoundingBox += ( this->Origin - this->BoxExtent );
		BoundingBox += ( this->Origin + this->BoxExtent );
		BoundingBox += ( B.Origin - B.BoxExtent );
		BoundingBox += ( B.Origin + B.BoxExtent );

		FBoxSphereBounds Result(BoundingBox);
		Result.SphereRadius = Min( Result.SphereRadius,
									Max(
									(this->Origin - Result.Origin).Size() + this->SphereRadius,
									(B.Origin - Result.Origin).Size() + B.SphereRadius
									)
								  );

		return Result;
	}

	friend FArchive& operator<< (FArchive& Ar, FBoxSphereBounds& Bounds)
	{
		return Ar << Bounds.Origin << Bounds.BoxExtent << Bounds.SphereRadius;
	}
};

class FQuat
{
public:
	FLOAT x, y, z, w;

public:
	FQuat()
		: x(0.f), y(0.f), z(0.f), w(1.f)
	{}

	FQuat(FLOAT InX, FLOAT InY, FLOAT InZ, FLOAT InW)
		: x(InX), y(InY), z(InZ), w(InW)
	{
	}

	FQuat(const FQuat& Q)
	{
		x = Q.x;
		y = Q.y;
		z = Q.z;
		w = Q.w;
	}

	FQuat& operator= (const FQuat& Q)
	{
		x = Q.x;
		y = Q.y;
		z = Q.z;
		w = Q.w;
		return *this;
	}

	FQuat(const FRotator& Rotator)
	{
		*this = MakeFromRotator(Rotator);
	}

	FQuat(const FMatrix& M);

	FQuat(const FVector& Axis, FLOAT Angle)
	{
		FromAxisAngle(Axis, Angle);
	}

	FQuat(const FVector& AxisX, const FVector& AxisY, const FVector& AxisZ)
	{
		FromBasicAxis(AxisX, AxisY, AxisZ);
	}

	FQuat(const FVector& From, const FVector& To)
	{
		const FVector Cross = From.CrossProduct(To);
		const FLOAT CrossMag = Cross.Size();

		if( CrossMag < KINDA_SMALL_NUMBER )
		{
			const FLOAT Dot = From.DotProduct(To);
			if( Dot > -KINDA_SMALL_NUMBER )
			{
				x = 0.0f;	y = 0.0f;	z = 0.0f;	w = 1.0f;		// no rotation
			}
			else
			{
				// anti-parallel
				FVector Vec = From.SizeSquared() > To.SizeSquared() ? From : To;
				Vec.Normalize();

				FVector AxisA, AxisB;
				Vec.FindBestAxisVectors(AxisA, AxisB);

				x = AxisA.x;	y = AxisA.y;	z = AxisA.z;	w = 0.0f;
			}
		}

		FLOAT Angle = appAsin(CrossMag);

		const FLOAT Dot = From.DotProduct(To);
		if( Dot < 0.0f )
		{
			Angle = PI - Angle;
		}
		FromAxisAngle(Cross, Angle);
	}

	FLOAT operator[] (INT i) const { return (&x)[i]; }
	FLOAT& operator[] (INT i) { return (&x)[i]; }
	operator const FLOAT* () const { return &x; }
	operator FLOAT* () { return &x; }

	void FromAxisAngle(const FVector& Axis, FLOAT Angle)
	{
		const FLOAT Norm = Length();
		if( Norm < KINDA_SMALL_NUMBER )
		{
			x = 0.f;	y = 0.f;	z = 0.f;	w = 1.f;
		}
		else
		{
			FVector AxisNorm = Axis;
			AxisNorm.Normalize();
			const FLOAT Half_Angle = 0.5f * Angle;
			const FLOAT S = appSin(Half_Angle);
			const FLOAT C = appCos(Half_Angle);

			x = S * AxisNorm.x;	y = S * AxisNorm.y;	z = S * AxisNorm.z;
			w = C;
		}
	}

	// warning: must be normalized quaternion
	void ToAxisAngle(FVector& Axis, FLOAT& Angle)
	{
		Angle = 2.0f * appAcos(w);

		const FLOAT s = appSqrt( Max(1.f - w*w, 0.f) );
		if( s > KINDA_SMALL_NUMBER )
		{
			Axis.x = x / s;
			Axis.y = y / s;
			Axis.z = z / s;
		}
		else
		{
			Axis.x = 1.0f;
			Axis.y = 0.0f;
			Axis.z = 0.0f;
		}

		//Angle = 2.f * appAcos(w);
		//Axis = FVector(x, y, z);
		//const FLOAT Norm = Axis.Size();
		//
		//if( Norm > KINDA_SMALL_NUMBER )
		//{
		//	Axis /= Norm;
		//}
		//else
		//{
		//	Angle = 1.f;
		//}

		//if( Angle > PI )
		//{
		//	Angle = PI - Angle;
		//	Axis = -Axis;
		//}
	}

	void FromBasicAxis(const FVector& AxisX, const FVector& AxisY, const FVector& AxisZ);

	void FromRotationMatrix(const FMatrix& M);
	void ToRotationMatrix(FMatrix& M) const;

	FLOAT Length() const
	{
		return appSqrt(x * x + y * y + z * z + w * w);
	}

	FQuat operator+ (const FQuat& Q) const
	{
		return FQuat(x + Q.x, y + Q.y, z + Q.z, w + Q.w);
	}

	FQuat operator- (const FQuat& Q) const
	{
		return FQuat(x - Q.x, y - Q.y, z - Q.z, w - Q.w);
	}

	//a*b*v as if whent rotate the vector v, b was applied first and then a was applied
	// q1*q2表示先以q1，后以q2进行旋转
	FQuat operator* (const FQuat& Q) const
	{
		return FQuat(
			w*Q.x + x*Q.w + y*Q.z - z*Q.y,
			w*Q.y - x*Q.z + y*Q.w + z*Q.x,
			w*Q.z + x*Q.y - y*Q.x + z*Q.w,
			w*Q.w - x*Q.x - y*Q.y - z*Q.z
			);
	}

	FQuat operator* (FLOAT Scale) const
	{
		return FQuat(x * Scale, y * Scale, z * Scale, w * Scale);
	}

	FVector RotateVector(const FVector& V) const
	{
		//FVector uv, uuv;
		//FVector qvec(x, y, z);
		//uv = qvec.CrossProduct(V);
		//uuv = qvec.CrossProduct(uv);
		//uv *= (2.0f * w);
		//uuv *= 2.0f;

		//return V + uv + uuv;


		// (q.W*q.W-qv.qv)v + 2(qv.v)qv + 2 q.W (qv x v)
		const FVector qv(x, y, z);
		FVector vOut = 2.f * w * (qv.CrossProduct(V));
		vOut += ((w * w) - (qv.DotProduct(qv))) * V;
		vOut += (2.f * (qv.DotProduct(V))) * qv;

		return vOut;
	}

	FVector InverseRotateVector(const FVector& V) const
	{
		return (-*this).RotateVector(V);
	}

	FQuat operator- () const
	{
		return FQuat(-x, -y, -z, w);
	}

	UBOOL operator != (const FQuat& Q) const
	{
		return x != Q.x || y != Q.y || z != Q.z || w != Q.w;
	}

	FORCEINLINE FQuat Inverse() const
	{
		check(IsNormalized());

		return FQuat(-x, -y, -z, w);
	}

	FLOAT DotProduct(const FQuat& Q) const
	{
		return x * Q.x + y * Q.y + z * Q.z + w * Q.w;
	}

	UBOOL Normalize()
	{
		const FLOAT SquaredNorm = x * x + y * y + z * z + w * w;
		if( SquaredNorm > DELTA )
		{
			if( Abs(SquaredNorm - 1.f) >= DELTA )
			{
				const FLOAT Scale = 1.f / appSqrt(SquaredNorm);
				x *= Scale;
				y *= Scale;
				z *= Scale;
				w *= Scale;
				return TRUE;
			}
			return FALSE;
		}
		else
		{
			x = 0.f;	y = 0.f;	z = 0.f;	w = 1.f;
			return FALSE;
		}
	}

	UBOOL IsNormalized() const
	{
		return Abs(1.0f - SizeSquared()) <= 0.01f;
	}

	FLOAT SizeSquared() const
	{
		return x * x + y * y + z * z + w * w;
	}

	FORCEINLINE FVector GetAxisX() const
	{
		return RotateVector(FVector(1.f, 0.f, 0.f));
	}

	FORCEINLINE FVector GetAxisY() const
	{
		return RotateVector(FVector(0.f, 1.f, 0.f));
	}

	FORCEINLINE FVector GetAxisZ() const
	{
		return RotateVector(FVector(0.f, 0.f, 1.f));
	}

	FQuat Log() const;
	FQuat Exp() const;

	FRotator Rotator() const;

	static FQuat MakeFromRotator(const FRotator& Rotator);

	static FQuat MakeFromEuler(const FVector& Euler);

	static FQuat SlerpQuat(const FQuat& q1, const FQuat& q2, FLOAT alpha, UBOOL bShortestPath);

	static FQuat LerpQuat(const FQuat& q1, const FQuat& q2, FLOAT alpha);

	static FQuat GenerateRandomQuat()
	{
		appRandInit(time(NULL));
		FLOAT Seed = appFrand();
		FLOAT R1 = appSqrt( 1.f - Seed );
		FLOAT R2 = appSqrt( Seed );
		FLOAT T1 = 2.f * PI * appFrand();
		FLOAT T2 = 2.f * PI * appFrand();

		return FQuat(appSin(T1) * R1, appCos(T1) * R1, appSin(T2) * R2, appCos(T2) * R2);
	}

	friend FArchive& operator<< (FArchive& Ar, FQuat& Q)
	{
		Ar << Q.x << Q.y << Q.z << Q.w;
		return Ar;
	}

	static const FQuat Identity;
};

/**
 *	return the shortest rotation between these two vectors
 */
FQuat FQuatFindBetween(const FVector& V1, const FVector& V2);


///////////////////////////////

//	  Z(Yaw) X(Roll)
//		|    /
//		|   /
//		|  /
//		| /
//		|/_________Y(Pitch)

//////////////////////////////
class FRotator
{
public:
	INT Pitch;			// 从正方向看逆时针旋转为正
	INT Yaw;			// 同上  Rotating around (running in circles), 0=East, +North, -South.
	INT Roll;			// 同上  Rotation about axis of screen, 0=Straight, +Clockwise, -CCW.

public:
	FRotator() {}
	FRotator(INT InPitch, INT InYaw, INT InRoll)
		: Pitch(InPitch), Yaw(InYaw), Roll(InRoll)
	{}

	FRotator(const FQuat& Quat);

	FRotator operator+ (const FRotator& Rot) const
	{
		return FRotator(Pitch + Rot.Pitch, Yaw + Rot.Yaw, Roll + Rot.Roll);
	}

	FRotator operator- (const FRotator& Rot) const
	{
		return FRotator(Pitch - Rot.Pitch, Yaw - Rot.Yaw, Roll - Rot.Roll);
	}

	UBOOL operator== (const FRotator& Rot) const
	{
		return Pitch == Rot.Pitch && Yaw == Rot.Yaw && Roll == Rot.Roll;
	}

	UBOOL operator!= (const FRotator& Rot) const
	{
		return !(*this == Rot);
	}

	FRotator operator* (FLOAT Scale) const
	{
		return FRotator( (INT)(Pitch * Scale), (INT)(Yaw * Scale), (INT)(Roll * Scale) );
	}

	friend FRotator operator* (FLOAT Scale, const FRotator& R)
	{
		return R.operator*(Scale);
	}

	FRotator& operator*= (FLOAT Scale)
	{
		Pitch = (INT)(Pitch * Scale);	Yaw = (INT)(Yaw * Scale);	Roll = (INT)(Roll * Scale);
		return *this;
	}

	FRotator& operator+= (const FRotator& Rot)
	{
		Pitch += Rot.Pitch; Yaw += Rot.Yaw; Roll += Rot.Roll;
		return *this;
	}

	FRotator& operator-= (const FRotator& Rot)
	{
		Pitch -= Rot.Pitch; Yaw -= Rot.Yaw; Roll -= Rot.Roll;
		return *this;
	}

	UBOOL IsZero() const
	{
		return ( Pitch & 65535 ) == 0 && ( Yaw & 65535 ) == 0 && ( Roll & 65535 ) == 0 ;
	}

	FVector Vector() const;
	FQuat Quaternion() const;
	FVector Euler() const;

	static FRotator MakeFromEuler(const FVector& Euler);

	FRotator Clamp() const
	{
		return FRotator(Pitch&65535, Yaw&65535, Roll&65535);
	}

	static INT NormalizeAxis(INT Angle)
	{
		Angle &= 0xFFFF;
		if( Angle > 32767 )
		{
			Angle -= 0x10000;
		}
		return Angle;
	}
	
	// fall within the range -32768 ~ 32768
	FRotator Normalize() const
	{
		FRotator Rot = *this;
		Rot.Pitch = NormalizeAxis(Rot.Pitch);
		Rot.Yaw = NormalizeAxis(Rot.Yaw);
		Rot.Roll = NormalizeAxis(Rot.Roll);
		return Rot;
	}

	// fall within the range 0 ~ 65535
	FRotator Denormalize() const
	{
		FRotator Rot = *this;
		Rot.Pitch = Rot.Pitch & 0xFFFF;
		Rot.Yaw = Rot.Yaw & 0xFFFF;
		Rot.Roll = Rot.Roll & 0xFFFF;
		return Rot;
	}

	void MakeShortestRoute()
	{
		Yaw = Yaw & 65535;
		if( Yaw > 32768 )
		{
			Yaw -= 65535;
		}
		else if( Yaw < -32768 )
		{
			Yaw += 65535;
		}

		Pitch = Pitch & 65535;
		if( Pitch > 32768 )
		{
			Pitch -= 65535;
		}
		else if( Pitch < -32768 )
		{
			Pitch += 65535;
		}

		Roll = Roll & 65535;
		if( Roll > 32768 )
		{
			Roll -= 65535;
		}
		else if( Roll < -32768 )
		{
			Roll += 65535;
		}
	}

	FString ToString() const
	{
		return FStringUtil::Sprintf(TEXT("Pitch = %i Yaw = %i Roll = %i"), Pitch, Yaw, Roll);
	}

	friend FArchive& operator<<(FArchive& Ar, FRotator& Rot)
	{
		return Ar << Rot.Pitch << Rot.Yaw << Rot.Roll;
	}

};

// Point of View
struct FTPOV
{
	FVector		Location;
	FRotator	Rotation;
	FLOAT		Fov;

	FTPOV() {}
	FTPOV(const FVector& InLocation, const FRotator& InRotation, FLOAT InFov)
		: Location(InLocation), Rotation(InRotation), Fov(InFov)
	{}

	friend FArchive& operator<<(FArchive& Ar, FTPOV& POV)
	{
		return Ar << POV.Location << POV.Rotation << POV.Fov;
	}
};

class FGlobalMath
{
public:
	enum {NUM_ANGLES = 16384};

	FGlobalMath()
	{
		for(INT i = 0; i < NUM_ANGLES; ++i)
		{
			TrigTab[i] = appSin(i * 2.0f * PI / (FLOAT)NUM_ANGLES);
		}
	}

	FLOAT SinTab(INT i) const
	{
		return TrigTab[ (i >> 2) & (NUM_ANGLES - 1) ];
	}

	FLOAT CosTab(INT i) const
	{
		return TrigTab[ ((i + 16384) >> 2) & (NUM_ANGLES - 1) ];
	}

	FLOAT SinFLOAT(FLOAT V) const
	{
		return SinTab((INT)((V * 65536.0f) / (2.0f * PI)));
	}

	FLOAT CosFLOAT(FLOAT V) const
	{
		return CosTab((INT)((V * 65536.0f) / (2.0f * PI)));
	}

private:
	FLOAT TrigTab	[NUM_ANGLES];
};

// in radians , fall within the range [-PI, PI]
inline FLOAT FindDeltaAngle(FLOAT A1, FLOAT A2)
{
	FLOAT Delta = A2 - A1;

	if( Delta > PI )
	{
		Delta = Delta - (2.0f * PI);
	}
	else if ( Delta < -PI )
	{
		Delta = Delta + (2.0f * PI);
	}

	return Delta;
}

#define FLOAT_NORMAL_THRESH				(0.0001f)
#define THRESH_POINT_ON_PLANE			(0.1f)
#define THRESH_POINT_ON_SIDE			(0.2f)
#define THRESH_POINTS_ARE_SAME			(0.002f)
#define THRESH_POINTS_ARE_NEAR			(0.015f)
#define THRESH_NORMALS_ARE_SAME			(0.00002f)
#define THRESH_VECTORS_ARE_NEAR			(0.0004f)

inline UBOOL FPointAreSame(const FVector& P1, const FVector& P2)
{
	FLOAT Temp;
	Temp = P1.x - P2.x;
	if( Abs(Temp) < THRESH_POINTS_ARE_SAME )
	{
		Temp = P1.y - P2.y;
		if( Abs(Temp) < THRESH_POINTS_ARE_SAME )
		{
			Temp = P1.z - P2.z;
			if( Abs(Temp) < THRESH_POINTS_ARE_SAME )
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

inline UBOOL FPointAreNear(const FVector& P1, const FVector& P2, FLOAT Dist)
{
	FLOAT Temp;
	Temp = P1.x - P2.x;
	if( Abs(Temp) >= Dist )
		return FALSE;

	Temp = P1.y - P2.y;
	if( Abs(Temp) >= Dist )
		return FALSE;

	Temp = P1.z - P2.z;
	if( Abs(Temp) >= Dist )
		return FALSE;

	return TRUE;
}

/**
 *	calculate the projection of a point on the plane defined by CCW points A, B, C
 */
inline FVector FPointPlaneProject(const FVector& Point, const FVector& A, const FVector& B, const FVector& C)
{
	FPlane Plane(A, B, C);
	return Point - Plane.PlaneDot(Point) * Plane.GetNormal();
}

// asume PlaneNormal is normalized
inline FLOAT FPointPlaneDist(const FVector& Point, const FVector& PlaneBase, const FVector& PlaneNormal)
{
	return (Point - PlaneBase).DotProduct(PlaneNormal);
}

inline FLOAT FDist(const FVector& V1, const FVector& V2)
{
	return appSqrt(Square(V1.x - V2.x) + Square(V1.y - V2.y) + Square(V1.z - V2.z));
}

inline FLOAT FDistSquared(const FVector& V1, const FVector& V2)
{
	return Square(V1.x - V2.x) + Square(V1.y - V2.y) + Square(V1.z - V2.z);
}

inline UBOOL FParallel(const FVector& N1, const FVector& N2)
{
	const FLOAT NDot = N1.DotProduct(N2);
	return Abs(NDot - 1.0f) <= THRESH_VECTORS_ARE_PARALLEL;
}

inline UBOOL FCoplanar(const FVector& Base1, const FVector& Normal1, const FVector& Base2, const FVector& Normal2)
{
	if( !FParallel(Normal1, Normal2) )
		return FALSE;
	else if( FPointPlaneDist(Base1, Base2, Normal2) > THRESH_POINT_ON_PLANE )
		return FALSE;
	else
		return TRUE;
}

// 线段与平面的交点
inline UBOOL FLinePlaneIntersection(FVector& OutPoint, const FVector& Start, const FVector& End, const FVector& PlaneBase, const FVector& PlaneNormal)
{
	const FPlane Plane(PlaneNormal, PlaneBase);

	return Plane.GetIntersectPoint(OutPoint, Start, End);
}

inline UBOOL FLinePlaneIntersection(FVector& OutPoint, const FVector& Start, const FVector& End, const FPlane& Plane)
{
	return Plane.GetIntersectPoint(OutPoint, Start, End);
}

// 检测点是否在Box内
inline UBOOL FPointBoxIntersection(const FVector& Point, const FBox& Box)
{
	if( Point.x >= Box.Min.x && Point.x <= Box.Max.x &&
		Point.y >= Box.Min.y && Point.y <= Box.Max.y &&
		Point.z >= Box.Min.z && Point.z <= Box.Max.z )
		return TRUE;
	else
		return FALSE;
}

// 检测线段与Box的交点
inline UBOOL FLineBoxIntersection
(
	const FBox& Box, 
	const FVector& Start,
	const FVector& End, 
	const FVector& Direction, 
	const FVector& OneOverDirection
)
{
	FVector	Time;
	UBOOL	bStartIsOutside = FALSE;

	if(Start.x < Box.Min.x)
	{
		bStartIsOutside = TRUE;
		if(End.x >= Box.Min.x)
		{
			Time.x = (Box.Min.x - Start.x) * OneOverDirection.x;
		}
		else
		{
			return FALSE;
		}
	}
	else if(Start.x > Box.Max.x)
	{
		bStartIsOutside = TRUE;
		if(End.x <= Box.Max.x)
		{
			Time.x = (Box.Max.x - Start.x) * OneOverDirection.x;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		Time.x = 0.0f;
	}

	if(Start.y < Box.Min.y)
	{
		bStartIsOutside = TRUE;
		if(End.y >= Box.Min.y)
		{
			Time.y = (Box.Min.y - Start.y) * OneOverDirection.y;
		}
		else
		{
			return FALSE;
		}
	}
	else if(Start.y > Box.Max.y)
	{
		bStartIsOutside = TRUE;
		if(End.y <= Box.Max.y)
		{
			Time.y = (Box.Max.y - Start.y) * OneOverDirection.y;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		Time.y = 0.0f;
	}

	if(Start.z < Box.Min.z)
	{
		bStartIsOutside = TRUE;
		if(End.z >= Box.Min.z)
		{
			Time.z = (Box.Min.z - Start.z) * OneOverDirection.z;
		}
		else
		{
			return FALSE;
		}
	}
	else if(Start.z > Box.Max.z)
	{
		bStartIsOutside = TRUE;
		if(End.z <= Box.Max.z)
		{
			Time.z = (Box.Max.z - Start.z) * OneOverDirection.z;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		Time.z = 0.0f;
	}

	if(bStartIsOutside)
	{
		const FLOAT	MaxTime = Max(Time.x,Max(Time.y,Time.z));

		if(MaxTime >= 0.0f && MaxTime <= 1.0f)
		{
			const FVector Hit = Start + Direction * MaxTime;
			const FLOAT BOX_SIDE_THRESHOLD = 0.1f;
			if(	Hit.x > Box.Min.x - BOX_SIDE_THRESHOLD && Hit.x < Box.Max.x + BOX_SIDE_THRESHOLD &&
				Hit.y > Box.Min.y - BOX_SIDE_THRESHOLD && Hit.y < Box.Max.y + BOX_SIDE_THRESHOLD &&
				Hit.z > Box.Min.z - BOX_SIDE_THRESHOLD && Hit.z < Box.Max.z + BOX_SIDE_THRESHOLD)
			{
				return TRUE;
			}
		}

		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

inline UBOOL FLineSphereIntersection(const FVector& Start, const FVector& Dir, const FVector& Origin, FLOAT Radius)
{
	const FVector	EO = Start - Origin;
	const FLOAT		v = (Dir.DotProduct( (Origin - Start) ));
	const FLOAT		disc = Radius * Radius - ((EO.DotProduct(EO)) - v * v);

	if( disc >= 0 )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

UBOOL FLineExtentBoxIntersection
(
	const FBox& Box,
	const FVector& Start,
	const FVector& End,
	const FVector& Extent,
	FVector& HitLocation,
	FVector& HitNormal,
	FLOAT& HitTime
);


inline UBOOL MakeFrustumPlanes(FLOAT A, FLOAT B, FLOAT C, FLOAT D, FPlane& OutPlane)
{
	const FLOAT LengthSquared = A * A + B * B + C * C;
	if( LengthSquared > DELTA * DELTA )
	{
		const FLOAT InvLength = 1.f / appSqrt(LengthSquared);
		OutPlane = FPlane(-A * InvLength, -B * InvLength, -C * InvLength, D * InvLength);
		return TRUE;
	}
	else
		return FALSE;
}

class FMatrix
{
public:
	FLOAT M[4][4];

public:
	FMatrix();
	FMatrix(const FPlane& InX, const FPlane& InY, const FPlane& InZ, const FPlane& InW);
	FMatrix(const FVector& InX, const FVector& InY, const FVector& InZ, const FVector& InW);
	FMatrix(const FMatrix& Other);
	FMatrix& operator= (const FMatrix& Other);

	void SetIdentity();

	FMatrix operator+ (const FMatrix& Other) const;
	FMatrix operator- (const FMatrix& Other) const;
	FMatrix operator* (const FMatrix& Other) const;
	FMatrix& operator*= (const FMatrix& Other);
	FMatrix operator- () const;

	/**
	 *	just multiplying float to all members
	 */
	FMatrix operator* (FLOAT Other) const;
	FMatrix& operator*= (FLOAT Other);

	UBOOL operator== (const FMatrix& Other) const;
	UBOOL operator!= (const FMatrix& Other) const
	{
		return !(*this == Other);
	}

	UBOOL Equals(const FMatrix& Other, FLOAT Tolerance = KINDA_SMALL_NUMBER) const;

	// homogeneous transform
	FVector4 TransformVector4(const FVector4& V) const;

	// regular transform

	/**
	 *	take into account translation part of the FMatrix
	 */
	FVector4 TransformVector(const FVector& V) const;
	FVector InverseTransformVector(const FVector& V) const;
	FVector InverseTransformVectorNoScale(const FVector& V) const;

	// normal transform
	FVector4 TransformNormal(const FVector& V) const;
	FVector InverseTransformNormal(const FVector& V) const;
	FVector InverseTransformNormalNoScale(const FVector& V) const;

	inline FLOAT Determinant() const;
	inline FLOAT RotDeterminant() const;
	inline FMatrix Transpose() const;
	inline FMatrix Inverse() const;
	inline FMatrix InverseSafe() const;

	inline void RemoveScale(FLOAT Tolerance = SMALL_NUMBER);
	inline void RemoveTranslation();

	inline FMatrix ConcatTranslation(const FVector& Translation) const;

	inline UBOOL ContainsNaN() const;

	inline void ScaleTranslation(const FVector& Scale3D);
	inline FMatrix ApplyScale(FLOAT Scale) const;

	inline FVector GetOrigin() const;
	inline FVector GetAxis(INT i) const;
	inline void GetAxes(FVector& X, FVector& Y, FVector& Z) const;

	inline void SetOrigin(const FVector& NewOrigin);
	inline void SetAxis(INT i, const FVector& Axis);
	inline void SetAxes(const FVector& X, const FVector& Y, const FVector& Z);
	inline FVector GetColumn(INT i) const;

	FRotator Rotator() const;

	void MakeTransform(const FVector& Scale, const FQuat& Rotation, const FVector& Trans);

	UBOOL GetFrustumNearPlane(FPlane& OutPlane) const;
	UBOOL GetFrustumFarPlane(FPlane& OutPlane) const;
	UBOOL GetFrustumLeftPlane(FPlane& OutPlane) const;
	UBOOL GetFrustumRightPlane(FPlane& OutPlane) const;
	UBOOL GetFrustumTopPlane(FPlane& OutPlane) const;
	UBOOL GetFrustumBottomPlane(FPlane& OutPlane) const;

	friend FArchive& operator<< (FArchive& Ar, FMatrix& M)
	{
		Ar << M.M[0][0] << M.M[0][1] << M.M[0][2] << M.M[0][3];
		Ar << M.M[1][0] << M.M[1][1] << M.M[1][2] << M.M[1][3];
		Ar << M.M[2][0] << M.M[2][1] << M.M[2][2] << M.M[2][3];
		Ar << M.M[3][0] << M.M[3][1] << M.M[3][2] << M.M[3][3];
		return Ar;
	}

	static const FMatrix Identity;
};

/**
  *		投影模型
		* Y
		|	  /|
		|	 / |
		|   /  |
		|  /   |
		| /    |
--------|------|-----------------> Z
		| \	   |
		|  \   |
        |   \  |
		|	 \ |
		|	  \|
*/

class FPerspectiveMatrix : public FMatrix
{
public:
	/**
	 *	@param	fFovy		half field of view in the y direction, in radians
	 *	@param	Aspect		aspect ratio, defined as view space width divided by height(w/h)
	 *	@param	ZNear		Z-value of the near view-plane
	 *	@param	ZFar		Z-value of the far view-plane
	 */
	FPerspectiveMatrix(FLOAT fFovy, FLOAT Aspect, FLOAT ZNear, FLOAT ZFar) :
		FMatrix(
			FPlane( 1.f / appTan(fFovy) / Aspect,	0.f,						0.f,							0.f ),
			FPlane( 0.f,							1.f / appTan(fFovy),		0.f,							0.f ),
			FPlane( 0.f,							0.f,						ZFar / (ZFar - ZNear),			1.f ),
			FPlane( 0.f,							0.f,						-ZNear * ZFar / (ZFar - ZNear), 0.f ))
		{
		}

	FPerspectiveMatrix(FLOAT fFovy, FLOAT fWidth, FLOAT fHeight, FLOAT ZNear, FLOAT ZFar) :
		FMatrix(
			FPlane( fHeight / appTan(fFovy) / fWidth,	0.f,								0.f,								0.f ),
			FPlane( 0.f,								1.f / appTan(fFovy),				0.0f,								0.f ),
			FPlane( 0.f,								0.f,								ZFar / (ZFar - ZNear),				1.f ),
			FPlane( 0.f,								0.f,								-ZNear * ZFar / (ZFar - ZNear),		0.f ))
	{
	}
#define Z_PRECISION		(0.001f)

	FPerspectiveMatrix(FLOAT fFovy, FLOAT Aspect, FLOAT ZNear) :
		FMatrix(
			FPlane( 1.f / appTan(fFovy) / Aspect,	0.f,						0.f,							0.f ),
			FPlane( 0.f,							1 / appTan(fFovy),			0.f,							0.f ),
			FPlane( 0.f,							0.f,						(1.f - Z_PRECISION),			1.f ),
			FPlane( 0.f,							0.f,						-ZNear * (1.f - Z_PRECISION),	0.f ))
		{
		}
};

class FOrthoMatrix : public FMatrix
{
public:
	FOrthoMatrix(FLOAT Width, FLOAT Height, FLOAT ZNear, FLOAT ZFar) :
	  FMatrix(
		  FPlane( 2.f / Width,	0.f,			0.f,					0.f ),
		  FPlane( 0.f,			2.f / Height,	0.f,					0.f ),
		  FPlane( 0.f,			0.f,			1.f / (ZFar - ZNear),	0.f ),
		  FPlane( 0.f,			0.f,			ZNear / (ZNear - ZFar), 1.f ))
	  {
	  }
};

class FTranslationMatrix : public FMatrix
{
public:

	FTranslationMatrix(const FVector& Delta) :
		FMatrix(
			FPlane(1.0f,	0.0f,	0.0f,	0.0f),
			FPlane(0.0f,	1.0f,	0.0f,	0.0f),
			FPlane(0.0f,	0.0f,	1.0f,	0.0f),
			FPlane(Delta.x,	Delta.y,Delta.z,1.0f))
	{
	}
};

class FRotationMatrix : public FMatrix
{
public:
	FRotationMatrix(const FQuat& Rot)
	{
		const FLOAT q00 = 2.0f * Rot.x * Rot.x;
		const FLOAT q11 = 2.0f * Rot.y * Rot.y;
		const FLOAT q22 = 2.0f * Rot.z * Rot.z;

		const FLOAT q01 = 2.0f * Rot.x * Rot.y;
		const FLOAT q02 = 2.0f * Rot.x * Rot.z;
		const FLOAT q03 = 2.0f * Rot.x * Rot.w;

		const FLOAT q12 = 2.0f * Rot.y * Rot.z;
		const FLOAT q13 = 2.0f * Rot.y * Rot.w;

		const FLOAT q23 = 2.0f * Rot.z * Rot.w;

		M[0][0] = 1.0f - q11 - q22;
		M[0][1] =        q01 + q23;
		M[0][2] =        q02 - q13;
		M[0][3] =			   0.f;

		M[1][0] =        q01 - q23;
		M[1][1] = 1.0f - q22 - q00;
		M[1][2] =        q12 + q03;
		M[1][3] =			   0.f;

		M[2][0] =        q02 + q13;
		M[2][1] =        q12 - q03;
		M[2][2] = 1.0f - q11 - q00;
		M[2][3] =			   0.f;

		M[3][0] =			   0.f;
		M[3][1] =			   0.f;
		M[3][2] =			   0.f;
		M[3][3] =			   1.f;
	}

	/*
		rotate order: Roll -> Pitch -> Yaw

				|     1		  0		  0	|
		Roll =	|     0	   cosR   -sinR |
				|     0	   sinR	   cosR |

				|  cosP		  0	   sinP |
		Pitch = |	  0		  1	      0 |
				| -sinP		  0	   cosP |

				|  cosY	  -sinY		  0 |
		Yaw =	|  sinY	   cosY		  0 |
				|     0		  0		  1 |
		注：逆时针旋转为正方向
	*/
	FRotationMatrix(const FRotator& Rot)
	{
		const FLOAT	SR	= GMath.SinTab(Rot.Roll);
		const FLOAT	SP	= GMath.SinTab(Rot.Pitch);
		const FLOAT	SY	= GMath.SinTab(Rot.Yaw);
		const FLOAT	CR	= GMath.CosTab(Rot.Roll);
		const FLOAT	CP	= GMath.CosTab(Rot.Pitch);
		const FLOAT	CY	= GMath.CosTab(Rot.Yaw);

		M[0][0]	= CP * CY;
		M[0][1]	= -CP * SY;
		M[0][2]	= SP;
		M[0][3]	= 0.f;

		M[1][0]	= SR * SP * CY + CR * SY;
		M[1][1]	= -SR * SP * SY + CR * CY;
		M[1][2]	= - SR * CP;
		M[1][3]	= 0.f;

		M[2][0]	= -CR * SP * CY + SR * SY;
		M[2][1]	= CY * SR + CR * SP * SY;
		M[2][2]	= CR * CP;
		M[2][3]	= 0.f;

		M[3][0]	= 0.f;
		M[3][1]	= 0.f;
		M[3][2]	= 0.f;
		M[3][3]	= 1.f;
	}
};

// see also FRotationMatrix
class FRotationTranslationMatrix : public FMatrix
{
public:
	FRotationTranslationMatrix(const FRotator& Rot, const FVector& Origin)
	{
		const FLOAT	SR	= GMath.SinTab(Rot.Roll);
		const FLOAT	SP	= GMath.SinTab(Rot.Pitch);
		const FLOAT	SY	= GMath.SinTab(Rot.Yaw);
		const FLOAT	CR	= GMath.CosTab(Rot.Roll);
		const FLOAT	CP	= GMath.CosTab(Rot.Pitch);
		const FLOAT	CY	= GMath.CosTab(Rot.Yaw);

		M[0][0]	= CP * CY;
		M[0][1]	= -CP * SY;
		M[0][2]	= SP;
		M[0][3]	= 0.f;

		M[1][0]	= SR * SP * CY + CR * SY;
		M[1][1]	= -SR * SP * SY + CR * CY;
		M[1][2]	= - SR * CP;
		M[1][3]	= 0.f;

		M[2][0]	= -CR * SP * CY + SR * SY;
		M[2][1]	= CY * SR + CR * SP * SY;
		M[2][2]	= CR * CP;
		M[2][3]	= 0.f;

		M[3][0]	= Origin.x;
		M[3][1]	= Origin.y;
		M[3][2]	= Origin.z;
		M[3][3]	= 1.f;
	}
};

class FScaleRotationTranslationMatrix : public FMatrix
{
public:
	FScaleRotationTranslationMatrix(const FVector& Scale, const FRotator& Rot, const FVector& Origin)
	{
		const FLOAT	SR	= GMath.SinTab(Rot.Roll);
		const FLOAT	SP	= GMath.SinTab(Rot.Pitch);
		const FLOAT	SY	= GMath.SinTab(Rot.Yaw);
		const FLOAT	CR	= GMath.CosTab(Rot.Roll);
		const FLOAT	CP	= GMath.CosTab(Rot.Pitch);
		const FLOAT	CY	= GMath.CosTab(Rot.Yaw);

		M[0][0]	= ( CP * CY ) * Scale.x;
		M[0][1]	= ( -CP * SY ) * Scale.x;
		M[0][2]	= SP * Scale.x;
		M[0][3]	= 0.f;

		M[1][0]	= ( SR * SP * CY + CR * SY ) * Scale.y;
		M[1][1]	= ( -SR * SP * SY + CR * CY ) * Scale.y;
		M[1][2]	= ( -SR * CP ) * Scale.y;
		M[1][3]	= 0.f;

		M[2][0]	= ( -CR * SP * CY + SR * SY) * Scale.z;
		M[2][1]	= ( CY * SR + CR * SP * SY ) * Scale.z;
		M[2][2]	= ( CR * CP ) * Scale.z;
		M[2][3]	= 0.f;

		M[3][0]	= Origin.x;
		M[3][1]	= Origin.y;
		M[3][2]	= Origin.z;
		M[3][3]	= 1.f;

	}
};

class FQuatRotationTranslationMatrix : public FMatrix
{
public:
	// make sure Quanternion is normalized
	FQuatRotationTranslationMatrix(const FVector& Scale, const FQuat& Rot, const FVector& Origin)
	{
#if !SHIPPING
		check(Rot.IsNormalized());
#endif

		const FLOAT q00 = 2.0f * Rot.x * Rot.x;
		const FLOAT q11 = 2.0f * Rot.y * Rot.y;
		const FLOAT q22 = 2.0f * Rot.z * Rot.z;

		const FLOAT q01 = 2.0f * Rot.x * Rot.y;
		const FLOAT q02 = 2.0f * Rot.x * Rot.z;
		const FLOAT q03 = 2.0f * Rot.x * Rot.w;

		const FLOAT q12 = 2.0f * Rot.y * Rot.z;
		const FLOAT q13 = 2.0f * Rot.y * Rot.w;

		const FLOAT q23 = 2.0f * Rot.z * Rot.w;

		M[0][0] = (1.0f - q11 - q22) * Scale.x;
		M[0][1] =        (q01 + q23) * Scale.x;
		M[0][2] =        (q02 - q13) * Scale.x;
		M[0][3] =			   0.f;

		M[1][0] =        (q01 - q23) * Scale.y;
		M[1][1] = (1.0f - q22 - q00) * Scale.y;
		M[1][2] =        (q12 + q03) * Scale.y;
		M[1][3] =			   0.f;

		M[2][0] =        (q02 + q13) * Scale.z;
		M[2][1] =        (q12 - q03) * Scale.z;
		M[2][2] = (1.0f - q11 - q00) * Scale.z;
		M[2][3] =			   0.f;

		M[3][0] =		  Origin.x;
		M[3][1] =		  Origin.y;
		M[3][2] =		  Origin.z;
		M[3][3] =			   1.f;
	}
};

class FInverseRotationMatrix : public FMatrix
{
public:
	//FInverseRotationMatrix(const FRotator& Rot) :
	//  FMatrix(
	//	  FMatrix(	// Yaw
	//		FPlane( +GMath.CosTab(-Rot.Yaw),	 -GMath.SinTab(-Rot.Yaw),						 0.0f,		0.0f),
	//		FPlane( +GMath.SinTab(-Rot.Yaw),	 +GMath.CosTab(-Rot.Yaw),						 0.0f,		0.0f),
	//		FPlane(					  0.0f,		                    0.0f,						 1.0f,		0.0f),
	//		FPlane(					  0.0f,						    0.0f,						 0.0f,		1.0f)) *
	//	  FMatrix(	// Pitch
	//		FPlane(	+GMath.CosTab(-Rot.Pitch),					    0.0f,	+GMath.SinTab(-Rot.Pitch),		0.0f),
	//		FPlane(					  0.0f,						    1.0f,				0.0f,				0.0f),
	//		FPlane(	-GMath.SinTab(-Rot.Pitch),					    0.0f,	+GMath.CosTab(-Rot.Pitch),		0.0f),
	//		FPlane(0.0f,										    0.0f,						 0.0f,		1.0f)) *
	//	  FMatrix(	// Roll
	//		FPlane(						1.0f,						0.0f,						 0.0f,		0.0f),
	//		FPlane(						0.0f,	+GMath.CosTab(-Rot.Roll),	 -GMath.SinTab(-Rot.Roll),		0.0f),
	//		FPlane(						0.0f,	+GMath.SinTab(-Rot.Roll),	 +GMath.CosTab(-Rot.Roll),		0.0f),
	//		FPlane(						0.0f,						0.0f,				0.0f,				1.0f))
	//	  )
	//{
	//}

	FInverseRotationMatrix(const FRotator& Rot) :
	  FMatrix(
		  FMatrix(	// Yaw
			FPlane( +GMath.CosTab(+Rot.Yaw),	 +GMath.SinTab(+Rot.Yaw),						 0.0f,		0.0f),
			FPlane( -GMath.SinTab(+Rot.Yaw),	 +GMath.CosTab(+Rot.Yaw),						 0.0f,		0.0f),
			FPlane(					  0.0f,		                    0.0f,						 1.0f,		0.0f),
			FPlane(					  0.0f,						    0.0f,						 0.0f,		1.0f)) *
		  FMatrix(	// Pitch
			FPlane(	+GMath.CosTab(+Rot.Pitch),					    0.0f,	-GMath.SinTab(+Rot.Pitch),		0.0f),
			FPlane(					  0.0f,						    1.0f,				0.0f,				0.0f),
			FPlane(	+GMath.SinTab(+Rot.Pitch),					    0.0f,	+GMath.CosTab(+Rot.Pitch),		0.0f),
			FPlane(0.0f,										    0.0f,						 0.0f,		1.0f)) *
		  FMatrix(	// Roll
			FPlane(						1.0f,						0.0f,						 0.0f,		0.0f),
			FPlane(						0.0f,	+GMath.CosTab(+Rot.Roll),	 +GMath.SinTab(+Rot.Roll),		0.0f),
			FPlane(						0.0f,	-GMath.SinTab(+Rot.Roll),	 +GMath.CosTab(+Rot.Roll),		0.0f),
			FPlane(						0.0f,						0.0f,				0.0f,				1.0f))
		  )
	{
	}
};

class FScaleMatrix : public FMatrix
{
public:

	/**
	 * Uniform scale.
	 */
	FScaleMatrix(FLOAT Scale) :
	  FMatrix(
		  FPlane(Scale,	0.0f,	0.0f,	0.0f),
		  FPlane(0.0f,	Scale,	0.0f,	0.0f),
		  FPlane(0.0f,	0.0f,	Scale,	0.0f),
		  FPlane(0.0f,	0.0f,	0.0f,	1.0f))
	  {
	  }

	/**
	 * Non-uniform scale.
	 */
	FScaleMatrix(const FVector& Scale) :
		FMatrix(
			FPlane(Scale.x,	0.0f,		0.0f,		0.0f),
			FPlane(0.0f,	Scale.y,	0.0f,		0.0f),
			FPlane(0.0f,	0.0f,		Scale.z,	0.0f),
			FPlane(0.0f,	0.0f,		0.0f,		1.0f))
	{
	}
};

// see also D3DXMatrixLookAtLH
class FBasisVectorMatrix : public FMatrix
{
public:
	FBasisVectorMatrix(const FVector& XAxis, const FVector& YAxis, const FVector& ZAxis, const FVector& Origin)
	{
		for( INT i = 0; i < 3; ++i )
		{
			M[i][0] = XAxis[i];
			M[i][1] = YAxis[i];
			M[i][2] = ZAxis[i];
			M[i][3] = 0.f;
		}
		M[3][0] = -Origin.DotProduct(XAxis);
		M[3][1] = -Origin.DotProduct(YAxis);
		M[3][2] = -Origin.DotProduct(ZAxis);
		M[3][3] = 1.f;
	}
};

//////////////// Global Functions ////////////////////////
FLOAT PointDistToLine(const FVector& Point, const FVector& Direction, const FVector& Origin, FVector& OutClosestPoint);

FLOAT PointDistToLine(const FVector& Point, const FVector& Direction, const FVector& Origin);

FLOAT PointDistToSegment(const FVector& Point, const FVector& StartPoint, const FVector& EndPoint, FVector& OutClosestPoint);

/**
 *	return closest point on a triangle to a point
 *	@param	Point	point to check distance for
 *	@param	A,B,C	counter clockwise ordering of points defining a triangle
 */
FVector ClosestPointOnTriangleToPoint(const FVector& Point, const FVector& A, const FVector& B, const FVector& C);

/*-----------------------------------------------------------------------------
			Interpolation Function
-----------------------------------------------------------------------------*/
/** 
 *	interpolate a normal vector to target with constant ratotion speed degrees.
 */
FVector VInterpNormalRotationTo(const FVector& Current, const FVector& Target, FLOAT DeltaTime, FLOAT RotationSpeedDegrees);

/**
 *	interpolate a vector to target with constant step
 */
FVector VInterpConstantTo(const FVector& Current, const FVector& Target, FLOAT DeltaTime, FLOAT InterpSpeed);

FVector VInterpTo(const FVector& Current, const FVector& Target, FLOAT DeltaTime, FLOAT InterpSpeed);

FRotator RInterpTo(const FRotator& Current, const FRotator& Target, FLOAT DeltaTime, FLOAT InterpSpeed);

FRotator RInterpTo(const FRotator& Current, const FRotator& Target, FLOAT Alpha, UBOOL bShortestPath = FALSE);

FLOAT FInterpTo(FLOAT Current, FLOAT Target, FLOAT DeltaTime, FLOAT InterpSpeed);

FLOAT FInterpEaseInOut(FLOAT A, FLOAT B, FLOAT Alpha, FLOAT Exp);

/**
 *	interpolate with ease-in (smoothly approaches B)
 *	@param	A		value to interpolate from
 *	@param	B		value to interpolate to
 *	@param	Alpha	interpolant
 *	@param	Exp		Exponent. Higher value result in more rapid deceleration
 */
FLOAT FInterpEaseIn(FLOAT A, FLOAT B, FLOAT Alpha, FLOAT Exp);

/**
 *	interpolate with ease-out (smoothly departs A)
 *	@param	A		value to interpolate from
 *	@param	B		value to interpolate to
 *	@param	Alpha	interpolant
 *	@param	Exp		Exponent. Higher value result in more rapid acceleration
 */
FLOAT FInterpEaseOut(FLOAT A, FLOAT B, FLOAT Alpha, FLOAT Exp);


////////////////////////// 骨骼动画基本数据 ///////////////////////////////
/**
 *	Below this weight threshold, animations won't be blended in
 */
#define ZERO_ANIMWEIGHT_THRESH	(0.00001f)

class FBoneAtom
{
public:
	FQuat	Rotation;
	FVector	Translation;
	FLOAT	Scale;

	static const FBoneAtom Identity;

	FORCEINLINE FBoneAtom() {};

	FORCEINLINE FBoneAtom(const FQuat& InRotation, const FVector& InTranslation, FLOAT InScale = 1.f)
		: Rotation(InRotation), Translation(InTranslation), Scale(InScale)
	{}

	FORCEINLINE FBoneAtom(const FRotator& InRotation, const FVector& InTranslation, FLOAT InScale = 1.f)
		: Rotation(InRotation), Translation(InTranslation), Scale(InScale)
	{}

	FORCEINLINE FBoneAtom(const FBoneAtom& InBoneAtom)
		: Rotation(InBoneAtom.Rotation), Translation(InBoneAtom.Translation), Scale(InBoneAtom.Scale)
	{}

	FORCEINLINE FMatrix ToMatrix() const
	{
		FMatrix OutMat;

#if !SHIPPING
		check(Rotation.IsNormalized());
#endif
		OutMat = FQuatRotationTranslationMatrix(FVector(Scale), Rotation, Translation);
		return OutMat;
	}

	FORCEINLINE void Blend(const FBoneAtom& Atom1, const FBoneAtom& Atom2, FLOAT Alpha)
	{
#if SHIPPING
		check(Atom1.Rotation.IsNormalized());
		check(Atom2.Rotation.IsNormalized());
#endif
		if( Alpha <= ZERO_ANIMWEIGHT_THRESH )
		{
			*this = Atom1;
		}
		else if( Alpha >= 1.f - ZERO_ANIMWEIGHT_THRESH )
		{
			*this = Atom2;
		}
		else
		{
			Translation = Lerp(Atom1.Translation, Atom2.Translation, Alpha);
			Scale		= Lerp(Atom1.Scale, Atom2.Scale, Alpha);
			Rotation	= FQuat::LerpQuat(Atom1.Rotation, Atom2.Rotation, Alpha);

			Rotation.Normalize();
		}
	}

	FORCEINLINE void BlendWith(const FBoneAtom& Atom, FLOAT Alpha)
	{
#if SHIPPING
		check(Atom.Rotation.IsNormalized());
		check(Rotation.IsNormalized());
#endif
		if( Alpha > ZERO_ANIMWEIGHT_THRESH )
		{
			if( Alpha <= 1.f - ZERO_ANIMWEIGHT_THRESH )
			{
				*this = Atom;
			}
			else
			{
				Translation = Lerp(Translation, Atom.Translation, Alpha);
				Scale		= Lerp(Scale, Atom.Scale, Alpha);
				Rotation	= FQuat::LerpQuat(Rotation, Atom.Rotation, Alpha);

				Rotation.Normalize();
			}
		}
	}

	FORCEINLINE FBoneAtom	operator* (const FBoneAtom& Other) const;
	FORCEINLINE void		operator*= (const FBoneAtom& Other);
	FORCEINLINE FBoneAtom	operator* (const FQuat& Other) const;
	FORCEINLINE void		operator*= (const FQuat& Other);

	FORCEINLINE void ScaleTranslation(const FVector& Scale3D)
	{
		Translation *= Scale3D;
	}

	FORCEINLINE void RemoveScale(FLOAT Tolerance = SMALL_NUMBER)
	{
		Scale = 1.f;
		if( Rotation.IsNormalized() == FALSE )
		{
			Rotation.Normalize();
		}
	}

	FORCEINLINE FBoneAtom	Inverse() const;
	FORCEINLINE FVector4	TransformFVector4(const FVector4& V) const;
	FORCEINLINE FVector		TransformFVector(const FVector& V) const;
	FORCEINLINE FVector		TransformNormal(const FVector& V) const;
	FORCEINLINE FBoneAtom	ApplyScale(FLOAT Scale) const;
	FORCEINLINE FVector		GetAxis(INT i) const;

	FORCEINLINE FVector		GetOrigin() const
	{
		return Translation;
	}

	FORCEINLINE void		SetOrigin(const FVector& Origin)
	{
		Translation = Origin;
	}

	inline friend FArchive& operator<< (FArchive& Ar, FBoneAtom& M)
	{
		Ar << M.Rotation;
		Ar << M.Translation;
		Ar << M.Scale;
		return Ar;
	}

	FORCEINLINE FQuat ToQuat() const
	{
		return Rotation;
	}

	FORCEINLINE FRotator Rotator() const
	{
		return Rotation.Rotator();
	}
};

FORCEINLINE void Multiply(FBoneAtom& OutBoneAtom, const FBoneAtom& A, const FBoneAtom& B)
{
	check(A.Rotation.IsNormalized());
	check(B.Rotation.IsNormalized());

	// 久违的推导啊
	//	When Q = quaternion, S = single scalar scale, and T = translation
	//	QST(A) = Q(A), S(A), T(A), and QST(B) = Q(B), S(B), T(B)

	//	QST (AxB) 

	// QST(A) = Q(A)*S(A)*P*-Q(A) + T(A)
	// QST(AxB) = Q(B)*S(B)*QST(A)*-Q(B) + T(B)
	// QST(AxB) = Q(B)*S(B)*[Q(A)*S(A)*P*-Q(A) + T(A)]*-Q(B) + T(B)
	// QST(AxB) = Q(B)*S(B)*Q(A)*S(A)*P*-Q(A)*-Q(B) + Q(B)*S(B)*T(A)*-Q(B) + T(B)
	// QST(AxB) = [Q(B)*Q(A)]*[S(B)*S(A)]*P*-[Q(B)*Q(A)] + Q(B)*S(B)*T(A)*-Q(B) + T(B)

	//	Q(AxB) = Q(B)*Q(A)
	//	S(AxB) = S(A)*S(B)
	//	T(AxB) = Q(B)*S(B)*T(A)*-Q(B) + T(B)
	
	OutBoneAtom.Rotation = B.Rotation * A.Rotation;
	OutBoneAtom.Scale = A.Scale * B.Scale;
	OutBoneAtom.Translation = B.Rotation.RotateVector(B.Scale * A.Translation) + B.Translation;
}

FORCEINLINE FBoneAtom FBoneAtom::operator *(const FBoneAtom &Other) const
{
	FBoneAtom Result;
	Multiply(Result, *this, Other);
	return Result;
}

FORCEINLINE void FBoneAtom::operator *=(const FBoneAtom &Other)
{
	Multiply(*this, *this, Other);
}

FORCEINLINE FBoneAtom FBoneAtom::operator *(const FQuat &Other) const
{
	FBoneAtom Result;
	FBoneAtom OtherFB(Other, FVector(0.f), 1.f);
	Multiply(Result, *this, OtherFB);
	return Result;
}

FORCEINLINE void FBoneAtom::operator *=(const FQuat &Other)
{
	FBoneAtom OtherFB(Other, FVector(0.f), 1.f);
	Multiply(*this, *this, OtherFB);
}

FORCEINLINE FBoneAtom FBoneAtom::Inverse() const
{
	//	Q(~A) = Q(~A)
	//	S(~A) = 1.f/S(A)
	//	T(~A) = - (Q(~A)*S(~A)*T(A)*Q(A))

	check(Rotation.IsNormalized());
	check(Scale != 0.0f);

	FBoneAtom Result(Rotation.Inverse(), FVector(0.f), 1.f / Scale);

	FVector temp = Result.Rotation.RotateVector(Result.Scale * Translation);
	Result.Translation = -temp;
	return Result;
}

FORCEINLINE FVector4 FBoneAtom::TransformFVector4(const FVector4 &V) const
{
	check(V.w == 1.0f || V.w == 0.f);

	FVector4 Transform(Rotation.RotateVector(V), 0.f);
	if( V.w == 1.0f )
	{
		Transform += FVector4(Translation, 1.0f);
	}
	return Transform;
}

FORCEINLINE FVector FBoneAtom::TransformFVector(const FVector &V) const
{
	return TransformFVector4(FVector4(V, 1.0f));
}

FORCEINLINE FVector FBoneAtom::TransformNormal(const FVector& V) const
{
	return TransformFVector4(FVector4(V, 0.0f));
}

FORCEINLINE FBoneAtom FBoneAtom::ApplyScale(FLOAT InScale) const
{
	FBoneAtom Result(*this);
	Result.Scale *= InScale;
	return Result;
}

FORCEINLINE FVector FBoneAtom::GetAxis(INT i) const
{
	if( i == 0 )
	{
		return Rotation.GetAxisX();
	}
	else if( i == 1 )
	{
		return Rotation.GetAxisY();
	}
	else if( i == 2 )
	{
		return Rotation.GetAxisZ();
	}
}

#endif