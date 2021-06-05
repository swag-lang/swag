
using Swag

func toto(x: s32, y: bool)->s32 => x

//#[Swag.printbc]
#test
{
	ptr := cast(const *TypeInfoFunc) @typeof(toto)
	@assert(@typeof(ptr.returnType) == @typeof(const *~64 TypeInfo))
    @assert(ptr.returnType == s32)
}
