#ifndef CPPUNIT_PROTECTORCHAIN_H
#define CPPUNIT_PROTECTORCHAIN_H

#include <protector/Protector.h>
#include <portability/CppUnitDeque.h>


CPPUNIT_NS_BEGIN

DLL_EXPORT_DEQUE(Protector)



/*! \brief Protector chain (Implementation).
 * Implementation detail.
 * \internal Protector that protect a Functor using a chain of nested Protector.
 */
class CPPUNIT_API ProtectorChain : public Protector
{
public:
  ~ProtectorChain();

  void push( Protector *protector );

  void pop();

  int count() const;

  bool protect( const Functor &functor,
                const ProtectorContext &context );

private:
  class ProtectFunctor;

private:
  typedef CppUnitDeque<Protector *> Protectors;
  Protectors m_protectors;

  typedef CppUnitDeque<Functor *> Functors;
};


CPPUNIT_NS_END


#endif // CPPUNIT_PROTECTORCHAIN_H

