#include "defintions.h"

//if the randomizer is active the return of your aim will also vary based on the randomness

//for now there are 2 weapon options, AK47 and M4A4 

//initially you start with an AK47 Rigid recoil control with returning back after shooting configuration but from there you can change it


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

	//This is the thread that actually does the no recoil, don't ask why it isn't in the main thread :\

	std::thread (MainThread).detach();

	//seed time bullshit for the randomizer
	srand(static_cast<unsigned int>(time(0)));

	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(15));

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
			DisplayStatusConfig(1);
		}

		else if (GetAsyncKeyState(VK_F4) & 1) {

			returnBackAfterShooting = !returnBackAfterShooting;
			DisplayStatusConfig(1);
		}

		else if (GetAsyncKeyState(0x48) & 1) {
			NoRecoilStatus = !NoRecoilStatus;
			SwitchKeyBind();
			//std::thread(SwitchKeyBind).join();
			DisplayStatusConfig(1);
			NoRecoilStatus = !NoRecoilStatus;
		}
	}
	//fully exit the program including the norecoil thread
	exit(0);
	//return  0;
}