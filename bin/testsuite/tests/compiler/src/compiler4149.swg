//#global testwarning
#global #[Swag.Warning("Wrn0006", Swag.WarningLevel.Error)]
#test
{
    var x: closure(y: s32)
    x = closure||(y)
    {
    }

    x(10)
}