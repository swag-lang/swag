#global testerror "Err0171"

typealias X = func()->s32

#test
{
    @print(X())
}