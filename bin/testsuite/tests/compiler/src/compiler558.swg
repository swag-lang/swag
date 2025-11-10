struct point
{
    x:     s32
    y:     s32
}

//#[Swag.PrintBc]
#test
{
    let pt = cast(*point) @alloc(cast(u64) #sizeof(point))
    @free(cast(*u8) pt)
}
