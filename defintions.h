#pragma once




void returnBackAfterComp(int* X, int* Y, int FullfinishOrMidFinish, int delay);

void SmoothMovementMove(int* X, int* Y, int* delay, int smoothness);

void moveMouse(int x, int y);

void ScrollThroughSmoothness();

void ScrollThroughWeapons();

void DisplayStatusConfig();

int FindTotalDisplacement(int* EitherXorY, int CountValueStoppedAt, int XorY);

double generateNumber();



//the delays of after each shot (it is not uniform because we must include Overhead compensation because depending on which smoothness you have the more function calls you will have to make)
//some smoothness types need more than one delays such as the soft one (delay was optained through trial and error)


namespace SmoothnessConfiguration {

	//AK47
	//6 elements per gun
	namespace A {
		int rigid = 1;
		int rigidDelay[2] = { 100000, 0 };

		int semiRigid = 2;
		int semiRigidDelay[2] = { 44500, 0 };

		int soft = 5;
		int softDelay[2] = { 4000, 20000 };
	}

	

	//M4A4 (WORK IN PROGRESS)
	namespace B {
		int rigid = 1;
		int rigidDelay[2] = { 90000, 0 };

		int semiRigid = 2;
		int semiRigidDelay[2] = { 44500, 0 };

		int soft = 5;
		int softDelay[2] = { 4000, 20000 };

	}


	//I store all gun delay/smoothness configuration in this array for accesibility (bullshit, but this basically allows me to switch smoothness whenwever I want with whatever gun selected) each gun has 6 core settings for smoothness and delays
	//for each new gun added this array will grow by 6 elements (manually will be added ofcourse...)
	int* GunConfArray[] = { &A::rigid,  A::rigidDelay, &A::semiRigid, A::semiRigidDelay, &A::soft, A::softDelay,  &B::rigid, B::rigidDelay, &B::semiRigid, B::semiRigidDelay, &B::soft, B::softDelay };
}





namespace Guns {
	const int AmountOfGuns = 2;

	//(I use the ascii table to switch between guns)

	//AK47
	namespace A {
		//double TotalDelayPerShot = (60.0f / RPM) * 1000.0f;
		//int RPM = 600;
		int size = 31;
		//int X[31] = { 0, 0, 0, 0, 20, 30, 10, -40, -90, -30, -20, -20, -20, 0, 80, 30, 50, 50, 30, 20, -20, -10, 0, 10, 0, -40, -90, -70, -30, 0 };
		//int Y[31] = {0, 30, 40, 70, 90, 70, 90, 30, 20, -10, 20, 0, 0, -10, 20, 30, -10, 20, 0, -10, -10, 10, 10, 10, 0, 10, 0, 0, -50, 10, 0 };
		int X[31] = { 0, 0, 0, 0, 20, 30, 10, -40, -90, -30, -20, -20, -20, 0, 80, 30, 50, 50, 30, 20, -20, -10, 0, 10, 0, -40, -90, -70, -30, 0 };
		int Y[31] = { 0, 40, 70, 90, 70, 80, 70, 20, -10, 20, 0, 0, -10, 20, 30, -10, 20, 0, -10, -10, 10, 10, 10, 0, 10, -10, 0, -50, 10, 0 };
		

	}

	//M4A4 (WORK IN PROGRESS)
	namespace B {
		int RPM = 666;
		int size = 31;
		int X[31] = { 0, 0, 0, 0, -10, 10, 20, 20, 30, -40, -40, -40, -40, -40, -50, 0, 30, 30, 20, 60, 30, 40, 20, 10, 0, 0, 10, 10, 0, 0, 0 };
		int Y[31] = { 10, 30, 40, 40, 60, 60, 60, 30, 20, 20,  20, 0, -10, 0, 10, 10, 0, 0, 0, 10, 0, 0, 10, 0, 10, 10, 0, 0, 0, 0, 0 };
	}
}

//notes: if GetAsyncKeyState is & 8000 or < 0 it will mean the key is being held
