#global testerror "Err0640"

typealias X = func()->s32

#test
{
    @assert(X == 0'u64)
}