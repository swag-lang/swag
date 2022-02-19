#global namespace File
#global public
using Swag

struct Folder
{
    type: typeinfo
    data: *void
}

struct TweakFile
{
    folders: Array'Folder
}

impl TweakFile
{
    // Register a new structure to be parsed
    mtd(T) registerFolder(folder: *T)
    {
        #assert(Reflection.isStruct(T), "type should be a struct")
        folders.add(@{@typeof(T), folder})
    }

    // Read and parse a file
    mtd parseFile(fileName: string) throw
    {
        buf := try File.readAllBytes(fileName)
        lines := Tokenize.splitLines(cast(string) buf.toSlice())
        try parse(lines)
    }

    // Parse a list of lines
    mtd parse(content: string) throw
    {
        lines := Tokenize.splitLines(content)
        try parse(lines)
    }

    // Parse a list of lines
    mtd parse(lines: Array'string) throw
    {
        Debug.assert(!folders.isEmpty())

        struct Ref
        {
            folder: *Folder
            src:    *Swag.TypeValue
            dst:    *Swag.TypeValue
        }

        var refs: Array'Ref
        var currentFolder: *Folder
        visit one, idx: lines
        {
            one = Tokenize.eatSpaces(one)
            if @countof(one) == 0 continue
            if one[0] == "#"'u8 continue // comment

            // Folder
            if one[0] == "/"'u8
            {
                one = Tokenize.eatCount(one, 1)
                folderName := catch StrConv.parseIdentifier(one)
                if @err
                    throw Format.toString("line %: invalid folder name", idx + 1)

                visit &folder: folders
                {
                    if folder.type.flatName == folderName
                    {
                        currentFolder = folder
                        break
                    }
                }

                if !currentFolder
                    throw Format.toString("line %: unknown folder '%'", idx + 1, folderName)
                continue
            }

            if !currentFolder
                currentFolder = folders.backPtr()

            // Value
            one = Tokenize.eatSpaces(one)
            id := catch StrConv.parseIdentifier(one)
            if @err
                throw Format.toString("line %: invalid value name", idx + 1)

            // Search value into folder
            field := Reflection.getField(currentFolder.type, id)
            if !field
                throw Format.toString("line %: value '%' not found in folder '%'", idx + 1, id, currentFolder.type.flatName)
            one = Tokenize.eatCount(one, @countof(id))
            one = Tokenize.eatSpaces(one)
            if !@countof(one)
                throw Format.toString("line %: missing value", idx + 1)

            if one[0] == "$"'u8
            {
                one = Tokenize.eatCount(one, 1)
                otherField := Reflection.getField(currentFolder.type, one)
                if !otherField
                    throw Format.toString("line %: reference '%' not found in folder '%'", idx + 1, one, currentFolder.type.flatName)
                if field.pointedType != otherField.pointedType
                    throw Format.toString("line %: reference '%' type mismatch", idx + 1, one)
                var ref: Ref{currentFolder, otherField, field}
                refs.add(ref)
                continue
            }

            // Get value and poke it
            one = catch StrConv.pokeValue(currentFolder.data + field.offset, field.pointedType, one)
            if @err
                throw Format.toString("line %: %", idx + 1, @err)

            // Be sure there's nothing left
            if @countof(one) and !Latin1.isSpace(one[0]) and one[0] != "#"'u8
                throw Format.toString("line %: syntax error in value", idx + 1)
            one = Tokenize.eatSpaces(one)
            if @countof(one) and one[0] != "#"'u8
                throw Format.toString("line %: invalid characters after value", idx + 1)
        }

        // Do references in a second pass
        visit ref: refs
        {
            ptrDst := cast(*u8) ref.folder.data + ref.dst.offset
            ptrSrc := cast(*u8) ref.folder.data + ref.src.offset
            Reflection.copy(ptrDst, ptrSrc, ref.dst.pointedType)
        }
    }
}
