#global testerror "Err0289"

typealias X = func()->s32

#test
{
    @print(X())
}