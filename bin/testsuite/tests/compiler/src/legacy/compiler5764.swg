#global testerror "Err0070"

func setErr1(err: any)
{
    let cxt  = @getcontext()
    let type = cast(const *Swag.TypeInfoStruct) null               //@kindof(err)
    let ptr  = cxt.errorAllocator.alloc(cast(u64) type.sizeof)
}
