#include <iostream>
#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include "configurable_factory.h"
#include "injector.h"

#include "SUT.h"
#include "AlternativeDatabase.h"


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
//    std::cout << "################# 1st approach" << std::endl;
//
//    SUT realInstance;
//    realInstance.doSomething();
//
//    std::cout << "-----------------" << std::endl;
//
//    Factory<DatabaseInterface>::replace<AlternativeDatabase>();
//    SUT alternativeInstance;
//    alternativeInstance.doSomething();
//    Factory<DatabaseInterface>::reset();
//
//
//    std::cout << "-----------------" << std::endl;
//
//    Factory<DatabaseInterface>::replace<ParamAlternativeDatabase, std::string>();
//    SUT parameterInstance;
//    parameterInstance.doSomething();
//    Factory<DatabaseInterface>::reset<std::string>();
//    Factory<DatabaseInterface>::reset();
//
//    std::cout << "-----------------" << std::endl;
//
//    //Factory<DatabaseInterface>::replace(my_factory);
//    SUT functionInstance;
//    functionInstance.doSomething();
//    Factory<DatabaseInterface>::reset();
//
//    std::cout << "-----------------" << std::endl;
//
//    MockDatabase* mock = new MockDatabase(1);
//    Factory<DatabaseInterface>::inject(mock);
//
//    MockDatabase* mock2 = new MockDatabase(2);
//    Factory<DatabaseInterface, factory::scopes::MYSCOPE>::inject(mock2);
//
//    //set expectation on mock here
//    SUT testInstance;
//    testInstance.doSomething();
//    Factory<DatabaseInterface>::reset();

    std::cout << "################# 3rd approach" << std::endl;

    boost::shared_ptr<DatabaseInterface> i1,i2,i3,i4,i5,i6,i7,i8,i9;

    bind<DatabaseInterface>::annotatedWith<factory::annotation::NEWANNOTATION>::to<AlternativeDatabase>();
    bind<DatabaseInterface>::to<AlternativeDatabase>();
    bind<int>::to(145);
    bind<int>::annotatedWith<factory::annotation::NEWANNOTATION>::to(157);

    bind<int>::in<LoggerScope>::to<int>(74);
    bind<int>::to(74);
    bind<int>::in<LoggerScope>::annotatedWith<factory::annotation::NEWANNOTATION>::to<int>(1234);

    i2 = Injector<DatabaseInterface, factory::annotation::NEWANNOTATION>::get();
    i1 = Injector<DatabaseInterface>::get();


    boost::shared_ptr<int> n1 = Injector<int>::get();
    std::cout << "n1 = " << *n1 << std::endl;

    boost::shared_ptr<int> n2 = Injector<int, factory::annotation::NEWANNOTATION>::get();
    std::cout << "n2 = " << *n2 << std::endl;

    Scope::enter<NOSCOPE>();

    Scope::enter<LoggerScope>();

    boost::shared_ptr<int> n3 = Injector<int, factory::annotation::NEWANNOTATION>::get();
    std::cout << "n3 = " << *n3 << std::endl;

    return 0;
}

