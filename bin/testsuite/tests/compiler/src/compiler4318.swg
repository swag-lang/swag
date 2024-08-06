#global testerror "Err0685"
struct(T) A {}

//#global testerror
//#global skip
#test
{
    var x: A'func(a: s32)
}
