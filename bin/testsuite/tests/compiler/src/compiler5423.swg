struct(T) A
{
    a:   [..] T
}

impl A
{
    #[Swag.Overload]
    mtd opCast()->[..] T
    {
        return me.toSlice()
    }

    /*#[Swag.Overload]
    mtd const opCast()->const [..] T
    {
        return toSlice()
    }*/

    #[Swag.Overload]
    mtd toSlice()->[..] T
    {
        return me.a
    }

    #[Swag.Overload]
    mtd const toSlice()->const [..] T
    {
        return a
    }
}

#test
{
    var a: A's32
}
