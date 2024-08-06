#global testerror "Err0180"

typealias X = func()->s32

#test
{
    @print(X())
}