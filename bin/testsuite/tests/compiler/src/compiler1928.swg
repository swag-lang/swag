#global testerror "Err0446"

alias A = [2] s32
var x: A

#test
{
    alias p = &x[0]
}