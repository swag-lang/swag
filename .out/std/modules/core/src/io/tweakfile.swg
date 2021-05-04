#global namespace file
#global public
using swag

struct Folder
{
    type: typeinfo
    data: *void
}

struct TweakFile
{
    folders: Array'Folder
}

private func roomInArray(addr: *void, type: const *TypeInfoStruct, valueType: typeinfo)->*void
{
    // We will treat the array as a buffer of bytes, just to be able to access the fields
    arr := cast(*Array'u8) addr
    sizeValue := valueType.sizeof

    var valueTypeStruct: const *TypeInfoStruct
    if reflection.isStruct(valueType) valueTypeStruct = autocast valueType

    // If we need to reallocate the buffer, we will also have to deal with
    // structures specials functions
    if arr.count >= arr.capacity
    {
        newCapacity := math.max(arr.count + 1, arr.capacity * 2)
        newBuffer := memory.alloc(newCapacity * sizeValue, arr.allocator)
        memory.clear(newBuffer, newCapacity * sizeValue)
        arr.capacity = newCapacity

        // Move old values to the new buffer
        memory.copy(newBuffer, arr.buffer, arr.count * sizeValue)
        if reflection.hasPostMove(valueType)
        {
            buf := newBuffer
            loop arr.count
            {
                valueTypeStruct.opPostMove(buf)
                buf += sizeValue
            }
        }

        memory.free(arr.buffer, arr.count * sizeValue)
        arr.buffer = newBuffer
    }

    arr.count += 1
    result := arr.buffer + ((arr.count - 1) * valueType.sizeof)
    if valueTypeStruct valueTypeStruct.opInit(result)

    return result
}

impl TweakFile
{
    private func setValue(addr: *void, field: typeinfo, buf: string)->string throw
    {
        patch := cast(*u8) addr
        one := parse.trimLeft(buf)

        // Array
        if reflection.isStructOfName(field, "Array")
        {
            structType := cast(const *TypeInfoStruct) field
            valueType := structType.generics[0].pointedType
            while @countof(one) and one[0] != "#"'u8
            {
                addrVal := roomInArray(addr, structType, valueType)
                one = try setValue(addrVal, valueType, one)
                one = parse.trimLeft(one)
            }

            return one
        }

        // Enum
        if reflection.isEnum(field)
        {
            enumType := cast(const *TypeInfoEnum) field
            enumFlags := reflection.hasAttribute(enumType, "swag.enumflags")

            while true
            {
                one = parse.trimLeft(one)
                id := try parse.getIdentifier(one)
                v := reflection.getEnumValue(field, id)
                if !v throw format.toString("unknown enum value '%'", id)
                switch enumType.sizeof
                {
                case 1:     :cast(*u8) patch |= :cast(const *u8) v.value
                case 2:     :cast(*u16) patch |= :cast(const *u16) v.value
                case 4:     :cast(*u32) patch |= :cast(const *u32) v.value
                case 8:     :cast(*u64) patch |= :cast(const *u64) v.value
                default:    throw format.toString("unsupported enum type '%'", enumType.rawType.flatName)
                }

                one = parse.moveStart(one, @countof(id))
                if !enumFlags break

                one = parse.trimLeft(one)
                if !@countof(one) break
                if one[0] == "#"'u8 break

                if one[0] != "|"'u8
                    throw "invalid enum flags format"
                one = parse.moveStart(one, 1)
            }

            eat := cast(u32) cast(int) (@dataof(one) - @dataof(buf))
            one = parse.moveStart(buf, cast(uint) eat)
            return one
        }

        // Basic types
        eat := 0'u32
        switch field
        {
        case s8:
            var val: s8 = ?
            (val, eat) = try parse.getS8(one)
            :cast(*s8) patch = val

        case s16:
            var val: s16 = ?
            (val, eat) = try parse.getS16(one)
            :cast(*s16) patch = val

        case s32:
            var val: s32 = ?
            (val, eat) = try parse.getS32(one)
            :cast(*s32) patch = val

        case s64, int:
            var val: s64 = ?
            (val, eat) = try parse.getS64(one)
            :cast(*s64) patch = val

        case u8:
            var val: u8 = ?
            (val, eat) = try parse.getU8(one)
            :cast(*u8) patch = val

        case u16:
            var val: u16 = ?
            (val, eat) = try parse.getU16(one)
            :cast(*u16) patch = val

        case u32:
            var val: u32 = ?
            (val, eat) = try parse.getU32(one)
            :cast(*u32) patch = val

        case u64, uint:
            var val: u64 = ?
            (val, eat) = try parse.getU64(one)
            :cast(*u64) patch = val

        case f32:
            var val: f32 = ?
            (val, eat) = try parse.getF32(one)
            :cast(*f32) patch = val

        case f64:
            var val: f64 = ?
            (val, eat) = try parse.getF64(one)
            :cast(*f64) patch = val

        case bool:
            var val: bool = ?
            (val, eat) = try parse.getBool(one)
            :cast(*bool) patch = val

        case rune:
            var val: rune = ?
            (val, eat) = try parse.getRune(one)
            :cast(*rune) patch = val

        case String:
            var val: String
            (val, eat) = try parse.getString(one)
            :cast(*String) patch = val

        default:
            throw format.toString("unsupported type '%'", field.flatName)
        }

        return parse.moveStart(one, cast(uint) eat)
    }

    // Register a new structure to be parsed
    func(T) registerFolder(using self, folder: *T)
    {
        #assert(reflection.isStruct(T), "type should be a struct")
        folders.add(@{@typeof(T), folder})
    }

    // Parse the tweak file
    func parse(using self, fileName: string) throw
    {
        lines := try file.readAllLines(fileName)

        var currentFolder: *Folder
        visit *line, idx: lines
        {
            one := line.toString()
            one = parse.trimLeft(one)
            if @countof(one) == 0 continue
            if one[0] == "#"'u8 continue // comment

            // Folder
            if one[0] == "/"'u8
            {
                one = parse.moveStart(one, 1)
                folderName := catch parse.getIdentifier(one)
                if @err
                    throw format.toString("%:%: invalid folder name", fileName, idx + 1)

                visit *folder: folders
                {
                    if folder.type.flatName == folderName
                    {
                        currentFolder = folder
                        break
                    }
                }

                if !currentFolder
                    throw format.toString("%:%: unknown folder '%'", fileName, idx + 1, folderName)
                continue
            }

            // Value
            if !currentFolder
                throw format.toString("%:%: value is not associated with a folder", fileName, idx + 1)
            one = parse.trimLeft(one)
            id := catch parse.getIdentifier(one)
            if @err
                throw format.toString("%:%: invalid value name", fileName, idx + 1)

            // Search value into folder
            field := reflection.getField(currentFolder.type, id)
            if !field throw format.toString("%:%: value '%' not found in folder '%'", fileName, idx + 1, id, currentFolder.type.flatName)
            one = parse.moveStart(one, @countof(id))

            // Get value and poke it
            one = catch setValue(currentFolder.data + field.offset, field.pointedType, one)
            if @err
                throw format.toString("%:%: %", fileName, idx + 1, @err)

            // Be sure there's nothing left
            if @countof(one) and !latin1.isSpace(one[0]) and one[0] != "#"'u8
                throw format.toString("%:%: syntax error in value", fileName, idx + 1)
            one = parse.trimLeft(one)
            if @countof(one) and one[0] != "#"'u8
                throw format.toString("%:%: invalid characters after value", fileName, idx + 1)
        }
    }
}
