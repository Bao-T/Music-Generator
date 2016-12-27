//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Sat Feb 14 21:55:38 PST 2015
// Last Modified: Sat Feb 14 21:55:40 PST 2015
// Filename:      midifile/src-library/MidiEventList.cpp
// Website:       http://midifile.sapp.org
// Syntax:        C++11
// vim:           ts=3 expandtab
//
// Description:   A class which stores a MidiEvents for a MidiFile track.
//



#include <vector>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <utility>
#include "../include/MidiEventList.h"

using namespace std;


//////////////////////////////
//
// MidiEventList::MidiEventList -- Constructor.
//

MidiEventList::MidiEventList(void) {
   reserve(1000);
}



//////////////////////////////
//
// MidiEventList::MidiEventList(MidiEventList&) -- Copy constructor.
//

MidiEventList::MidiEventList(const MidiEventList& other) {
   list.reserve(other.list.size());
   auto it = other.list.begin();
   std::generate_n(std::back_inserter(list), other.list.size(), [&]() -> MidiEvent* {
      return new MidiEvent(**it++);
   });
}



//////////////////////////////
//
// MidiEventList::MidiEventList(MidiEventList&&) -- Move constructor.
//

MidiEventList::MidiEventList(MidiEventList&& other) {
    list = std::move(other.list);
    other.list.clear();
}



//////////////////////////////
//
// MidiEventList::~MidiEventList -- Deconstructor.  Deallocate all stored
//   data.
//

MidiEventList::~MidiEventList() {
   clear();
}



//////////////////////////////
//
// MidiEventList::operator[] --
//

MidiEvent&  MidiEventList::operator[](int index) {
   return *list[index];
}


const MidiEvent&  MidiEventList::operator[](int index) const {
   return *list[index];
}



//////////////////////////////
//
// MidiEventList::back -- Return the last element in the list.
//

MidiEvent& MidiEventList::back(void) {
   return *list.back();
}

// Alias for back:

MidiEvent& MidiEventList::last(void) {
   return back();
}



//////////////////////////////
//
// MidiEventList::getEvent -- The same thing as operator[], for
//      internal use when operator[] would look more messy.
//

MidiEvent& MidiEventList::getEvent(int index) {
     return *list[index];
}



//////////////////////////////
//
// MidiEventList::clear -- De-allocate any MidiEvents present in the list
//    and set the size of the list to 0.
//

void MidiEventList::clear(void) {
   for (int i=0; i<(int)list.size(); i++) {
      if (list[i] != NULL) {
         delete list[i];
         list[i] = NULL;
      }
   }
   list.resize(0);
}



//////////////////////////////
//
// MidiEventList::data --
//

MidiEvent** MidiEventList::data(void) {
   return list.data();
}



//////////////////////////////
//
// MidiEventList::reserve --  Pre-allocate space in the list for storing
//     elements.
//

void MidiEventList::reserve(int rsize) {
   if (rsize > (int)list.size()) {
      list.reserve(rsize);
   }
}


//////////////////////////////
//
// MidiEventList::getSize -- Return the number of MidiEvents stored in the list.
//

int MidiEventList::getSize(void) const {
   return (int)list.size();
}


int MidiEventList::size(void) const {
   return getSize();
}



//////////////////////////////
//
// MidiEventList::append -- add a MidiEvent at the end of the list.  Returns
//     the index of the appended event.
//

int MidiEventList::append(MidiEvent& event) {
   MidiEvent* ptr = new MidiEvent(event);
   list.push_back(ptr);
   return (int)list.size()-1;
}


int MidiEventList::push(MidiEvent& event) {
   return append(event);
}


int MidiEventList::push_back(MidiEvent& event) {
   return append(event);
}


//////////////////////////////
//
// MidiEventList::linkNotePairs -- Match note-ones and note-offs together
//   There are two models that can be done if two notes are overlapping
//   on the same pitch: the first note-off affects the last note-on,
//   or the first note-off affects the first note-on.  Currently  the
//   first note-off affects the last note-on, but both methods could
//   be implemented with user selectability.  The current state of the
//   track is assumed to be in time-sorted order.  Returns the number
//   of linked notes (note-on/note-off pairs).
//

int MidiEventList::linkEventPairs(void) {
   return linkNotePairs();
}


