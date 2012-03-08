////////////////////////////////////////////////////////////
//
// DAGON - An Adventure Game Engine
// Copyright (c) 2011 Senscape s.r.l.
// All rights reserved.
//
// NOTICE: Senscape permits you to use, modify, and
// distribute this file in accordance with the terms of the
// license agreement accompanying it.
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "DGConfig.h"
#include "DGFont.h"
#include "DGLog.h"

////////////////////////////////////////////////////////////
// Implementation - Constructor
////////////////////////////////////////////////////////////

DGFont::DGFont() {
    config = &DGConfig::getInstance();
    log = &DGLog::getInstance();
    
    _isLoaded = false;
    
    this->setType(DGObjectFont);
}

////////////////////////////////////////////////////////////
// Implementation - Destructor
////////////////////////////////////////////////////////////

DGFont::~DGFont() {
    // Nothing to do here
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

void DGFont::clear() {
    if (_isLoaded) {
        glDeleteTextures(128, _textures);
        free(_textures);
        FT_Done_Face(_face);
    }   
}

bool DGFont::isLoaded() {
    return _isLoaded;
}

void DGFont::print(int x, int y, const char* text, ...) {
	// We should keep this entire buffer separated from this function
	char aux[DGMaxFeedLength];
	char line[DGMaxFeedLength];
	char lines[DGMaxLogHistory][DGMaxFeedLength]; // Max rows of text
    
    const char* c;
    float h = (float)_height / .63f;
	unsigned int size = 0;
    
	va_list	ap;
	
	if (text == NULL)
		*line=0;
	else {
		va_start(ap, text);
	    vsprintf(line, text, ap);
		va_end(ap);
	}
	
	strcpy(aux, "");
	for (c = line; *c; c++) {
		if (*c == '\n') {
			strcpy(lines[size], aux);
			strcpy(aux, "");
			size++;
		}
		else {
			strncat(aux, c, 1);
		}
	}
	
	// This is to copy the last line
	strcpy(lines[size], aux);
	size++;
	
	glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TRANSFORM_BIT);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	for (int i = 0; i < size; i++) {
		c = lines[i];
        
		glPushMatrix();
		glTranslatef(x, y - h * i, 0);
        
		for (int j = 0; j < strlen(lines[i]); j++) {
			int ch = *c;
			
			GLfloat texCoords[] = {	0, 0,
				0, _glyph[ch].y,
				_glyph[ch].x, _glyph[ch].y,
				_glyph[ch].x, 0 };
            
            GLshort coords[] = { 0, 0,
				0, _glyph[ch].rows,
                _glyph[ch].width, _glyph[ch].rows,
				_glyph[ch].width, 0 };
			
			glBindTexture(GL_TEXTURE_2D, _textures[ch]);
			
			glTranslatef((GLfloat)_glyph[ch].left, 0, 0);
            
			glPushMatrix();
			glTranslatef(0, -(GLfloat)_glyph[ch].top + _height, 0);
			
			glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
			glVertexPointer(2, GL_SHORT, 0, coords);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			
			glPopMatrix();
			glTranslatef((GLfloat)(_glyph[ch].advance >> 6), 0, 0);
			
			c++;
		}
		
		glPopMatrix();
	}
	
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	
	glPopAttrib();  
}

// FIXME: This is a repeated method from DGRender - it would be best to avoid this
void DGFont::setColor(int color) {
    uint32_t aux = color;
    
	uint8_t b = (aux & 0x000000ff);
	uint8_t g = (aux & 0x0000ff00) >> 8;
	uint8_t r = (aux & 0x00ff0000) >> 16;
	uint8_t a = (aux & 0xff000000) >> 24;
    
	glColor4f((float)(r / 255.0f), (float)(g / 255.0f), (float)(b / 255.0f), (float)(a / 255.f));
}

void DGFont::setDefault(unsigned int heightOfFont) {	
    _height = (float)heightOfFont;
    
	// WARNING: No way of determining that 49052.. Careful!
	if (FT_New_Memory_Face(*_library, DGDefFontBinary, 49052, 0, &_face)) {
        log->error(DGModFont, "%s", DGMsg260004);
        return;
    }
	
    _loadFont();
    
    _isLoaded = true;
}

void DGFont::setLibrary(FT_Library* library) {
    _library = library;
}

void DGFont::setResource(const char* fromFileName, unsigned int heightOfFont) {
    _height = (float)heightOfFont;
    
    if (FT_New_Face(*_library, fromFileName, 0, &_face)) {
        log->error(DGModFont, "%s: %s", DGMsg260003, fromFileName);
        return;
    }
	
    _loadFont();
    
    _isLoaded = true;  
}

////////////////////////////////////////////////////////////
// Implementation - Private methods
////////////////////////////////////////////////////////////

void DGFont::_loadFont() {
    unsigned char ch;
    
    FT_Set_Char_Size(_face, _height << 6, _height << 6, 96, 96);
    
    _textures = (GLuint*)malloc(128 * sizeof(GLuint));
	glGenTextures(128, _textures);
	
	for (ch = 0; ch < 128; ch++) {
		int width, height;
		float x, y;
        
        FT_Bitmap bitmap;
        FT_BitmapGlyph bitmapGlyph;
		FT_Glyph glyph;
		GLubyte* expandedData;
		
		if (FT_Load_Glyph(_face, FT_Get_Char_Index(_face, ch), FT_LOAD_DEFAULT)) {
			log->error(DGModFont, "%s: %c", DGMsg260005, ch);
            return;
        }
		
		if (FT_Get_Glyph(_face->glyph, &glyph)) {
			log->error(DGModFont, "%s: %c", DGMsg260006, ch);
            return;
        }
		
		FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);
		bitmapGlyph = (FT_BitmapGlyph)glyph;
		
		bitmap = bitmapGlyph->bitmap;
		
		width = _next(bitmap.width);
		height = _next(bitmap.rows);
		
		expandedData = (GLubyte*)malloc(2 * width * height); // Is there a sizeof() missing here?
		
		for (int j = 0; j < height; j++) {
			for (int i = 0; i < width; i++) {
				expandedData[2 * (i + j * width)] = expandedData[2 * (i + j * width) + 1] = 
				(i >= bitmap.width || j >= bitmap.rows) ?
				0 : bitmap.buffer[i + bitmap.width * j];
			}
		}
		
		x = (float)bitmap.width / (float)width;
		y = (float)bitmap.rows / (float)height;
		
		_glyph[ch].x = x; 
		_glyph[ch].y = y;
		_glyph[ch].width = bitmap.width;
		_glyph[ch].rows = bitmap.rows;
		_glyph[ch].left = bitmapGlyph->left;
		_glyph[ch].top = bitmapGlyph->top;
		_glyph[ch].advance = _face->glyph->advance.x;
		
		glBindTexture(GL_TEXTURE_2D, _textures[ch]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
					 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, expandedData);
		
		free(expandedData);
	}
}

int DGFont::_next(int a) {
    int rval = 1;
    while (rval < a) rval <<= 1;
    return rval;
}