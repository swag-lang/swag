#global testerror "Err0647"

typealias X = func()->s32

#test
{
    @assert(X() == 0'u64)
}