/*
--- Part Two ---
The line is moving more quickly now, but you overhear airport security talking about how passports with invalid data are getting through. Better add some data validation, quick!

You can continue to ignore the cid field, but each other field has strict rules about what values are valid for automatic validation:

byr (Birth Year) - four digits; at least 1920 and at most 2002.
iyr (Issue Year) - four digits; at least 2010 and at most 2020.
eyr (Expiration Year) - four digits; at least 2020 and at most 2030.
hgt (Height) - a number followed by either cm or in:
If cm, the number must be at least 150 and at most 193.
If in, the number must be at least 59 and at most 76.
hcl (Hair Color) - a # followed by exactly six characters 0-9 or a-f.
ecl (Eye Color) - exactly one of: amb blu brn gry grn hzl oth.
pid (Passport ID) - a nine-digit number, including leading zeroes.
cid (Country ID) - ignored, missing or not.
Your job is to count the passports where all required fields are both present and valid according to the above rules. Here are some example values:

byr valid:   2002
byr invalid: 2003

hgt valid:   60in
hgt valid:   190cm
hgt invalid: 190in
hgt invalid: 190

hcl valid:   #123abc
hcl invalid: #123abz
hcl invalid: 123abc

ecl valid:   brn
ecl invalid: wat

pid valid:   000000001
pid invalid: 0123456789
Here are some invalid passports:

eyr:1972 cid:100
hcl:#18171d ecl:amb hgt:170 pid:186cm iyr:2018 byr:1926

iyr:2019
hcl:#602927 eyr:1967 hgt:170cm
ecl:grn pid:012533040 byr:1946

hcl:dab227 iyr:2012
ecl:brn hgt:182cm pid:021572410 eyr:2020 byr:1992 cid:277

hgt:59cm ecl:zzz
eyr:2038 hcl:74454a iyr:2023
pid:3556412378 byr:2007
Here are some valid passports:

pid:087499704 hgt:74in ecl:grn iyr:2012 eyr:2030 byr:1980
hcl:#623a2f

eyr:2029 ecl:blu cid:129 byr:1989
iyr:2014 pid:896056539 hcl:#a97842 hgt:165cm

hcl:#888785
hgt:164cm byr:2001 iyr:2015 cid:88
pid:545766238 ecl:hzl
eyr:2022

iyr:2010 hgt:158cm hcl:#b6652a ecl:blu byr:1944 eyr:2021 pid:093154719
Count the number of valid passports - those that have all required fields and valid values. Continue to treat cid as optional. In your batch file, how many passports are valid?
*/

using Core

func day4B()
{
    day4B0()
    day4B1()
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
func day4B0()
{
    let dataPath = Path.combine(Path.getDirectoryName(#curlocation.fileName), "4.txt")
    let lines    = assume File.readAllLines(dataPath)

    var total, pass: u32
    foreach l in lines
    {
        foreach r in Tokenize.split(l, ' ''u8)
        {
            let res1 = Tokenize.split(r, ':''u8)
            switch res1[0]
            {
            case "byr":
                let val = catch StrConv.toS32(res1[1])
                if val >= 1920 and val <= 2002 do
                    pass |= 1
            case "iyr":
                let val = catch StrConv.toS32(res1[1])
                if val >= 2010 and val <= 2020 do
                    pass |= 2
            case "eyr":
                let val = catch StrConv.toS32(res1[1])
                if val >= 2020 and val <= 2030 do
                    pass |= 4
            case "hgt":
                let val = catch StrConv.parseS32(res1[1])
                if @err != null do
                    break
                if val.eat == @countof(res1[1]) do
                    break
                let sub = res1[1][val.eat to ]
                if sub != "cm" and sub != "in" do
                    break
                if sub == "cm" and val.value >= 150 and val.value <= 193 do
                    pass |= 8
                elif sub == "in" and val.value >= 59 and val.value <= 76 do
                    pass |= 8
            case "hcl":
                if @countof(res1[1]) != 7 or res1[1][0] != '#' do
                    break
                discard catch StrConv.parseX64(res1[1][1 to ])
                if @err == null do
                    pass |= 16
            case "ecl":
                if Slice.contains(["amb", "blu", "brn", "gry", "grn", "hzl", "oth"], res1[1]) do
                    pass |= 32
            case "pid":
                discard catch StrConv.toS32(res1[1])
                if @err == null and @countof(res1[1]) == 9 do
                    pass |= 64
            }
        }

        if @index == lines.count - 1 or lines[@index + 1].length == 0
        {
            if pass & 0b1111111 == 0b1111111 do
                total += 1
            pass = 0
        }
    }

    //Console.print("total: ", total)
    @assert(total == 224)
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
func day4B1()
{
    let dataPath = Path.combine(Path.getDirectoryName(#curlocation.fileName), "4.txt")
    let lines    = assume File.readAllLines(dataPath)

    assume
    {
        using Parser
        let byr_r = RegExp.create("19[2-9][0-9]|200[0-2]")
        let iyr_r = RegExp.create("201[0-9]|2020")
        let eyr_r = RegExp.create("202[0-9]|2030")
        let hgt_r = RegExp.create("((1[5678][0-9])|(19[0-3]))cm|(59|6[0-9]|7[0-6])in")
        let hcl_r = RegExp.create("#([a-f0-9]){6}")
        let ecl_r = RegExp.create("amb|blu|brn|gry|grn|hzl|oth")
        let pid_r = RegExp.create("[0-9]{9}")
    }

    var map: HashTable'(string, { r: Parser.RegExp, b: u32 })
    map.add("byr", {byr_r, 1})
    map.add("iyr", {iyr_r, 2})
    map.add("eyr", {eyr_r, 4})
    map.add("hgt", {hgt_r, 8})
    map.add("hcl", {hcl_r, 16})
    map.add("ecl", {ecl_r, 32})
    map.add("pid", {pid_r, 64})

    var total, pass: u32
    foreach l in lines
    {
        foreach r in Tokenize.split(l, ' ''u8)
        {
            let res1 = Tokenize.split(r, ':''u8)
            let it   = map.tryFind(res1[0])
            if it and it.value.r.match(res1[1]) do
                pass |= it.value.b
        }

        if @index == lines.count - 1 or lines[@index + 1].length == 0
        {
            total += cast(s32) (pass == 0b1111111)
            pass = 0
        }
    }

    //Console.print("total: ", total)
    @assert(total == 224)
}
