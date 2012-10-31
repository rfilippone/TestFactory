#ifndef BIND_H_
#define BIND_H_

#include "builder.h"
#include "annotation.h"
#include "scope.h"

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
        	SCOPE s;
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
        ScopedBindingStore<TYPE, ANNOTATION, SCOPE>::provider = SCOPE::template scope<TYPE>(boost::bind<boost::shared_ptr<TYPE> >(builder<TYPE, ANNOTATION, IMPL>));
        ScopedBindingStore<TYPE, ANNOTATION, SCOPE>::set();
    }

    template <typename ANNOTATION, class SCOPE, typename IMPL> static void annotatedWith_in_to(IMPL value)
    {
        ScopedBindingStore<TYPE, ANNOTATION, SCOPE>::provider = SCOPE::template scope<TYPE>(boost::bind<boost::shared_ptr<TYPE> >(builder<TYPE, ANNOTATION, IMPL>, value));
        ScopedBindingStore<TYPE, ANNOTATION, SCOPE>::set();
    }
};


#endif /* BIND_H_ */
