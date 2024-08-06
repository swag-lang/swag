#global testerror "Err0278"

typealias X = func()->s32

#test
{
    @print(X())
}