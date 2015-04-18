{
	deltat = 0.01
	run = loop
	{
		VECTOR v1;
		
		// Define a minimal shader
		SHADER myshader(MATRIX modelview)
		{
			VERTEX.POSITION = modelview * VERTEX.POSITION;
			VERTEX.COLOR0 = DATA(4);
		}
		SHADER vertexWeighting(	MATRIX modelview,
					MATRIX weightedModelview)
		{
			VECTOR v;
			VECTOR wv;
			VECTOR tmp;

			v = modelview * VERTEX.POSITION;
			wv = weightedModelview * VERTEX.POSITION;
			
			tmp = v - wv;
			v = VERTEX.WEIGHT * tmp;
			VERTEX.POSITION = v + wv;
		}
	}
	{
		1
		{
			"marotation" = linear(100.0,0.0);
			"coolrotation" = cos(10.0);
		}
		{
			opengl->Push();
			opengl->Translate(20.0,15.0,-50.0);
			Star01->RotateY("coolrotation");
			Star01->Render();
			opengl->Pop();
		}
	}
}