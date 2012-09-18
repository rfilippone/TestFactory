#include <iostream>
#include "DatabaseInterface.h"

DatabaseInterface::~DatabaseInterface()
{
}

void DatabaseInterface::accessDB()
{
    std::cout << "Interface  access" << std::endl;
}
