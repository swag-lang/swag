#global testerror "Err0182"

typealias X = func()->s32

#test
{
    @print(X())
}