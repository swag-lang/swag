/*
--- Part Two ---
Now, you're ready to choose a directory to delete.

The total disk space available to the filesystem is 70000000. To run the update, you need unused space of at least 30000000. You need to find a directory you can delete that will free up enough space to run the update.

In the example above, the total size of the outermost directory (and thus the total amount of used space) is 48381165; this means that the size of the unused space must currently be 21618835, which isn't quite the 30000000 required by the update. Therefore, the update still requires a directory with total size of at least 8381165 to be deleted before it can run.

To achieve this, you have the following options:

Delete directory e, which would increase unused space by 584.
Delete directory a, which would increase unused space by 94853.
Delete directory d, which would increase unused space by 24933642.
Delete directory /, which would increase unused space by 48381165.
Directories e and a are both too small; deleting them would not free up enough space. However, directories d and / are both big enough! Between these, choose the smallest: d, increasing unused space by 24933642.

Find the smallest directory that, if deleted, would free up enough space on the filesystem to run the update. What is the total size of that directory?
*/
using Core

func day7B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#curlocation.fileName), "7.txt")
    let lines    = assume File.readAllLines(dataPath)

    var folder:  String
    var folders: HashTable'(String, s32)

    for i in lines
    {
        let l = #ref lines[i]
        if l[0] == '$' do
            l.remove(0, 1)

        let cmd = Tokenize.split(l, ' ''u8)
        switch cmd[0]
        {
        case "cd":
            if cmd[1] == "/" do
                folder = ""
            elif cmd[1] == ".." do
                folder = Path.getDirectoryName(folder)
            else do
                folder = Path.combine(folder, cmd[1])

        case "ls", "dir":
            break

        default:
            let size = assume StrConv.toS32(cmd[0])
            var tmp  = folder
            for
            {
                folders.findOrAdd(tmp).value += size
                if !tmp.length do
                    break
                tmp = Path.getDirectoryName(tmp)
            }
        }
    }

    var total     = folders.find("").value
    let needSpace = 30_000_000 - (70_000_000 - total)
    foreach name, val in folders
    {
        if val >= needSpace and val < total do
            total = val
    }

    //Console.print("total: ", total)
    @assert(total == 942298)
}
