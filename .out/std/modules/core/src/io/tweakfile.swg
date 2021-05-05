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

impl TweakFile
{
    // Register a new structure to be parsed
    func(T) registerFolder(using self, folder: *T)
    {
        #assert(reflection.isStruct(T), "type should be a struct")
        folders.add(@{@typeof(T), folder})
    }

    // Parse a file
    func parse(using self, fileName: string) throw
    {
        buf := try file.readAllBytes(fileName)
        lines := tokenize.splitLines(cast(string) buf.toSlice())
        try self.parse(&lines)
    }

    // Parse a list of lines
    func parse(using self, lines: *Array'string) throw
    {
        var currentFolder: *Folder
        visit one, idx: :lines
        {
            one = tokenize.eatSpaces(one)
            if @countof(one) == 0 continue
            if one[0] == "#"'u8 continue // comment

            // Folder
            if one[0] == "/"'u8
            {
                one = tokenize.eatCount(one, 1)
                folderName := catch parse.getIdentifier(one)
                if @err
                    throw format.toString("line %: invalid folder name", idx + 1)

                visit *folder: folders
                {
                    if folder.type.flatName == folderName
                    {
                        currentFolder = folder
                        break
                    }
                }

                if !currentFolder
                    throw format.toString("line %: unknown folder '%'", idx + 1, folderName)
                continue
            }

            // Value
            if !currentFolder
                throw format.toString("line %: value is not associated with a folder", idx + 1)
            one = tokenize.eatSpaces(one)
            id := catch parse.getIdentifier(one)
            if @err
                throw format.toString("line %: invalid value name", idx + 1)

            // Search value into folder
            field := reflection.getField(currentFolder.type, id)
            if !field throw format.toString("line %: value '%' not found in folder '%'", idx + 1, id, currentFolder.type.flatName)
            one = tokenize.eatCount(one, @countof(id))

            // Get value and poke it
            one = catch parse.pokeValue(currentFolder.data + field.offset, field.pointedType, one)
            if @err
                throw format.toString("line %: %", idx + 1, @err)

            // Be sure there's nothing left
            if @countof(one) and !latin1.isSpace(one[0]) and one[0] != "#"'u8
                throw format.toString("line %: syntax error in value", idx + 1)
            one = tokenize.eatSpaces(one)
            if @countof(one) and one[0] != "#"'u8
                throw format.toString("line %: invalid characters after value", idx + 1)
        }
    }
}
