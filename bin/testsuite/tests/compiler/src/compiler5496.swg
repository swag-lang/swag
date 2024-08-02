#global testerror "Err0283"

typealias X = func()->s32

#test
{
    @print(X())
}