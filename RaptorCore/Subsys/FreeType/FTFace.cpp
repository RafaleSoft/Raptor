
#include "FTFace.h"
#include "FTEngine.h"



/////////////////////////////////////////////////////////////////////////////

FTFace::FTFace( )
	:face(0)
{
	for( int i= 0; i < 256; ++i )
		indices[i]= 0;
}

/////////////////////////////////////////////////////////////////////////////

FTFace::~FTFace()
{
	if( face != 0 )
	{
		FT_Error error = FT_Done_Face(face);
		face = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////

bool FTFace::open( const char* filename )
{
	if( face != 0 )
		return false; // already open?!
	
	FT_Error error = FT_New_Face( FTEngine::getStaticEngine().getEngine(),
								  filename,
								  0,
								  &face );
	if ( error == FT_Err_Unknown_File_Format )
	{
	  //... the font file could be opened and read, but it appears
	  //... that its font format is unsupported
	}
	else if ( error )
	{
	  //... another error code means that the font file could not
	  //... be opened or read, or that it is broken...
	}

	return makeIndicesTable();
}

/////////////////////////////////////////////////////////////////////////////

bool FTFace::setResolutions( int x_resolution, int y_resolution ) const
{
	if( face == 0 )
		return false;

	FT_Error error = FT_Set_Char_Size(face,    /* handle to face object           */
									  0,       /* char_width in 1/64th of points  */
									  16*64,   /* char_height in 1/64th of points */
									  x_resolution,     /* horizontal device resolution    */
									  y_resolution );   /* vertical device resolution      */

	return (0 == error);
}

/////////////////////////////////////////////////////////////////////////////

bool FTFace::setSizes(unsigned int width,unsigned int height) const
{
	if( face == 0 )
		return false;

	FT_Error error = FT_Set_Pixel_Sizes(face,    /* handle to face object           */
										width,   /* nominal width in pixels         */
										height );/* nominal height in pixels        */

	return (0 == error);
}

/////////////////////////////////////////////////////////////////////////////

bool FTFace::selectSize(unsigned int size_index) const
{
	if( face == 0 )
		return false;
	if ((face->num_fixed_sizes <= 0) || (face->num_fixed_sizes <= (int)size_index))
		return false;

	FT_Error error = FT_Select_Size(face,			/* handle to face object					*/
									size_index );   /* index of the available bitmap strikes	*/

	return (0 == error);
}

/////////////////////////////////////////////////////////////////////////////

int FTFace::CP_Translate( int code ) const
{
	//if( CP_table == 0 )
	//	return code;

	//return CP_table[code];
	return code;
}

/////////////////////////////////////////////////////////////////////////////

bool FTFace::makeIndicesTable()
{
	if( face == 0 )
		return false;

	// First, look for a Unicode charmap
	int n = face->num_charmaps;
	int i;

	for( i= 0; i < n; ++i )
	{
		short unsigned platform = face->charmaps[i]->platform_id;
		short unsigned encoding = face->charmaps[i]->encoding_id;
		if ((platform == 3 && encoding == 1 )  ||
			(platform == 0 && encoding == 0 ) )
		{
			FT_CharMap char_map = face->charmaps[i];
			FT_Set_Charmap(face,char_map);
			break;
		}
	}

	if( i == n )
	{
		int num_glyphs = face->num_glyphs;

		for( int ascii_code= 0; ascii_code < 256; ++ascii_code )
		{
			int glyph_index = FT_Get_Char_Index(face,CP_Translate(ascii_code));
			if( glyph_index < 0 || glyph_index >= num_glyphs )
				glyph_index= 0;
			indices[ascii_code]= glyph_index;
		}
	}
	else
	{
		for( int ascii_code= 0; ascii_code < 256; ++ascii_code )
		{
			int glyph_index = FT_Get_Char_Index(face,ascii_code);
			if( glyph_index < 0 )
				glyph_index = 0;  // FIXME! default code

			indices[ascii_code]= glyph_index;
		}
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
