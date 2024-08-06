#global testerror "Err0229"

typealias X = func()->s32

#test
{
    @print(X())
}