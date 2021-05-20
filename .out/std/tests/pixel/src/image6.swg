using Core, Pixel

//#[Swag.printbc]
#test
{
    pathName := dataPath()
    files := try Directory.enumerateFiles(pathName)
    visit f: files
    {
        img := try Image.load(f.fullname)
    }
}