#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>


using std::vector;
using std::cout;
using std::cin;
using std::endl;

class Notes
{
public:
	//note value to be played
	int note;
	//how long it will be played
	int duration;
	//constructor//
	Notes(int note, int duration) : note(note), duration(duration) {  };
};
class Algorithm
{
private:
	//note value that we are starting on
	int startNote = 1;

	//vector to store the notes in
	std::vector<std::vector<int>> storage = { {} };
	// how many measures in song
	long int duration = 50;
	// notes E and B in C scale
	int singleStepNotes[21] = { 4,16,28,40,52,64,76,88,100,112,124,11,23,35,47,59,71,83,95,107,119 };
	//to store number of beats user wants in the measure
	int beatsInMeasure;
	//how long do you want the song to be?
	int loopMeasures;

public:
	//
	int firstNum, secondNum;
	//constructor
	Algorithm(int i) {};
	
	//run the Algorithm
	void startAlgorithm()
	{
		//menu
		cout << "Welcome to the Magic Music Generator\nPlease choose from the list below and enter the note you would like to start with: \n";
		cout << "C3 = 36, C4 = 48, C5 = 60, C6 = 72, C7 = 84\n";
		cout << "D3 = 38, D4 = 50, D5 = 62, D6 = 74, D7 = 86\n";
		cout << "E3 = 40, E4 = 52, E5 = 64, E6 = 76, E7 = 88\n";
		cout << "F3 = 41, F4 = 53, F5 = 65, F6 = 77, F7 = 89\n";
		cout << "G3 = 43, G4 = 55, G5 = 67, G6 = 79, G7 = 91\n";
		cout << "A3 = 45, A4 = 57, A5 = 69, A6 = 81, A7 = 93\n";
		cout << "B3 = 47, B4 = 59, B5 = 71, B6 = 83, B7 = 95\n";
		cin >> startNote;
		cout << "\nPlease enter the a value for your first number: ";
		cin >> firstNum;
		cout << "\nPlease enter a value for your second number: ";
		cin >> secondNum;
		cout << "\nPlease enter how many beats are in a measure:";
		cin >> beatsInMeasure;
		cout << "\nPlease enter how many times you would like to loop over measures: ";
		cin >> loopMeasures;
	
		
		//used as counter
		int i;
		//to store the current note value
		int currentNote;
		//
		int currentNum;
		//
		int overflow;
		_asm
		{
			/*Accesses i, currentNote, currentNum, overflow, startNote, and firstNum
			 *i=0, currentNote = startNote, currentNum = firstNum, overflow = 0 */
			mov i, 0
			mov overflow, 0
				mov ecx, this
				mov eax, [ecx].startNote
				mov currentNote, eax
				mov eax, [ecx].firstNum
				mov currentNum, eax
		}

		while (i < duration * 4)
		{
			/*Main Algorithm for note Generation*/

			//the current beat in measure - should be <= the number that the user defined
			int currentMeasureBeat = 0;
			//previous note value that was played
			int prevNote;
			//to store all of the values for notes in the measure
			vector<int> measure;

			/*Checks if a note's duration is between two measures
			 * Keeps track of how many beats goes past the next measure
			 * Accounts for note displacement to keep the algorithm's structure*/
			while (overflow != 0)
			{
				measure.push_back(128);
				_asm
				{
					//go to next beat
					inc currentMeasureBeat
					//dec how many beats we have left
					dec overflow
				}

			}
			
			/*Identifies the loop counter
			 *Will keep generating new notes in a loop until the loop duration is met
			 *Determines which series of notes are identified as loop sequeneces*/
			while (currentMeasureBeat < beatsInMeasure*loopMeasures)
			{
				//Checks to see if a previously generated note is on the same beat as the currently generating note
				//Switches duration between firstNum and secondNum every time an overlap occurs
				if (checkOverlap(currentMeasureBeat) == true)
				{
				
					if (currentNum == firstNum)
						currentNum = secondNum;
					else currentNum = firstNum;
					
					
				}

				// Creates a series of notes to later be converted into a 2-data value array of note and its corresponding duration
				// For this loop, note E with duration 4 will be seen as EEEE
				int tempmeasure[10] = { 0,0,0,0,0,0,0,0,0,0};
				_asm
				{
					
					mov ecx,currentNum
					mov esi, 0
					mov edx, this // pointer to private memebers in class
					mov eax, [edx].beatsInMeasure
					imul eax, [edx].loopMeasures // multiply eax
					putNotesInMeasure :
						CMP currentMeasureBeat, eax // compares for overflow
						// Creates note amount defined by currentNum
						JG loopElse
							mov ebx, currentNote
							mov tempmeasure[TYPE tempmeasure*esi], ebx
							inc esi
							inc i
							inc currentMeasureBeat
						JMP loopDone
						//Keeps track if a note overlaps two measures between loops
						//	else
						loopElse:
							inc overflow
						loopDone:
					loop putNotesInMeasure
				}

				for (int k = 0; k < 10; k++)
				{
					if (tempmeasure[k] != 0)
						measure.push_back(tempmeasure[k]);
				}

		

				//}

				_asm
				{
					/*Uses currentNote, prevNote
					 *Checks note validity
					 *assigns the next correct note within the series
					 */
				
					mov eax, currentNote
					mov prevNote, eax
					inc currentNote
					
					//Comparison restricts the range of the notes
					CMP currentNote, 95
					JLE lessthan
						mov currentNote, 36
					lessthan:
					//Resets registers to compare if the current note needs to be incremented once or twice
					mov ebx, 0 // used to determine if a note needs to only be incremented once, set to false (false increments twice)
					mov ecx, 21
					mov esi, 0
					mov edx, this
						/*	for (int j = 0; j < 21;j++)
						{
						if (prevNote == singleStepNotes[j])
						badNote = true;
						}
						if (badNote == false)
						{
						currentNote++;
						if (currentNote > 95)
						currentNote = 36;
						}*/
					//Checks singleStepNotes array to see if the previous note is within it
					//if it is, increment once, if not, increment twice
					L1 :
							mov eax, [edx].singleStepNotes[TYPE singleStepNotes*esi]
						CMP prevNote, eax
						JNE notEqual3
							mov ebx, 1 // set bool to true
						notEqual3:
							inc esi
					loop L1
					// Checks ebx bool and increments accordingly
					CMP ebx, 0
					JNE notEqual2
						inc currentNote
					CMP currentNote, 95
					JLE lessthan2
						mov currentNote, 36
					lessthan2:
					notEqual2:

				}
				/****************************************/
			}
			storage.push_back(measure);

		}
	}

