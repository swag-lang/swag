#global testerror "Err0730"
func toto(x: { a, b: s32 }) {}

#test
{
    toto({1})
}
