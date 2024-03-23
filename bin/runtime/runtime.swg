using Swag

// Special attribute that cannot be defined in the boostrap, because
// we need typeinfo to be defined
namespace Swag
{
    #[AttrUsage(AttributeUsage.All)]
    attr Using(what: typeinfo...)
}

////////////////////////////////////////////////////////////

func __concreteAlias(type1: typeinfo)->typeinfo
{
    if type1.kind != TypeInfoKind.Alias or type1.flags & TypeInfoFlags.Strict do
        return type1
    let typeAlias = cast(const *TypeInfoAlias) type1
    return __concreteAlias(typeAlias.rawType)
}

func @typecmp(typeA, typeB: typeinfo, cmpFlags: TypeCmpFlags)->bool
{
    if cast(const *u8) typeA == cast(const *u8) typeB do
        return true
    if !typeA and !typeB do
        return true
    if !typeA or !typeB do
        return false

    let type1 = __concreteAlias(typeA)
    let type2 = __concreteAlias(typeB)

    // Cast from any. Fine to cast from a struct or array to a reference
    if cmpFlags & .CastAny and
       type1.kind == TypeInfoKind.Pointer and
       type1.flags & .PointerRef and
       type1.flags & .Const
    {
        let typePointer = cast(const *Swag.TypeInfoPointer) type1
        return @typecmp(typePointer.pointedType, type2, cmpFlags)
    }

    if (type1.kind != type2.kind) or (type1.sizeof != type2.sizeof) or (type1.flags != type2.flags) do
        return false

    // For tuples, we compare field by field
    if type1.flags & .Tuple
    {
        let type1S = cast(const *TypeInfoStruct) type1
        let type2S = cast(const *TypeInfoStruct) type2
        loop i: type1S.fields
        {
            let field1 = type1S.fields[i]
            let field2 = type2S.fields[i]
            if field1.name != field2.name do
                return false
            if !@typecmp(field1.pointedType, field2.pointedType, cmpFlags) do
                return false
        }

        return true
    }

    return @stringcmp(type1.fullname, type2.fullname)
}

////////////////////////////////////////////////////////////

#[Safety("", false)]
func @itftableof(structType, itfType: const *TypeInfoStruct)->const *void
{
    loop i: structType.interfaces
    {
        if @stringcmp(structType.interfaces[i].name, itfType.fullname) do
            return structType.interfaces[i].value
    }

    return null
}

////////////////////////////////////////////////////////////

const __MAX_ARGUMENTS = 512
var __argv:           [__MAX_ARGUMENTS] string
var __argc            = 0'u32

func __convertArgcArgv(args: const ^u8)
{
    var cmdLine = args
    __argc = 0
    while dref cmdLine
    {
        while dref cmdLine == ` ` do
            cmdLine += 1
        if dref cmdLine == 0 do
            break

        if dref cmdLine == `"`
        {
            cmdLine += 1
            let start = cmdLine
            while dref cmdLine and dref cmdLine != `"` do
                cmdLine += 1
            __argv[__argc] = cast(string) @mkslice(start, cast(u64) (cmdLine - start))
            if dref cmdLine do
                cmdLine += 1
        }
        else
        {
            let start = cmdLine
            while dref cmdLine and dref cmdLine != ` ` do
                cmdLine += 1
            __argv[__argc] = cast(string) @mkslice(start, cast(u64) (cmdLine - start))
        }

        __argc += 1
        if __argc == __MAX_ARGUMENTS do
            return
        while dref cmdLine and dref cmdLine == ` ` do
            cmdLine += 1
    }
}

func @args()->const [..] string
{
    if __argc == 0
    {
        let ptr = @dataof(@pinfos().args) orelse __getCommandLine()
        __convertArgcArgv(ptr)
    }

    return @mkslice(&__argv[0], cast(u64) __argc)
}

////////////////////////////////////////////////////////////

var __sysAlloc: SystemAllocator

// Make a build configuration copy
const __buildCfg = #run dref @compiler().getBuildCfg()

func @rtflags() => @getcontext().runtimeFlags
func @dbgalloc() => cast(*DebugAllocator) @getcontext().debugAllocator
func @sysalloc() => &__sysAlloc

func __setupRuntime(flags: RuntimeFlags)
{
    let cxt = @getcontext()
    cxt.runtimeFlags = flags

    cxt.tempAllocator.setCapacity(__buildCfg.tempAllocatorCapacity, cxt.allocator)
    cxt.errorAllocator.setCapacity(__buildCfg.errorAllocatorCapacity, cxt.allocator)

    if __buildCfg.debugAllocator
    {
        let alloc: *DebugAllocator = @alloc(@sizeof(DebugAllocator))
        @init(alloc, 1)
        alloc.captureAllocStack = __buildCfg.debugAllocatorCaptureStack
        alloc.detectLeaks = __buildCfg.debugAllocatorLeaks
        alloc.setup(cxt.allocator)
        cxt.allocator = dref alloc
        cxt.debugAllocator = alloc
    }
}

func __closeRuntime()
{
    @getcontext().tempAllocator.release()
    @getcontext().errorAllocator.release()

    if !(@rtflags() & .FromCompiler) and __buildCfg.debugAllocator do
        @dbgalloc().printLeaks()

    __exit()
}

func __dropGlobalVariables()
{
    visit g: @gvtd()
    {
        var ptr = cast(^u8) g.ptr
        loop x: g.count
        {
            g.opDrop(ptr)
            ptr += g.sizeOf
        }
    }
}
