var silentMode: s32

func silent(value: bool)
{
    silentMode += value ? 1 : -1
}

#test
{
    silentMode = 0
    silent(true)
    @assert(silentMode == 1)
    silent(false)
    @assert(silentMode == 0)
}