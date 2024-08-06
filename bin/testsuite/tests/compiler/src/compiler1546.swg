#global testerror "Err0253"

using Swag

struct X
{
}

#test
{
    Swag.@assert()
}