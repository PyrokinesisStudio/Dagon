////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2011-2013 Senscape s.r.l.
// All rights reserved.
//
// This Source Code Form is subject to the terms of the
// Mozilla Public License, v. 2.0. If a copy of the MPL was
// not distributed with this file, You can obtain one at
// http://mozilla.org/MPL/2.0/.
//
////////////////////////////////////////////////////////////

#ifndef DG_CURSOR_H
#define DG_CURSOR_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGAction.h"
#include "DGPlatform.h"

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////

#define DGDefCursorSize 128
#define DGMaxCursorSize 128
#define DGMinCursorSize 32

class DGConfig;
class DGTexture;

typedef struct {
    int type;
    DGTexture* image;
    DGPoint origin;
} DGCursorData;

////////////////////////////////////////////////////////////
// Interface - Singleton class
////////////////////////////////////////////////////////////

// TODO: Define origin for cursors (for better placement)
class DGCursorManager : public DGObject {
    DGConfig* config;
    
    std::vector<DGCursorData> _arrayOfCursors;
    std::vector<DGCursorData>::iterator _current;
    int _arrayOfCoords[8];
    int _half;
    bool _hasAction;
    bool _hasImage;
    bool _isDragging;
    bool _onButton; // Decide in Control?
    DGAction* _pointerToAction;    
    int _size;
    int _x;
    int _y;
    
    void _set(int type);
    
    // Private constructor/destructor
    DGCursorManager();
    ~DGCursorManager();
    // Stop the compiler generating methods of copy the object
    DGCursorManager(DGCursorManager const& copy);            // Not implemented
    DGCursorManager& operator=(DGCursorManager const& copy); // Not implemented
    
public:
    static DGCursorManager& getInstance() {
        // The only instance
        // Guaranteed to be lazy initialized
        // Guaranteed that it will be destroyed correctly
        static DGCursorManager instance;
        return instance;
    }

    DGAction* action();
    void bindImage();
    int* arrayOfCoords();
    bool hasAction();
    bool hasImage();
    bool isDragging();
    void load(int type, const char* imageFromFile, int offsetX = 0, int offsetY = 0);
    bool onButton();
    DGPoint position();
    void removeAction();
    void setDragging(bool flag);
    void setOnButton(bool flag);    
    void setAction(DGAction* action);
    void setCursor(int type);
    void setSize(int size);
    void updateCoords(int x, int y);
};

#endif // DG_CURSOR_H