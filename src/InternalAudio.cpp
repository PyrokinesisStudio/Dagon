////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2011-2016 Senscape s.r.l.
// All rights reserved.
//
// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was
// not distributed with this file, You can obtain one at
// http://mozilla.org/MPL/2.0/.
//
////////////////////////////////////////////////////////////

#include "InternalAudio.h"

namespace dagon {

InternalAudio::InternalAudio(bool temp) : Audio() {
  _isTemporary = temp;
  setType(kObjectInternalAudio);
}

bool InternalAudio::isTemporary() const {
  return _isTemporary;
}

}
