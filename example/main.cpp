#include <iostream>
#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "configurable_factory.h"
#include "SUT.h"
#include "AlternativeDatabase.h"


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

/**
 * This is the basic builder for a type TYPE with an annotation ANNOTATION bound to a type IMPL
 * @return boost::shared_ptr<TYPE> A shared pointer to a new instance of @code IMPL
 */
template <typename TYPE, typename ANNOTATION, typename IMPL> boost::shared_ptr<TYPE> builder()
{
    boost::shared_ptr<TYPE> result(boost::make_shared<IMPL>());
    std::cout << "[builder] \"" << ANNOTATION::name() << "\" " << typeid(TYPE).name() << " -> " << typeid(IMPL).name() << " = " << result.get() << std::endl;
    return result;
}

/**
 * This is the basic builder for a type TYPE with an annotation ANNOTATION bound to an instance @code value of type @code IMPL
 * @return @code boost::shared_ptr<TYPE> A shared pointer to a new instance of @code IMPL with value @code value
 */
template <typename TYPE, typename ANNOTATION, typename IMPL> boost::shared_ptr<TYPE> builder(IMPL value)
{
  boost::shared_ptr<TYPE> res(boost::make_shared<IMPL>(value));
  std::cout << "[builder] \"" << ANNOTATION::name() << "\" " << typeid(TYPE).name() << " = " << value << std::endl;
  return res;
}

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
    template<typename TYPE> static boost::function<boost::shared_ptr<TYPE>()> scope(boost::function<boost::shared_ptr<TYPE>()> unscopedProvider)
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

    template<typename TYPE> static boost::function<boost::shared_ptr<TYPE>()> scope(boost::function<boost::shared_ptr<TYPE>()> unscopedProvider);

private:
    static LoggerScope* m_instance;
    LoggerScope() {}
    ~LoggerScope() {};
    LoggerScope( const LoggerScope& );
    const LoggerScope& operator=( const LoggerScope& );

    template<typename TYPE> static boost::shared_ptr<TYPE> scopedProvider(boost::function<boost::shared_ptr<TYPE>()> unscopedProvider)
    {
        std::cout << "LoggerScope scoping called" << std::endl;
        boost::shared_ptr<TYPE> res = unscopedProvider();
        std::cout << "LoggerScope scoping finished" << std::endl;
        return res;
    }
};
LoggerScope* LoggerScope::m_instance = NULL;

template<typename TYPE> boost::function<boost::shared_ptr<TYPE>()> LoggerScope::scope(boost::function<boost::shared_ptr<TYPE>()> unscopedProvider)
{
    std::cout << "LoggerScope::scope called" << std::endl;

    return boost::bind(LoggerScope::scopedProvider<TYPE>, unscopedProvider);
}


template <typename TYPE, typename ANNOTATION=factory::annotation::FREE> class Injector;
template <typename TYPE, typename ANNOTATION, typename SCOPE> class ScopedBindingStore;

