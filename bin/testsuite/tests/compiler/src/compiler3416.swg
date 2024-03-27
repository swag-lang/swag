#global testerror "Err0026"
#test
{
    var a: [100] s32
    var v: closure(s32 ) = closure|a|(x: s32)
    {
    }
}
