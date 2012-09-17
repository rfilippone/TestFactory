#include <iostream>

#include <configurable_factory.h>

using namespace std;



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
    ~RealDatabase() {}

    void accessDB() { std::cout << "Real DB: access ... this is slow " << m_name << std::endl; }

    std::string m_name;
};

class ParamAlternativeDatabase : public DatabaseInterface
{
public:
    ParamAlternativeDatabase(std::string name) : m_name(name) { }
    ~ParamAlternativeDatabase() {}

    void accessDB() { std::cout << "ParamAlternative  DB access " << m_name << std::endl; }

    //virtual void abc() =0;
    std::string m_name;
};


class AlternativeDatabase : public DatabaseInterface
{
public:
    AlternativeDatabase() {}
    ~AlternativeDatabase() {}

    void accessDB() { std::cout << "Alternative DB access" << std::endl; }
};

class MockDatabase : public DatabaseInterface
{
public:
    MockDatabase(int n) : m_n(n) {}
    ~MockDatabase() {}

    void accessDB() { std::cout << "Mock DB access " << m_n << std::endl; }

    int m_n;
};

DatabaseInterface* my_factory()
{
    return new MockDatabase(1);
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
        m_db2(Factory<DatabaseInterface, LOCAL>::get<AlternativeDatabase>())
    {
//        int* val = build<int>(1);
//        std::cout << *val << std::endl;

        long* val2 = Factory<long>::get();
        long* val3 = Factory<long>::get<long, long>(1);

        long* sval2 = build<long>();

        delete val2;
        delete val3;
        delete sval2;
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

    SUT realInstance;
    realInstance.doSomething();

    std::cout << "-----------------" << std::endl;

//    Factory<DatabaseInterface>::replace<AlternativeDatabase>();
//    SUT alternativeInstance;
//    alternativeInstance.doSomething();

//    std::cout << "-----------------" << std::endl;

    Factory<DatabaseInterface>::replace<ParamAlternativeDatabase, std::string>();
    SUT parameterInstance;
    parameterInstance.doSomething();

//    std::cout << "-----------------" << std::endl;

//    Factory<DatabaseInterface>::replace(my_factory);
//    SUT functionInstance;
//    functionInstance.doSomething();

//    std::cout << "-----------------" << std::endl;

//    MockDatabase* mock = new MockDatabase(1);
//    Factory<DatabaseInterface>::inject(mock);

//    MockDatabase* mock2 = new MockDatabase(2);
//    Factory<DatabaseInterface, LOCAL>::inject(mock2);

//    //set expectation on mock here
//    SUT testInstance;
//    testInstance.doSomething();

    return 0;
}

