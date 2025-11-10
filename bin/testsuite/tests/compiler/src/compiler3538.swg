struct Thing
{
    number: s32
}

var G = 0

impl Thing
{
    //#[Swag.PrintBc]
    #[Swag.NoInline]
    func initxxx(number: s32)->Thing
    {
        //@print("Thing", number, "\n")
        return Thing{number: number}
    }

    mtd opDrop()
    {
        //@print("~Thing", number, "\n")
        G += 1
    }
}

struct Team
{
    a:     Thing
    b:     Thing
    c:     Thing
}

//#[Swag.PrintBc]
//#[Swag.Optimize(.All, false)]
#test
{
    {
        var team = Team{a: Thing.initxxx(1), b: Thing.initxxx(2), c: Thing.initxxx(3)}
    }

    @assert(G == 3)
}
