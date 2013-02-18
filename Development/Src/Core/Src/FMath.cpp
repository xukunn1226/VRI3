#include "CorePCH.h"
#include "FMath.h"
#include <d3dx9.h>

const FVector2 FVector2::ZERO(0.f, 0.f);
const FVector2 FVector2::UNIT_X(1.f, 0.f);
const FVector2 FVector2::UNIT_Y(0.f, 1.f);
const FVector2 FVector2::UNIT_SCALE(1.f, 1.f);
const FVector2 FVector2::NEGATIVE_UNIT_X(-1.f, 0.f);
const FVector2 FVector2::NEGATIVE_UNIT_Y(0.f, -1.f);

const FVector FVector::ZERO(0.f, 0.f, 0.f);
const FVector FVector::UNIT_X(1.f, 0.f, 0.f);
const FVector FVector::UNIT_Y(0.f, 1.f, 0.f);
const FVector FVector::UNIT_Z(0.f, 0.f, 1.f);
const FVector FVector::UNIT_SCALE(1.f, 1.f, 1.f);
const FVector FVector::NEGATIVE_UNIT_X(-1.f, 0.f, 0.f);
const FVector FVector::NEGATIVE_UNIT_Y(0.f, -1.f, 0.f);
const FVector FVector::NEGATIVE_UNIT_Z(0.f, 0.f, -1.f);

const FQuat	FQuat::Identity(0.f, 0.f, 0.f, 1.f);

const FMatrix FMatrix::Identity(FPlane(1,0,0,0),FPlane(0,1,0,0),FPlane(0,0,1,0),FPlane(0,0,0,1));

FVector FVector::RotateAngleAxis(const INT Angle, const FVector &Axis) const
{
	const FLOAT S	= GMath.SinTab(Angle);
	const FLOAT C	= GMath.CosTab(Angle);

	const FLOAT XX	= Axis.x * Axis.x;
	const FLOAT YY	= Axis.y * Axis.y;
	const FLOAT ZZ	= Axis.z * Axis.z;

	const FLOAT XY	= Axis.x * Axis.y;
	const FLOAT YZ	= Axis.y * Axis.z;
	const FLOAT ZX	= Axis.z * Axis.x;

	const FLOAT XS	= Axis.x * S;
	const FLOAT YS	= Axis.y * S;
	const FLOAT ZS	= Axis.z * S;

	const FLOAT OMC	= 1.f - C;

	return FVector(
		(OMC * XX + C ) * x + (OMC * XY - ZS) * y + (OMC * ZX + YS) * z,
		(OMC * XY + ZS) * x + (OMC * YY + C ) * y + (OMC * YZ - XS) * z,
		(OMC * ZX - YS) * x + (OMC * YZ + XS) * y + (OMC * ZZ + C ) * z
		);
}

FVector::FVector(const FVector4& V)
: x(V.x), y(V.y), z(V.z)
{}

// 可以这么理解这个函数，标准的X正方向（1，0，0）可以通过此旋转(R->P->Y)转至this方向
// 或者说this可以通过（Y->P->R)转至X正方向（1，0，0）
// 所求解为坐标系A'相对坐标系A的逆旋转，可得表达式（1，0，0）×Rot'【R->P->Y】 = P；
// 因此有（1，0，0）× Rot【Y->P->R】 = P ；OK，这就好理解下面的算法了
FRotator FVector::Rotation() const
{
	FRotator Rot;

	Rot.Yaw = -(INT)(appAtan2(y, x) * (FLOAT)MAXWORD / (2.f*PI));
	Rot.Pitch = (INT)(appAtan2(z, appSqrt(x*x+y*y)) * (FLOAT)MAXWORD / (2.f*PI));
	Rot.Roll = 0;

	return Rot;
}

void FVector::FindBestAxisVectors(FVector& Axis1, FVector& Axis2) const
{
	const FLOAT NX = Abs(x);
	const FLOAT NY = Abs(y);
	const FLOAT NZ = Abs(z);

	// Find best basis vectors.
	if( NZ>NX && NZ>NY )	Axis1 = FVector(1,0,0);
	else					Axis1 = FVector(0,0,1);

	Axis1 = (Axis1 - *this * (Axis1.DotProduct( *this ))).SafeNormal();
	Axis2 = Axis1.CrossProduct( *this );
}

FRotator FVector4::Rotation() const
{
	FRotator Rot;

	Rot.Yaw = -(INT)(appAtan2(y, x) * (FLOAT)MAXWORD / (2.f*PI) + 0.5f);
	Rot.Pitch = (INT)(appAtan2(z, appSqrt(x*x+y*y)) * (FLOAT)MAXWORD / (2.f*PI) + 0.5f);
	Rot.Roll = 0;

	return Rot;
}

FPlane FPlane::TransformPlaneByOrtho(const FMatrix& M) const
{
	const FVector4 Normal = M.TransformNormal(GetNormal());

	return FPlane( Normal, d - ( (FVector)Normal).DotProduct( M.TransformVector(FVector(0,0,0)) ) );
}

FBox::FBox(const FVector* Pts, INT Count)
: Min(0.f, 0.f, 0.f), Max(0.f, 0.f, 0.f), IsValid(FALSE)
{
	for( INT i = 0; i < Count; ++i )
	{
		*this += Pts[i];
	}
}

FBox FBox::TransformBy(const FMatrix& M) const
{
	FVector Vertices[8] = 
	{
		FVector(Min),
		FVector(Min.x, Min.y, Max.z),
		FVector(Min.x, Max.y, Min.z),
		FVector(Max.x, Min.y, Min.z),
		FVector(Max.x, Max.y, Min.z),
		FVector(Max.x, Min.y, Max.z),
		FVector(Min.x, Max.y, Max.z),
		FVector(Max)
	};

	FBox NewBox;
	for(INT VertexIndex = 0;VertexIndex < ARRAY_COUNT(Vertices);VertexIndex++)
	{
		FVector4 ProjectedVertex = M.TransformVector(Vertices[VertexIndex]);
		NewBox += ((FVector)ProjectedVertex) / ProjectedVertex.w;
	}

	return NewBox;
}

/** 
* Transforms and projects a world bounding box to screen space
*
* @param	ProjM - projection matrix
* @return	transformed box
*/
FBox FBox::TransformProjectBy(const FMatrix& ProjM) const
{
	FVector Vertices[8] = 
	{
		FVector(Min),
		FVector(Min.x, Min.y, Max.z),
		FVector(Min.x, Max.y, Min.z),
		FVector(Max.x, Min.y, Min.z),
		FVector(Max.x, Max.y, Min.z),
		FVector(Max.x, Min.y, Max.z),
		FVector(Min.x, Max.y, Max.z),
		FVector(Max)
	};

	FBox NewBox;
	for(INT VertexIndex = 0;VertexIndex < ARRAY_COUNT(Vertices);VertexIndex++)
	{
		FVector4 ProjectedVertex = ProjM.TransformVector(Vertices[VertexIndex]);
		NewBox += ((FVector)ProjectedVertex) / ProjectedVertex.w;
	}

	return NewBox;
}

