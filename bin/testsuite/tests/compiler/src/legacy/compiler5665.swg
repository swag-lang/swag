#global skip
#global testwarning "Wrn0010"
#global #[Swag.Warning("Wrn0010", Swag.WarningLevel.Enable)]

//#global skip

//#[Swag.PrintBc]
//#[Swag.Optimize("", false)]
func toto(a, b: s32)->s32
{
    switch a
    {
    case 0:
        if b == 1:
            return 10

    default:
        return 0
    }

    return -1
}