	//compares to see if there is already a note in this beat for this measure
	bool checkOverlap(int measureIndex)
	{
		for (int it = 0; it != storage.size(); it++)
		{
			if (!storage[it].empty())
			{
				if (storage[it][measureIndex] != NULL && measureIndex != 0)
				{
					if (storage[it][measureIndex - 1] != storage[it][measureIndex])
						return true;
				}
				if (measureIndex == 0)
					return true;
			}
		}
		return false;
	}

	//converts our notes into the values that the library will accept
	vector<vector<Notes>> ConvertAlgorithm()
	{

		int tempNoteDuration;
		vector<vector<Notes>> convertedNotes;
		for (int i = 0; i < storage.size(); i++)
		{
			vector<Notes> tempNoteVector;
			int tempNote = -1;
			for (int k = 0; k < storage[i].size(); k++)
			{
				if (storage[i][k] != tempNote)
				{
					tempNote = storage[i][k];

					tempNoteVector.push_back(Notes(tempNote, 1));
				}
				else
				{
					tempNoteVector.back().duration += 1;
				}
			}
			convertedNotes.push_back(tempNoteVector);
		}
		return convertedNotes;
	}
	int get_BeatsInMeasure() { return beatsInMeasure; }
	int get_loopMeasure() { return loopMeasures; }
};