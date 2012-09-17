#include <iostream>

#include <configurable_factory.h>

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


SCOPE(MYSCOPE);
SCOPE(MYSCOPE2);


class DatabaseInterface
{
public:
    virtual ~DatabaseInterface() {}
    virtual void accessDB() { std::cout << "Interface  access" << std::endl; }
};

class RealDatabase : public DatabaseInterface
{
public:
    RealDatabase(std::string name) : m_name(name) { }
    virtual ~RealDatabase() {}

    void accessDB() { std::cout << "Real DB: access ... this is slow " << m_name << std::endl; }

    std::string m_name;
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


class AlternativeDatabase : public DatabaseInterface
{
public:
    AlternativeDatabase() {}
    virtual ~AlternativeDatabase() {}

    void accessDB() { std::cout << "Alternative DB access" << std::endl; }
};

class MockDatabase : public DatabaseInterface
{
public:
    MockDatabase(int n) : m_n(n) {}
    virtual ~MockDatabase() {}

    void accessDB() { std::cout << "Mock DB access " << m_n << std::endl; }

    int m_n;
};

DatabaseInterface* my_factory()
{
    return new MockDatabase(3);
}


template<typename T> inline T* build()
{
    return Factory<T>::get();
}



class SUT
{
public:
//    SUT() : m_db(Factory<DatabaseInterface>::get<RealDatabase>("PPS")),
//        m_db1(Factory<DatabaseInterface, LOCAL>::get<AlternativeDatabase>())
//    {
//    }

    const static std::string param;

    SUT() : m_db(Factory<DatabaseInterface>::get()),
        m_db1(Factory<DatabaseInterface>::get<RealDatabase>(param)),
        m_db2(Factory<DatabaseInterface, factory::scopes::MYSCOPE>::get<AlternativeDatabase>())
    {
    }

    ~SUT()
    {
        delete m_db;
        delete m_db1;
        delete m_db2;
    }

    void doSomething()
    {
        m_db->accessDB();
        m_db1->accessDB();
        m_db2->accessDB();
    }

    DatabaseInterface* m_db;
    DatabaseInterface* m_db1;
    DatabaseInterface* m_db2;
};

const std::string SUT::param("PPS");


int main()
{

    CF<DatabaseInterface>::Named<int>::InScope<factory::scopes::MYSCOPE>::get()->accessDB();

    std::cout << factory::scopes::MYSCOPE::idx << factory::scopes::MYSCOPE::name << std::endl;
    std::cout << factory::scopes::MYSCOPE2::idx << factory::scopes::MYSCOPE2::name << std::endl;

//    SUT realInstance;
//    realInstance.doSomething();
//
//    std::cout << "-----------------" << std::endl;
//
//    Factory<DatabaseInterface>::replace<AlternativeDatabase>();
//    SUT alternativeInstance;
//    alternativeInstance.doSomething();
//
//    std::cout << "-----------------" << std::endl;
//
//    Factory<DatabaseInterface>::replace<ParamAlternativeDatabase, std::string>();
//    SUT parameterInstance;
//    parameterInstance.doSomething();
//
//    std::cout << "-----------------" << std::endl;
//
//    Factory<DatabaseInterface>::replace(my_factory);
//    SUT functionInstance;
//    functionInstance.doSomething();
//
//    std::cout << "-----------------" << std::endl;

    MockDatabase* mock = new MockDatabase(1);
    Factory<DatabaseInterface>::inject(mock);

    MockDatabase* mock2 = new MockDatabase(2);
    Factory<DatabaseInterface, factory::scopes::MYSCOPE>::inject(mock2);

    //set expectation on mock here
    SUT testInstance;
    testInstance.doSomething();

    return 0;
}

