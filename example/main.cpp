#include <iostream>
#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <boost/mpl/if.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/any.hpp>

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


template <typename T, typename ANNOTATION, typename R> static boost::shared_ptr<T> builder()
{
    boost::shared_ptr<T> res(boost::make_shared<R>());
    std::cout << "[Injector] \"" << ANNOTATION::name() << "\" " << typeid(T).name() << " -> " << typeid(R).name() << " = " << res.get() << std::endl;
    return res;
}

template <typename T, typename ANNOTATION, typename R> static boost::shared_ptr<T> builder(R value)
{
  boost::shared_ptr<T> res(boost::make_shared<R>(value));
  std::cout << "[Injector] \"" << ANNOTATION::name() << "\" " << typeid(T).name() << " = " << value << std::endl;
  return res;
}

//class NoScope
//{
//};
//
//class LoggerScope
//{
//};
//

struct is_a_scope
{
    virtual long getIdx() = 0;

protected:
    static long next()
    {
        static long counter = 0;
        return (++counter);
    }
    is_a_scope() {}
    virtual ~is_a_scope() {}
    is_a_scope( const is_a_scope& );
    const is_a_scope& operator=( const is_a_scope& );
};

struct NOSCOPE : public is_a_scope
{
    template<typename T> static boost::function<boost::shared_ptr<T>()> scope(boost::function<boost::shared_ptr<T>()> unscopedProvider)
    {
        std::cout << "NOSCOPE::scope called" << std::endl;
        return unscopedProvider;
    }

    static NOSCOPE* instance()
    {
        if (!m_instance)
        {
            m_instance = new NOSCOPE();
        }
        return m_instance;
    }

    static std::string name()
    {
        return "NOSCOPE";
    }

    static long const idx()
    {
        static long counter = is_a_scope::next();
        return counter;
    }

    long getIdx()
    {
        return idx();
    }
private:
    static NOSCOPE* m_instance;
    NOSCOPE() {}
    ~NOSCOPE() {};
    NOSCOPE( const NOSCOPE& );
    const NOSCOPE& operator=( const NOSCOPE& );
};
NOSCOPE* NOSCOPE::m_instance = NULL;

struct LoggerScope : public is_a_scope
{
    template<typename T> static boost::function<boost::shared_ptr<T>()> scope(boost::function<boost::shared_ptr<T>()> unscopedProvider)
    {
        std::cout << "LoggerScope::scope called" << std::endl;

        return boost::bind(LoggerScope::scopedProvider<T>, unscopedProvider);
    }

    template<typename T> static boost::shared_ptr<T> scopedProvider(boost::function<boost::shared_ptr<T>()> unscopedProvider)
    {
        std::cout << "LoggerScope scoping called" << std::endl;
        boost::shared_ptr<T> res = unscopedProvider();
        std::cout << "LoggerScope scoping finished" << std::endl;
        return res;
    }

    static LoggerScope* instance()
    {
        if (!m_instance)
        {
            m_instance = new LoggerScope();
        }
        return m_instance;
    }

    static std::string name()
    {
        return "LoggerScope";
    }

    static long const idx()
    {
        static long counter = is_a_scope::next();
        return counter;
    }

    long getIdx()
    {
        return idx();
    }
private:
    static LoggerScope* m_instance;
    LoggerScope() {}
    ~LoggerScope() {};
    LoggerScope( const NOSCOPE& );
    const LoggerScope& operator=( const NOSCOPE& );
};
LoggerScope* LoggerScope::m_instance = NULL;


template <typename T, typename ANNOTATION=factory::annotation::FREE> class Injector;
template <typename T, typename ANNOTATION, typename SCOPE> class ScopedBindingStore;

