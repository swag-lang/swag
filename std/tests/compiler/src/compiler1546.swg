#module "compiler1546"
#unittest error

using swag

struct X
{
}

#test
{
    swag.@assert()
}