#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <boost/mpl/if.hpp>

#include "configurable_factory.h"
#include "SUT.h"
#include "AlternativeDatabase.h"


using namespace std;
using namespace boost::mpl;

namespace factory {

namespace names {
//marker
struct is_a_name
{
private:
    is_a_name();
    ~is_a_name();
    is_a_name( const is_a_name& );
    const is_a_name& operator=( const is_a_name& );
};
struct UNNAMED : private is_a_name {
    static std::string name()
    {
        return "UNNAMED";
    }
private:
    UNNAMED();
    ~UNNAMED();
    UNNAMED( const UNNAMED& );
    const UNNAMED& operator=( const UNNAMED& );
};


struct NEWNAME : private is_a_name {
    static std::string name()
    {
        return "NEWNAME";
    }
private:
    NEWNAME();
    ~NEWNAME();
    NEWNAME( const NEWNAME& );
    const NEWNAME& operator=( const NEWNAME& );
};

}
}

template <typename T, typename SELECTOR1=void, typename SELECTOR2=void> class CF
{
    typedef typename if_c<boost::is_base_of<factory::scopes::is_a_scope, SELECTOR1>::value,
                          SELECTOR1,
                          typename if_c<boost::is_base_of<factory::scopes::is_a_scope, SELECTOR2>::value,
                                        SELECTOR2,
                                        factory::scopes::ROOT>::type
                         >::type scope;

    typedef typename if_c<boost::is_base_of<factory::names::is_a_name, SELECTOR1>::value,
                          SELECTOR1,
                          typename if_c<boost::is_base_of<factory::names::is_a_name, SELECTOR2>::value,
                                        SELECTOR2,
                                        factory::names::UNNAMED>::type
                         >::type name;
public:
    static boost::shared_ptr<T> get()
    {
        std::cout << "[CF]  {N:" << name::name() << ", S:" << scope::name() << "} " << typeid(T).name() << std::endl;
        return boost::make_shared<T>();
    }
};

class ParamAlternativeDatabase : public DatabaseInterface
{
public:
    ParamAlternativeDatabase(std::string name) : m_name(name) { }
    virtual ~ParamAlternativeDatabase() {}

    void accessDB() { std::cout << "ParamAlternative  DB access " << m_name << std::endl; }

    //virtual void abc() =0;
    std::string m_name;
};

class MockDatabase : public DatabaseInterface
{
public:
    MockDatabase(int n) : m_n(n) {}
    virtual ~MockDatabase() {}

    void accessDB() { std::cout << "Mock DB access " << m_n << std::endl; }

    int m_n;
};

boost::shared_ptr<DatabaseInterface> my_factory()
{
    return boost::make_shared<MockDatabase>(3);
}


template<typename T> inline T* build()
{
    return Factory<T>::get();
}

int main()
{
    CF<DatabaseInterface>::get();

    CF<DatabaseInterface, factory::scopes::MYSCOPE2>::get();
    CF<DatabaseInterface, void, factory::scopes::MYSCOPE2>::get();
    CF<DatabaseInterface, factory::scopes::MYSCOPE2, void >::get();

    CF<DatabaseInterface, factory::names::NEWNAME>::get();
    CF<DatabaseInterface, void, factory::names::NEWNAME>::get();
    CF<DatabaseInterface, factory::names::NEWNAME, void >::get();

    CF<DatabaseInterface, factory::scopes::MYSCOPE2, factory::names::NEWNAME>::get();
    CF<DatabaseInterface, factory::names::NEWNAME, factory::scopes::MYSCOPE2>::get();

    SUT realInstance;
    realInstance.doSomething();

    std::cout << "-----------------" << std::endl;

    Factory<DatabaseInterface>::replace<AlternativeDatabase>();
    SUT alternativeInstance;
    alternativeInstance.doSomething();
    Factory<DatabaseInterface>::reset();


    std::cout << "-----------------" << std::endl;

    Factory<DatabaseInterface>::replace<ParamAlternativeDatabase, std::string>();
    SUT parameterInstance;
    parameterInstance.doSomething();
    Factory<DatabaseInterface>::reset<std::string>();
    Factory<DatabaseInterface>::reset();

    std::cout << "-----------------" << std::endl;

    Factory<DatabaseInterface>::replace(my_factory);
    SUT functionInstance;
    functionInstance.doSomething();
    Factory<DatabaseInterface>::reset();

    std::cout << "-----------------" << std::endl;

    MockDatabase* mock = new MockDatabase(1);
    Factory<DatabaseInterface>::inject(mock);

    MockDatabase* mock2 = new MockDatabase(2);
    Factory<DatabaseInterface, factory::scopes::MYSCOPE>::inject(mock2);

    //set expectation on mock here
    SUT testInstance;
    testInstance.doSomething();
    Factory<DatabaseInterface>::reset();

    return 0;
}

