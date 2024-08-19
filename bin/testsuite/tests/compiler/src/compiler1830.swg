#global testerror "Err0633"

typealias X = func()->s32

#test
{
    @assert(X == 0'u64)
}