FSphere::FSphere(const FVector* Pts, INT Count)
: FPlane(0.f, 0.f, 0.f, 0.f)
{
	if( Count > 0 )
	{
		FBox Box(Pts, Count);
		*this = FSphere( (Box.Min + Box.Max) * 0.5f, 0.f );

		FVector Center(x, y, z);
		for( INT i = 0; i < Count; ++i )
		{
			const FLOAT Dist = FDistSquared(Pts[i], Center);
			if( Dist > d )
			{
				d = Dist;
			}
		}
		d = appSqrt(d);
	}
}

FSphere FSphere::TransformBy(const FMatrix &M) const
{
	FSphere	Result;

	(FVector&)Result = M.TransformVector(*this);

	const FVector XAxis(M.M[0][0],M.M[0][1],M.M[0][2]);
	const FVector YAxis(M.M[1][0],M.M[1][1],M.M[1][2]);
	const FVector ZAxis(M.M[2][0],M.M[2][1],M.M[2][2]);

	Result.d = appSqrt( Max(XAxis.DotProduct(XAxis), 
						Max(YAxis.DotProduct(YAxis), ZAxis.DotProduct(ZAxis))))	* d;

	return Result;
}

FRotator::FRotator(const FQuat& Quat)
{
	*this = FQuatRotationTranslationMatrix(FVector(1.f), Quat, FVector(0.0f)).Rotator();
}

// X Axis为默认朝向
FVector FRotator::Vector() const
{
	return FRotationMatrix(*this).GetAxis(0);
}

FQuat FRotator::Quaternion() const
{
	return FQuat(FRotationMatrix(*this));
}

// warning: the order is Roll -> Pitch -> Yaw
FVector FRotator::Euler() const
{
	return FVector( Roll * (180.0f / 32768.0f), Pitch * (180.0f / 32768.0f), Yaw * (180.0f / 32768.0f) );
}

FRotator FRotator::MakeFromEuler(const FVector &Euler)
{
	return FRotator( appFloor(Euler.y * (32768.0f / 180.0f)), appFloor(Euler.z * (32768.0f / 180.0f)), appFloor(Euler.x * (32768.0f / 180.0f)) );
}

FQuat::FQuat(const FMatrix& M)
{
	FromRotationMatrix(M);
}

void FQuat::FromBasicAxis(const FVector &AxisX, const FVector &AxisY, const FVector &AxisZ)
{
	FMatrix M = FMatrix::Identity;
	const FLOAT normX = AxisX.Size();
	const FLOAT normY = AxisY.Size();
	const FLOAT normZ = AxisZ.Size();

	for( INT i = 0; i < 3; ++i )
	{
		M.M[0][i] = AxisX[i] / normX;
		M.M[1][i] = AxisY[i] / normY;
		M.M[2][i] = AxisZ[i] / normZ;
	}

	FromRotationMatrix(M);
}

void FQuat::FromRotationMatrix(const FMatrix& M)
{
	const FLOAT fTrace = M.M[0][0] + M.M[1][1] + M.M[2][2];
	FLOAT fRoot;

	if( fTrace > 0.f )
	{
		fRoot = appSqrt( fTrace + 1.f );
		w = fRoot * 0.5f;
		fRoot = 0.5f / fRoot;

		x = ( M.M[1][2] - M.M[2][1] ) * fRoot;
		y = ( M.M[2][0] - M.M[0][2] ) * fRoot;
		z = ( M.M[0][1] - M.M[1][0] ) * fRoot;
	}
	else
	{
		INT i = 0;

		if( M.M[1][1] > M.M[0][0] )
			i = 1;

		if( M.M[2][2] > M.M[i][i] )
			i = 2;

		static const INT next[3] = { 1, 2, 0 };
		const INT j = next[i];
		const INT k = next[j];

		fRoot = M.M[i][i] - M.M[j][j] - M.M[k][k] + 1.f;

		FLOAT InvR = 1.f / appSqrt(fRoot);

		FLOAT qt[4];
		qt[i] = 0.5f * ( 1.f / InvR );

		fRoot = 0.5f * InvR;

		qt[3] = ( M.M[j][k] - M.M[k][j] ) * fRoot;
		qt[j] = ( M.M[i][j] + M.M[j][i] ) * fRoot;
		qt[k] = ( M.M[i][k] + M.M[k][i] ) * fRoot;

		x = qt[0];	y = qt[1];	z = qt[2];	w = qt[3];
	}
}

// the quat must be normailized
void FQuat::ToRotationMatrix(FMatrix& M) const
{
	const FLOAT q00 = 2.0f * x * x;
	const FLOAT q11 = 2.0f * y * y;
	const FLOAT q22 = 2.0f * z * z;

	const FLOAT q01 = 2.0f * x * y;
	const FLOAT q02 = 2.0f * x * z;
	const FLOAT q03 = 2.0f * x * w;

	const FLOAT q12 = 2.0f * y * z;
	const FLOAT q13 = 2.0f * y * w;

	const FLOAT q23 = 2.0f * z * w;

	M.M[0][0] = 1.0f - q11 - q22;
	M.M[0][1] =        q01 + q23;
	M.M[0][2] =        q02 - q13;

	M.M[1][0] =        q01 - q23;
	M.M[1][1] = 1.0f - q22 - q00;
	M.M[1][2] =        q12 + q03;

	M.M[2][0] =        q02 + q13;
	M.M[2][1] =        q12 - q03;
	M.M[2][2] = 1.0f - q11 - q00;
}

FRotator FQuat::Rotator() const
{
	return FQuatRotationTranslationMatrix(FVector(1.f), *this, FVector(0.0f)).Rotator();
}

FQuat FQuat::MakeFromRotator(const FRotator &Rotator)
{
	return FQuat(FRotationMatrix(Rotator));
}

FQuat FQuat::MakeFromEuler(const FVector& Euler)
{
	return FQuat(FRotationMatrix(FRotator::MakeFromEuler(Euler)));
}

FQuat FQuat::Log() const
{
	FQuat Result;
	Result.w = 0.f;

	if ( Abs(w) < 1.f )
	{
		const FLOAT Angle = appAcos(w);
		const FLOAT SinAngle = appSin(Angle);

		if ( Abs(SinAngle) >= SMALL_NUMBER )
		{
			const FLOAT Scale = Angle/SinAngle;
			Result.x = Scale*x;
			Result.y = Scale*y;
			Result.z = Scale*z;

			return Result;
		}
	}

	Result.x = x;
	Result.y = y;
	Result.z = z;

	return Result;
}

