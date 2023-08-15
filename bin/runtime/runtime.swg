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
    if type1.kind != TypeInfoKind.Alias or type1.flags & TypeInfoFlags.Strict
        return type1
    let typeAlias = cast(const *TypeInfoAlias) type1
    return __concreteAlias(typeAlias.rawType)
}

func @typecmp(typeA, typeB: typeinfo, cmpFlags: TypeCmpFlags)->bool
{
    if cast(const*u8) typeA == cast(const*u8) typeB
        return true
    if !typeA and !typeB
        return true
    if !typeA or !typeB
        return false

    let type1 = __concreteAlias(typeA)
    let type2 = __concreteAlias(typeB)

    if (type1.kind != type2.kind) or (type1.sizeof != type2.sizeof) or (type1.flags != type2.flags)
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
            if field1.name != field2.name
                return false
            if !@typecmp(field1.pointedType, field2.pointedType, cmpFlags)
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
        if @stringcmp(structType.interfaces[i].name, itfType.fullname)
            return structType.interfaces[i].value
    }

    return null
}


////////////////////////////////////////////////////////////

const __MAX_ARGUMENTS = 512
var __argv: [__MAX_ARGUMENTS] string
var __argc = 0'u32

func __convertArgcArgv(args: const ^u8)
{
    var cmdLine = args
    __argc = 0
    while dref cmdLine
    {
        while dref cmdLine == ` `'u8 cmdLine += 1
        if dref cmdLine == 0 break

        if dref cmdLine == `"`'u8
        {
            cmdLine += 1
            let start = cmdLine
            while dref cmdLine and dref cmdLine != `"`'u8 cmdLine += 1
            __argv[__argc] = cast(string) @mkslice(start, cast(u64) (cmdLine - start))
            if dref cmdLine cmdLine += 1
        }
        else
        {
            let start = cmdLine
            while dref cmdLine and dref cmdLine != ` `'u8 cmdLine += 1
            __argv[__argc] = cast(string) @mkslice(start, cast(u64) (cmdLine - start))
        }

        __argc += 1
        if __argc == __MAX_ARGUMENTS
            return
        while dref cmdLine and dref cmdLine == ` `'u8 cmdLine += 1
    }
}

func @args()->const [..] string
{
    if __argc == 0
    {
        let ptr = @dataof(@getpinfos().args) orelse __getCommandLine()
        __convertArgcArgv(ptr)
    }

    return @mkslice(&__argv[0], cast(u64) __argc)
}

////////////////////////////////////////////////////////////

var __runtimeFlags: RuntimeFlags = Zero
var __debugAlloc:   *DebugAllocator
var __sysAlloc:     SystemAllocator

// Make a build configuration copy
const __buildCfg = #run dref @compiler().getBuildCfg()

func @rtflags()  => __runtimeFlags
func @dbgalloc() => __debugAlloc
func @sysalloc() => &__sysAlloc

func __setupRuntime(flags: RuntimeFlags)
{
    __runtimeFlags = flags

    let cxt = @getcontext()
    cxt.tempAllocator.setCapacity(acast __buildCfg.scratchAllocatorCapacity, cxt.allocator)

    if __buildCfg.debugAllocator
    {
        __debugAlloc = @alloc(@sizeof(DebugAllocator))
        @init(__debugAlloc)
        __debugAlloc.captureAllocStack = __buildCfg.debugAllocatorCaptureStack
        __debugAlloc.detectLeaks       = __buildCfg.debugAllocatorLeaks
        __debugAlloc.setup(cxt.allocator)
        cxt.allocator = dref __debugAlloc
    }
}

func __closeRuntime()
{
    @getcontext().tempAllocator.release()

    if !(__runtimeFlags & .FromCompiler) and __buildCfg.debugAllocator
        __debugAlloc.printLeaks()

    __exit()
}

func __dropGlobalVariables()
{
    visit g: @gvtd()
    {
        g.opDrop(g.ptr)
    }
}