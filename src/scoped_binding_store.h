#ifndef SCOPED_BINDING_STORE_H_
#define SCOPED_BINDING_STORE_H_

#include <boost/function.hpp>
#include "injector.h"

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


#endif /* SCOPED_BINDING_STORE_H_ */
