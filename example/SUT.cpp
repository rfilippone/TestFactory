#include "SUT.h"
#include "RealDatabase.h"
#include "AlternativeDatabase.h"

const std::string SUT::param("PPS");

SUT::SUT() :
        m_db(Factory<DatabaseInterface>::get()),
        m_db1(Factory<DatabaseInterface>::get<RealDatabase>(param)),
        m_db2(Factory<DatabaseInterface, factory::scopes::MYSCOPE>::get<AlternativeDatabase>())
{
}

SUT::~SUT()
{
}

void SUT::doSomething()
{
    m_db->accessDB();
    m_db1->accessDB();
    m_db2->accessDB();
}
