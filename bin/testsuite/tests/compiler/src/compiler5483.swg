struct String
{
    aa:  u64
}

impl String
{
    #[Swag.Implicit]
    mtd opCast()->string
    {
        return "a"
    }

    #[Swag.Implicit]
    mtd opAffect(x: string)
    {
        me.aa = 60
    }
}

#[Swag.Inline]
func tmpPath1(name: string)->String
{
    return name
}

func save(str: string, options: any = null)
{
    @assert(options == null)
}

//#[Swag.PrintBc]
#test
{
    save(tmpPath1("__gameover.png"))
}
