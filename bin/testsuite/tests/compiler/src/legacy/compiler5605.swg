#global testwarning "Wrn0010"
#global #[Swag.Warning("Wrn0010", Swag.WarningLevel.Enable)]

#test
{
    for 5:
        if @index == 0:
            @assert(true)
}
