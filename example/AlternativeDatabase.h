
#ifndef ALTERNATIVEDATABASE_H_
#define ALTERNATIVEDATABASE_H_

#include "DatabaseInterface.h"

class AlternativeDatabase : public DatabaseInterface
{
public:
    AlternativeDatabase();
    virtual ~AlternativeDatabase();

    void accessDB();
};

#endif /* ALTERNATIVEDATABASE_H_ */
