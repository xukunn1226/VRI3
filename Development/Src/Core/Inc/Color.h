#ifndef __COLOR_H__
#define __COLOR_H__

class FColor;

class FLinearColor
{
public:
	BYTE	B, G, R, A;

	DWORD& DWColor() { return *((DWORD*)this); }
	const DWORD& DWColor() const { return *((DWORD*)this); }

	FLinearColor() {}
	FLinearColor(BYTE InR, BYTE InG, BYTE InB, BYTE InA = 255)
		: R(InR), G(InG), B(InB), A(InA)
	{}
	FLinearColor(const FColor& C);

	friend FArchive& operator<< (FArchive& Ar, FLinearColor& C)
	{
		Ar << C.DWColor();
	}

	UBOOL operator== (const FLinearColor& C) const
	{
		return DWColor() == C.DWColor();
	}

	UBOOL operator!= (const FLinearColor& C) const
	{
		return DWColor() != C.DWColor();
	}

	FLinearColor& operator+= (const FLinearColor& C)
	{
		B = (BYTE)Min((INT)B + (INT)C.B, 255);
		G = (BYTE)Min((INT)G + (INT)C.G, 255);
		R = (BYTE)Min((INT)R + (INT)C.R, 255);
		A = (BYTE)Min((INT)A + (INT)C.A, 255);
		return *this;
	}

	static FLinearColor MakeRandomColor();
};

class FColor
{
public:
	FLOAT	R, G, B, A;

	FColor() {}
	FColor(FLOAT InR, FLOAT InG, FLOAT InB, FLOAT InA = 1.0f)
		: R(InR), G(InG), B(InB), A(InA)
	{}
	FColor(const FLinearColor& C)
		: R(appPow(C.R/255.0f, 2.2f)), G(appPow(C.G/255.0f, 2.2f)), B(appPow(C.B/255.0f, 2.2f)), A(C.A/255.0F)
	{
	}
	
	friend FArchive& operator<< (FArchive& Ar, FColor& Color)
	{
		return Ar << Color.R << Color.G << Color.B << Color.A;
	}

	FLOAT& operator[] ( INT i )
	{
		check( i > -1 && i < 4 );
		return (&R)[i];
	}

	const FLOAT& operator[] ( INT i ) const
	{
		check( i > -1 && i < 4 );
		return (&R)[i];
	}

	FColor operator+ (const FColor& C) const
	{
		return FColor(
						this->R + C.R,
						this->G + C.G,
						this->B + C.B,
						this->A + C.A
					 );
	}

	FColor& operator+= (const FColor& C)
	{
		R += C.R;
		G += C.G;
		B += C.B;
		A += C.A;
		return *this;
	}

	FColor operator- (const FColor& C) const
	{
		return FColor(
						this->R - C.R,
						this->G - C.G,
						this->B - C.B,
						this->A - C.A
					 );
	}

	FColor& operator-= (const FColor& C)
	{
		R -= C.R;
		G -= C.G;
		B -= C.B;
		A -= C.A;
		return *this;
	}

	FColor operator* (const FColor& C) const
	{
		return FColor(
						R * C.R,
						G * C.G,
						B * C.B,
						A * C.A
						);
	}

	FColor& operator*= (const FColor& C)
	{
		R *= C.R;
		G *= C.G;
		B *= C.B;
		A *= C.A;
		return *this;
	}

	FColor operator* (FLOAT Scalar) const
	{
		return FColor(
						R * Scalar,
						G * Scalar,
						B * Scalar,
						A * Scalar
						);
	}

	friend FColor operator* (FLOAT Scalar, const FColor& C)
	{
		return C * Scalar;
	}

	FColor& operator*= (FLOAT Scalar)
	{
		R *= Scalar;
		G *= Scalar;
		B *= Scalar;
		A *= Scalar;
		return *this;
	}

	FColor operator/ (FLOAT Scalar) const
	{
		const FLOAT InvScalar = 1.0f / Scalar;
		return FColor(
						R * InvScalar,
						G * InvScalar,
						B * InvScalar,
						A * InvScalar
						);
	}

	FColor& operator/= (FLOAT Scalar)
	{
		const FLOAT InvScalar = 1.0f / Scalar;
		R *= InvScalar;
		G *= InvScalar;
		B *= InvScalar;
		A *= InvScalar;
		return *this;
	}

	UBOOL operator== (const FColor& C) const
	{
		return R == C.R && G == C.G && B == C.B && A == C.A;
	}

	UBOOL operator!= (const FColor& C) const
	{
		return R != C.R || G != C.G || B != C.B || A != C.A;
	}

	UBOOL EqualTo(const FColor& C, FLOAT Tolerance = KINDA_SMALL_NUMBER) const
	{
		return Abs(R - C.R) < Tolerance && Abs(G - C.G) < Tolerance && Abs(B - C.B) < Tolerance && Abs(A - C.A) < Tolerance;
	}

	FLinearColor Quantize() const
	{
		return FLinearColor((BYTE)Clamp((INT)(R * 255.0f), 0, 255),
							(BYTE)Clamp((INT)(G * 255.0f), 0, 255),
							(BYTE)Clamp((INT)(B * 255.0f), 0, 255),
							(BYTE)Clamp((INT)(A * 255.0f), 0, 255));
	}

	/** 
	 *	return a desaturated color, with 0 meaning no desaturation & 1 meaning full desaturation
	 *	@param	Desaturation		Desaturation factor in range[0...1]
	 *	@return						Desaturation color
	 */
	FColor Desaturate(FLOAT Desaturation) const;

	FLinearColor ConvertToLinear() const;

	static FColor ConvertToColor(const FLinearColor& C);
	static const FColor White;
	static const FColor Black;
};


#endif