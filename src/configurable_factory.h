#ifndef CONFIGURABLE_FACTORY_H
#define CONFIGURABLE_FACTORY_H

#include <iostream>
#include <boost/type_traits.hpp>
#include <boost/preprocessor.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

namespace factory
{
namespace scopes
{
//marker
struct is_a_scope
{
private:
    is_a_scope();
    ~is_a_scope();
    is_a_scope( const is_a_scope& );
    const is_a_scope& operator=( const is_a_scope& );
};

struct ROOT : private is_a_scope
{
    static long next()
    {
        static long counter = 0;
        return (++counter);
    }

    static std::string name()
    {
        return "ROOT";
    }

    static long const idx()
    {
        return 0;
    }
private:
    ROOT();
    ~ROOT();
    ROOT( const ROOT& );
    const ROOT& operator=( const ROOT& );
};
}
}

#define SCOPE(NAME) \
namespace factory { namespace scopes {\
struct NAME : private is_a_scope {\
    static std::string name() { return BOOST_STRINGIZE(NAME); }\
    static long const idx() { static long counter = ROOT::next(); return counter; }\
private:\
    NAME();\
    ~NAME();\
    NAME( const NAME& );\
    const NAME& operator=( const NAME& );\
};\
} }

template<typename INTF, typename P1, typename Scope=factory::scopes::ROOT> class P1Factory
{
public:
    static boost::shared_ptr<INTF> (*factory)(P1 p1);
    static bool replaced;

    static void reset()
    {
        factory = NULL;
        replaced = false;
    }
private:
    P1Factory();
    ~P1Factory();
    P1Factory( const P1Factory& );
    const P1Factory& operator=( const P1Factory& );
};
template<typename INTF, typename P1, typename Scope> boost::shared_ptr<INTF> (* P1Factory<INTF, P1, Scope>::factory)(P1) = NULL;
template<typename INTF, typename P1, typename Scope> bool                    P1Factory<INTF, P1, Scope>::replaced = false;


template<typename INTF, typename P1, typename SUB, typename Scope=factory::scopes::ROOT> class P1FactoryReplacer
{
public:
    static void replace()
    {
        P1Factory<INTF, P1, Scope>::factory = P1FactoryReplacer<INTF, P1, SUB, Scope>::create;
        P1Factory<INTF, P1, Scope>::replaced = true;
    }

    static boost::shared_ptr<INTF> create(P1 p1)
    {
        std::cout << "[Factory] ....... " << typeid(INTF).name() << " {"<<  Scope::name() << "} (" << typeid(P1).name()<<  ") -> replaced by " << typeid(SUB).name() << std::endl;
        return boost::make_shared<SUB>(p1);
    }
private:
    P1FactoryReplacer();
    ~P1FactoryReplacer();
    P1FactoryReplacer( const P1FactoryReplacer& );
    const P1FactoryReplacer& operator=( const P1FactoryReplacer& );
};


template<typename INTF, typename Scope=factory::scopes::ROOT> class Factory
{
public:

    static boost::shared_ptr<INTF> get()
    {
        if (injected_instance != NULL)
        {
            std::cout << "[Factory] replaced " << typeid(INTF).name() << " {"<<  Scope::name() << "} with injected instance" << std::endl;
            return injected_instance;
        }

        if (factory != NULL)
        {
            std::cout << "[Factory] created " << typeid(INTF).name() << " {"<<  Scope::name() << "} calling injected factory" << std::endl;
            return factory();
        }

        std::cout << "[Factory] created " << typeid(INTF).name() << " {"<<  Scope::name() << "}" << std::endl;
        return boost::make_shared<INTF>();
    }

    template <typename IMPL> static boost::shared_ptr<INTF> get()
    {
        if (injected_instance != NULL)
        {
            std::cout << "[Factory] replaced " << typeid(INTF).name() << " {"<<  Scope::name() << "} -> " << typeid(IMPL).name() << " with injected instance" << std::endl;
            return injected_instance;
        }

        if (factory != NULL)
        {
            std::cout << "[Factory] created " << typeid(INTF).name() << " {"<<  Scope::name() << "} -> " << typeid(IMPL).name() << " calling injected factory" << std::endl;
            return factory();
        }

        std::cout << "[Factory] created " << typeid(INTF).name() << " {"<<  Scope::name() << "} -> " << typeid(IMPL).name() << std::endl;
        return boost::make_shared<IMPL>();
    }

    template <typename IMPL, typename P1> static boost::shared_ptr<INTF> get(P1 p1)
    {
        if (injected_instance != NULL)
        {
            std::cout << "[Factory] replaced " << typeid(INTF).name() << " {"<<  Scope::name() << "} -> " << typeid(IMPL).name() << "(" << typeid(P1).name()<<  ") with injected instance" << std::endl;
            return injected_instance;
        }

        if (P1Factory<INTF,P1,Scope>::replaced)
        {
            std::cout << "[Factory] created " << typeid(INTF).name() << " {"<<  Scope::name() << "} -> " << typeid(IMPL).name() << "(" << typeid(P1).name()<<  ") calling injected factory" << std::endl;
            return P1Factory<INTF,P1,Scope>::factory(p1);
        }

        std::cout << "[Factory] created " << typeid(INTF).name() << " {"<<  Scope::name() << "} -> " << typeid(IMPL).name() << "(" << typeid(P1).name()<<  ")" << std::endl;
        return boost::make_shared<IMPL>(p1);
    }

    //------------------------------------------------------------------------
    //Factory configuration methods

    //Replace the returned type with SUB
    template <typename SUB> static void replace()
    {
        BOOST_STATIC_ASSERT(boost::is_abstract<SUB>::value==false);
        Factory<INTF, Scope>::factory = create<SUB>;
    }

    //Replace the returned type with SUB(P1)
    template <typename SUB, typename P1> static void replace()
    {
        BOOST_STATIC_ASSERT(boost::is_abstract<SUB>::value==false);
        P1FactoryReplacer<INTF, P1, SUB, Scope>::replace();
    }

    //Replace the returned type with a user specified factory
    static void replace(boost::shared_ptr<INTF> (*new_factory)())
    {
        Factory<INTF, Scope>::factory = new_factory;
    }

    //Replace the returned type with a user specified instance
    template <typename INJ> static void inject(INJ* inj)
    {
        Factory<INTF, Scope>::injected_instance = boost::shared_ptr<INTF>(inj);
    }

    static void reset()
    {
        factory = NULL;
        injected_instance.reset();
    }

    template <typename P1> static void reset()
    {
        P1Factory<INTF, P1, Scope>::reset();
    }


private:
    Factory();
    ~Factory();
    Factory( const Factory& );
    const Factory& operator=( const Factory& );

    template <typename SUB> static boost::shared_ptr<INTF> create()
    {
        std::cout << "[Factory] ....... " << typeid(INTF).name() << " {"<<  Scope::name() << "} -> replaced by " << typeid(SUB).name() << std::endl;
        return boost::make_shared<SUB>();
    }

    static boost::shared_ptr<INTF> (*factory)();
    static boost::shared_ptr<INTF> injected_instance;
};

template<typename INTF,typename Scope> boost::shared_ptr<INTF> (* Factory<INTF,Scope>::factory)() = NULL;
template<typename INTF,typename Scope> boost::shared_ptr<INTF> Factory<INTF,Scope>::injected_instance;


#endif // CONFIGURABLE_FACTORY_H
