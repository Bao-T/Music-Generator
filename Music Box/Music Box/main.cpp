//Copyright(c) 1999 - 2015, Craig Stuart Sapp
//All rights reserved.
//
//Redistribution and use in source and binary forms, with or without
//modification, are permitted provided that the following conditions are met :
//
//1. Redistributions of source code must retain the above copyright notice, this
//list of conditions and the following disclaimer.
//2. Redistributions in binary form must reproduce the above copyright notice,
//and the following disclaimer in the documentation and / or other materials
//provided with the distribution.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
//ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
//ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

/*Whitworth University CS 278 Final Project
 *Structured Noise Generator
 *Bao Tran, Vina Le, Michael Gamlem III
 *12/16/2016
 *Using Craig Studart Sapp's midi file creation library.
 *Modified main function and created an algorithm class for implementation
 */





#include <iostream>
#include "../include/MidiFile.h"
#include "AssemblerAlgorithm.h"
#include <vector>
using namespace std;

typedef unsigned char uchar;

///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
	Algorithm output(3);
	output.startAlgorithm();
	vector<vector<Notes>> myTracks = output.ConvertAlgorithm();

	MidiFile outputfile;        // create an empty MIDI file with one track
	outputfile.absoluteTicks();  // time information stored as absolute time
								 // (will be coverted to delta time when written)
	outputfile.addTrack(myTracks.size());     // Add five tracks to the MIDI file
	vector<uchar> midievent;     // temporary storage for MIDI events
	midievent.resize(3);        // set the size of the array to 3 bytes
	int tpq = 120;              // default value in MIDI file is 48
	outputfile.setTicksPerQuarterNote(tpq);


	int trackdepth = 0;
	for (int track = 1; track < myTracks.size(); track++)
	{

		vector<int> melody;
		vector<int> mrhythm;

		for (int k = 0; k < 25; k++)
		{

			melody.push_back(myTracks[track][k % myTracks[track].size()].note);
			mrhythm.push_back(myTracks[track][k % myTracks[track].size()].duration);
		}




		int i = 0;
		int actiontime = tpq*trackdepth*output.get_BeatsInMeasure()*output.get_loopMeasure(); // delay between tracks
		midievent[2] = 120;       // store attack/release velocity for note command


		while (i < melody.size()) {
			// temporary storage for MIDI event time
			midievent[0] = 0x90;     // store a note on command (MIDI channel 1)
			midievent[1] = melody[i];
			outputfile.addEvent(track, actiontime, midievent);
			actiontime += tpq * mrhythm[i];
			midievent[0] = 0x80;     // store a note on command (MIDI channel 1)
			outputfile.addEvent(track, actiontime, midievent);
			i++;
		}
		trackdepth++;
	}

	outputfile.sortTracks();         // make sure data is in correct order
	outputfile.write("Sick Beats.mid"); // write Standard MIDI File twinkle.mid
	return 0;
}