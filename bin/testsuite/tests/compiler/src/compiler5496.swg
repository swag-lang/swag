#global testerror "Err0228"

typealias X = func()->s32

#test
{
    @print(X())
}