int MidiEventList::linkNotePairs(void) {

   // Note-on states:
   // dimension 1: MIDI channel (0-15)
   // dimension 2: MIDI key     (0-127)  (but 0 not used for note-ons)
   // dimension 3: List of active note-ons or note-offs.
   vector<vector<vector<MidiEvent*> > > noteons;
   noteons.resize(16);
   int i;
   for (i=0; i<(int)noteons.size(); i++) {
      noteons[i].resize(128);
   }

   // Controller linking: The following General MIDI controller numbers are
   // also monitored for linking within the track (but not between tracks).
   // hex dec  name                                    range
   // 40  64   Hold pedal (Sustain) on/off             0..63=off  64..127=on
   // 41  65   Portamento on/off                       0..63=off  64..127=on
   // 42  66   Sustenuto Pedal on/off                  0..63=off  64..127=on
   // 43  67   Soft Pedal on/off                       0..63=off  64..127=on
   // 44  68   Legato Pedal on/off                     0..63=off  64..127=on
   // 45  69   Hold Pedal 2 on/off                     0..63=off  64..127=on
   // 50  80   General Purpose Button                  0..63=off  64..127=on
   // 51  81   General Purpose Button                  0..63=off  64..127=on
   // 52  82   General Purpose Button                  0..63=off  64..127=on
   // 53  83   General Purpose Button                  0..63=off  64..127=on
   // 54  84   Undefined on/off                        0..63=off  64..127=on
   // 55  85   Undefined on/off                        0..63=off  64..127=on
   // 56  86   Undefined on/off                        0..63=off  64..127=on
   // 57  87   Undefined on/off                        0..63=off  64..127=on
   // 58  88   Undefined on/off                        0..63=off  64..127=on
   // 59  89   Undefined on/off                        0..63=off  64..127=on
   // 5A  90   Undefined on/off                        0..63=off  64..127=on
   // 7A 122   Local Keyboard On/Off                   0..63=off  64..127=on

   // first keep track of whether the controller is an on/off switch:
   vector<pair<int, int> > contmap;
   contmap.resize(128);
   pair<int, int> zero(0, 0);
   fill(contmap.begin(), contmap.end(), zero);
   contmap[64].first  = 1;   contmap[64].second = 0;
   contmap[65].first  = 1;   contmap[65].second = 1;
   contmap[66].first  = 1;   contmap[66].second = 2;
   contmap[67].first  = 1;   contmap[67].second = 3;
   contmap[68].first  = 1;   contmap[68].second = 4;
   contmap[69].first  = 1;   contmap[69].second = 5;
   contmap[80].first  = 1;   contmap[80].second = 6;
   contmap[81].first  = 1;   contmap[81].second = 7;
   contmap[82].first  = 1;   contmap[82].second = 8;
   contmap[83].first  = 1;   contmap[83].second = 9;
   contmap[84].first  = 1;   contmap[84].second = 10;
   contmap[85].first  = 1;   contmap[85].second = 11;
   contmap[86].first  = 1;   contmap[86].second = 12;
   contmap[87].first  = 1;   contmap[87].second = 13;
   contmap[88].first  = 1;   contmap[88].second = 14;
   contmap[89].first  = 1;   contmap[89].second = 15;
   contmap[90].first  = 1;   contmap[90].second = 16;
   contmap[122].first = 1;   contmap[122].second = 17;

   // dimensions:
   // 1: mapped controller (0 to 17)
   // 2: channel (0 to 15)
   vector<vector<MidiEvent*> > contevents;
   contevents.resize(18);
   vector<vector<int> > oldstates;
   oldstates.resize(18);
   for (int i=0; i<18; i++) {
      contevents[i].resize(16);
      fill(contevents[i].begin(), contevents[i].end(), nullptr);
      oldstates[i].resize(16);
      fill(oldstates[i].begin(), oldstates[i].end(), -1);
   }

   // Now iterate through the MidiEventList keeping track of note and
   // select controller states and linking notes/controllers as needed.
   int channel;
   int key;
   int contnum;
   int contval;
   int conti;
   int contstate;
   int counter = 0;
   MidiEvent* mev;
   MidiEvent* noteon;
   for (i=0; i<getSize(); i++) {
      mev = &getEvent(i);
      mev->unlinkEvent();
      if (mev->isNoteOn()) {
         // store the note-on to pair later with a note-off message.
         key = mev->getKeyNumber();
         channel = mev->getChannel();
         noteons[channel][key].push_back(mev);
      } else if (mev->isNoteOff()) {
         key = mev->getKeyNumber();
         channel = mev->getChannel();
         if (noteons[channel][key].size() > 0) {
            noteon = noteons[channel][key].back();
            noteons[channel][key].pop_back();
            noteon->linkEvent(mev);
            counter++;
         }
      } else if (mev->isController()) {
         contnum = mev->getP1();
         if (contmap[contnum].first) {
            conti     = contmap[contnum].second;
            channel   = mev->getChannel();
            contval   = mev->getP2();
            contstate = contval < 64 ? 0 : 1;
            if ((oldstates[conti][channel] == -1) && contstate) {
               // a newly initialized onstate was detected, so store for
               // later linking to an off state.
               contevents[conti][channel] = mev;
               oldstates[conti][channel] = contstate;
            } else if (oldstates[conti][channel] == contstate) {
               // the controller state is redundant and will be ignored.
            } else if ((oldstates[conti][channel] == 0) && contstate) {
               // controller is currently off, so store on-state for next link
               contevents[conti][channel] = mev;
               oldstates[conti][channel] = contstate;
            } else if ((oldstates[conti][channel] == 1) && (contstate == 0)) {
               // controller has just been turned off, so link to
               // stored on-message.
               contevents[conti][channel]->linkEvent(mev);
               oldstates[conti][channel] = contstate;
               // not necessary, but maybe use for something later:
               contevents[conti][channel] = mev;
            }
         }
      }
   }
   return counter;
}



//////////////////////////////
//
// MidiEventList::clearLinks -- remove all note-on/note-off links.
//

void MidiEventList::clearLinks(void) {
   for (int i=0; i<(int)getSize(); i++) {
      getEvent(i).unlinkEvent();
   }
}


///////////////////////////////////////////////////////////////////////////
//
// protected functions --
//


//////////////////////////////
//
// MidiEventList::detach -- De-allocate any MidiEvents present in the list
//    and set the size of the list to 0.
//


void MidiEventList::detach(void) {
   list.resize(0);
}



//////////////////////////////
//
// MidiEventList::push_back_no_copy -- add a MidiEvent at the end of
//     the list.  The event is not copied, but memory from the
//     remote location is used.  Returns the index of the appended event.
//

int MidiEventList::push_back_no_copy(MidiEvent* event) {
   list.push_back(event);
   return (int)list.size()-1;
}



//////////////////////////////
//
// MidiEventList::operator=(MidiEventList) -- Assignment.
//

MidiEventList& MidiEventList::operator=(MidiEventList other) {
   list.swap(other.list);
   return *this;
}


