#ifndef CONFIGURABLE_FACTORY_H
#define CONFIGURABLE_FACTORY_H

#include <boost/type_traits.hpp>

template <typename INTF, typename IMPL> INTF* create()
{
    return new IMPL();
}

#define LOCAL 1

template<typename INTF, typename P1, int Scope=0> class P1Factory
{
public:
    static INTF* (*factory)(P1 p1);
    static bool replaced;
};
template<typename INTF, typename P1, int Scope> INTF*      (* P1Factory<INTF, P1, Scope>::factory)(P1) = NULL;
template<typename INTF, typename P1, int Scope> bool       P1Factory<INTF, P1, Scope>::replaced = false;


template<typename INTF, typename P1, typename SUB, int Scope=0> class P1FactoryReplacer
{
public:
    static void replace()
    {
        P1Factory<INTF, P1, Scope>::factory = P1FactoryReplacer<INTF, P1, SUB, Scope>::create;
        P1Factory<INTF, P1, Scope>::replaced = true;
    }

    static INTF* create(P1 p1)
    {        
        return new SUB(p1);
    }
};


template<typename INTF, int Scope=0> class Factory
{
public:

    static INTF* get()
    {
        if (injected_instance != NULL)
        {
            return injected_instance;
        }

        if (factory != NULL)
        {
            return factory();
        }

        return new INTF();
    }

    template <typename IMPL> static INTF* get()
    {
        if (injected_instance != NULL)
        {
            return injected_instance;
        }

        if (factory != NULL)
        {
            return factory();
        }

        return new IMPL();
    }

    template <typename IMPL, typename P1> static INTF* get(P1 p1)
    {
        if (injected_instance != NULL)
        {
            return injected_instance;
        }


        if (P1Factory<INTF,P1,Scope>::replaced)
        {
            return P1Factory<INTF,P1,Scope>::factory(p1);
        }

        return new IMPL(p1);
    }

    //------------------------------------------------------------------------
    //Factory configuration methods

    //Replace the returned type with SUB
    template <typename SUB> static void replace()
    {       
        BOOST_STATIC_ASSERT(boost::is_abstract<SUB>::value==false);
        Factory<INTF, Scope>::factory = create<INTF, SUB>;
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

    static INTF* (*factory)();
    static INTF* injected_instance;
};

template<typename INTF,int Scope> INTF* (* Factory<INTF,Scope>::factory)() = NULL;
template<typename INTF,int Scope> INTF* Factory<INTF,Scope>::injected_instance = NULL;


#endif // CONFIGURABLE_FACTORY_H
