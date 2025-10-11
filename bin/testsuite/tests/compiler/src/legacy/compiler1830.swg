#global testerror "Err0601"

alias X = func()->s32

#test
{
    @assert(X == 0'u64)
}
