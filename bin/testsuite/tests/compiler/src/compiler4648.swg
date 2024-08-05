#global testerror "Err0542"
struct A { r: s32 }

#test
{
    var x: [..] A
    let y = x.r
}