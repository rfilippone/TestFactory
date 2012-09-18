
#ifndef SUT_H_
#define SUT_H_

#include <boost/shared_ptr.hpp>

#include "configurable_factory.h"
#include "DatabaseInterface.h"

SCOPE(MYSCOPE);
SCOPE(MYSCOPE2);

class SUT
{
public:

    const static std::string param;

    SUT();
    ~SUT();

    void doSomething();

    boost::shared_ptr<DatabaseInterface> m_db;
    boost::shared_ptr<DatabaseInterface> m_db1;
    boost::shared_ptr<DatabaseInterface> m_db2;
};


#endif /* SUT_H_ */
