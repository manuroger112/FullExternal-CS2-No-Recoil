#include "defintions.h"

//GLOBALS DEFINITION
bool NoRecoilStatus = true;

int NoRecoilKey = VK_LBUTTON;
std::string keyNames[] = { "LBUTTON", "RBUTTON",  "MBUTTON", "X1BUTTON", "X2BUTTON", "BACKSPACE", "CTRL", "ALT", "X", "Y", "Z" };
int currentKeyIndex = 1;

int count = 0;
int returnBackSmoothness = 15;
int returnDelay = 1;

int CurrentSmoothnessIndex = 1;
int CurrentSmoothness = 1;
int CurrentSmoothnessDelay[2] = { 100000, 0 };
std::string CurrentSmoothnessName = " ";

int CurrentWeaponIndex = 1;
int CurrentSize = 0;
int* CurrentRawWeaponX = nullptr;
int* CurrentRawWeaponY = nullptr;
int* CurrentWeaponX = nullptr;
int* CurrentWeaponY = nullptr;
std::string CurrentGunName = " ";

float CS2sensitivity = 0.0f;
bool randomizer = false;
double randomNumber = 1.0f;

bool returnBackAfterShooting = true;

//the delays of after each shot (it is not uniform because we must include Overhead compensation because depending on which smoothness you have the more function calls you will have to make)
//some smoothness types need more than one delays such as the soft one (delay was optained through trial and error)
namespace SmoothnessConfiguration {
	//AK47
	//6 elements per gun
	namespace A {
		int rigid = 1;
		int rigidDelay[2] = { 100000, 0 };

		int semiRigid = 2;
		int semiRigidDelay[2] = { 25000, 40000 };

		int soft = 5;
		int softDelay[2] = { 4000, 20000 };
	}
	//M4A4
	namespace B {
		int rigid = 1;
		int rigidDelay[2] = { 90000, 0 };

		int semiRigid = 2;
		int semiRigidDelay[2] = { 44500, 0 };

		int soft = 5;
		int softDelay[2] = { 4000, 10000 };

	}
	//M4A1S
	namespace C {
		int rigid = 1;
		int rigidDelay[2] = { 90000, 0 };

		int semiRigid = 2;
		int semiRigidDelay[2] = { 44500, 0 };

		int soft = 5;
		int softDelay[2] = { 4000, 10000 };

	}

	//I store all gun delay/smoothness configuration in this array for accesibility (bullshit, but this basically allows me to switch smoothness whenever I want with whatever gun selected) each gun has 6 core settings for smoothness and delays
	//for each new gun added this array will grow by 6 elements (manually will be added ofcourse...)
	int* GunConfArray[] = { &A::rigid,  A::rigidDelay, &A::semiRigid, A::semiRigidDelay, &A::soft, A::softDelay,  &B::rigid, B::rigidDelay, &B::semiRigid, B::semiRigidDelay, &B::soft, B::softDelay, &C::rigid, C::rigidDelay, &C::semiRigid, C::semiRigidDelay, &C::soft, C::softDelay };
}

namespace Guns {
	const int AmountOfGuns = 3;
	//AK47
	namespace A {
		//double TotalDelayPerShot = (60.0f / RPM) * 1000.0f;
		int size = 31;

		int RawX[31] = { 0, 0, 0, 0, 0, 40, 40, -40, -90, -30, -20, -20, -20, 0, 80, 30, 50, 50, 30, 20, -20, -10, 0, 10, 0, -40, -90, -70, -30, -10, 0 };
		int RawY[31] = { 0, 40, 40, 80, 80, 80, 80,  20, -10, 20, 0, 0, -10, 20, 30, -10, 20, 0, -10, -10, 10, 10, 10, 0, 10, -10, 0, -50, 10, -10, 0 };

		//Normalized compensation values array (array where the sensitivity configurated compensation values sit)
		int X[31];
		int Y[31];
	}
	//M4A4
	namespace B {
		int size = 31;

		int RawX[31] = { 0, 0, 0, 0, 0, -10, 10, 20, 20, 30, -40, -40, -40, -40, -40, -50, 0, 30, 30, 20, 60, 30, 40, 20, 10, 0, 0, 10, 10, 0, 0 };
		int RawY[31] = { 0, 10, 30, 40, 40, 60, 60, 60, 30, 20, 20,  20, 0, -10, 0, 10, 10, 0, 0, 0, 10, 0, 0, 10, 0, 10, 10, 0, 0, 0, 0 };

		//Normalized compensation values array (array where the sensitivity configurated compensation values sit)
		int X[31];
		int Y[31];
	}
	//M4A1-S
	namespace C {
		int size = 31;