FQuat FQuat::Exp() const
{
	const FLOAT Angle = appSqrt(x*x + y*y + z*z);
	const FLOAT SinAngle = appSin(Angle);

	FQuat Result;
	Result.w = appCos(Angle);

	if ( Abs(SinAngle) >= SMALL_NUMBER )
	{
		const FLOAT Scale = SinAngle/Angle;
		Result.x = Scale*x;
		Result.y = Scale*y;
		Result.z = Scale*z;
	}
	else
	{
		Result.x = x;
		Result.y = y;
		Result.z = z;
	}

	return Result;
}

// result is NOT normalized.
FQuat FQuat::SlerpQuat(const FQuat &q1, const FQuat &q2, FLOAT alpha, UBOOL bShortestPath)
{
	const FLOAT CosAngle = q1.DotProduct(q2);

	FLOAT c1, c2;
	if( ( 1.f - Abs(CosAngle) ) < KINDA_SMALL_NUMBER )
	{
		c1 = 1.f - alpha;
		c2 = alpha;
	}
	else
	{
		FLOAT Angle = appAcos(Abs(CosAngle));
		FLOAT SinAngle = appSin(Angle);
		c1 = appSin(Angle * (1.f - alpha)) / SinAngle;
		c2 = appSin(Angle * alpha) / SinAngle;
	}

	if( bShortestPath && CosAngle < 0.f )
		c1 = -c1;

	return FQuat(c1 * q1.x + c2 * q2.x, c1 * q1.y + c2 * q2.y, c1 * q1.z + c2 * q2.z, c1 * q1.w + c2 * q2.w);
}

// 快速线性quat插值算法
// result is NOT normalized
FQuat FQuat::LerpQuat(const FQuat& q1, const FQuat& q2, FLOAT alpha)
{
	const FLOAT Dot = q1.DotProduct(q2);

	const FLOAT Bias = Dot > 0.f ? 1.0f : -1.0f;
	return q1 * Bias * (1.0f - alpha) + q2 * alpha;
}

FBoxSphereBounds FBoxSphereBounds::TransformBy(const FMatrix& M) const
{
	FBoxSphereBounds Result;

	Result.Origin = M.TransformVector(Origin);
	Result.BoxExtent = FVector(0.f, 0.f, 0.f);
	FLOAT	Signs[2] = { -1.0f, 1.0f };
	for(INT X = 0;X < 2;X++)
	{
		for(INT Y = 0;Y < 2;Y++)
		{
			for(INT Z = 0;Z < 2;Z++)
			{
				FVector	Corner = M.TransformNormal(FVector(Signs[X] * BoxExtent.x,Signs[Y] * BoxExtent.y,Signs[Z] * BoxExtent.z));
				Result.BoxExtent.x = Max(Corner.x,Result.BoxExtent.x);
				Result.BoxExtent.y = Max(Corner.y,Result.BoxExtent.y);
				Result.BoxExtent.z = Max(Corner.z,Result.BoxExtent.z);
			}
		}
	}

	const FVector XAxis(M.M[0][0],M.M[0][1],M.M[0][2]);
	const FVector YAxis(M.M[1][0],M.M[1][1],M.M[1][2]);
	const FVector ZAxis(M.M[2][0],M.M[2][1],M.M[2][2]);

	Result.SphereRadius = appSqrt(Max(XAxis.DotProduct(XAxis),Max(YAxis.DotProduct(YAxis),ZAxis.DotProduct(ZAxis)))) * SphereRadius;

	return Result;
}

FMatrix::FMatrix()
{
	SetIdentity();
}

FMatrix::FMatrix(const FPlane &InX, const FPlane &InY, const FPlane &InZ, const FPlane &InW)
{
	M[0][0] = InX.x; M[0][1] = InX.y;  M[0][2] = InX.z;  M[0][3] = InX.d;
	M[1][0] = InY.x; M[1][1] = InY.y;  M[1][2] = InY.z;  M[1][3] = InY.d;
	M[2][0] = InZ.x; M[2][1] = InZ.y;  M[2][2] = InZ.z;  M[2][3] = InZ.d;
	M[3][0] = InW.x; M[3][1] = InW.y;  M[3][2] = InW.z;  M[3][3] = InW.d;
}

FMatrix::FMatrix(const FVector &InX, const FVector &InY, const FVector &InZ, const FVector &InW)
{
	M[0][0] = InX.x; M[0][1] = InX.y;  M[0][2] = InX.z;  M[0][3] = 0.0f;
	M[1][0] = InY.x; M[1][1] = InY.y;  M[1][2] = InY.z;  M[1][3] = 0.0f;
	M[2][0] = InZ.x; M[2][1] = InZ.y;  M[2][2] = InZ.z;  M[2][3] = 0.0f;
	M[3][0] = InW.x; M[3][1] = InW.y;  M[3][2] = InW.z;  M[3][3] = 1.0f;
}

FMatrix::FMatrix(const FMatrix& Other)
{
	appMemcpy(M, Other.M, 16 * sizeof(FLOAT));
}

FMatrix& FMatrix::operator= (const FMatrix& Other)
{
	appMemcpy(M, Other.M, 16 * sizeof(FLOAT));
	return *this;
}

void FMatrix::SetIdentity()
{
	M[0][0] = 1; M[0][1] = 0;  M[0][2] = 0;  M[0][3] = 0;
	M[1][0] = 0; M[1][1] = 1;  M[1][2] = 0;  M[1][3] = 0;
	M[2][0] = 0; M[2][1] = 0;  M[2][2] = 1;  M[2][3] = 0;
	M[3][0] = 0; M[3][1] = 0;  M[3][2] = 0;  M[3][3] = 1;
}

FMatrix FMatrix::operator+ (const FMatrix& Other) const
{
	FMatrix Mat;
	for( INT i = 0; i < 4; ++i )
	{
		for( INT j = 0; j < 4; ++j )
		{
			Mat.M[i][j] = M[i][j] + Other.M[i][j];
		}
	}
	return Mat;
}

FMatrix FMatrix::operator- (const FMatrix& Other) const
{
	FMatrix Mat;
	for( INT i = 0; i < 4; ++i )
	{
		for( INT j = 0; j < 4; ++j )
		{
			Mat.M[i][j] = M[i][j] - Other.M[i][j];
		}
	}
	return Mat;
}

FMatrix FMatrix::operator- () const
{
	FMatrix Mat;
	for( INT i = 0; i < 4; ++i )
	{
		for( INT j = 0; j < 4; ++j )
		{
			Mat.M[i][j] = -M[i][j];
		}
	}
	return Mat;
}

