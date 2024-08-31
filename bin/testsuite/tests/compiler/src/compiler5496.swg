#global testerror "Err0210"

typealias X = func()->s32

#test
{
    @print(X())
}