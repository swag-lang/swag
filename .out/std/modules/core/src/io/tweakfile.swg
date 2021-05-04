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
            one = catch parse.pokeValue(currentFolder.data + field.offset, field.pointedType, one)
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
