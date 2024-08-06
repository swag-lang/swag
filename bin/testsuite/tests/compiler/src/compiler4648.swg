#global testerror "Err0531"
struct A { r: s32 }

#test
{
    var x: [..] A
    let y = x.r
}