#global testerror "Err0644"

typealias X = func()->s32

#test
{
    @assert(X() == 0'u64)
}