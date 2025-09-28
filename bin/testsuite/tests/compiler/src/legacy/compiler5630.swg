#global testwarning "Wrn0003"
#global #[Swag.Warning("Wrn0003", Swag.WarningLevel.Enable)]

#test
{
    if var x = 1
    {
        @assert(#offsetof(x) == 0)
    }
}