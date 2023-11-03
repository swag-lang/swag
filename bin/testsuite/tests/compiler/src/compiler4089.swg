#global testerror "Err"
using Swag

struct Err{}

func toto() throw
{
    throw Err{}
}

func titi() throw
{
    toto()
}

#test
{
    try titi()
}