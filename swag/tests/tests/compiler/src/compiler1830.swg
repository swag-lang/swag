
#global testerror

alias X = func()->s32

#test
{
    @assert(X() == 0)
}