/*
  Copyright (C) 2001-2004 Stephane Magnenat & Luc-Olivier de Charrière
  for any question or comment contact us at nct@ysagoon.com or nuage@ysagoon.com

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#ifndef __GRAPHICCONTEXT_H
#define __GRAPHICCONTEXT_H

#include "GAGSys.h"
#include <map>
#include <vector>
#include <string>

class Font;
class Sprite;

class DrawableSurface
{
protected:
	//! the underlying SDL or glSDL surface
	SDL_Surface *surface;
	//! The clipping rect, we do not draw outside it
	SDL_Rect clipRect;
	//! Flags, can be a combination of ResolutionFlags
	Uint32 flags;
	//! if true, we only want to redraw part of screen at each frame. The backend has to queue commands to correcly handle double buffering
	bool partialRedraw;
	//! if true, surface is locked and suitable for direct pixel access
	bool locked;
	
	//! lock only if necessary
	void lock(void) { if (!locked) { SDL_LockSurface(surface); locked = true; } }
	//! unlock only if necessary
	void unlock(void) { if (locked) { SDL_UnlockSurface(surface); locked = false; } }
	
	//! A delayed drawing command that can be applyed to the surface
	class DrawCommand
	{
	public:
		virtual void apply(DrawableSurface *ds) = 0;
		virtual ~DrawCommand() { }
	};
	
	class SetClipRectCommand: public DrawCommand
	{
	protected:
		int x, y, w, h;
	public:
		SetClipRectCommand(int x, int y, int w, int h) { this->x=x; this->y=y; this->w=w; this->h=h; }
		virtual void apply(DrawableSurface *ds) { ds->setClipRect(x, y, w, h); }
	};
	
	class SetNoClipRectCommand: public DrawCommand
	{
	public:
		virtual void apply(DrawableSurface *ds) { ds->setClipRect(); }
	};
	
	class DrawSpriteCommand: public DrawCommand
	{
	protected:
		int x, y, index;
		Sprite *sprite;
	public:
		DrawSpriteCommand(int x, int y, Sprite *sprite, int index) { this->x=x; this->y=y; this->sprite=sprite; this->index=index; }
		virtual void apply(DrawableSurface *ds) { ds->drawSprite(x, y, sprite, index); }
	};
	
	class DrawPixelCommand: public DrawCommand
	{
	protected:
		int x, y;
		Uint8 r, g, b, a;
	public:
		DrawPixelCommand(int x, int y, Uint8 r, Uint8 g, Uint8 b, Uint8 a) { this->x=x; this->y=y; this->r=r; this->g=g; this->b=b; this->a=a; }
		virtual void apply(DrawableSurface *ds) { ds->drawPixel(x, y, r, g, b, a); }
	};
	
	class DrawFilledRectCommand: public DrawCommand
	{
	protected:
		int x, y, w, h;
		Uint8 r, g, b, a;
	public:
		DrawFilledRectCommand(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a) { this->x=x; this->y=y; this->w=w; this->h=h; this->r=r; this->g=g; this->b=b; this->a=a; }
		virtual void apply(DrawableSurface *ds) { ds->drawFilledRect(x, y, w, h, r, g, b, a); }
	};
	
	class DrawRectCommand: public DrawCommand
	{
	protected:
		int x, y, w, h;
		Uint8 r, g, b, a;
	public:
		DrawRectCommand(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a) { this->x=x; this->y=y; this->w=w; this->h=h; this->r=r; this->g=g; this->b=b; this->a=a; }
		virtual void apply(DrawableSurface *ds) { ds->drawRect(x, y, w, h, r, g, b, a); }
	};
	
	class DrawLineCommand: public DrawCommand
	{
	protected:
		int x1, y1, x2, y2;
		Uint8 r, g, b, a;
	public:
		DrawLineCommand(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a) { this->x1=x1; this->y1=y1; this->x2=x2; this->y2=y2; this->r=r; this->g=g; this->b=b; this->a=a; }
		virtual void apply(DrawableSurface *ds) { ds->drawLine(x1, y1, x2, y2, r, g, b, a); }
	};
	
	class DrawCircleCommand: public DrawCommand
	{
	protected:
		int x, y, ray;
		Uint8 r, g, b, a;
	public:
		DrawCircleCommand(int x, int y, int ray, Uint8 r, Uint8 g, Uint8 b, Uint8 a) { this->x=x; this->y=y; this->ray=ray; this->r=r; this->g=g; this->b=b; this->a=a; }
		virtual void apply(DrawableSurface *ds) { ds->drawCircle(x, y, ray, r, g, b, a); }
	};
	
	class DrawStringCommand: public DrawCommand
	{
	protected:
		int x, y, w;
		Font *font;
		std::string msg;
	public:
		DrawStringCommand(int x, int y, int w, Font *font, const char *msg) { this->x=x; this->y=y; this->w=w; this->font=font; this->msg=msg; }
		virtual void apply(DrawableSurface *ds) { ds->drawString(x, y, w, font, msg.c_str()); }
	};
	
	class DrawSurfaceCommand: public DrawCommand
	{
	protected:
		int x, y;
		DrawableSurface *s;
	public:
		DrawSurfaceCommand(int x, int y, DrawableSurface *s) { this->x=x; this->y=y; this->s=s; }
		virtual void apply(DrawableSurface *ds) { ds->drawSurface(x, y, s); }
	};
		
	//! the commands vector used for partial redraw
	std::vector<DrawCommand *> drawCommands;
	
public:
	enum GraphicContextType
	{
		GC_SDL=0,
		GC_GL=1
	};

	enum Alpha
	{
		ALPHA_TRANSPARENT=0,
		ALPHA_OPAQUE=255
	};

	enum ResolutionFlags
	{
		DEFAULT=0,
		DOUBLEBUF=1,
		FULLSCREEN=2,
		HWACCELERATED=4,
		RESIZABLE=8,
	};

public:
	DrawableSurface();
	virtual ~DrawableSurface(void) { if (surface) SDL_FreeSurface(surface); }
	virtual bool setRes(int w, int h, int depth=32, Uint32 flags=DEFAULT, Uint32 type=GC_SDL);
	virtual void setAlpha(bool usePerPixelAlpha=false, Uint8 alphaValue=ALPHA_OPAQUE);
	virtual int getW(void) { if(surface) return surface->w; else return 0; }
	virtual int getH(void) { if(surface) return surface->h; else return 0; }
	virtual int getDepth(void) { if(surface) return surface->format->BitsPerPixel; else return 0; }
	virtual int getFlags(void) { return flags; }
	virtual void setClipRect(int x, int y, int w, int h);
	virtual void setClipRect(void);
	virtual void loadImage(const char *name);
	virtual void drawSprite(int x, int y, Sprite *sprite, int index=0);
	virtual void drawPixel(int x, int y, Uint8 r, Uint8 g, Uint8 b, Uint8 a=ALPHA_OPAQUE);
	virtual void drawRect(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a=ALPHA_OPAQUE);
	virtual void drawFilledRect(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a=ALPHA_OPAQUE);
	virtual void drawVertLine(int x, int y, int l, Uint8 r, Uint8 g, Uint8 b, Uint8 a=ALPHA_OPAQUE);
	virtual void drawHorzLine(int x, int y, int l, Uint8 r, Uint8 g, Uint8 b, Uint8 a=ALPHA_OPAQUE);
	virtual void drawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a=ALPHA_OPAQUE);
	virtual void drawCircle(int x, int y, int ray, Uint8 r, Uint8 g, Uint8 b, Uint8 a=ALPHA_OPAQUE);
	virtual void drawString(int x, int y, Font *font, int i);
	virtual void drawString(int x, int y, Font *font, const char *msg);
	virtual void drawString(int x, int y, int w, Font *font, const char *msg);
	virtual void drawSurface(int x, int y, DrawableSurface *surface);
	virtual void updateRects(SDL_Rect *rects, int size) { }
	virtual void updateRect(int x, int y, int w, int h) { }
	virtual void *getPixelPointer(void)  { return surface->pixels; }
};

class GraphicContext:public DrawableSurface
{
private:
	SDL_Rect **modes;
	
protected:
	int minW, minH;
	//! draw all queued command due to the double buffering
	void drawQueudCommands(void);
	//! clear all queued commands
	void clearQueudCommands(void);

public:
	GraphicContext();
	virtual ~GraphicContext(void);

	//! this must be called before any Drawable Surface method.
	virtual bool setRes(int w, int h, int depth=32, Uint32 flags=DEFAULT, Uint32 type=GC_SDL);
	virtual void setMinRes(int w=0, int h=0);
	virtual void setCaption(const char *title, const char *icon) { SDL_WM_SetCaption(title, icon); }
	virtual void beginVideoModeListing(void);
	virtual bool getNextVideoMode(int *w, int *h);
		
	virtual void loadImage(const char *name);

	virtual void nextFrame(void);
	virtual void updateRects(SDL_Rect *rects, int size);
	virtual void updateRect(int x, int y, int w, int h);
	virtual void printScreen(const char *filename);
	virtual void setPartialRedraw(bool value) { partialRedraw = value; }
};


class Font
{
public:
	enum Shape
	{
		STYLE_NORMAL = 0x00,
		STYLE_BOLD = 0x01,
		STYLE_ITALIC = 0x02,
		STYLE_UNDERLINE = 0x04,
	};
	
	struct Style
	{
		Shape shape;
		Uint8 r, g, b, a;
		
		Style() { shape = STYLE_NORMAL; r = 255; g = 255; b = 255; a = DrawableSurface::ALPHA_OPAQUE; }
		
		Style(Shape shape, Uint8 r, Uint8 g, Uint8 b, Uint8 a = DrawableSurface::ALPHA_OPAQUE)
		{
			this->shape = shape;
			this->r = r;
			this->g = g;
			this->b = b;
			this->a = a;
		}
		
		bool operator<(const Style &o) const
		{
			Uint32 w0 = (r<<24) | (g<<16) | (b<<8) | a;
			Uint32 w1 = (o.r<<24) | (o.g<<16) | (o.b<<8) | o.a;
			if (w0 == w1)
				return shape < o.shape;
			else
				return w0 < w1;
		}
	};

public:
	virtual ~Font() { }

	// width and height
	virtual int getStringWidth(const char *string) const = 0;
	virtual int getStringWidth(const char *string, int len) const;
	virtual int getStringWidth(const int i) const;
	virtual int getStringHeight(const char *string) const = 0;
	virtual int getStringHeight(const char *string, int len) const;
	virtual int getStringHeight(const int i) const;

	// Style and color
	virtual void setStyle(Style style) = 0;
	virtual void pushStyle(Style style) = 0;
	virtual void popStyle(void) = 0;
	virtual Style getStyle(void) const = 0;
	
protected:
	friend class DrawableSurface;
	virtual void drawString(SDL_Surface *Surface, int x, int y, int w, const char *text, SDL_Rect *clip=NULL) = 0;
};


union Color32
{
	Uint32 id;
	struct
	{
		Uint8 r, g, b, a;
	} channel;

	Color32() { channel.r=channel.g=channel.b=0; channel.a=DrawableSurface::ALPHA_OPAQUE; }
	Color32(Uint8 r, Uint8 g, Uint8 b, Uint8 a=DrawableSurface::ALPHA_OPAQUE) { channel.r=r; channel.g=g; channel.b=b; channel.a=a; }
	Color32(Uint32 v) { id=v; }
	bool operator<(const Color32 &o) const { return id<o.id; }
};


class Sprite
{
protected:
	struct Surface
	{
		SDL_Surface *s;
		
		//! allocate the internal surface suitable for fast blit, free the source
		Surface(SDL_Surface *source);
		~Surface();
	};

	struct RotatedImage
	{
		SDL_Surface *orig;
		typedef std::map<Color32, Surface *> RotationMap;
		RotationMap rotationMap;

		RotatedImage(SDL_Surface *s) { orig=s; }
		~RotatedImage();
	};

	std::vector <Surface *> images;
	std::vector <RotatedImage *> rotated;
	Color32 actColor;

	friend class GraphicContext;
	// Support functions
	//! Load a frame from two file pointers
	void loadFrame(SDL_RWops *frameStream, SDL_RWops *rotatedStream);
	//! Check if index is within bound and assert false otherwise
	void checkBound(int index);

public:
	Sprite() { }
	virtual ~Sprite();
	
	//! Load a sprite from the file, return true if any frame have been loaded
	bool load(const char *filename);

	//! Draw the sprite frame index at pos (x,y) on an SDL Surface with the clipping rect clip
	virtual void draw(SDL_Surface *dest, const SDL_Rect *clip, int x, int y, int index);

	//! Set the (r,g,b) color to a sprite's base color
	virtual void setBaseColor(Uint8 r, Uint8 g, Uint8 b) { actColor=Color32(r, g, b); }

	//! Return the width of index frame of the sprite
	virtual int getW(int index);
	//! Return the height of index frame of the sprite
	virtual int getH(int index);
	//! Return the number of frame in this sprite
	virtual int getFrameCount(void);
};


#endif
