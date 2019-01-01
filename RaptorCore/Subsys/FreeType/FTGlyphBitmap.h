
#ifndef __FTGlyphBitmap_h
#define __FTGlyphBitmap_h

#include "FTGlyph.h"

/////////////////////////////////////////////////////////////////////////////

class FTGlyphBitmap : public FTGlyph
{
public:
	FTGlyphBitmap(FTFace* _instance,int ascii_code);

	virtual ~FTGlyphBitmap();

	unsigned int getRowSize() const { return m_rowSize; };

	unsigned char* getBitmap() const { return m_pBuffer; }
	//bool expandBitmap(unsigned char* buffer,unsigned int stride) const;

	int getDeltaX() const { return m_delta_x; }
	int getDeltaY() const { return m_delta_y; }

private:
	virtual bool create();

	unsigned int m_rowSize;
	unsigned char* m_pBuffer;

	int m_delta_x;
	int m_delta_y;
};

/////////////////////////////////////////////////////////////////////////////

#endif // ifndef __FTGlyphBitmap_h
