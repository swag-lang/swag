#global testerror "Err0302"

using Swag

struct X
{
}

#test
{
    Swag.@assert()
}