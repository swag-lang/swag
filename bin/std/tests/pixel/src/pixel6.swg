using Core, Pixel, Threading

struct TH
{
    using base: Thread
    filename:   String
}

func entry(th: Thread)
{
    param := cast(const *TH) &th
    img := assume Image.load(param.filename)
}

//#[Swag.PrintBc]
#test
{
    pathName := dataPath()
    files := try Directory.enumerateFiles(pathName)

    var ths: Array'TH
    ths.resize(files.count)

    validImgs := 0
    visit f: files
    {
        if !Image.canLoad(f.fullname)
            continue
        th := &ths[validImgs]
        th.filename = f.fullname
        try Thread.init(th, &entry)
        try th.start()
        validImgs += 1
    }

    loop i: validImgs
        ths[i].wait()
}