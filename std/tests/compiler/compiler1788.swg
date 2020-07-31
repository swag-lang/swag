#[swag.printbc]
#run
{
    G += 1
}

#init
{
    if !@isbytecode()
        @assert(G == 1)
}

#[swag.nobss]
var G: s64 = 0
