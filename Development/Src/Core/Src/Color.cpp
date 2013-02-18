#include "CorePCH.h"
#include "Color.h"

const FColor FColor::White(1.0f, 1.0f, 1.0f);
const FColor FColor::Black(0.0f, 0.0f, 0.0f);


FColor FColor::Desaturate(FLOAT Desaturation) const
{
	FLOAT L = R * 0.3f + G * 0.59f + B * 0.11f;
	return Lerp(*this, FColor(L, L, L, 0.0f), Desaturation);
}

FLinearColor FColor::ConvertToLinear() const
{
	return FLinearColor(Clamp( (INT)(appPow(R, 1.0f/2.2f)),	0, 255 ),
						Clamp( (INT)(appPow(G, 1.0f/2.2f)),	0, 255 ),
						Clamp( (INT)(appPow(B, 1.0f/2.2f)),	0, 255 ),
						Clamp( (INT)(A * 255.0f),			0, 255 ));
}

FColor FColor::ConvertToColor(const FLinearColor &C)
{
	return FColor(C.R/255.0f, C.G/255.0f, C.B/255.0f, C.A/255.0f);
}

FLinearColor::FLinearColor(const FColor& C)
:	A(Clamp((INT)(C.A * 255.0f), 0, 255)),
	R(Clamp((INT)(appPow(C.R, 1.0f/2.2f) * 255.0f), 0, 255)),
	G(Clamp((INT)(appPow(C.G, 1.0f/2.2f) * 255.0f), 0, 255)),
	B(Clamp((INT)(appPow(C.B, 1.0f/2.2f) * 255.0f), 0, 255))
{}

FLinearColor FLinearColor::MakeRandomColor()
{
	FLinearColor C;
	C.B = appRand() % 255;
	C.G = appRand() % 255;
	C.G = appRand() % 255;
	C.A = appRand() % 255;
	return C;
}