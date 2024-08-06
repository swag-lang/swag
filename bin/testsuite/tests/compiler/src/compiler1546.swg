#global testerror "Err0303"

using Swag

struct X
{
}

#test
{
    Swag.@assert()
}