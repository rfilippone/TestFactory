#ifndef BUILDER_H_
#define BUILDER_H_

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

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


#endif /* BUILDER_H_ */
