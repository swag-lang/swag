//#global skip
//#global testerror "San0002"
using Swag

//#[Swag.PrintGenBc]
#[Swag.Safety("", true)]
func free(buffer: ^void, size: u64, allocator: nl IAllocator = null, hint: nl string = null, loc = #callerlocation)
{
    var req: AllocatorRequest
    let talloc = allocator orelse @getcontext().allocator
    talloc.req(&req)
}