FMatrix FMatrix::operator* (const FMatrix& Other) const
{
	FMatrix Result;
	for( INT i = 0; i < 4; ++i )
	{
		for( INT j = 0; j < 4; ++j )
		{
			Result.M[i][j] = M[i][0] * Other.M[0][j] +
							 M[i][1] * Other.M[1][j] +
							 M[i][2] * Other.M[2][j] +
							 M[i][3] * Other.M[3][j];
		}
	}
	return Result;
}

FMatrix& FMatrix::operator*= (const FMatrix& Other)
{
	FMatrix Result;
	Result = (*this) * Other;
	*this = Result;
	return *this;
}

FMatrix FMatrix::operator* (FLOAT Other) const
{
	FMatrix Result;

	for(INT X = 0; X < 4; ++X)
	{
		for(INT Y = 0; Y < 4; ++Y)
		{
			Result.M[X][Y] = M[X][Y] * Other;
		}
	}
	return Result;
}

FMatrix& FMatrix::operator*= (FLOAT Other)
{
	for(INT X = 0; X < 4; ++X)
	{
		for(INT Y = 0; Y < 4; ++Y)
		{
			M[X][Y] *= Other;
		}
	}
	return *this;
}

UBOOL FMatrix::operator== (const FMatrix& Other) const
{
	for( INT i = 0; i < 4; ++i )
	{
		for( INT j = 0; j < 4; ++j )
		{
			if( M[i][j] != Other.M[i][j] )
				return FALSE;
		}
	}
	return TRUE;
}

UBOOL FMatrix::Equals(const FMatrix &Other, FLOAT Tolerance) const
{
	for( INT i = 0; i < 4; ++i )
	{
		for( INT j = 0; j < 4; ++j )
		{
			if( Abs(M[i][j] - Other.M[i][j]) > Tolerance )
				return FALSE;
		}
	}

	return TRUE;
}

FVector4 FMatrix::TransformVector4(const FVector4 &V) const
{
	FVector4 Result;
	Result.x = V.x * M[0][0] + V.y * M[1][0] + V.z * M[2][0] + V.w * M[3][0];
	Result.y = V.x * M[0][1] + V.y * M[1][1] + V.z * M[2][1] + V.w * M[3][1];
	Result.z = V.x * M[0][2] + V.y * M[1][2] + V.z * M[2][2] + V.w * M[3][2];
	Result.w = V.x * M[0][3] + V.y * M[1][3] + V.z * M[2][3] + V.w * M[3][3];
	return Result;
}

FVector4 FMatrix::TransformVector(const FVector& V) const
{
	return TransformVector4(FVector4(V, 1.f));
}

// correctly handles scaling in this matrix
FVector FMatrix::InverseTransformVector(const FVector& V) const
{
	FMatrix InvSelf = Inverse();
	return InvSelf.TransformVector(V);
}

/** Faster version of InverseTransformFVector that assumes no scaling. WARNING: Will NOT work correctly if there is scaling in the matrix. */
FVector FMatrix::InverseTransformVectorNoScale(const FVector& V) const
{
	check( Abs(1.f - Abs(RotDeterminant())) < KINDA_SMALL_NUMBER );
	
	FVector t, Result;

	t.x = V.x - M[3][0];
	t.y = V.y - M[3][1];
	t.z = V.z - M[3][2];

	Result.x = t.x * M[0][0] + t.y * M[0][1] + t.z * M[0][2];
	Result.y = t.x * M[1][0] + t.y * M[1][1] + t.z * M[1][2];
	Result.z = t.x * M[2][0] + t.y * M[2][1] + t.z * M[2][2];

	return Result;
}

FVector4 FMatrix::TransformNormal(const FVector& V) const
{
	return TransformVector4(FVector4(V, 0.f));
}

/** Faster version of InverseTransformNormal that assumes no scaling. WARNING: Will NOT work correctly if there is scaling in the matrix. */
FVector FMatrix::InverseTransformNormal(const FVector& V) const
{
	FMatrix InvSelf = Inverse();
	return InvSelf.TransformNormal(V);
}

FVector FMatrix::InverseTransformNormalNoScale(const FVector& V) const
{
	check( Abs(1.f - Abs(RotDeterminant())) < KINDA_SMALL_NUMBER );

	return FVector( V.x * M[0][0] + V.y * M[0][1] + V.z * M[0][2],
					V.x * M[1][0] + V.y * M[1][1] + V.z * M[1][2],
					V.x * M[2][0] + V.y * M[2][1] + V.z * M[2][2] );
}

FMatrix FMatrix::Transpose() const
{
	FMatrix	Result;

	Result.M[0][0] = M[0][0];
	Result.M[0][1] = M[1][0];
	Result.M[0][2] = M[2][0];
	Result.M[0][3] = M[3][0];

	Result.M[1][0] = M[0][1];
	Result.M[1][1] = M[1][1];
	Result.M[1][2] = M[2][1];
	Result.M[1][3] = M[3][1];

	Result.M[2][0] = M[0][2];
	Result.M[2][1] = M[1][2];
	Result.M[2][2] = M[2][2];
	Result.M[2][3] = M[3][2];

	Result.M[3][0] = M[0][3];
	Result.M[3][1] = M[1][3];
	Result.M[3][2] = M[2][3];
	Result.M[3][3] = M[3][3];

	return Result;
}

FLOAT FMatrix::Determinant() const
{
	return	M[0][0] * (
				M[1][1] * (M[2][2] * M[3][3] - M[2][3] * M[3][2]) -
				M[2][1] * (M[1][2] * M[3][3] - M[1][3] * M[3][2]) +
				M[3][1] * (M[1][2] * M[2][3] - M[1][3] * M[2][2])
				) -
			M[1][0] * (
				M[0][1] * (M[2][2] * M[3][3] - M[2][3] * M[3][2]) -
				M[2][1] * (M[0][2] * M[3][3] - M[0][3] * M[3][2]) +
				M[3][1] * (M[0][2] * M[2][3] - M[0][3] * M[2][2])
				) +
			M[2][0] * (
				M[0][1] * (M[1][2] * M[3][3] - M[1][3] * M[3][2]) -
				M[1][1] * (M[0][2] * M[3][3] - M[0][3] * M[3][2]) +
				M[3][1] * (M[0][2] * M[1][3] - M[0][3] * M[1][2])
				) -
			M[3][0] * (
				M[0][1] * (M[1][2] * M[2][3] - M[1][3] * M[2][2]) -
				M[1][1] * (M[0][2] * M[2][3] - M[0][3] * M[2][2]) +
				M[2][1] * (M[0][2] * M[1][3] - M[0][3] * M[1][2])
				);
}

