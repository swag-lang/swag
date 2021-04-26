using core

//#[swag.printbc]
#main
{
    var opt: directory.EnumerationOptions
    opt.wantDirectories = false

    total := 861
    f := try directory.enumerate("f:/swag/src", opt)
    visit one: f
    {
        //console.printf("read %\n", one.fullname)
        b := try file.readAllBytes(one.fullname)

        hasOne := false
        pz := b.buffer
        while :pz
        {
            pz += 1

            findString := false
            if memory.compare(pz, @dataof("verifyError("), 12) == 0
                findString = true

            if !findString && :pz == "\""'u8
            {
                pz += 1
                while :pz && :pz != "\""'u8
                    pz += 1
                continue
            }

            if findString
            {
                var str: String
                while :pz != "\""'u8  && :pz != "}"'u8
                {
                    pz += 1
                }

                if :pz == "}"'u8
                    continue

                pzStart := pz
                pz += 1
                while :pz != "\""'u8
                {
                    str += :pz
                    pz += 1
                }

                if str.length < 7
                    continue

                tutu := format.toString("Msg%{D04}", total)
                pzScan := pzStart
                while pzScan != pz
                {
                    :pzScan = " "'u8
                    pzScan += 1
                }

                :pzScan = " "'u8
                memory.copy(pzStart, tutu.buffer, tutu.length)

                hasOne = true
                console.printf("constexpr static const char* Msg%{D04} = \"%\";\n", total, str)
                total += 1
            }
        }

        if hasOne
        {
            //var dst: String = "f:/src1/"
            //dst += path.getFileName(one.fullname)
            try file.writeAllBytes(one.fullname, b)
        }
    }

    console.printf("total: %\n", total)
}