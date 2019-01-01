#include "StdAfx.h"
#include "MicroLex.h"

#include <string.h>	// strlen,...
#include <fstream>
#include <errno.h>

//	The structure necessary to parse multiple files
//	( due to file inclusion )
typedef struct source_stream_t
{
    // CFile source;
	ifstream	source;
	char	buffer[BUFFER_SIZE];
	int		bufferpos;
	int		maxpos;
} source_stream;

typedef struct microlex_file_t 
{
	unsigned int ID;
	//CFile file;
    ofstream    file;
    string      filename;
} microlex_file;


StreamManager::StreamManager()
{
	current_source = -1;
	numline = 1;
	linepos = 1;

}

StreamManager::~StreamManager()
{
}

bool StreamManager::PushSourceFile(const string& fname)
{
	source_stream *newsrc = new source_stream;

	//CFileException	exc;
	//if (FALSE == newsrc->source.Open(fname.data(),CFile::modeRead|CFile::shareDenyWrite,&exc))
    newsrc->source.open(fname.data(),ios_base::binary|ios_base::in);
    if ((!newsrc->source.is_open()) || (!newsrc->source.good()))
	{
		//cout << "File " << LPCTSTR(exc.m_strFileName) << " could not be opened: error " << exc.m_cause << endl;
		cout << "File " << fname.data() << " could not be opened: error " << errno << endl;
		return false;
	}

	newsrc->bufferpos = BUFFER_SIZE;
	newsrc->maxpos = BUFFER_SIZE;
	
	sources.push_back(newsrc);
	current_source++;

	return true;
}

void StreamManager::PopSourceFile()
{
	//sources[current_source]->source.Close();
	sources[current_source]->source.close();

	source_stream* src = sources[current_source];
	sources.erase(sources.begin() + current_source);
	delete src;
	current_source--;
}

char StreamManager::GetChar(void)
{
	char c;

	if (current_source < 0)
		return EOF;

	source_stream	&src = *sources[current_source];

	if (src.bufferpos == src.maxpos)
	{
        if ((src.source.is_open()) && (src.source.good()) && (!src.source.eof()))
		{
            src.source.read(src.buffer,BUFFER_SIZE);
			// a long long size is highly improbable
            src.maxpos = (int)src.source.gcount();
			src.bufferpos = 0;

			if (src.maxpos == 0)
			{
				src.bufferpos = -1;
				c = EOF;
				PopSourceFile();
				//c = GetChar();
				c = NEW_LINE;
			}
			else
				c = src.buffer[src.bufferpos++];
		}
		else
		{
			src.bufferpos = -1;
			c = EOF;
			PopSourceFile();
			//c = GetChar();
			c = NEW_LINE;

		}
	}
	else
		c = src.buffer[src.bufferpos++];

	if (current_source == 0)
	{
		if (c == NEW_LINE)
		{
			numline++;
			linepos = 0;
		}
		else
			linepos++;
	}

	return c;
}


int StreamManager::ParseInclude(char &car)
{
	string include;
	while ((car!=NEW_LINE)&&(car!=EOF))
	{
		if (!(IS_SEPARATOR(car)))
			include += car;
		car = GetChar();
	}

	if (false == PushSourceFile(include))
		return -1;

	return 0;
}


char StreamManager::ReadToken(char delim,string &token)
{
	char c = GetChar();
	while ((c != delim) && (!IS_SEPARATOR(c)))
	{
		if (c == EOF)
		{
			cout << "Unexpected end of file !" << endl;
			return EOF;
		}
		else
			token+=c;
		c = GetChar();
	}

	return c;
}

int StreamManager::OpenMicroLexFile(const string& fname)
{
	//CFileException	exc;
	microlex_file* mf = new microlex_file;

	//if (FALSE == mf->file.Open(fname.data(),CFile::modeCreate|CFile::modeWrite|CFile::shareExclusive,&exc))
	mf->file.open(fname.data());
    if ((!mf->file.is_open()) || (!mf->file.good()))
	{
		cout << "MicroLex file [" << fname.data() << "] could not be created: " << errno << endl;
		return -1;
	}
	else
	{
		mf->ID = microlex_files.size();
        mf->filename = fname;
		microlex_files.push_back(mf);
		return mf->ID; 
	}
}

void StreamManager::CloseMicroLexFiles(void)
{
	for (unsigned int i=0;i<microlex_files.size();i++)
	{
		//microlex_files[i]->file.Close();
		microlex_files[i]->file.close();
	}
}

string StreamManager::GetFileName(unsigned int file_ID)
{
	if (file_ID >= microlex_files.size())
	{
		cout << "Internal error: invalid file ID" << endl;
		return "";
	}
	else
		//return microlex_files[file_ID]->file.GetFileName();
		return microlex_files[file_ID]->filename;
}

void StreamManager::WriteData(unsigned int file_ID,const char *data)
{
	if (file_ID >= microlex_files.size())
	{
		cout << "Internal error: invalid file ID" << endl;
		return;
	}

	try
	{
		//CFile &file = microlex_files[file_ID]->file;
		ofstream &file = microlex_files[file_ID]->file;

		file.write(data,strlen(data));
		//file.Write(data,strlen(data));
	}
    catch(...)
    {
    }
    /*
	catch(CFileException *ex)
	{
		switch(ex->m_cause)
		{
			case CFileException::generic:
				cout << "Unknown error" << endl;
				break;
			case CFileException::fileNotFound:
				cout << "File not found" << endl;
				break;
			case CFileException::badPath:
				cout << "Bad path" << endl;
				break;
			case CFileException::tooManyOpenFiles:
				cout << "Too many files opened" << endl;
				break;
			case CFileException::accessDenied:
				cout << "Access denied" << endl;
				break;
			case CFileException::invalidFile:
				cout << "Invalid files" << endl;
				break;
			case CFileException::removeCurrentDir:
				cout << "Cannot remove current directory" << endl;
				break;
			case CFileException::directoryFull:
				cout << "Directory full" << endl;
				break;
			case CFileException::badSeek:
				cout << "File pointer error" << endl;
				break;
			case CFileException::hardIO:
				cout << "Hardware error" << endl;
				break;
			case CFileException::sharingViolation:
				cout << "Sharing violation" << endl;
				break;
			case CFileException::lockViolation:
				cout << "Lock violation" << endl;
				break;
			case CFileException::diskFull:
				cout << "Disk full" << endl;
				break;
			case CFileException::endOfFile:
				cout << "End of file" << endl;
				break;
		}
	}
    */
}