		int RawX[31] = { 0, 0, 0, 0 , 0, -10, 0, 30, 10, 30, -10, -40, -20, -30, -20, -20, -30, -30, 10, -10, 0, 20, 40, 60, 10, 0 };
		int RawY[31] = { 0, 10, 10, 30 , 30, 40, 40, 50, 10, 10, 10, 20, 0, -10, 0, 0, -10, 0, 10, 0, 10, 0, 0, 20, 0, 0 };

		//Normalized compensation values array (array where the sensitivity configurated compensation values sit)
		int X[31];
		int Y[31];
	}
}



//Function definitions
//these shitty functions ARE CLEARLY NOT clean, they are messy and damn disgusting but they work :\


//This is used to find the total displacement from where you started to where you stopped shooting with norecoil, I use this to calculate where to actually return your mouse to after you shoot
int FindTotalDisplacement(int* EitherXorY, int CountValueStoppedAt, int XorY) {
	//Get the x and y relative displacement coordinates in order to return to right before you started using the no recoil

	//find x displacement relative to start if identifier (XorY) is 0 
	if (XorY == 0) {
		int TotalXDisplacement = 0;
		for (int i = 0; i < CountValueStoppedAt; i++) {
			for (int j = 0; j < CurrentSmoothness; j++) {
				TotalXDisplacement += EitherXorY[i] / CurrentSmoothness;
			}

		}
		TotalXDisplacement = -(TotalXDisplacement - EitherXorY[0]);
		return TotalXDisplacement;
	}

	//find y displacement relative to start if identifier (XorY) is 1 
	else if (XorY == 1) {
		int TotalYDisplacement = 0;
		for (int i = 0; i < CountValueStoppedAt; i++) {
			for (int j = 0; j < CurrentSmoothness; j++) {
				TotalYDisplacement += EitherXorY[i] / CurrentSmoothness;
			}
		}
		TotalYDisplacement = -(TotalYDisplacement - EitherXorY[0]);
		return TotalYDisplacement;
	}

	return 0;
}

void returnBackAfterComp(int* X, int* Y, int FullfinishOrMidFinish, int delay) {

	int DivisionXFix = 0;
	int DivisionYFix = 0;

	//After using NoRecoil the program tries to return back your aim to the spot right before NoRecoil started to shot, the return will never be completely accurate as there are innacuracies in the displacement smoothing division
	//since we are obligated work with integers when using SendInput so the division will be rounded up/down. 
	// This insane looking shit code finds the innacuracy value so that at the end of the return we can move our mouse once more to fix that innacuracy (UNDER)

	//dividing and then multiplying by the same thing shouldn't do shit, but what I am doing here actually is 
	//finding the true total displacement from where the shooting stopped to where is started, dividing with our smoothing
	//coefficient, and since we are working with integers will round the result of the division. I then multiply back with our smoothing coefficient, and when doing so
	//we will get a slightly different number from our displacement, from there we just subtract the true displacement by the mutated one in order to find what I like to call the recoil return innacuracy value

	//using this we can move our mouse by this innacuracy value after the full return movement to exactly return back from were you exactly started

	//0 for full finish 1 for midfinish 

	if (FullfinishOrMidFinish == 0) {

		DivisionXFix = FindTotalDisplacement(X, count + 1, 0) - ((FindTotalDisplacement(X, count + 1, 0) / returnBackSmoothness) * returnBackSmoothness);
		DivisionYFix = FindTotalDisplacement(Y, count + 1, 1) - ((FindTotalDisplacement(Y, count + 1, 1) / returnBackSmoothness) * returnBackSmoothness);

	}

	else if (FullfinishOrMidFinish == 1) {
		DivisionXFix = FindTotalDisplacement(X, count + 1, 0) - ((FindTotalDisplacement(X, count + 1, 0) / returnBackSmoothness) * returnBackSmoothness);
		DivisionYFix = FindTotalDisplacement(Y, count + 1, 1) - ((FindTotalDisplacement(Y, count + 1, 1) / returnBackSmoothness) * returnBackSmoothness);
	}


	for (int i = 0; i < returnBackSmoothness; i++) {

		moveMouse(static_cast<int>((FindTotalDisplacement(X, count + 1, 0) / returnBackSmoothness) / randomNumber), static_cast<int>((FindTotalDisplacement(Y, count + 1, 1) / returnBackSmoothness) / randomNumber));
		std::this_thread::sleep_for(std::chrono::milliseconds(delay));

	}
	//fix innacuracy at end of return because c++ rounds decimals
	moveMouse(DivisionXFix / randomNumber, DivisionYFix / randomNumber);


}

//Self explanitory..
void SmoothMovementMove(int* X, int* Y, int* delay, int smoothness) {

	for (int i = 0; i < smoothness; i++) {

		std::this_thread::sleep_for(std::chrono::microseconds(delay[0]));
		moveMouse(static_cast<int>((X[count] / smoothness) / randomNumber), static_cast<int>((Y[count] / smoothness) / randomNumber));


	}
	std::this_thread::sleep_for(std::chrono::microseconds(delay[1]));
}

