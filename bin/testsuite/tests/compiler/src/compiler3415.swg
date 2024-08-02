#global testerror "Err0362"
#test
{
    a,b,c,d,e,f,var g = 666'u64
    var v: closure(s32) = closure|a,b,c,d,e,f,g|(x: s32) { }
}