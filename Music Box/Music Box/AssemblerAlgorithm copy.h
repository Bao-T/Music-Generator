#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>
using std::vector;

class Notes
{
public:
	int note;
	int duration;
	Notes(int note, int duration) : note(note), duration(duration) {  };
};
class Algorithm
{
private:
	int startNote;
	unsigned short int firstNum, secondNum;
	std::vector<std::vector<int>> storage = { {} };
	long int duration = 20; // how many measures in song
	int singleStepNotes[21] = { 4,16,28,40,52,64,76,88,100,112,124,11,23,35,47,59,71,83,95,107,119 }; // notes E and B in C scale
public:
	Algorithm(int note, unsigned short int firstNum, unsigned short int secondNum) : startNote(note), firstNum(firstNum), secondNum(secondNum) {};
	void startAlgorithm()
	{
		__asm{
			//int i = 0;
			DWORD i
			mov eax, 0
			mov i, eax
			//int overflow = 0;
			DWORD overflow
			mov overflow, eax
			//int currentNote = startNote;
			DWORD currentNote
			mov eax, startNote
			mov currentNote, startNote
			//int currentNum = firstNum;
			DWORD currentNum
			mov eax, firstNum
			mov currentNum, eax
		}
		while (i < duration * 4) //Can this be done in ASM?
		{
			__asm{
				//int currentMeasureBeat=0;
				DWORD currentMeasureBeat
				mov eax, 0
				mov currentMeasureBeat, eax
				//int prevNote;
				DWORD prevNote
			}
			vector<int> measure; //can this be a ASM data structure?
			while (overflow != 0) //Can this be done in ASM?
			{
				measure.push_back(prevNote);
				__asm inc currentMeasureBeat;
				overflow--; //is dec an ASM operation?
			}
			while (currentMeasureBeat < 16) //Can this be done in ASM?
			{
				if (checkOverlap(currentMeasureBeat) == true) //review if in ASM
				{
					if (currentNum == firstNum)
						currentNum = secondNum;
					else currentNum = firstNum;
				}
				for (auto k = 0; k < currentNum; k++) //Can this be done in ASM?
				{
					if (currentMeasureBeat < 16)
					{
						measure.push_back(currentNote);
						i++;
						currentMeasureBeat++;
					}
					else
					{
						overflow++;
					}
				
				}
				//increments to next note in C scale;
				__asm{
					//prevNote = currentNote;
					mov eax, currentNote
					mov prevNote, eax
					//currentNote++;
					inc currentNote
				}
				if (currentNote > 127)
				{
					//currentNote = 0;
					__asm {
						mov eax, 0
						mov currentNote, eax
					}
				} 
				bool badNote = false;
				for (int j = 0; j < 21;j++)
				{
					if (prevNote == singleStepNotes[j])
						badNote = true;
				}
				if (badNote == false)
				{
					//currentNote++;
					__asm inc currentNote
					if (currentNote > 127)
					{
						//currentNote = 0;
						__asm{
							mov eax, 0
							mov currentNote, eax
						}
					}		
				}
				/****************************************/
			}
			storage.push_back(measure);
			
		}
	}
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
	vector<vector<Notes>> ConvertAlgorithm()
	{
		
		int tempNoteDuration;
		vector<vector<Notes>> convertedNotes;
		for (int i = 0; i < storage.size();i++)
		{
			vector<Notes> tempNoteVector;
			//int tempNote = -1;
			__asm{
				mov eax, -1
				mov tempNote, eax
			}
			for (int k = 0; k < storage[i].size();k++)
			{
				if (storage[i][k] != tempNote)
				{
					tempNote = storage[i][k];
					
					tempNoteVector.push_back(Notes(tempNote, 1));
				}
				else
				{
					tempNoteVector.back().duration+=1;
				}
			}
			convertedNotes.push_back(tempNoteVector);
		}
		return convertedNotes;
	}
};