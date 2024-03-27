//#global testwarning
#global #[Swag.Warn("Wrn0006", Swag.WarnLevel.Error)]
#test
{
    var x: closure(y: s32)
    x = closure||(y)
    {
    }

    x(10)
}