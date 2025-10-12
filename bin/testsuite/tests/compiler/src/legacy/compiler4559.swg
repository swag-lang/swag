const CellSize = 128
const Margin   = 4.0

//#[Swag.PrintBc]
//#[Swag.Optimize(.All, false)]
#test
{
    let y  = 2
    let yi = 0.0 + (y * CellSize) + Margin
    @assert(yi == 260)
}
