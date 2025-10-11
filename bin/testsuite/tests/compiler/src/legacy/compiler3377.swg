#global testerror "Err0491"
func toto(x: { a, b: s32 }) {}

#test
{
    toto({1})
}
