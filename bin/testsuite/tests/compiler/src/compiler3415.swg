#global testerror "Err0261"
#test
{
    a,b,c,d,e,f,var g = 666'u64
    var v: func||(s32) = func|a,b,c,d,e,f,g|(x: s32) { }
}