FLOAT FMatrix::RotDeterminant() const
{
	return
		M[0][0] * (M[1][1] * M[2][2] - M[1][2] * M[2][1]) -
		M[1][0] * (M[0][1] * M[2][2] - M[0][2] * M[2][1]) +
		M[2][0] * (M[0][1] * M[1][2] - M[0][2] * M[1][1]);
}

FMatrix FMatrix::Inverse() const
{
	FMatrix Result;
	D3DXMatrixInverse( (D3DXMATRIX*)&Result, NULL, (const D3DXMATRIX*)this );
	return Result;
}

FMatrix FMatrix::InverseSafe() const
{
	FMatrix Result;
	const FLOAT	Det = Determinant();

	if(Det == 0.0f)
		return FMatrix::Identity;

	const FLOAT	RDet = 1.0f / Det;

	Result.M[0][0] = RDet * (
			M[1][1] * (M[2][2] * M[3][3] - M[2][3] * M[3][2]) -
			M[2][1] * (M[1][2] * M[3][3] - M[1][3] * M[3][2]) +
			M[3][1] * (M[1][2] * M[2][3] - M[1][3] * M[2][2])
			);
	Result.M[0][1] = -RDet * (
			M[0][1] * (M[2][2] * M[3][3] - M[2][3] * M[3][2]) -
			M[2][1] * (M[0][2] * M[3][3] - M[0][3] * M[3][2]) +
			M[3][1] * (M[0][2] * M[2][3] - M[0][3] * M[2][2])
			);
	Result.M[0][2] = RDet * (
			M[0][1] * (M[1][2] * M[3][3] - M[1][3] * M[3][2]) -
			M[1][1] * (M[0][2] * M[3][3] - M[0][3] * M[3][2]) +
			M[3][1] * (M[0][2] * M[1][3] - M[0][3] * M[1][2])
			);
	Result.M[0][3] = -RDet * (
			M[0][1] * (M[1][2] * M[2][3] - M[1][3] * M[2][2]) -
			M[1][1] * (M[0][2] * M[2][3] - M[0][3] * M[2][2]) +
			M[2][1] * (M[0][2] * M[1][3] - M[0][3] * M[1][2])
			);

	Result.M[1][0] = -RDet * (
			M[1][0] * (M[2][2] * M[3][3] - M[2][3] * M[3][2]) -
			M[2][0] * (M[1][2] * M[3][3] - M[1][3] * M[3][2]) +
			M[3][0] * (M[1][2] * M[2][3] - M[1][3] * M[2][2])
			);
	Result.M[1][1] = RDet * (
			M[0][0] * (M[2][2] * M[3][3] - M[2][3] * M[3][2]) -
			M[2][0] * (M[0][2] * M[3][3] - M[0][3] * M[3][2]) +
			M[3][0] * (M[0][2] * M[2][3] - M[0][3] * M[2][2])
			);
	Result.M[1][2] = -RDet * (
			M[0][0] * (M[1][2] * M[3][3] - M[1][3] * M[3][2]) -
			M[1][0] * (M[0][2] * M[3][3] - M[0][3] * M[3][2]) +
			M[3][0] * (M[0][2] * M[1][3] - M[0][3] * M[1][2])
			);
	Result.M[1][3] = RDet * (
			M[0][0] * (M[1][2] * M[2][3] - M[1][3] * M[2][2]) -
			M[1][0] * (M[0][2] * M[2][3] - M[0][3] * M[2][2]) +
			M[2][0] * (M[0][2] * M[1][3] - M[0][3] * M[1][2])
			);

	Result.M[2][0] = RDet * (
			M[1][0] * (M[2][1] * M[3][3] - M[2][3] * M[3][1]) -
			M[2][0] * (M[1][1] * M[3][3] - M[1][3] * M[3][1]) +
			M[3][0] * (M[1][1] * M[2][3] - M[1][3] * M[2][1])
			);
	Result.M[2][1] = -RDet * (
			M[0][0] * (M[2][1] * M[3][3] - M[2][3] * M[3][1]) -
			M[2][0] * (M[0][1] * M[3][3] - M[0][3] * M[3][1]) +
			M[3][0] * (M[0][1] * M[2][3] - M[0][3] * M[2][1])
			);
	Result.M[2][2] = RDet * (
			M[0][0] * (M[1][1] * M[3][3] - M[1][3] * M[3][1]) -
			M[1][0] * (M[0][1] * M[3][3] - M[0][3] * M[3][1]) +
			M[3][0] * (M[0][1] * M[1][3] - M[0][3] * M[1][1])
			);
	Result.M[2][3] = -RDet * (
			M[0][0] * (M[1][1] * M[2][3] - M[1][3] * M[2][1]) -
			M[1][0] * (M[0][1] * M[2][3] - M[0][3] * M[2][1]) +
			M[2][0] * (M[0][1] * M[1][3] - M[0][3] * M[1][1])
			);

	Result.M[3][0] = -RDet * (
			M[1][0] * (M[2][1] * M[3][2] - M[2][2] * M[3][1]) -
			M[2][0] * (M[1][1] * M[3][2] - M[1][2] * M[3][1]) +
			M[3][0] * (M[1][1] * M[2][2] - M[1][2] * M[2][1])
			);
	Result.M[3][1] = RDet * (
			M[0][0] * (M[2][1] * M[3][2] - M[2][2] * M[3][1]) -
			M[2][0] * (M[0][1] * M[3][2] - M[0][2] * M[3][1]) +
			M[3][0] * (M[0][1] * M[2][2] - M[0][2] * M[2][1])
			);
	Result.M[3][2] = -RDet * (
			M[0][0] * (M[1][1] * M[3][2] - M[1][2] * M[3][1]) -
			M[1][0] * (M[0][1] * M[3][2] - M[0][2] * M[3][1]) +
			M[3][0] * (M[0][1] * M[1][2] - M[0][2] * M[1][1])
			);
	Result.M[3][3] = RDet * (
			M[0][0] * (M[1][1] * M[2][2] - M[1][2] * M[2][1]) -
			M[1][0] * (M[0][1] * M[2][2] - M[0][2] * M[2][1]) +
			M[2][0] * (M[0][1] * M[1][2] - M[0][2] * M[1][1])
			);

	return Result;
}

// Remove any scaling from this matrix (ie magnitude of each row is 1)
void FMatrix::RemoveScale(FLOAT Tolerance)
{
	for(INT i=0; i<3; i++)
	{
		const FLOAT SquareSum = (M[i][0] * M[i][0]) + (M[i][1] * M[i][1]) + (M[i][2] * M[i][2]);
		if(SquareSum > Tolerance)
		{
			const FLOAT Scale = 1.f / appSqrt(SquareSum);
			M[i][0] *= Scale; 
			M[i][1] *= Scale; 
			M[i][2] *= Scale; 
		}
	}
}

