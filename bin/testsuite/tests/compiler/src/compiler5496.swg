#global testerror "Err0290"

typealias X = func()->s32

#test
{
    @print(X())
}