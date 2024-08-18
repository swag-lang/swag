using Core, Pixel, Core.Threading

struct TH
{
    using base:     Thread
    filename:       String
}

func entry(th: Thread)
{
    var param = cast(const *TH) &th
    var img   = assume Image.load(param.filename)
}

//#[Swag.PrintBc]
#test
{
    var pathName = dataPath()
    var files    = try Directory.enumerateFiles(pathName)

    var ths: Array'TH
    ths.resize(files.count)

    var validImgs = 0
    visit f in files
    {
        if !Image.canLoad(f.fullname) do
            continue
        var th = &ths[validImgs]
        th.filename = f.fullname
        try Thread.init(th, &entry)
        try th.start()
        validImgs += 1
    }

    loop i in validImgs do
        ths[i].wait()
}
