#global testerror "Err0196"

typealias X = func()->s32

#test
{
    @assert(X() == 0'u64)
}