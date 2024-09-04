#global testerror "Err0634"

using Swag

struct X
{
}

#test
{
    Swag.@assert()
}