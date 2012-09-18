
#ifndef SUT_H_
#define SUT_H_

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

    DatabaseInterface* m_db;
    DatabaseInterface* m_db1;
    DatabaseInterface* m_db2;
};


#endif /* SUT_H_ */
