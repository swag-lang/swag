const CellSize  = 128
const Margin    = 4.0

//#[Swag.PrintBc]
//#[Swag.Optim("", false)]
#test
{
    y := 2
    yi := 0.0 + (y*CellSize) + Margin
    @assert(yi == 260)
}