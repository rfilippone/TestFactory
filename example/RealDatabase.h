
#ifndef REALDATABASE_H_
#define REALDATABASE_H_

#include <string>
#include "DatabaseInterface.h"

class RealDatabase : public DatabaseInterface
{
public:
    RealDatabase(std::string name);
    virtual ~RealDatabase();

    void accessDB();

    std::string m_name;
};


#endif /* REALDATABASE_H_ */
