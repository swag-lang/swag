//#global skip
using Swag

func toto() throw
{
    throw "error in toto"
}

func titi() throw
{
    toto()
}

#test
{
    try titi()
}