void moveMouse(int NormalizedX, int NormalizedY) {
	INPUT input = {};
	input.type = INPUT_MOUSE;
	input.mi.dx = NormalizedX;
	input.mi.dy = NormalizedY;
	input.mi.dwFlags = MOUSEEVENTF_MOVE;
	input.mi.time = 0;
	input.mi.mouseData = 0;

	SendInput(1, &input, sizeof(INPUT));
}

//Generate a random number between 0.8 and 1.2 so that I make the compensation a little random to avoid detection ( avoiding detection by doing this shit is just a complete assumption :\ )
double generateNumber() {
	double randomValue = static_cast<double>(rand()) / RAND_MAX;

	return 0.8 + randomValue * (1.2 - 0.8);
}

//handles everything about switching smoothness (not just for "scrolling")
void ScrollThroughSmoothness() {

	//this shit is just used to make the changes of the Smoothness based on what gun you have (it is used to accordingly index the array for the right elements of the right gun)
	int CurrentGunHandlerIndex = (CurrentWeaponIndex - 1) * 6;

	switch (CurrentSmoothnessIndex) {

	case 1: {
		CurrentSmoothnessName = "Rigid";
		CurrentSmoothness = *SmoothnessConfiguration::GunConfArray[CurrentGunHandlerIndex + 0];
		CurrentSmoothnessDelay[0] = SmoothnessConfiguration::GunConfArray[CurrentGunHandlerIndex + 1][0];
		CurrentSmoothnessDelay[1] = SmoothnessConfiguration::GunConfArray[CurrentGunHandlerIndex + 1][1];
		break;
	}
	case 2: {
		CurrentSmoothnessName = "Semi Rigid";
		CurrentSmoothness = *SmoothnessConfiguration::GunConfArray[CurrentGunHandlerIndex + 2];
		CurrentSmoothnessDelay[0] = SmoothnessConfiguration::GunConfArray[CurrentGunHandlerIndex + 3][0];
		CurrentSmoothnessDelay[1] = SmoothnessConfiguration::GunConfArray[CurrentGunHandlerIndex + 3][1];
		break;
	}
	case 3: {
		CurrentSmoothnessName = "Soft";
		CurrentSmoothness = *SmoothnessConfiguration::GunConfArray[CurrentGunHandlerIndex + 4];
		CurrentSmoothnessDelay[0] = SmoothnessConfiguration::GunConfArray[CurrentGunHandlerIndex + 5][0];
		CurrentSmoothnessDelay[1] = SmoothnessConfiguration::GunConfArray[CurrentGunHandlerIndex + 5][1];
		break;
	}
	}
	DisplayStatusConfig(1);
}

//handles everything about switching weapons (not just for "scrolling")
void ScrollThroughWeapons() {

	switch (CurrentWeaponIndex) {

	case 1: {
		CurrentGunName = "AK47";
		CurrentRawWeaponX = Guns::A::RawX;
		CurrentRawWeaponY = Guns::A::RawY;
		CurrentWeaponX = Guns::A::X;
		CurrentWeaponY = Guns::A::Y;
		CurrentSize = Guns::A::size;
		break;
	}

	case 2: {
		CurrentGunName = "M4A4";
		CurrentRawWeaponX = Guns::B::RawX;
		CurrentRawWeaponY = Guns::B::RawY;
		CurrentWeaponX = Guns::B::X;
		CurrentWeaponY = Guns::B::Y;
		CurrentSize = Guns::B::size;
		break;
	}


	case 3: {

		CurrentGunName = "M4A1-S";
		CurrentRawWeaponX = Guns::C::RawX;
		CurrentRawWeaponY = Guns::C::RawY;
		CurrentWeaponX = Guns::C::X;
		CurrentWeaponY = Guns::C::Y;
		CurrentSize = Guns::C::size;
		break;
	}


	//for future guns implementation |
	//                               V

	case 4: {



		break;
	}


	}

	//Change the recoil compensation pattern values based on your ingame sensitivity
	for (int k = 0; k < CurrentSize; k++) {
		CurrentWeaponX[k] = CurrentRawWeaponX[k] / CS2sensitivity;
		CurrentWeaponY[k] = CurrentRawWeaponY[k] / CS2sensitivity;

		//round to the nearest 10th as the format of recoil comp values are set in 10s only
		CurrentWeaponX[k] = std::round(10 * CurrentWeaponX[k]) / 10;
		CurrentWeaponY[k] = std::round(10 * CurrentWeaponY[k]) / 10;
	}

	//changed the gun so now we must set the smoothness settings for that gun accordingly (delays etc..)
	ScrollThroughSmoothness();
	DisplayStatusConfig(1);
}

