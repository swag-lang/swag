#global testerror
#test
{
    a,b,c,d,e,f,g := 666'u64
    var v: closure(s32) = @|a,b,c,d,e,f,g|(x: s32) { }
}