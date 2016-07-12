////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2011-2014 Senscape s.r.l.
// All rights reserved.
//
// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was
// not distributed with this file, You can obtain one at
// http://mozilla.org/MPL/2.0/.
//
////////////////////////////////////////////////////////////

#ifndef DAGON_TIMERMANAGER_H_
#define DAGON_TIMERMANAGER_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <SDL2/SDL_mutex.h>
#include <SDL2/SDL_thread.h>

#include "Platform.h"

namespace dagon {

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

class System;

enum DGTimerTypes {
  DGTimerInternal,
  DGTimerManual,
  DGTimerNormal
};

typedef struct {
  int handle;
  bool isEnabled;
  bool isLoopable;
  bool hasTriggered;
  double lastTime;
  int luaHandler;
  void (*handler)();
  int luaObject;
  double trigger;
  int type;
} DGTimer;

////////////////////////////////////////////////////////////
// Interface - Singleton class
////////////////////////////////////////////////////////////

class TimerManager {
  System* system;
  
  std::vector<DGTimer> _arrayOfTimers;
  int _handles; // Maintains a count of handles
  int _luaObject;
  
  bool _isRunning;
  
  SDL_mutex* _mutex;
  SDL_Thread* _thread;
  DGTimer* _lookUp(int handle);
  
  static int _runThread(void *ptr);
  
  TimerManager();
  TimerManager(TimerManager const&);
  TimerManager& operator=(TimerManager const&);
  ~TimerManager();
  
public:
  static TimerManager& instance() {
    static TimerManager timerManager;
    return timerManager;
  }

  // Gets
  std::vector<DGTimer>& timers(); // THIS BREAKS ENCAPSULATION.
                                  // Make sure you don't mutate the vector on accident.
  double timeElapsed(const DGTimer &timer); // How long since it was started?
                                            // Caller should ensure it makes sense to query
                                            // this timer for it's elapsed time.
  
  bool checkManual(int handle, int precision = 1000);
  int create(double trigger, bool shouldLoop, int handlerForLua, int luaObject = 0); // Returns a handle
  int createInternal(double trigger, void (*callback)()); // For timers created by the engine (not Lua)
  int createManual(double trigger, int precision = 1000); // For timers that are manually checked by the user
  void destroy(int handle);
  void disable(int handle);
  void enable(int handle);
  void process();
  void setLuaObject(int luaObject);
  void setSystem(System* theSystem);
  void terminate();
  bool update();
};
  
}

#endif // DAGON_TIMERMANAGER_H_
