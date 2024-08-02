#global testerror "Err0309"

using Swag

struct X
{
}

#test
{
    Swag.@assert()
}