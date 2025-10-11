//#global skip

//#[Swag.PrintGenBc]
#test
{
    let pt = {x: 1'f32, y: 1'f32}
    let l  = @sqrt((pt.x * pt.x) + (pt.y * pt.y))
    @assert(l > 0)
}
