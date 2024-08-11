#global testerror "Err0169"

typealias X = func()->s32

#test
{
    @print(X())
}