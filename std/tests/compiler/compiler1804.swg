#test
{
    // using can be used with a variable or a function parameter
    {
        struct Point { x: s32; y: s32; }
        var pt: Point
        using pt
    }

    {
        struct Point { x: s32; y: s32; }
        var pt: Point
    }
}