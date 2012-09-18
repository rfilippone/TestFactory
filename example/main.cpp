#include <iostream>

#include "configurable_factory.h"
#include "SUT.h"
#include "AlternativeDatabase.h"

using namespace std;

namespace factory {

namespace names {

struct UNNAMED {};
}

}

template <typename T, typename NAME, typename SCOPE> class CCF
{
public:
    static T* get()
    {
        return new T();
    }
};


template <typename T> class CF
{
   template <typename NAME> class Named {
        template <typename SCOPE> class InScope {
        public:
            static T* get()
            {
                return CCF<T, NAME, SCOPE>::get();
            }
        };
   public:
        static T* get()
        {
            return CCF<T, NAME, factory::scopes::ROOT>::get();
        }
    };

    template <typename SCOPE> class InScope {
        template <typename NAME> class Named {
        public:
            static T* get()
            {
                return CCF<T, NAME, SCOPE>::get();
            }
        };
    public:
        static T* get()
        {
           return CCF<T, factory::names::UNNAMED, SCOPE>::get();
        }
     };

public:
    static T* get()
    {
        return CCF<T, factory::names::UNNAMED, factory::scopes::ROOT>::get();
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

#define CREATE(T) CF<DatabaseInterface>::get()
#define CREATE_S(T, S) CF<DatabaseInterface>::InScope<S>::get()
#define CREATE_N(T, N) CF<DatabaseInterface>::Named<N>::get()
#define CREATE_SN(T, S, N) CF<DatabaseInterface>::Named<N>::get()

int main()
{
    DatabaseInterface* a = CREATE(DatabaseInterface);
    a->accessDB();

    DatabaseInterface* b = CREATE_S(DatabaseInterface, factory::scopes::MYSCOPE);
    b->accessDB();

    DatabaseInterface* c = CREATE_S(DatabaseInterface, int);
    c->accessDB();

    DatabaseInterface* d = CREATE_SN(DatabaseInterface, factory::scopes::MYSCOPE, int);
    d->accessDB();

    std::cout << factory::scopes::MYSCOPE::idx() << " - " << factory::scopes::MYSCOPE::name() << std::endl;
    std::cout << factory::scopes::MYSCOPE2::idx() << " - " << factory::scopes::MYSCOPE2::name() << std::endl;

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

