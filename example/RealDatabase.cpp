#include <iostream>
#include "RealDatabase.h"


RealDatabase::RealDatabase(std::string name) :
    m_name(name)
{
}

RealDatabase::~RealDatabase()
{
}
void RealDatabase::accessDB()
{
    std::cout << "Real DB: access ... this is slow " << m_name << std::endl;
}
