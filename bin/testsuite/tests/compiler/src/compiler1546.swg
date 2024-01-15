#global testerror "Err0112"

using Swag

struct X
{
}

#test
{
    Swag.@assert()
}