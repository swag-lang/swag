#global testerror "Err0493"
struct A { r: s32 }

#test
{
    var x: [..] A
    let y = x.r
}