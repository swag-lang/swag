//using Core

struct X { a, b, c: s32 }

#[Swag.Macro, Swag.Warning("Wrn0006", Swag.WarningLevel.Disable)]
func(what: Swag.SafetyWhat) safety(expr51: #code bool, message: string = null, loc = #callerlocation)
{
    #if #safety(what)
    {
        if !#inject(expr51) do
            @panic(message, loc)
    }
}

struct String {}
impl String
{
    #[Swag.Inline]
    mtd opIndex(idx: u64)->u8
    {
        safety'(Swag.SafetyWhat.BoundCheck)(idx == 0)
        return 'x''u8
    }

    mtd opCast()->string => "x"
}

func toString()->String
{
    return {}
}

#[Swag.Compiler, Swag.ConstExpr]
func(T) GEN()->String
{
    //var sb: StrConv.StringBuilder
    //sb.appendFormat("X", #nameof(T))
    return toString()
}

#assert(GEN'X()[0] == 'x''u8)
