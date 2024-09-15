#include <Windows.h>
#include <iostream>
#include <thread>
#include <ctime>  
#include "defintions.h"

//for now there are 2 weapon options, AK47 and M4A4 

//initially you start with an AK47 Rigid recoil control with returning back after shooting configuration but from there you can change it

//GLOBALS
//VK_MENU
int NoRecoilKey = VK_LBUTTON;

int count = 0;
int returnBackSmoothness = 15;
int returnDelay = 1;

int CurrentSmoothnessIndex = 1;
int CurrentSmoothness = SmoothnessConfiguration::A::rigid;
int CurrentSmoothnessDelay[2] = {SmoothnessConfiguration::A::rigidDelay[0], SmoothnessConfiguration::A::rigidDelay[1]};
std::string CurrentSmoothnessName = " ";

int CurrentWeaponIndex = 1;
int CurrentSize = 0;
int* CurrentWeaponX = nullptr;
int* CurrentWeaponY = nullptr;
std::string CurrentGunName = " ";

float CS2sensitivity;
bool randomizer = false;
double randomNumber = 1.0f;

bool returnBackAfterShooting = true;

int main() {
	
	//some random ass effect
	for (int i = 0; i < 15; i++) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		std::cout << char(91 + i);
		system("cls");
	}

	std::cout << "Please enter your CS2 sensitivity: ";
	std::cin >> CS2sensitivity;
	
	ScrollThroughWeapons();

	//seed time bullshit for the randomizer
	srand(static_cast<unsigned int>(time(0)));
	
	while (true) {
		
		
		//Close program
		if (GetAsyncKeyState(VK_END) & 1) {
			std::cout << "!Goodbye!" << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(300));
			break;
		}

		//Changes the weapon
		else if (GetAsyncKeyState(VK_F1) & 1) {
			
			CurrentWeaponIndex++;
			if (CurrentWeaponIndex == Guns::AmountOfGuns + 1) {
				CurrentWeaponIndex = 1;
			}
			ScrollThroughWeapons();
			
		}

		//Changes the smoothness
		else if (GetAsyncKeyState(VK_F2) & 1) {
			
			CurrentSmoothnessIndex++;
			if (CurrentSmoothnessIndex == 4) {
				CurrentSmoothnessIndex = 1;
			}
			ScrollThroughSmoothness();
		}
				
		//enables shitty randomizer
		else if (GetAsyncKeyState(VK_F3) & 1) {
			randomizer = !randomizer;

			if (randomizer == false) {
				randomNumber = 1;
			}

			DisplayStatusConfig();
		}
		
		else if (GetAsyncKeyState(VK_F4) & 1) {

			returnBackAfterShooting = !returnBackAfterShooting;
			DisplayStatusConfig();
			
		}

		//Handle what happens if you stop using the norecoil before finishing the whole magazine
		else if (!(GetAsyncKeyState(NoRecoilKey) < 0)) {

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
			if (count == CurrentSize -1) {
				
				if (returnBackAfterShooting == true) {
					returnBackAfterComp(CurrentWeaponX, CurrentWeaponY, 0, returnDelay);
				}
				count = 0;
				if (randomizer == true) {
					randomNumber = generateNumber();
				}
				continue;
			}

			count++;
			SmoothMovementMove(CurrentWeaponX, CurrentWeaponY, CurrentSmoothnessDelay, CurrentSmoothness);
			
		}	
	}
	return  0;
}


//Function definitions
//these shitty functions ARE CLEARLY NOT fucking clean, they are messy and fucking disgusting but they work :\



