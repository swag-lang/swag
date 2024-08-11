#global testerror "Err0664"

using Swag

struct X
{
}

#test
{
    Swag.@assert()
}