template <typename TYPE> class bind
{
public:
    template <typename IMPL> static void to()
    {
        Injector<TYPE>::unscopedProvider = boost::bind(builder<TYPE, factory::annotation::FREE, IMPL>);
    }

    template <typename IMPL> static void to(IMPL value)
    {
        Injector<TYPE>::unscopedProvider = boost::bind(builder<TYPE, factory::annotation::FREE, IMPL>, value);
    }

    template <typename ANNOTATION> class annotatedWith
    {
    public:

        template <typename SCOPE> class in
        {
        public:

            template <typename IMPL> static void to()
            {
                annotatedWith_in_to<ANNOTATION, SCOPE, IMPL>();
            }

            template <typename IMPL> static void to(IMPL value)
            {
                annotatedWith_in_to<ANNOTATION, SCOPE, IMPL>(value);
            }
        };

        template <typename IMPL> static void to()
        {
            annotatedWith_to<ANNOTATION, IMPL>();
        }

        template <typename IMPL> static void to(IMPL value)
        {
            annotatedWith_to<ANNOTATION, IMPL>(value);
        }
    };


    template <typename SCOPE> class in
    {
    public:

        template <typename ANNOTATION> class annotatedWith
        {
        public:
            template <typename IMPL> static void to()
            {
                annotatedWith_in_to<ANNOTATION, SCOPE, IMPL>();
            }

            template <typename IMPL> static void to(IMPL value)
            {
                annotatedWith_in_to<ANNOTATION, SCOPE, IMPL>(value);
            }
        };

        template <typename IMPL> static void to()
        {
            annotatedWith_in_to<factory::annotation::FREE, SCOPE, IMPL>();
        }

        template <typename IMPL> static void to(IMPL value)
        {
            annotatedWith_in_to<factory::annotation::FREE, SCOPE, IMPL>(value);
        }
    };

private:
    template <typename ANNOTATION, typename IMPL> static void annotatedWith_to()
    {
        Injector<TYPE, ANNOTATION>::unscopedProvider = boost::bind(builder<TYPE, ANNOTATION, IMPL>);
    }

    template <typename ANNOTATION, typename IMPL> static void annotatedWith_to(IMPL value)
    {
        Injector<TYPE, ANNOTATION>::unscopedProvider = boost::bind(builder<TYPE, ANNOTATION, IMPL>, value);
    }

    template <typename ANNOTATION, typename SCOPE, typename IMPL> static void annotatedWith_in_to()
    {
        ScopedBindingStore<TYPE, ANNOTATION, SCOPE>::provider = LoggerScope::scope<TYPE>(boost::bind<boost::shared_ptr<TYPE> >(builder<TYPE, ANNOTATION, IMPL>));
        ScopedBindingStore<TYPE, ANNOTATION, SCOPE>::set();
    }

    template <typename ANNOTATION, typename SCOPE, typename IMPL> static void annotatedWith_in_to(IMPL value)
    {
        ScopedBindingStore<TYPE, ANNOTATION, SCOPE>::provider = LoggerScope::scope<TYPE>(boost::bind<boost::shared_ptr<TYPE> >(builder<TYPE, ANNOTATION, IMPL>, value));
        ScopedBindingStore<TYPE, ANNOTATION, SCOPE>::set();
    }
};



template <typename TYPE, typename ANNOTATION, typename SCOPE> class ScopedBindingStore
{
public:

    static void set()
    {
        Injector<TYPE, ANNOTATION>::providerMap.insert(std::make_pair(SCOPE::idx(), provider));
    }

    static boost::function<boost::shared_ptr<TYPE>()> provider;
};
template<typename TYPE, typename ANNOTATION, typename SCOPE> boost::function<boost::shared_ptr<TYPE>()> ScopedBindingStore<TYPE, ANNOTATION, SCOPE>::provider = NULL;


struct Scope
{
    template<typename SCOPE> static void enter()
    {
        currentScope = SCOPE::instance();
    }

    static void exit()
    {
        currentScope = NULL;
    }
    static is_a_scope* currentScope;
};
is_a_scope* Scope::currentScope = NULL;


template <typename TYPE, typename ANNOTATION> class Injector
{
public:
    static boost::shared_ptr<TYPE> get()
    {
        if (Scope::currentScope)
        {
            typename std::map<long, boost::function<boost::shared_ptr<TYPE>()> >::iterator iter = providerMap.find(Scope::currentScope->getIdx());
            if (iter != providerMap.end())
            {
                return iter->second();
            }
        }
        return unscopedProvider();
    }

private:
    static std::map<long, boost::function<boost::shared_ptr<TYPE>()> > providerMap;
    static boost::function<boost::shared_ptr<TYPE>()> unscopedProvider;

    friend class bind<TYPE>;
    template <class , class , class> friend class ScopedBindingStore;    
};
template<typename TYPE, typename ANNOTATION> boost::function<boost::shared_ptr<TYPE>()> Injector<TYPE, ANNOTATION>::unscopedProvider = boost::bind(builder<TYPE, ANNOTATION, TYPE>);
template<typename TYPE, typename ANNOTATION> std::map<long, boost::function<boost::shared_ptr<TYPE>()> > Injector<TYPE, ANNOTATION>::providerMap;

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

