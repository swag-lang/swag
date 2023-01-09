//#global testwarnings
#global #[Swag.Warn("wrn0004", Swag.WarnLevel.Error)]
#test
{
    var x: closure(y: s32)
    x = closure||(y)
    {
    }

    x(10)
}