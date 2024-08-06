#global testerror "Err0254"

using Swag

struct X
{
}

#test
{
    Swag.@assert()
}