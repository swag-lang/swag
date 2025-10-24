#global testerror "Err0389"
struct A { r: s32 }

#test
{
    var x: [..] A
    let y  = x.r
}
