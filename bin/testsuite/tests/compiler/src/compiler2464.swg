//#global testerror

#test
{
    let x0: s8 = -127's16 - 1
    @assert(x0 == -128)
    let x1: s16 = -32767's16 - 1
    @assert(x1 == -32768)
}