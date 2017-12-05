{
	deltat = 0.01
	run = loop
	{
		SHADER vertexWeighting
		{
			{
				VECTOR normal;
				VECTOR tangent;
				VECTOR binormal;
				VECTOR viewer;
				MATRIX m = GL_MODELVIEW;
			}
					
			VERTEX = m * VERTEX.POSITION;
			normal = VERTEX.NORMAL;
			tangent = VERTEX.PARAM0;
			binormal = VERTEX.PARAM1;
			viewer = DATA(7) - VERTEX.POSITION;
			viewer = NORM(viewer);
		}
	}
}