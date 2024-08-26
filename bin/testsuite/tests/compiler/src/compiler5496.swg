#global testerror "Err0209"

typealias X = func()->s32

#test
{
    @print(X())
}