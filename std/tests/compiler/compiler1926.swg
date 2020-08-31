#module "compiler1926"
#unittest error

alias A = [] s32
var x: A
#test
{
    x[0] = 1
}