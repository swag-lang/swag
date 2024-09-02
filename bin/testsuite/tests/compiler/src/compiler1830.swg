#global testerror "Err0647"

alias X = func()->s32

#test
{
    @assert(X == 0'u64)
}