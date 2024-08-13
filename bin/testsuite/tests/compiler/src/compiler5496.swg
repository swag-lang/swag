#global testerror "Err0170"

typealias X = func()->s32

#test
{
    @print(X())
}