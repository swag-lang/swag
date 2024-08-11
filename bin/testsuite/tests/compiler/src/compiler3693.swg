#global testerror "Err0053"
func ReleaseDC() throw { }
func GetFontData() throw { }

//#[Swag.PrintBc]
func getDataFromHFONT() throw
{
    defer ReleaseDC()
    GetFontData()
}

//#[Swag.PrintBc]
#test
{
    catch getDataFromHFONT()
}