//This is used to find the total displacement from where you started to where you stopped shooting with norecoil, I use this to calculate where to actually return your mouse to after you shoot
int FindTotalDisplacement(int* EitherXorY, int CountValueStoppedAt, int XorY) {
	//Get the x and y relative displacement coordinates in order to return to right before you started using the no recoil
	
	//find x displacement relative to start if identifier (XorY) is 0 
	if (XorY == 0) {
		int TotalXDisplacement = 0;
		for (int i = 0; i < CountValueStoppedAt; i++) {
			TotalXDisplacement += EitherXorY[i];
		}
		TotalXDisplacement = -(TotalXDisplacement - EitherXorY[0]);
		return TotalXDisplacement;
	}

	//find y displacement relative to start if identifier (XorY) is 1 
	else if (XorY == 1) {
		int TotalYDisplacement = 0;
		for (int i = 0; i < CountValueStoppedAt; i++) {
			TotalYDisplacement += EitherXorY[i];
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

	//If you take a look at this shit it will break your head, dividing and then multiplying by the same thing shouldn't do shit, but what I am doing here actually is 
	//first finding the true total displacement from where the shooting stopped to where is started and then finding the smoothed fractional displacement by divinding the true displacement with the
	//smoothing coefficient, since we must be working with integers C++ rounds up/down the division value accordingly, we then multiply back by the smoothing coefficient and because of this necessary rounding
	//the value will not be the same as the true displacement, but a mutation of that, from there we just subtract the true displacement by the mutated one in order to find what I like to call the recoil return innacuracy value
	
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
	//time of the event
	input.mi.time = 0;
	//additional data with mouse events, its generally 0 for basic mouse movements or button presses
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
		//std::cout << "Current smoothness selection: Rigid" << std::endl;
		CurrentSmoothness = *SmoothnessConfiguration::GunConfArray[CurrentGunHandlerIndex + 0];
		CurrentSmoothnessDelay[0] = SmoothnessConfiguration::GunConfArray[CurrentGunHandlerIndex + 1][0];
		CurrentSmoothnessDelay[1] = SmoothnessConfiguration::GunConfArray[CurrentGunHandlerIndex + 1][1];
		break;
	}
	case 2: {
		CurrentSmoothnessName = "Semi Rigid";
		//std::cout << "Current smoothness selection: Semi Rigid" << std::endl;
		CurrentSmoothness = *SmoothnessConfiguration::GunConfArray[CurrentGunHandlerIndex + 2];
		CurrentSmoothnessDelay[0] = SmoothnessConfiguration::GunConfArray[CurrentGunHandlerIndex + 3][0];
		CurrentSmoothnessDelay[1] = SmoothnessConfiguration::GunConfArray[CurrentGunHandlerIndex + 3][1];
		break;
	}
	case 3: {
		CurrentSmoothnessName = "Soft";
		//std::cout << "Current smoothness selection: Soft" << std::endl;
		CurrentSmoothness = *SmoothnessConfiguration::GunConfArray[CurrentGunHandlerIndex + 4];
		CurrentSmoothnessDelay[0] = SmoothnessConfiguration::GunConfArray[CurrentGunHandlerIndex + 5][0];
		CurrentSmoothnessDelay[1] = SmoothnessConfiguration::GunConfArray[CurrentGunHandlerIndex + 5][1];
		break;
	}
	}
	DisplayStatusConfig();
}

//handles everything about switching weapons (not just for "scrolling")
void ScrollThroughWeapons() {

	switch (CurrentWeaponIndex) {

	case 1: {
		CurrentGunName = "AK47";
		//std::cout << "Current weapon selection: AK47 " << std::endl;
		CurrentWeaponX = Guns::A::X;
		CurrentWeaponY = Guns::A::Y;
		CurrentSize = Guns::A::size;
		break;
	}

	case 2: {
		CurrentGunName = "M4A4";
		//std::cout << "Current weapon selection: M4A4 " << std::endl;
		CurrentWeaponX = Guns::B::X;
		CurrentWeaponY = Guns::B::Y;
		CurrentSize = Guns::B::size;

		break;
	}

	//for future guns implementation |
	//                               V

	case 3: {



		break;
	}

	case 4: {



		break;
	}


	}

	//Change the recoil compensation pattern values based on your ingame sensitivity
	for (int k = 0; k < CurrentSize; k++) {
		CurrentWeaponX[k] /= CS2sensitivity / 1.1;
		CurrentWeaponY[k] /= CS2sensitivity / 1.1;

		//round to the nearest 10th as the format of recoil comp values are set in 10s only
		std::round(10 * CurrentWeaponX[k]) / 10;
		std::round(10 * CurrentWeaponY[k]) / 10;
	}

	//changed the gun so now we must set the smoothness settings for that gun accordingly (delays etc..)
	ScrollThroughSmoothness();
	DisplayStatusConfig();
}

void DisplayStatusConfig() {
	system("cls");
	std::cout << "CS2Recoilcomper 1.0 " << std::endl;
	std::cout << "-------------------------------------------" << std::endl;
	std::cout << "|Current weapon selection: " << CurrentGunName << std::endl;
	std::cout << "|Current smoothness selection: " << CurrentSmoothnessName << std::endl;
	std::cout << "|Randomizer is set to " << randomizer << std::endl;
	std::cout << "|Returning after shooting is set to " << returnBackAfterShooting << std::endl;
	std::cout << "-------------------------------------------" << std::endl;
}