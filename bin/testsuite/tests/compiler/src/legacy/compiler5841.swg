//#global testerror "Err0051"
//#global skip

var G = true

struct A
{
    drawingBegin = true
}

#[Swag.Macro]
func assert(expr: bool, msg: string)
{
    if !expr:
        @panic("toto", #curlocation)
}

impl A
{
    mtd opCount()->u64
    {
        return 1
    }

    #[Swag.Inline]
    mtd opSlice(a, b: u64)->string
    {
        assert(G, "msg")
        //assert(a == 0, "msg")
        //assert(b == 1, "msg")
        return "aa"
    }
}

func toto()->A
{
    return {}
}

#test
{
    const b = #run toto()
    @assert(b == "aa")
}