using core, image

//#[Swag.printbc]
#test
{
    pathName := dataPath()
    files := try Directory.enumerateFiles(pathName)
    visit f: files
    {
        img := try image.load(f.fullname)
    }
}