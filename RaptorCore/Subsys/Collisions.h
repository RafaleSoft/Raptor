#if !defined(__COLLISIONS_H__)
#define __COLLISIONS_H__


RAPTOR_NAMESPACE_BEGIN


//////////////////////////////////////////////////////////////////////
// Engine private objects
//////////////////////////////////////////////////////////////////////
class CCollisionStatus
{
	public:
		unsigned int m_intersect;
		unsigned int m_leftX;
		unsigned int m_rightX;
		unsigned int m_upY;
		unsigned int m_downY;
		unsigned int m_backZ;
		unsigned int m_frontZ;

	public:
		CCollisionStatus():
		m_intersect(0),m_leftX(0),m_rightX(0),
		m_upY(0),m_downY(0),m_backZ(0),m_frontZ(0)
		{ };

		virtual ~CCollisionStatus() {};

		bool isValid(void);
		void clear(void);
};

__inline bool CCollisionStatus::isValid()
{ return ((m_leftX+m_rightX == 8) && (m_upY+m_downY == 8) && (m_backZ+m_frontZ == 8)); }

__inline void CCollisionStatus::clear(void)
{ m_intersect = m_leftX = m_rightX = m_upY = m_downY = m_backZ = m_frontZ = 0; }

class CCubeBTree
{
	public:
		C3DEngine		*m_engine;
		CPhysics		*m_node;
		GL_MATRIX		transform;

		CCubeBTree		*m_up;
		CCubeBTree		*m_down;
		CCubeBTree		*m_left;
		CCubeBTree		*m_right;
		CCubeBTree		*m_front;
		CCubeBTree		*m_back;

	public:
		CCubeBTree():
		m_up(NULL),m_down(NULL),m_left(NULL),m_right(NULL),m_front(NULL),m_back(NULL),
		m_node(NULL),m_engine(NULL)
		{ };

		CCubeBTree(CPhysics *node,C3DEngine *engine):
		m_up(NULL),m_down(NULL),m_left(NULL),m_right(NULL),m_front(NULL),m_back(NULL),
		m_node(node),m_engine(engine)
		{ };

		virtual ~CCubeBTree();

		//	Store the inverse of the transform
		//	( the inverse is used to determine collisions
		//	with other nodes )
		GL_MATRIX &SetTransform(GL_MATRIX &M);
		CCubeBTree *Insert(CPhysics *obj,GL_MATRIX &M,CCollisionStatus &clip);
};


RAPTOR_NAMESPACE_END


#endif // __COLLISIONS_H__

