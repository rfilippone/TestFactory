#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <boost/mpl/if.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

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
        boost::shared_ptr<T> res(boost::make_shared<T>());
        std::cout << "[CF]  {N:" << name::name() << ", S:" << scope::name() << "} " << typeid(T).name() << " = "<< res.get() << std::endl;
        return res;
    }
};


namespace factory {

namespace annotation {
//marker
struct is_an_annotation
{
private:
    is_an_annotation();
    ~is_an_annotation();
    is_an_annotation( const is_an_annotation& );
    const is_an_annotation& operator=( const is_an_annotation& );
};
struct FREE : private is_an_annotation {
    static std::string name()
    {
        return "FREE";
    }
private:
    FREE();
    ~FREE();
    FREE( const FREE& );
    const FREE& operator=( const FREE& );
};


struct NEWANNOTATION : private is_an_annotation {
    static std::string name()
    {
        return "NEWANNOTATION";
    }
private:
    NEWANNOTATION();
    ~NEWANNOTATION();
    NEWANNOTATION( const NEWANNOTATION& );
    const NEWANNOTATION& operator=( const NEWANNOTATION& );
};

}
}

struct ScopeType
{
	typedef ScopeType type;
	template <typename T, typename ANNOTATION, typename R> static boost::shared_ptr<T> get();
	template <typename T, typename ANNOTATION, typename R> static boost::shared_ptr<T> get(R value);
};


struct NoScope : public ScopeType
{
	typedef NoScope type;
	template <typename T, typename ANNOTATION, typename R> static boost::shared_ptr<T> get()
	{
		boost::shared_ptr<T> res(boost::make_shared<R>());
		std::cout << "[Injector] {" << typeid(type).name() << "} \"" << ANNOTATION::name() << "\" " << typeid(T).name() << " -> " << typeid(R).name() << " = " << res.get() << std::endl;
		return res;
	}

	template <typename T, typename ANNOTATION, typename R> static boost::shared_ptr<T> get(R value)
	{
	    boost::shared_ptr<T> res(boost::make_shared<R>(value));
	    std::cout << "[Injector] {" << typeid(type).name() << "} \"" << ANNOTATION::name() << "\" " << typeid(T).name() << " = " << value << std::endl;
	    return res;
	}
};


template <typename T, typename ANNOTATION=factory::annotation::FREE> class Injector;

template <typename T> class bind
{
public:
    template <typename R> static void to()
    {
        Injector<T>::bprovider = boost::bind(NoScope::get<T, factory::annotation::FREE, R>);
    }

    template <typename R> static void to(R value)
    {
        Injector<T>::bprovider = boost::bind(NoScope::get<T, factory::annotation::FREE, R>, value);
    }

    template <typename A> class annotatedWith
    {
    public:
        template <typename R> static void to()
        {
            annotatedWith_to<A, R>();
        }

        template <typename R> static void to(R value)
        {
            annotatedWith_to<A, R>(value);
        }
    };


    template <typename A> class in
    {
    public:


    };

private:
    template <typename A, typename R> static void annotatedWith_to()
    {
        Injector<T, A>::bprovider = boost::bind(NoScope::get<T, A, R>);
    }

    template <typename A, typename R> static void annotatedWith_to(R value)
    {
        Injector<T, A>::bprovider = boost::bind(NoScope::get<T, A, R>, value);
    }
};

template <typename T, typename ANNOTATION> class Injector
{

public:
    static boost::shared_ptr<T> get()
    {
        return bprovider();
    }

private:
    static boost::function<boost::shared_ptr<T>()> bprovider;
//    static std::map<ScopeType, int >& getScopeMap()
//    {
//        static std::map<ScopeType, int > scopeMap;
//        return scopeMap;
//    }

    friend class bind<T>;
};
template<typename T, typename ANNOTATION> boost::function<boost::shared_ptr<T>()> Injector<T, ANNOTATION>::bprovider = boost::bind(NoScope::get<T, ANNOTATION, T>);



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
    std::cout << "################# 1st approach" << std::endl;

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

    std::cout << "################# 2nd approach" << std::endl;

    boost::shared_ptr<DatabaseInterface> p1,p2,p3,p4,p5,p6,p7,p8,p9;

    p1 = CF<DatabaseInterface>::get();

    p2 = CF<DatabaseInterface, factory::scopes::MYSCOPE2>::get();
    p3 = CF<DatabaseInterface, void, factory::scopes::MYSCOPE2>::get();
    p4 = CF<DatabaseInterface, factory::scopes::MYSCOPE2, void >::get();

    p5 = CF<DatabaseInterface, factory::names::NEWNAME>::get();
    p6 = CF<DatabaseInterface, void, factory::names::NEWNAME>::get();
    p7 = CF<DatabaseInterface, factory::names::NEWNAME, void >::get();

    p8 = CF<DatabaseInterface, factory::scopes::MYSCOPE2, factory::names::NEWNAME>::get();
    p9 = CF<DatabaseInterface, factory::names::NEWNAME, factory::scopes::MYSCOPE2>::get();

    std::cout << "################# 3rd approach" << std::endl;

    boost::shared_ptr<DatabaseInterface> i1,i2,i3,i4,i5,i6,i7,i8,i9;

    bind<DatabaseInterface>::annotatedWith<factory::annotation::NEWANNOTATION>::to<AlternativeDatabase>();
    bind<DatabaseInterface>::to<AlternativeDatabase>();
    bind<int>::to(145);
    bind<int>::annotatedWith<factory::annotation::NEWANNOTATION>::to(157);

    i2 = Injector<DatabaseInterface, factory::annotation::NEWANNOTATION>::get();
    i1 = Injector<DatabaseInterface>::get();


    boost::shared_ptr<int> n1 = Injector<int>::get();
    std::cout << "n1 = " << *n1 << std::endl;

    boost::shared_ptr<int> n2 = Injector<int, factory::annotation::NEWANNOTATION>::get();
    std::cout << "n2 = " << *n2 << std::endl;

    return 0;
}

