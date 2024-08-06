#global testerror "Err0653"

typealias X = func()->s32

#test
{
    @assert(X == 0'u64)
}