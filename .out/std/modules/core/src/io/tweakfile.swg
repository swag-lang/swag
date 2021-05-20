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
    func(T) registerFolder(using self, folder: *T)
    {
        #assert(Reflection.isStruct(T), "type should be a struct")
        folders.add(@{@typeof(T), folder})
    }

    // Read and parse a file
    func parse(using self, fileName: string) throw
    {
        buf := try File.readAllBytes(fileName)
        lines := tokenize.splitLines(cast(string) buf.toSlice())
        try self.parse(lines)
    }

    // Parse a list of lines
    func parse(using self, lines: Array'string) throw
    {
        var currentFolder: *Folder
        visit one, idx: lines
        {
            one = tokenize.eatSpaces(one)
            if @countof(one) == 0 continue
            if one[0] == "#"'u8 continue // comment

            // Folder
            if one[0] == "/"'u8
            {
                one = tokenize.eatCount(one, 1)
                folderName := catch Parse.getIdentifier(one)
                if @err
                    throw Format.toString("line %: invalid folder name", idx + 1)

                visit *folder: folders
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

            // Value
            if !currentFolder
                throw Format.toString("line %: value is not associated with a folder", idx + 1)
            one = tokenize.eatSpaces(one)
            id := catch Parse.getIdentifier(one)
            if @err
                throw Format.toString("line %: invalid value name", idx + 1)

            // Search value into folder
            field := Reflection.getField(currentFolder.type, id)
            if !field throw Format.toString("line %: value '%' not found in folder '%'", idx + 1, id, currentFolder.type.flatName)
            one = tokenize.eatCount(one, @countof(id))

            // Get value and poke it
            one = catch Parse.pokeValue(currentFolder.data + field.offset, field.pointedType, one)
            if @err
                throw Format.toString("line %: %", idx + 1, @err)

            // Be sure there's nothing left
            if @countof(one) and !latin1.isSpace(one[0]) and one[0] != "#"'u8
                throw Format.toString("line %: syntax error in value", idx + 1)
            one = tokenize.eatSpaces(one)
            if @countof(one) and one[0] != "#"'u8
                throw Format.toString("line %: invalid characters after value", idx + 1)
        }
    }
}
