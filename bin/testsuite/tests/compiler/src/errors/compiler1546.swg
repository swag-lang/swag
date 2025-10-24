#global testerror "Err0333"

using Swag

struct X {}

#test
{
    Swag.@assert()
}