void FMatrix::RemoveTranslation()
{
	M[3][0] = 0.f;
	M[3][1] = 0.f;
	M[3][2] = 0.f;
}

FMatrix FMatrix::ConcatTranslation(const FVector& Translation) const
{
	FMatrix Result = *this;
	Result.M[3][0] += Translation.x;
	Result.M[3][1] += Translation.y;
	Result.M[3][3] += Translation.z;
	return Result;
}

UBOOL FMatrix::ContainsNaN() const
{
	for(INT X = 0; X < 4; ++X)
	{
		for(INT Y = 0; Y < 4; ++Y)
		{
			if( appIsNaN(M[X][Y]) || !appIsFinite(M[X][Y]) )
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

void FMatrix::ScaleTranslation(const FVector& Scale3D)
{
	M[3][0] *= Scale3D.x;
	M[3][1] *= Scale3D.y;
	M[3][2] *= Scale3D.z;
}

FMatrix FMatrix::ApplyScale(FLOAT Scale) const
{
	return FScaleMatrix(Scale) * (*this);
}

FVector FMatrix::GetOrigin() const
{
	return FVector(M[3][0], M[3][1], M[3][2]);
}

FVector FMatrix::GetAxis(INT i) const
{
	check( i >= 0 && i <= 2 );
	return FVector(M[i][0], M[i][1], M[i][2]);
}

void FMatrix::GetAxes(FVector &X, FVector &Y, FVector &Z) const
{
	X.x = M[0][0];	X.y = M[0][1];	X.z = M[0][2];
	Y.x = M[1][0];	Y.y = M[1][1];	Y.z = M[1][2];
	Z.x = M[2][0];	Z.y = M[2][1];	Z.z = M[2][2];
}

void FMatrix::SetOrigin(const FVector& NewOrigin)
{
	M[3][0] = NewOrigin.x;
	M[3][1] = NewOrigin.y;
	M[3][2] = NewOrigin.z;
}

void FMatrix::SetAxis(INT i, const FVector& Axis)
{
	check( i >= 0 && i <= 2 );
	M[i][0] = Axis.x;
	M[i][1] = Axis.y;
	M[i][2] = Axis.z;
}

void FMatrix::SetAxes(const FVector& X, const FVector& Y, const FVector& Z)
{
	M[0][0] = X.x;	M[0][1] = X.y;	M[0][2] = X.z;
	M[1][0] = Y.x;	M[1][1] = Y.y;	M[1][2] = Y.z;
	M[2][0] = Z.x;	M[2][1] = Z.y;	M[2][2] = Z.z;
}

FVector FMatrix::GetColumn(INT i) const
{
	check(i >= 0 && i <=2);
	return FVector(M[0][i], M[1][i], M[2][i]);
}

// 有待检验
// convert Matrix to Euler, reference: http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToEuler/index.htm
FRotator FMatrix::Rotator() const
{
	FRotator Rotator;

	// 方法1
	if( M[0][2] > 1.0f - 0.002f )
	{
		Rotator.Pitch = 32768;
		Rotator.Yaw = 0;
		Rotator.Roll = appFloor( appAtan2( -M[1][0], M[2][0] ) * 32768.0f / PI );
	}
	else if( M[0][2] < 0.002f - 1.0f )
	{
		Rotator.Pitch = -32768;
		Rotator.Yaw = 0;
		Rotator.Roll = appFloor( appAtan2( -M[1][0], M[2][0] ) * 32768.0f / PI );
	}
	else
	{
		Rotator.Pitch = appFloor( appAsin(M[0][2]) * 32768.0f / PI );
		Rotator.Yaw = appFloor( appAtan2( -M[0][1], M[0][0] ) * 32768.0f / PI );
		Rotator.Roll = appFloor( appAtan2( -M[1][2], M[2][2] ) * 32768.0f / PI );
	}

	// 方法2
	//const FVector		XAxis	= GetAxis( 0 );
	//const FVector		YAxis	= GetAxis( 1 );
	//const FVector		ZAxis	= GetAxis( 2 );

	//FRotator	Rotator	= FRotator( 
	//								appFloor(appAtan2( XAxis.z, appSqrt(Square(XAxis.x)+Square(XAxis.y)) ) * 32768.f / PI), 
	//								appFloor(appAtan2( -XAxis.y, XAxis.x ) * 32768.f / PI), 
	//								0 
	//							);
	//
	//const FVector		SYAxis	= FRotationMatrix( Rotator ).GetAxis(1);
	//Rotator.Roll		= appFloor(appAtan2( ZAxis.DotProduct(SYAxis), YAxis.DotProduct(SYAxis) ) * 32768.f / PI);

	return Rotator;
}

void FMatrix::MakeTransform(const FVector& Scale, const FQuat& Rotation, const FVector& Trans)
{
	SetIdentity();

	M[0][0] = Scale.x;
	M[1][1] = Scale.y;
	M[2][2] = Scale.z;

	FMatrix Rot;
	Rotation.ToRotationMatrix(Rot);

	*this = *this * Rot;
	this->SetOrigin(Trans);
}

// Matrix = ViewMatrix * ProjMatrix
UBOOL FMatrix::GetFrustumNearPlane(FPlane& OutPlane) const
{
	return MakeFrustumPlanes(
		M[0][3] + M[0][2],
		M[1][3] + M[1][2],
		M[2][3] + M[2][2],
		M[3][3] + M[3][2],
		OutPlane
		);
}

UBOOL FMatrix::GetFrustumFarPlane(FPlane &OutPlane) const
{
	return MakeFrustumPlanes(
		M[0][3] - M[0][2],
		M[1][3] - M[1][2],
		M[2][3] - M[2][2],
		M[3][3] - M[3][2],
		OutPlane
		);
}

UBOOL FMatrix::GetFrustumLeftPlane(FPlane &OutPlane) const
{
	return MakeFrustumPlanes(
		M[0][3] + M[0][0],
		M[1][3] + M[1][0],
		M[2][3] + M[2][0],
		M[3][3] + M[3][0],
		OutPlane
		);
}

UBOOL FMatrix::GetFrustumRightPlane(FPlane &OutPlane) const
{
	return MakeFrustumPlanes(
		M[0][3] - M[0][0],
		M[1][3] - M[1][0],
		M[2][3] - M[2][0],
		M[3][3] - M[3][0],
		OutPlane
		);
}

UBOOL FMatrix::GetFrustumBottomPlane(FPlane& OutPlane) const
{
	return MakeFrustumPlanes(
		M[0][3] + M[0][1],
		M[1][3] + M[1][1],
		M[2][3] + M[2][1],
		M[3][3] + M[3][1],
		OutPlane
		);
}

UBOOL FMatrix::GetFrustumTopPlane(FPlane &OutPlane) const
{
	return MakeFrustumPlanes(
		M[0][3] - M[0][1],
		M[1][3] - M[1][1],
		M[2][3] - M[2][1],
		M[3][3] - M[3][1],
		OutPlane
		);
}

/*----------------------------------------------------------------------------
								Global Function Implementation
------------------------------------------------------------------------------*/
FVector ClosestPointOnLine(const FVector& LineStart, const FVector& LineEnd, const FVector& Point)
{
	FLOAT A = (LineStart - Point).DotProduct(LineEnd - LineStart);
	FLOAT B = (LineEnd - LineStart).SizeSquared();
	FLOAT T = ::Clamp(-A/B, 0.f, 1.f);

	FVector ClosestPoint = LineStart + T * (LineEnd - LineStart);
	return ClosestPoint;
}

void CreateOrthonormalBasis(FVector& XAxis, FVector& YAxis, FVector& ZAxis)
{
	XAxis = XAxis.ProjectOnPlane(ZAxis);
	YAxis = YAxis.ProjectOnPlane(ZAxis);

	if( XAxis.SizeSquared() < DELTA * DELTA )
	{
		XAxis = YAxis.CrossProduct(ZAxis);
	}

	if( YAxis.SizeSquared() < DELTA * DELTA )
	{
		YAxis = ZAxis.CrossProduct(XAxis);
	}

	XAxis.Normalize();
	YAxis.Normalize();
	ZAxis.Normalize();
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
)
{
	FBox BoxExt = Box;
	BoxExt.Min.x -= Extent.x;
	BoxExt.Min.y -= Extent.y;
	BoxExt.Min.z -= Extent.z;
	BoxExt.Max.x += Extent.x;
	BoxExt.Max.y += Extent.y;
	BoxExt.Max.z += Extent.z;

	const FVector Dir = End - Start;

	FVector Time;
	UBOOL bStartIsOutside = FALSE;
	FLOAT FaceDir[3] = {1, 1, 1};

	/////////// X
	if( Start.x < BoxExt.Min.x )
	{
		if( Dir.x <= 0.0f )
		{
			return FALSE;
		}
		else
		{
			bStartIsOutside = TRUE;
			FaceDir[0] = -1;
			Time.x = (BoxExt.Min.x - Start.x) / Dir.x;
		}
	}
	else if( Start.x > BoxExt.Max.x )
	{
		if( Dir.x >= 0.0f )
		{
			return FALSE;
		}
		else
		{
			bStartIsOutside = TRUE;
			Time.x = (BoxExt.Max.x - Start.x) / Dir.x;
		}
	}
	else
	{
		Time.x = 0.0f;
	}

	/////////// Y
	if( Start.y < BoxExt.Min.y )
	{
		if( Dir.y <= 0.0f )
		{
			return FALSE;
		}
		else
		{
			bStartIsOutside = TRUE;
			FaceDir[1] = -1;
			Time.y = (BoxExt.Min.y - Start.y) / Dir.y;
		}
	}
	else if( Start.y > BoxExt.Max.y )
	{
		if( Dir.y >= 0.0f )
		{
			return FALSE;
		}
		else
		{
			bStartIsOutside = TRUE;
			Time.y = (BoxExt.Max.y - Start.y) / Dir.y;
		}
	}
	else
	{
		Time.y = 0.0f;
	}

	/////////// Z
	if( Start.z < BoxExt.Min.z )
	{
		if( Dir.z <= 0.0f )
		{
			return FALSE;
		}
		else
		{
			bStartIsOutside = TRUE;
			FaceDir[2] = -1;
			Time.z = (BoxExt.Min.z - Start.z) / Dir.z;
		}
	}
	else if( Start.z > BoxExt.Max.z )
	{
		if( Dir.z >= 0.0f )
		{
			return FALSE;
		}
		else
		{
			bStartIsOutside = TRUE;
			Time.z = (BoxExt.Max.z - Start.z) / Dir.z;
		}
	}
	else
	{
		Time.z = 0.0f;
	}

	if( !bStartIsOutside )
	{
		HitLocation = Start;
		HitNormal = FVector(0, 0, 1);
		HitTime = 0.0f;
		return TRUE;
	}
	else
	{
		if( Time.y > Time.z )
		{
			HitTime = Time.y;
			HitNormal = FVector(0, FaceDir[1], 0);
		}
		else
		{
			HitTime = Time.z;
			HitNormal = FVector(0, 0, FaceDir[2]);
		}

		if( Time.x > HitTime )
		{
			HitTime = Time.x;
			HitNormal = FVector(FaceDir[0], 0, 0);
		}

		if( HitTime >= 0.0f && HitTime <= 1.0f )
		{
			HitLocation = Start + Dir * HitTime;
			const FLOAT THRESH_BOX_SIDE = 0.1f;
			if( HitLocation.x > BoxExt.Min.x - THRESH_BOX_SIDE && HitLocation.x < BoxExt.Max.x + THRESH_BOX_SIDE &&
				HitLocation.y > BoxExt.Min.y - THRESH_BOX_SIDE && HitLocation.y < BoxExt.Max.y + THRESH_BOX_SIDE &&
				HitLocation.z > BoxExt.Min.z - THRESH_BOX_SIDE && HitLocation.z < BoxExt.Max.z + THRESH_BOX_SIDE )
			{
				return TRUE;
			}
		}

		return FALSE;
	}
}

FQuat FQuatFindBetween(const FVector& V1, const FVector& V2)
{
	return FQuat(V1, V2);
}


FLOAT PointDistToLine(const FVector& Point, const FVector& Direction, const FVector& Origin, FVector& OutClosestPoint)
{
	const FVector SafeDir = Direction.SafeNormal();
	OutClosestPoint = Origin + (SafeDir * ((Point - Origin).DotProduct(SafeDir)));
	return (OutClosestPoint - Point).Size();
}

FLOAT PointDistToLine(const FVector& Point, const FVector& Direction, const FVector& Origin)
{
	const FVector SafeDir = Direction.SafeNormal();
	const FVector OutClosestPoint = Origin + (SafeDir * ((Point - Origin).DotProduct(SafeDir)));
	return (OutClosestPoint - Point).Size();
}

FLOAT PointDistToSegment(const FVector& Point, const FVector& StartPoint, const FVector& EndPoint, FVector& OutClosestPoint)
{
	const FVector Segment = EndPoint - StartPoint;
	const FVector StartToPoint = Point - StartPoint;
	const FLOAT Dot1 = StartToPoint.DotProduct(Segment);
	if( Dot1 <= 0.f )
	{
		OutClosestPoint = StartPoint;
		return StartToPoint.Size();
	}

	const FLOAT Dot2 = Segment.DotProduct(Segment);
	if( Dot2 <= Dot1 )
	{
		OutClosestPoint = EndPoint;
		return (Point - EndPoint).Size();
	}

	OutClosestPoint = StartPoint + Segment * (Dot1 / Dot2);
	return (Point - OutClosestPoint).Size();
}

FVector ClosestPointOnTriangleToPoint(const FVector& Point, const FVector& A, const FVector& B, const FVector& C)
{
	const FVector BA = A - B;
	const FVector AC = C - A;
	const FVector CB = B - C;
	const FVector TriNormal = BA.CrossProduct(CB);

	const FPlane Planes[3] = { FPlane(B, TriNormal.CrossProduct(BA)), FPlane(A, TriNormal.CrossProduct(AC)), FPlane(C, TriNormal.CrossProduct(CB)) };
	INT BitmaskHalfPlane = 0;

	for(INT i = 0; i < 3; ++i)
	{
		if( Planes[i].PlaneDot(Point) > 0.f )		// > 0,点位于平面反面
		{
			BitmaskHalfPlane |= (1 << i);
		}
	}

	FVector Result = Point;
	switch(BitmaskHalfPlane)
	{
	case 0:		// 000	inside
		return FPointPlaneProject(Point, A, B, C);
	case 1:		// 001	positive segment BA
		PointDistToSegment(Point, B, A, Result);
		break;
	case 2:		// 010	positive segment AC
		PointDistToSegment(Point, A, C, Result);
		break;
	case 3:		// 011	point A
		return A;
	case 4:		// 100	positive segment BC
		PointDistToSegment(Point, B, C, Result);
		break;
	case 5:		// 101	point B
		return B;
	case 6:		// 110	point C
		return C;
	default:
		debugf(TEXT("Impossible result!"));
		break;
	}
	return Result;
}

FVector VInterpNormalRotationTo(const FVector& Current, const FVector& Target, FLOAT DeltaTime, FLOAT RotationSpeedDegrees)
{
	FQuat DeltaQuat = FQuatFindBetween(Current, Target);

	FVector DeltaAxis;
	FLOAT DeltaAngle;
	DeltaQuat.ToAxisAngle(DeltaAxis, DeltaAngle);

	const FLOAT RotationStepRadians = RotationSpeedDegrees * (PI / 180.f) * DeltaTime;
	if( Abs(DeltaAngle) > RotationStepRadians )
	{
		DeltaAngle = Clamp(DeltaAngle, -Abs(RotationStepRadians), Abs(RotationStepRadians));
		DeltaQuat = FQuat(DeltaAxis, DeltaAngle);
		return DeltaQuat.RotateVector(Current);
		//return FQuatRotationTranslationMatrix(DeltaQuat, FVector(0.f)).TransformNormal(Current);
	}
	return Target;
}

FVector VInterpConstantTo(const FVector& Current, const FVector& Target, FLOAT DeltaTime, FLOAT InterpSpeed)
{
	const FVector Delta = Target - Current;
	const FLOAT DeltaM = Delta.Size();
	const FLOAT ConstantStep = DeltaTime * InterpSpeed;

	if( DeltaM > ConstantStep )
	{
		if( ConstantStep > 0.f )
		{
			const FVector DeltaNormal = Delta / DeltaM;
			return Current + DeltaNormal * ConstantStep;
		}
		else
		{
			return Current;
		}
	}
	return Target;
}

FVector VInterpTo(const FVector& Current, const FVector& Target, FLOAT DeltaTime, FLOAT InterpSpeed)
{
	if( InterpSpeed <= 0.f )
	{
		return Target;
	}

	const FVector Delta = Target - Current;
	if( Delta.SizeSquared() < KINDA_SMALL_NUMBER )
	{
		return Target;
	}

	const FVector DeltaMove = Delta * Clamp(DeltaTime * InterpSpeed, 0.f, 1.f);
	return Current + DeltaMove;
}

FRotator RInterpTo(const FRotator& Current, const FRotator& Target, FLOAT DeltaTime, FLOAT InterpSpeed)
{
	if( DeltaTime == 0.f || Current == Target )
	{
		return Current;
	}

	if( InterpSpeed <= 0.f )
	{
		return Target;
	}

	const FLOAT ConstantStep = DeltaTime * InterpSpeed;
	const FRotator DeltaMove = (Target - Current).Normalize() * Clamp(ConstantStep, 0.f, 1.f);

	if( DeltaMove.IsZero() )
	{
		return Target;
	}

	return (Current + DeltaMove).Normalize();
}

FRotator RInterpTo(const FRotator& Current, const FRotator& Target, FLOAT Alpha, UBOOL bShortestPath)
{
	FRotator DeltaRot = Target - Current;
	if( bShortestPath )
	{
		DeltaRot = DeltaRot.Normalize();
	}
	return Current + Alpha * DeltaRot;
}

FLOAT FInterpTo(FLOAT Current, FLOAT Target, FLOAT DeltaTime, FLOAT InterpSpeed)
{
	if( InterpSpeed == 0.f )
	{
		return Target;
	}

	const FLOAT Delta = Target - Current;

	if( Delta * Delta < SMALL_NUMBER )
	{
		return Target;
	}

	const FLOAT DeltaMove = Delta * Clamp(DeltaTime * InterpSpeed, 0.f, 1.f);
	return Current + DeltaMove;
}

FLOAT FInterpEaseInOut(FLOAT A, FLOAT B, FLOAT Alpha, FLOAT Exp)
{
	FLOAT ModifiedAlpha;

	if( Alpha < 0.5f )
	{
		ModifiedAlpha = 0.5f * appPow(2.f * Alpha, Exp);
	}
	else
	{
		ModifiedAlpha = 1.f - 0.5f * appPow(2.f * (1.f - Alpha), Exp);
	}

	return Lerp<FLOAT>(A, B, ModifiedAlpha);
}

FLOAT FInterpEaseIn(FLOAT A, FLOAT B, FLOAT Alpha, FLOAT Exp)
{
	return Lerp<FLOAT>(A, B, appPow(Alpha, Exp));
}

FLOAT FInterpEaseOut(FLOAT A, FLOAT B, FLOAT Alpha, FLOAT Exp)
{
	return Lerp<FLOAT>(A, B, appPow(Alpha, 1.f / Exp));
}

////////////////////////// FBoneAtom Implementation //////////////////////////
const FBoneAtom FBoneAtom::Identity(FQuat(0.f, 0.f, 0.f, 1.f), FVector(0.f, 0.f, 0.f), 1.f);