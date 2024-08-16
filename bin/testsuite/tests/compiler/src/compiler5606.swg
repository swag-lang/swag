#global testwarning "Wrn0010"
#global #[Swag.Warning("Wrn0010", Swag.WarningLevel.Enable)]

#test
{
    visit "5" do
        if #index == 0 do
            @assert(true)
}
