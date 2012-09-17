#ifndef CONFIGURABLE_FACTORY_H
#define CONFIGURABLE_FACTORY_H

#include <boost/type_traits.hpp>
#include <boost/preprocessor.hpp>

namespace factory
{
namespace scopes
{

class ROOT
{
public:
    static long next()
    {
        static long counter = 0;
        return (++counter);
    }

    const static std::string name;
    const static long idx = 0;
};
const std::string ROOT::name("ROOT");
}
}

#define SCOPE(NAME) \
namespace factory { namespace scopes {\
class NAME {\
public:\
    const static std::string name;\
    const static long idx;\
};\
const std::string NAME::name(BOOST_STRINGIZE(NAME));\
const long NAME::idx = ROOT::next();\
} }




template<typename INTF, typename P1, typename Scope=factory::scopes::ROOT> class P1Factory
{
public:
    static INTF* (*factory)(P1 p1);
    static bool replaced;
};
template<typename INTF, typename P1, typename Scope> INTF*      (* P1Factory<INTF, P1, Scope>::factory)(P1) = NULL;
template<typename INTF, typename P1, typename Scope> bool       P1Factory<INTF, P1, Scope>::replaced = false;


template<typename INTF, typename P1, typename SUB, typename Scope=factory::scopes::ROOT> class P1FactoryReplacer
{
public:
    static void replace()
    {
        P1Factory<INTF, P1, Scope>::factory = P1FactoryReplacer<INTF, P1, SUB, Scope>::create;
        P1Factory<INTF, P1, Scope>::replaced = true;
    }

    static INTF* create(P1 p1)
    {
        std::cout << "[Factory] ....... " << typeid(INTF).name() << " {"<<  Scope::name << "} (" << typeid(P1).name()<<  ") -> replaced by " << typeid(SUB).name() << std::endl;
        return new SUB(p1);
    }
};


template<typename INTF, typename Scope=factory::scopes::ROOT> class Factory
{
public:

    static INTF* get()
    {
        if (injected_instance != NULL)
        {
            std::cout << "[Factory] replaced " << typeid(INTF).name() << " {"<<  Scope::name << "} with injected instance" << std::endl;
            return injected_instance;
        }

        if (factory != NULL)
        {
            std::cout << "[Factory] created " << typeid(INTF).name() << " {"<<  Scope::name << "} calling injected factory" << std::endl;
            return factory();
        }

        std::cout << "[Factory] created " << typeid(INTF).name() << " {"<<  Scope::name << "}" << std::endl;
        return new INTF();
    }

    template <typename IMPL> static INTF* get()
    {
        if (injected_instance != NULL)
        {
            std::cout << "[Factory] replaced " << typeid(INTF).name() << " {"<<  Scope::name << "} -> " << typeid(IMPL).name() << " with injected instance" << std::endl;
            return injected_instance;
        }

        if (factory != NULL)
        {
            std::cout << "[Factory] created " << typeid(INTF).name() << " {"<<  Scope::name << "} -> " << typeid(IMPL).name() << " calling injected factory" << std::endl;
            return factory();
        }

        std::cout << "[Factory] created " << typeid(INTF).name() << " {"<<  Scope::name << "} -> " << typeid(IMPL).name() << std::endl;
        return new IMPL();
    }

    template <typename IMPL, typename P1> static INTF* get(P1 p1)
    {
        if (injected_instance != NULL)
        {
            std::cout << "[Factory] replaced " << typeid(INTF).name() << " {"<<  Scope::name << "} -> " << typeid(IMPL).name() << "(" << typeid(P1).name()<<  ") with injected instance" << std::endl;
            return injected_instance;
        }

        if (P1Factory<INTF,P1,Scope>::replaced)
        {
            std::cout << "[Factory] created " << typeid(INTF).name() << " {"<<  Scope::name << "} -> " << typeid(IMPL).name() << "(" << typeid(P1).name()<<  ") calling injected factory" << std::endl;
            return P1Factory<INTF,P1,Scope>::factory(p1);
        }

        std::cout << "[Factory] created " << typeid(INTF).name() << " {"<<  Scope::name << "} -> " << typeid(IMPL).name() << "(" << typeid(P1).name()<<  ")" << std::endl;
        return new IMPL(p1);
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
    static void replace(INTF* (*new_factory)())
    {
        Factory<INTF, Scope>::factory = new_factory;
    }

    //Replace the returned type with a user specified instance
    template <typename INJ> static void inject(INJ* inj)
    {
        Factory<INTF, Scope>::injected_instance = inj;
    }


    static void reset()
    {
        factory = NULL;
        injected_instance = NULL;
    }

private:
    template <typename SUB> static INTF* create()
    {
        std::cout << "[Factory] ....... " << typeid(INTF).name() << " {"<<  Scope::name << "} -> replaced by " << typeid(SUB).name() << std::endl;
        return new SUB();
    }

    static INTF* (*factory)();
    static INTF* injected_instance;
};

template<typename INTF,typename Scope> INTF* (* Factory<INTF,Scope>::factory)() = NULL;
template<typename INTF,typename Scope> INTF* Factory<INTF,Scope>::injected_instance = NULL;


#endif // CONFIGURABLE_FACTORY_H
