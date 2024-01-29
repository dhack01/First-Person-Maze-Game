#include <iostream>
#include <chrono>
using namespace std;
#include <Windows.h>

float fPlayerX = 8.0f; //player cords and where player is looking
float fPlayerY = 8.0f;
float fPlayerA = 0.0f;

int nMapHeight = 16; //map
int nMapWidth = 16;

int nScreenWidth = 120;
int nScreenHeight = 40;

float fFOV = 3.14 / 4.0;
float fDepth = 16.0f;

int main()
{
	// Creating Screen Buffer to write too
	wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0; //dumb vartible windows requires

	wstring map;

	map += L"################"; //way i create the map so it is more readable
	map += L"#..............#";
	map += L"#..............#";
	map += L"#.....#........#";
	map += L"#.....#........#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#####....#.....#";
	map += L"#........#.....#";
	map += L"#........#.....#";
	map += L"#........#######";
	map += L"#..............#";
	map += L"#..............#";
	map += L"################";

	auto tp1 = chrono::system_clock::now();
	auto tp2 = chrono::system_clock::now();

	while (true) { //the main game loop
		tp2 = chrono::system_clock::now();
		chrono::duration<float> elapsedTime = tp2 - tp1;
		tp1 = tp2;
		float fElapsedTime = elapsedTime.count();


		//controls VV
		if (GetAsyncKeyState((unsigned short)'A') & 0x8000) {   //LEFT
			fPlayerA -= (0.8f) * fElapsedTime;
		}
		if (GetAsyncKeyState((unsigned short)'D') & 0x8000) {   //RIGHT
			fPlayerA += (0.8f) * fElapsedTime;
		}
		if (GetAsyncKeyState((unsigned short)'W') & 0x8000) {  //FORWARD
			fPlayerX += sinf(fPlayerA) * 5.0f * fElapsedTime;
			fPlayerY += cosf(fPlayerA) * 5.0f * fElapsedTime;
			if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#') {
				fPlayerX -= sinf(fPlayerA) * 5.0f * fElapsedTime;
				fPlayerY -= cosf(fPlayerA) * 5.0f * fElapsedTime;
			}
		}
		if (GetAsyncKeyState((unsigned short)'S') & 0x8000) {  //BACKWARD
			fPlayerX -= sinf(fPlayerA) * 5.0f * fElapsedTime;
			fPlayerY -= cosf(fPlayerA) * 5.0f * fElapsedTime;
			if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#') {
				fPlayerX += sinf(fPlayerA) * 5.0f * fElapsedTime;
				fPlayerY += cosf(fPlayerA) * 5.0f * fElapsedTime;
			}
		}

		
		for (int x = 0; x < nScreenWidth; x++) {
			//calculates the expected rayangle for each collumn 
			float fRayAngle = (fPlayerA - fFOV / 2.0f) + ((float)x / (float)nScreenWidth) * fFOV;

			float fDistanceToWall = 0;
			bool bHitWall = false;

			float fEyeX = sinf(fRayAngle); // Unit vector for ray in player space
			float fEyeY = cosf(fRayAngle);
			
			while (!bHitWall && fDistanceToWall < fDepth) {

				fDistanceToWall += 0.1f;

				int nTestX = (int)(fPlayerX + fEyeX * fDistanceToWall);
				int nTestY = (int)(fPlayerY + fEyeY * fDistanceToWall);

				if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight) { // checks if ray is out of bounds of map
					bHitWall = true;
					fDistanceToWall = fDepth;	// set the distance to the max
				}
				else {
					if (map[nTestY * nMapWidth + nTestX] == '#') { //checks if ray hit a wall
						bHitWall = true;
					}
				}

			}

			//Calculating distance to ceiling and floor
			int nCeiling = (float)(nScreenHeight / 2.0) - nScreenHeight / ((float)fDistanceToWall);
			int nFloor = nScreenHeight - nCeiling;

			short nShade = ' ';

			//deciding what shading for the wallVVVV
			if (fDistanceToWall <= fDepth / 4.0f)          nShade = 0x2588;     // walls closest
			else if (fDistanceToWall < fDepth / 3.0f)      nShade = 0x2593;
			else if (fDistanceToWall < fDepth / 2.0f)	   nShade = 0x2592;
			else if (fDistanceToWall < fDepth)             nShade = 0x2591;
			else										   nShade = ' ';	   // out of bounds

			for (int y = 0; y < nScreenHeight; y++) {
				if (y < nCeiling) 
					screen[y * nScreenWidth + x] = ' ';
				else if(y > nCeiling && y <= nFloor)
					screen[y * nScreenWidth + x] = nShade;
				else {
					short fShade = ' ';
					float b = 1.0f - (((float)y - nScreenHeight / 2.0f) / ((float)nScreenHeight / 2.0f));
					if (b < 0.25)          fShade = '#';     // FLOOR SHADING
					else if (b < 0.5)      fShade = 'x';
					else if (b < 0.75)	   fShade = '.';
					else if (b < 0.9)	   fShade = '-';
					else				   fShade = ' ';
					screen[y * nScreenWidth + x] = fShade;
				}
			}
		}



		screen[nScreenWidth * nScreenHeight - 1] = '\0';
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
	}
}


