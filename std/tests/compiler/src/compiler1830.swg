#module "compiler1830"
#unittest error

alias X = func()->s32

#test
{
    @assert(X() == 0)
}