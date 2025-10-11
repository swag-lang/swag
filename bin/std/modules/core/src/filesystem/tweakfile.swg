#global namespace File
#global public
using Swag, Core.Errors

// Represents a folder mapping to a specific structure type with its data
struct Folder
{
    type:     typeinfo     // Type information of the structure
    data:     [*] void     // Pointer to the actual structure instance
}

// Represents a tweak file parser that can load configuration into registered folders
struct TweakFile
{
    folders: Array'Folder // List of registered folders (structure instances) to fill
}

impl TweakFile
{
    // Registers a new structure to be parsed as a folder
    mtd(T) registerFolder(folder: *T)
    {
        #if !Reflection.isStruct(T) do
            #error("type should be a struct")
        .folders.add({#typeof(T), folder})
    }

    // Reads and parses a tweak file from disk
    mtd parseFile(fileName: string) throw
    {
        var buf   = try File.readAllBytes(fileName)
        let lines = Tokenize.splitLines(cast(string) buf.toSlice())
        try .parse(lines)
    }

    // Parses tweak content from a string
    #[Overload]
    mtd parse(content: string) throw
    {
        let lines = Tokenize.splitLines(content)
        try .parse(lines)
    }

    // Parses tweak content from an array of lines
    #[Overload]
    mtd parse(lines: Array'string) throw
    {
        Debug.assert(!.folders.isEmpty())

        struct Ref
        {
            folder:     *Folder        // Folder where the reference is defined
            src:        *TypeValue     // Source field that is being referenced
            dst:        *TypeValue     // Destination field to copy to
        }

        var refs:          Array'Ref     // Pending references to resolve
        var currentFolder: *Folder       // Current folder being parsed

        foreach oneit, idx in lines
        {
            var one = Tokenize.eatSpaces(oneit)
            if @countof(one) == 0 do
                continue
            if one[0] == '#' do
                continue

            // Folder declaration line (starts with '/')
            if one[0] == '/'
            {
                one = Tokenize.eatCount(one, 1)
                let folderName = catch StrConv.parseIdentifier(one)
                if @err != null do
                    throw SyntaxError{"invalid folder name", line: idx + 1}

                foreach &folder in .folders
                {
                    if folder.type.name == folderName
                    {
                        currentFolder = folder
                        break
                    }
                }

                if !currentFolder do
                    throw SyntaxError{Errors.mkString(Format.toString("unknown folder [[%]]", folderName)), line: idx + 1}
                continue
            }

            if !currentFolder do
                currentFolder = .folders.backPtr()

            // Value declaration
            one = Tokenize.eatSpaces(one)
            let id = catch StrConv.parseIdentifier(one)
            if @err != null do
                throw SyntaxError{"invalid value name", line: idx + 1}

            // Search value in folder type
            let field = Reflection.getField(currentFolder.type, id)
            if !field do
                throw SyntaxError{Errors.mkString(Format.toString("value [[%]] not found in folder [[%]]", id, currentFolder.type.name)), line: idx + 1}
            one = Tokenize.eatCount(one, @countof(id))
            one = Tokenize.eatSpaces(one)
            if !@countof(one) do
                throw SyntaxError{"missing value", line: idx + 1}

            // Reference to another field using '$' syntax
            if one[0] == '$'
            {
                one = Tokenize.eatCount(one, 1)
                let otherField = Reflection.getField(currentFolder.type, one)
                if !otherField do
                    throw SyntaxError{Errors.mkString(Format.toString("reference [[%]] not found in folder [[%]]", one, currentFolder.type.name)), line: idx + 1}
                if field.pointedType != otherField.pointedType do
                    throw SyntaxError{Errors.mkString(Format.toString("reference [[%]] type mismatch", one)), line: idx + 1}
                let refr: Ref{currentFolder, otherField, field}
                refs.add(refr)
                continue
            }

            // Parse and assign value to field
            one = catch StrConv.parseValue(cast([*] u8) currentFolder.data + field.offset, field.pointedType, one)
            if @err != null do
                throw SyntaxError{"invalid value", line: idx + 1}

            // Ensure nothing invalid follows the value
            if @countof(one) and !Latin1.isSpace(one[0]) and one[0] != '#' do
                throw SyntaxError{"syntax error in value", line: idx + 1}
            one = Tokenize.eatSpaces(one)
            if @countof(one) and one[0] != '#' do
                throw SyntaxError{"invalid characters after value", line: idx + 1}
        }

        // Resolve references in a second pass
        while refs.count
        {
            var toDo = 0'u64
            var refr = refs[toDo]

            for
            {
                var restart = false
                for var j = 0; j < refs.count; j += 1
                {
                    if j == toDo do
                        continue
                    let ref1 = refs[j]
                    if refr.src.name == ref1.dst.name
                    {
                        if j == 0 do
                            throw SyntaxError{"tweakfile cycle between '$' references !"}
                        toDo    = j
                        refr    = refs[toDo]
                        restart = true
                        break
                    }
                }

                if !restart do
                    break
            }

            let ptrDst = cast([*] u8) refr.folder.data + refr.dst.offset
            let ptrSrc = cast([*] u8) refr.folder.data + refr.src.offset
            Reflection.copy(ptrDst, ptrSrc, refr.dst.pointedType)

            refs.removeAt(toDo)
        }
    }
}
