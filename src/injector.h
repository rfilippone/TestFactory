#ifndef INJECTOR_H_
#define INJECTOR_H_

#include "scope.h"
#include "bind.h"
#include "scoped_binding_store.h"

template <typename TYPE, typename ANNOTATION> class Injector
{
public:
    static boost::shared_ptr<TYPE> get()
    {
        if (Scope::getCurrentScope())
        {
            typename std::map<long, boost::function<boost::shared_ptr<TYPE>()> >::iterator iter = providerMap.find(Scope::getCurrentScope()->getIdx());
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


#endif /* INJECTOR_H_ */
