//Yeah... I know this looks completely crazy. Here I just declare my variables and define them in the definition source file accordingly, the reason for this is that if I would define them here when including my header file
//into my 2 sources, the variables will have duplicated definitions in 2 different locations which you CANNOT do... . What I can do is declare my variables so that I can use them in other source files and define them (allocate them memory)
//in the definition source file so that I avoid have more than one definitions :)

#pragma once
#include <Windows.h>
#include <iostream>
#include <thread>
#include <ctime> 

extern bool NoRecoilStatus;

extern int NoRecoilKey;
extern std::string keyNames[];
extern int currentKeyIndex;
extern int count;
extern int returnBackSmoothness;
extern int returnDelay;
extern int CurrentSmoothnessIndex;
extern int CurrentSmoothness;
extern int CurrentSmoothnessDelay[2];
extern std::string CurrentSmoothnessName;
extern int CurrentWeaponIndex;
extern int CurrentSize;
extern int* CurrentRawWeaponX;
extern int* CurrentRawWeaponY;
extern int* CurrentWeaponX;
extern int* CurrentWeaponY;
extern std::string CurrentGunName;
extern float CS2sensitivity;
extern bool randomizer;
extern double randomNumber;
extern bool returnBackAfterShooting;

namespace SmoothnessConfiguration {
	//AK47
	namespace A {
		extern int rigid;
		extern int rigidDelay[2];
		extern int semiRigid;
		extern int semiRigidDelay[2];
		extern int soft;
		extern int softDelay[2];
	}
	//M4A4
	namespace B {
		extern int rigid;
		extern int rigidDelay[2];
		extern int semiRigid;
		extern int semiRigidDelay[2];
		extern int soft;
		extern int softDelay[2];
	}
	extern int* GunConfArray[];
}
//Someone would ask why are they named A B C, and I would say I have no fucking clue :|
namespace Guns {
	extern const int AmountOfGuns;
	//AK47
	namespace A {
		extern int size;
		extern int X[31];
		extern int Y[31];
	}
	//M4A4
	namespace B {
		extern int RPM;
		extern int size;
		extern int X[31];
		extern int Y[31];
	}
	//M4A1S
	namespace C {
		extern int RPM;
		extern int size;
		extern int X[31];
		extern int Y[31];
	}
}
void MainThread();

void returnBackAfterComp(int* X, int* Y, int FullfinishOrMidFinish, int delay);

void SmoothMovementMove(int* X, int* Y, int* delay, int smoothness);

void moveMouse(int x, int y);

void ScrollThroughSmoothness();

void ScrollThroughWeapons();

void DisplayStatusConfig(int StatusIndex);

void SwitchKeyBind();

int FindTotalDisplacement(int* EitherXorY, int CountValueStoppedAt, int XorY);

double generateNumber();

//notes: if GetAsyncKeyState is & 8000 or < 0 it will mean the key is being held