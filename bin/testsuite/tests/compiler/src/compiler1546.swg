#global testerror "Err0687"

using Swag

struct X
{
}

#test
{
    Swag.@assert()
}