template <typename T> class bind
{
public:
    template <typename R> static void to()
    {
        Injector<T>::unscopedProvider = boost::bind(builder<T, factory::annotation::FREE, R>);
    }

    template <typename R> static void to(R value)
    {
        Injector<T>::unscopedProvider = boost::bind(builder<T, factory::annotation::FREE, R>, value);
    }

    template <typename A> class annotatedWith
    {
    public:

        template <typename S> class in
        {
        public:

            template <typename R> static void to()
            {
                annotatedWith_in_to<A, S, R>();
            }

            template <typename R> static void to(R value)
            {
                annotatedWith_in_to<A, S, R>(value);
            }
        };

        template <typename R> static void to()
        {
            annotatedWith_to<A, R>();
        }

        template <typename R> static void to(R value)
        {
            annotatedWith_to<A, R>(value);
        }
    };


    template <typename S> class in
    {
    public:

        template <typename A> class annotatedWith
        {
        public:
            template <typename R> static void to()
            {
                annotatedWith_in_to<A, S, R>();
            }

            template <typename R> static void to(R value)
            {
                annotatedWith_in_to<A, S, R>(value);
            }
        };

        template <typename R> static void to()
        {
            annotatedWith_in_to<factory::annotation::FREE, S, R>();
        }

        template <typename R> static void to(R value)
        {
            annotatedWith_in_to<factory::annotation::FREE, S, R>(value);
        }
    };

private:
    template <typename A, typename R> static void annotatedWith_to()
    {
        Injector<T, A>::unscopedProvider = boost::bind(builder<T, A, R>);
    }

    template <typename A, typename R> static void annotatedWith_to(R value)
    {
        Injector<T, A>::unscopedProvider = boost::bind(builder<T, A, R>, value);
    }

//    template <typename S, typename R> static void in_to()
//    {
//        ScopedBindingStore<T, factory::annotation::FREE, S>::provider = LoggerScope::scope<T>(boost::bind<boost::shared_ptr<T> >(builder<T, factory::annotation::FREE, R>));
//        ScopedBindingStore<T, factory::annotation::FREE, S>::set();
//    }
//
//    template <typename S, typename R> static void in_to(R value)
//    {
//        ScopedBindingStore<T, factory::annotation::FREE, S>::provider = LoggerScope::scope<T>(boost::bind<boost::shared_ptr<T> >(builder<T, factory::annotation::FREE, R>, value));
//        ScopedBindingStore<T, factory::annotation::FREE, S>::set();
//    }

    template <typename A, typename S, typename R> static void annotatedWith_in_to()
    {
        ScopedBindingStore<T, A, S>::provider = LoggerScope::scope<T>(boost::bind<boost::shared_ptr<T> >(builder<T, A, R>));
        ScopedBindingStore<T, A, S>::set();
    }

    template <typename A, typename S, typename R> static void annotatedWith_in_to(R value)
    {
        ScopedBindingStore<T, A, S>::provider = LoggerScope::scope<T>(boost::bind<boost::shared_ptr<T> >(builder<T, A, R>, value));
        ScopedBindingStore<T, A, S>::set();
    }

};



template <typename T, typename ANNOTATION, typename SCOPE> class ScopedBindingStore
{
public:

    static void set()
    {
        Injector<T,ANNOTATION>::providerMap.insert(std::make_pair(SCOPE::idx(),provider));
    }

    static boost::function<boost::shared_ptr<T>()> provider;
};
template<typename T, typename ANNOTATION, typename SCOPE> boost::function<boost::shared_ptr<T>()> ScopedBindingStore<T, ANNOTATION, SCOPE>::provider = NULL;


struct Scope
{
    template<typename S> static void enter()
    {
        currentScope = S::instance();
    }

    static void exit()
    {
        currentScope = NULL;
    }
    static is_a_scope* currentScope;
};
is_a_scope* Scope::currentScope = NULL;


template <typename T, typename ANNOTATION> class Injector
{

public:
    static boost::shared_ptr<T> get()
    {
        if (Scope::currentScope)
        {
            std::cout << "get " << Scope::currentScope->getIdx() << std::endl;

            typename std::map<long, boost::function<boost::shared_ptr<T>()> >::iterator iter = providerMap.find(Scope::currentScope->getIdx());
            if (iter != providerMap.end())
            {
                return iter->second();
            }
        }
        return unscopedProvider();
    }

    static std::map<long, boost::function<boost::shared_ptr<T>()> > providerMap;
private:
    static boost::function<boost::shared_ptr<T>()> unscopedProvider;

    friend class bind<T>;
};
template<typename T, typename ANNOTATION> boost::function<boost::shared_ptr<T>()> Injector<T, ANNOTATION>::unscopedProvider = boost::bind(builder<T, ANNOTATION, T>);
template<typename T, typename ANNOTATION> std::map<long, boost::function<boost::shared_ptr<T>()> > Injector<T, ANNOTATION>::providerMap;

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
//
//    std::cout << "################# 2nd approach" << std::endl;
//
//    boost::shared_ptr<DatabaseInterface> p1,p2,p3,p4,p5,p6,p7,p8,p9;
//
//    p1 = CF<DatabaseInterface>::get();
//
//    p2 = CF<DatabaseInterface, factory::scopes::MYSCOPE2>::get();
//    p3 = CF<DatabaseInterface, void, factory::scopes::MYSCOPE2>::get();
//    p4 = CF<DatabaseInterface, factory::scopes::MYSCOPE2, void >::get();
//
//    p5 = CF<DatabaseInterface, factory::names::NEWNAME>::get();
//    p6 = CF<DatabaseInterface, void, factory::names::NEWNAME>::get();
//    p7 = CF<DatabaseInterface, factory::names::NEWNAME, void >::get();
//
//    p8 = CF<DatabaseInterface, factory::scopes::MYSCOPE2, factory::names::NEWNAME>::get();
//    p9 = CF<DatabaseInterface, factory::names::NEWNAME, factory::scopes::MYSCOPE2>::get();
//
    std::cout << "################# 3rd approach" << std::endl;

    boost::shared_ptr<DatabaseInterface> i1,i2,i3,i4,i5,i6,i7,i8,i9;

    bind<DatabaseInterface>::annotatedWith<factory::annotation::NEWANNOTATION>::to<AlternativeDatabase>();
    bind<DatabaseInterface>::to<AlternativeDatabase>();
    bind<int>::to(145);
    bind<int>::annotatedWith<factory::annotation::NEWANNOTATION>::to(157);

    //bind<int>::in<LoggerScope>::to<int>(74);
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

