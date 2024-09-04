#global testerror "Err0027"
#test
{
    var a: [100] s32
    var v: func||(s32) = func|a|(x: s32) { }
}