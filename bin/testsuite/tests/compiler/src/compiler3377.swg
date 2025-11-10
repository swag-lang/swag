#global testerror "Err0562"
func toto(x: { a, b: s32 }) {}

#test
{
    toto({1})
}
