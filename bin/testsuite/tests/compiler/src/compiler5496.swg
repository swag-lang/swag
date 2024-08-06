#global testerror "Err0179"

typealias X = func()->s32

#test
{
    @print(X())
}