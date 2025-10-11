func average(x: any)
{
    let t = cast(*f32) @dataof(x)
    @assert(dref t == 10.0)
    //var t1 = #typeof(x)
}

//#[Swag.PrintBc]
#test
{
    average(10.0)
}
