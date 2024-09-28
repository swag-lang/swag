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
    if type1.kind != TypeInfoKind.Alias or type1.flags & TypeInfoFlags.Strict:
        return type1
    let typeAlias = cast(const *TypeInfoAlias) type1
    return __concreteAlias(typeAlias.rawType)
}

func __is(toType, fromType: const *TypeInfoStruct)->{offset: u32, yes: bool}
{
    if @typecmp(toType, fromType, .Strict):
        return {0, true}
    if !toType or !fromType:
        return {0, false}

    foreach ft in fromType.usingFields
    {
        if ft.pointedType.kind == .Struct
        {
            let typeStruct = cast(const *Swag.TypeInfoStruct) ft.pointedType
            if @typecmp(toType, typeStruct, .Strict):
                return {ft.offset, true}
        }
        elif ft.pointedType.kind == .Pointer
        {
            let ptr = cast(const *Swag.TypeInfoPointer) ft.pointedType
            let typeStruct = cast(const *Swag.TypeInfoStruct) ptr.pointedType
            if @typecmp(toType, typeStruct, .Strict):
                return {ft.offset, true}      
        }
    }    

    return {0, false}
}

func @as(toType, fromType: const *TypeInfoStruct, ptr: *void)->*void
{
    let (offset, yes) = __is(toType, fromType)
    if !yes:
        return null
    return cast(^u8) ptr + offset
}

func @is(toType, fromType: const *TypeInfoStruct)->bool
{
    let (?, yes) = __is(toType, fromType)
    return yes
}

func @typecmp(typeA, typeB: typeinfo, cmpFlags: TypeCmpFlags)->bool
{
    if cast(const *u8) typeA == cast(const *u8) typeB:
        return true
    if !typeA and !typeB:
        return true
    if !typeA or !typeB:
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

    if (type1.kind != type2.kind) or (type1.sizeof != type2.sizeof):
        return false

    // HACK for now : do not take care of Nullable flag
    let flags1 = type1.flags & ~.Nullable
    let flags2 = type2.flags & ~.Nullable
    if flags1 != flags2:
        return false

    // For tuples, we compare field by field
    if type1.flags & .Tuple
    {
        let type1S = cast(const *TypeInfoStruct) type1
        let type2S = cast(const *TypeInfoStruct) type2
        for i in type1S.fields
        {
            let field1 = type1S.fields[i]
            let field2 = type2S.fields[i]

            var namesAreEqual = false
            if field1.flags & .AutoName:
                namesAreEqual = true
            if field2.flags & .AutoName:
                namesAreEqual = true

            if !namesAreEqual and field1.name != field2.name:
                return false

            if !@typecmp(field1.pointedType, field2.pointedType, cmpFlags):
                return false
        }

        return true
    }

    // HACK for now : do not take care of Nullable flag
    var idx1 = 0
    var idx2 = 0
    if type1.flags & .Nullable:
        idx1 = 3
    if type2.flags & .Nullable:
        idx2 = 3            

    if cmpFlags & .UnScoped:
        return @stringcmp(type1.name[idx1..], type2.name[idx2..])

    return @stringcmp(type1.fullname[idx1..], type2.fullname[idx2..])
}

////////////////////////////////////////////////////////////

#[Safety("", false)]
func @itftableof(structType, itfType: const *TypeInfoStruct)->const *void
{
    for i in structType.interfaces
    {
        if @stringcmp(structType.interfaces[i].name, itfType.fullname):
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
        while dref cmdLine == ' ':
            cmdLine += 1
        if dref cmdLine == 0:
            break

        if dref cmdLine == '"'
        {
            cmdLine += 1
            let start = cmdLine
            while dref cmdLine and dref cmdLine != '"':
                cmdLine += 1
            __argv[__argc] = cast(string) @mkslice(start, cast(u64) (cmdLine - start))
            if dref cmdLine:
                cmdLine += 1
        }
        else
        {
            let start = cmdLine
            while dref cmdLine and dref cmdLine != ' ':
                cmdLine += 1
            __argv[__argc] = cast(string) @mkslice(start, cast(u64) (cmdLine - start))
        }

        __argc += 1
        if __argc == __MAX_ARGUMENTS:
            return
        while dref cmdLine and dref cmdLine == ' ':
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

func @rtflags()  => @getcontext().runtimeFlags
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
        let alloc: *DebugAllocator = @alloc(#sizeof(DebugAllocator))
        @init(alloc, 1)
        alloc.captureAllocStack = __buildCfg.debugAllocatorCaptureStack
        alloc.detectLeaks       = __buildCfg.debugAllocatorLeaks
        alloc.setup(cxt.allocator)
        cxt.allocator      = dref alloc
        cxt.debugAllocator = alloc
    }
}

func __closeRuntime()
{
    @getcontext().tempAllocator.release()
    @getcontext().errorAllocator.release()

    if !(@rtflags() & .FromCompiler) and __buildCfg.debugAllocator:
        @dbgalloc().printLeaks()

    __exit()
}

func __dropGlobalVariables()
{
    foreach g in @gvtd()
    {
        var ptr = cast(^u8) g.ptr
        for x in g.count
        {
            g.opDrop(ptr)
            ptr += g.sizeOf
        }
    }
}
