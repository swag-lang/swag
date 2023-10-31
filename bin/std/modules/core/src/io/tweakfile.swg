#global namespace File
#global public
using Swag

struct Folder
{
    type: typeinfo
    data: ^void
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
        #if !Reflection.isStruct(T) #error "type should be a struct"
        folders.add({@typeof(T), folder})
    }

    // Read and parse a file
    mtd parseFile(fileName: string) throw
    {
        var buf = try File.readAllBytes(fileName)
        var lines = Tokenize.splitLines(cast(string) buf.toSlice())
        try parse(lines)
    }

    // Parse a list of lines
    mtd parse(content: string) throw
    {
        var lines = Tokenize.splitLines(content)
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
            if one[0] == `#` continue // comment

            // Folder
            if one[0] == `/`
            {
                one = Tokenize.eatCount(one, 1)
                let folderName = catch StrConv.parseIdentifier(one)
                if @err()
                    throw cast(string) Format.toString("line %: invalid folder name", idx + 1)

                visit &folder: folders
                {
                    if folder.type.name == folderName
                    {
                        currentFolder = folder
                        break
                    }
                }

                if !currentFolder
                    throw cast(string) Format.toString("line %: unknown folder '%'", idx + 1, folderName)
                continue
            }

            if !currentFolder
                currentFolder = folders.backPtr()

            // Value
            one = Tokenize.eatSpaces(one)
            let id = catch StrConv.parseIdentifier(one)
            if @err()
                throw cast(string) Format.toString("line %: invalid value name", idx + 1)

            // Search value into folder
            let field = Reflection.getField(currentFolder.type, id)
            if !field
                throw cast(string) Format.toString("line %: value '%' not found in folder '%'", idx + 1, id, currentFolder.type.name)
            one = Tokenize.eatCount(one, @countof(id))
            one = Tokenize.eatSpaces(one)
            if !@countof(one)
                throw cast(string) Format.toString("line %: missing value", idx + 1)

            if one[0] == `$`
            {
                one = Tokenize.eatCount(one, 1)
                let otherField = Reflection.getField(currentFolder.type, one)
                if !otherField
                    throw cast(string) Format.toString("line %: reference '%' not found in folder '%'", idx + 1, one, currentFolder.type.name)
                if field.pointedType != otherField.pointedType
                    throw cast(string) Format.toString("line %: reference '%' type mismatch", idx + 1, one)
                var refr: Ref{currentFolder, otherField, field}
                refs.add(refr)
                continue
            }

            // Get value and poke it
            one = catch StrConv.pokeValue(cast(^u8) currentFolder.data + field.offset, field.pointedType, one)
            if @err()
                throw cast(string) Format.toString("line %: %", idx + 1, @errmsg)

            // Be sure there's nothing left
            if @countof(one) and !Latin1.isSpace(one[0]) and one[0] != `#`
                throw cast(string) Format.toString("line %: syntax error in value", idx + 1)
            one = Tokenize.eatSpaces(one)
            if @countof(one) and one[0] != `#`
                throw cast(string) Format.toString("line %: invalid characters after value", idx + 1)
        }

        // Do references in a second pass
        while refs.count
        {
            var toDo = 0'u64
            var refr = refs[toDo]

            loop
            {
                var restart = false
                for var j = 0; j < refs.count; j += 1
                {
                    if j == toDo
                        continue
                    var ref1 = refs[j]
                    if refr.src.name == ref1.dst.name
                    {
                        if j == 0
                            throw "tweakfile cycle between '$' references !"
                        toDo = j
                        refr = refs[toDo]
                        restart = true
                        break
                    }
                }

                if !restart
                    break
            }

            let ptrDst = cast(^u8) refr.folder.data + refr.dst.offset
            let ptrSrc = cast(^u8) refr.folder.data + refr.src.offset
            Reflection.copy(ptrDst, ptrSrc, refr.dst.pointedType)

            refs.removeAt(toDo)
        }
    }
}
