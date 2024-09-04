#global testerror "Err0435"

alias A = [2] s32
var x: A

#test
{
    alias p = &x[0]
}