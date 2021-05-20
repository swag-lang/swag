using core, image

//#[Swag.printbc]
#test
{
    pathName := dataPath()
    files := try directory.enumerateFiles(pathName)
    visit f: files
    {
        img := try image.load(f.fullname)
    }
}