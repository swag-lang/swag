#global testerror "Err0406"

func replace(dst, src: string, inSwg, inCpp: &s32)
{
    visit &f in files
    {
        var idx = 0'u64
        loop
        {
            idx = f.content.indexOf(src, idx, Utf8.ComparisonType.Latin1NoCase)
            if idx == Swag.U64.Max:
                break

            if Path.getExtension(f.name) == ".swg"
                inSwg += 1
            elif Path.getFileName(f.name) != "ErrorIds.cpp" and Path.getFileName(f.name) != "ErrorList.h":
                inCpp += 1

            if f.newContent[idx..idx+6] != dst[..6]
            {
                Console.printf("% => % %\n", f.content[idx..idx+6], dst, f.name)
                loop i in 7: f.newContent[idx+i] = dst[i]
                f.changed = true
            }

            idx += 1
        }
    }
}