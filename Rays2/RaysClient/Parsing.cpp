
#define READLINE buffer = Text[line++];buffer.TrimLeft(); 
#define HEADER  READLINE\
				if (buffer[0] != '{') return false;else exit++;
#define TAIL	READLINE\
				if (buffer[0] != '}') return false;else exit--;


CCamera *CRaysClientDoc::ParseCamera(int &line)
{
	//	search datas
	int exit=0;
	CString buffer;

	// temp camera
	CCamera *camera = new CCamera();

	HEADER
	while (exit>0)
	{
		READLINE

		if (buffer[0]=='}') {exit--;continue;}
		if (buffer.Find("Origin")==0) {/*exit = ParseVertex();*/ continue;}
		if (buffer.Find("Focale")) 
			{ camera->focale=(float)atof(buffer.GetBuffer(1)+7); continue;}
		if (buffer.Find("Aperture")) 
			{ camera->aperture=(float)atof(buffer.GetBuffer(1)+9); continue;}
		if (buffer.Find("ObjectPlane")) 
			{ camera->object_plane=(float)atof(buffer.GetBuffer(1)+12); continue;}
		if (buffer.Find("Width")) 
			{ camera->width=atoi(buffer.GetBuffer(1)+6); continue;}
		if (buffer.Find("Height")) 
			{ camera->height=atoi(buffer.GetBuffer(1)+7); continue;}
		if (buffer.Find("Depth")) 
			{ camera->depth=atoi(buffer.GetBuffer(1)+6); continue;}
		if (buffer.Find("Variance")) 
			{ camera->variance=atoi(buffer.GetBuffer(1)+9); continue;}
		if (buffer.Find("ReflectionDepth")) 
			{ camera->reflection_depth=atoi(buffer.GetBuffer(1)+16); continue;}
		if (buffer.Find("RefractionDepth")) 
			{ camera->refraction_depth=atoi(buffer.GetBuffer(1)+16); continue;}
		else exit=-1;
	}

	if (exit<0)
	{
		delete camera;
		return NULL;
	}
	else
		return camera;
}


bool CRaysClientDoc::Compile()
{	
	bool stop = false;
	int line = 0;
	CString Line;

	// Clear any data
	PurgeObjects();

	while ((!stop)&&(line<Text.GetSize()))
	{
		Line = Text[line++];
		Line.TrimLeft();

		//	skip comment lines
		if (Line[0] == '*') continue;

		//	skip empty lines
		if (Line.GetLength() == 0) continue;

		if (Line.Find("AxisCamera") == 0) 
		{ 
			stop = (NULL!=(Objects[nbObjects++]=ParseCamera(line))); 
			continue; 
		}

		stop = true;
	}
	if (stop)
	{
		CString msg;
		msg.Format("Line %d:",line-1);
		Cout(msg+Line);
	}
	return (Compiled=!stop);
}