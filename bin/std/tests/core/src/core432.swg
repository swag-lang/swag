using Core

const C = "totototototototototototototototototototototototototototototototototototototototo"
struct Err
{
    x: String
}

func toto() throw
{
    throw Err{String.from(C)}
}

#[Swag.PrintBc]
#test
{
    catch toto()
    let err = cast(Err) @err()
    @assert(err.x == C)   
    @assert(err.x.allocator != null)
    @assert(@kindof(err.x.allocator) == Swag.ScratchAllocator)
}
