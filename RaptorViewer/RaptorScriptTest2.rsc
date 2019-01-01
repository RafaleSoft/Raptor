{
	deltat = 0.01
	run = loop
	{
		OBJECT Mine;
		Mine = load "Mine";

		SHADER vertexWeighting()
		{
			{
				VECTOR v;
				VECTOR wv;
				VECTOR tmp;
			}
			v.x = DATA(0) * VERTEX.POSITION;
			v.y = DATA(1) * VERTEX.POSITION;
			v.z = DATA(2) * VERTEX.POSITION;
			v.w = DATA(3) * VERTEX.POSITION;

			wv.x = DATA(4) * VERTEX.POSITION;
			wv.y = DATA(5) * VERTEX.POSITION;
			wv.z = DATA(6) * VERTEX.POSITION;
			wv.w = DATA(7) * VERTEX.POSITION;

			tmp = v - wv;
			v = VERTEX.WEIGHT * tmp;
			VERTEX = v + wv;
		}
	}
	{
		1
		{
			"marotation" = linear(100.0,0.0);
			"coolrotation" = cos(1.0);
		}
		{
			opengl->Enable("GL_TEXTURE_2D");
			opengl->Disable("GL_LIGHTING");
			opengl->Push();
			opengl->Translate(0.0,0.0,-200.0);
			opengl->RotateX(90.0);
			Mine->RotateZ("coolrotation");
			Mine->Render();
			opengl->Pop();
		}
	}
}
