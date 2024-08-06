#global testerror "Err0687"
struct(T) A {}

//#global testerror
//#global skip
#test
{
    var x: A'func(a: s32)
}
