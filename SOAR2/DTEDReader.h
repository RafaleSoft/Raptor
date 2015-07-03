#pragma once

class CDTEDReader
{
public:
	CDTEDReader(void);
	virtual ~CDTEDReader(void);

	unsigned short* dted_read(unsigned int &size_x,unsigned int &size_y,const char *infile);

	unsigned short* dted_resize(unsigned short *image,
							unsigned int &size_x,
							unsigned int &size_y,
							unsigned int n);
};