void SwitchKeyBind() {

	DisplayStatusConfig(2);
	//Here I just choose probably the most common used keybinds, took their virtual key code which windows uses to determine which key is pressed and made it so that you can select the key you preferably want
	while (true) {

		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		if (GetAsyncKeyState(VK_RIGHT) & 1) {
			currentKeyIndex++;
			if (currentKeyIndex == 12) {
				currentKeyIndex = 1;
			}

			DisplayStatusConfig(2);
		}

		else if (GetAsyncKeyState(VK_LEFT) & 1) {
			currentKeyIndex--;
			if (currentKeyIndex == 0) {
				currentKeyIndex = 11;
			}

			DisplayStatusConfig(2);
		}
		//0x48 is for H
		else if (GetAsyncKeyState(0x48) & 1) {
			break;
		}



	}

	switch (currentKeyIndex) {

		//based on the current key index I switch my key eg: index 1 so key is set to 1 or LBUTTON
		//initially the code keys were in hex but I found it easier to convert them in decimal

	case 1: {
		NoRecoilKey = 1;
		break;
	}

	case 2: {
		NoRecoilKey = 2;
		break;
	}

	case 3: {
		NoRecoilKey = 4;
		break;
	}

	case 4: {
		NoRecoilKey = 5;
		break;
	}

	case 5: {
		NoRecoilKey = 6;
		break;
	}

	case 6: {
		NoRecoilKey = 8;
		break;
	}

	case 7: {
		NoRecoilKey = 17;
		break;
	}

	case 8: {
		NoRecoilKey = 18;
		break;
	}

	case 9: {
		NoRecoilKey = 88;
		break;
	}

	case 10: {
		NoRecoilKey = 89;
		break;
	}

	case 11: {
		NoRecoilKey = 90;
		break;
	}



	}


}

//Goofy argument to determine what kind of status information to display
void DisplayStatusConfig(int StatusIndex) {

	if (StatusIndex == 1) {
		system("cls");
		std::cout << "CS2Recoilcomper 1.0 Press H for keybind Info" << std::endl;
		std::cout << "-------------------------------------------" << std::endl;
		std::cout << "|Current weapon selection: " << CurrentGunName << std::endl;
		std::cout << "|Current smoothness selection: " << CurrentSmoothnessName << std::endl;
		std::cout << "|Randomizer is set to " << randomizer << std::endl;
		std::cout << "|Returning after shooting is set to " << returnBackAfterShooting << std::endl;
		std::cout << "|Current NoRecoil Key: " << keyNames[currentKeyIndex - 1] << std::endl;
		std::cout << "-------------------------------------------" << std::endl;
	}

	else if (StatusIndex == 2) {
		system("cls");

		std::cout << "Static set keybinds :\n \nF1 Cycle Guns, F2 Cycle Smoothness,\nF3 Enable/Disable Randomness, \nF4 Enable/Disable Aim Return, \nEND Quit \n";

		std::cout << "\n \n \nKEY SWITCHING MODE, GO THROUGH OPTIONS USING LEFT/RIGHT ARROW KEYS \n";
		std::cout << "TO APPLY CHANGES TO CURRENT SELECTION/EXIT, PRESS AGAIN H \n";
		std::cout << "\n-----NoRecoil Key-----\n";
		std::cout << "Current selection : " << keyNames[currentKeyIndex - 1] << std::endl;

	}

	else {
		std::cout << "Uknown Status Display Index \n";
	}

}


void MainThread() {

	while (true) {

		if (NoRecoilStatus == false) {
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			continue;
		}

		//Handle what happens if you stop using the norecoil before finishing the whole magazine
		if (!(GetAsyncKeyState(NoRecoilKey) < 0)) {

			if (count != 0) {
				if (returnBackAfterShooting == true) {
					returnBackAfterComp(CurrentWeaponX, CurrentWeaponY, 0, returnDelay);
				}
				count = 0;
				if (randomizer == true) {
					randomNumber = generateNumber();
				}
				continue;
			}
		}

		//Handle what happens if norecoil was kept throughout the whole magazine
		else if (GetAsyncKeyState(NoRecoilKey) < 0) {


			//game glitches at max array value right so I just subtract 1 so it doesnt write the last (ghetto shit)
			if (count == CurrentSize - 1) {

				if (returnBackAfterShooting == true) {
					returnBackAfterComp(CurrentWeaponX, CurrentWeaponY, 0, returnDelay);
				}
				count = 0;
				if (randomizer == true) {
					randomNumber = generateNumber();
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
				continue;
			}

			count++;
			SmoothMovementMove(CurrentWeaponX, CurrentWeaponY, CurrentSmoothnessDelay, CurrentSmoothness);

		}
	}

	
}