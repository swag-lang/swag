In 'Swag.DocKind.Examples' mode, swag will generate a documentation like this one. Each file will be a chapter or a sub chapter.

File names must start with a number of 3 digits, and can be followed by another number for a sub part.

    100_my_title.swg            => will generate a '<h1>My title<<h1>' heading
    101_001_my_sub_title.swg    => will generate a '<h2>My sub title<<h2>' heading
    102_002_my_sub_title.swg    => will generate a '<h2>My sub title<<h2>' heading
    110_my_other_title.swg      => will generate a '<h1>My other title<<h1>' heading
    111_my_other_title.md       => you can mix with '.md' files

In that mode, the comments in the code you want to be interpreted as documentation (and not swag comments) must start with `\/\*\*`.

    /**
    This is a valid documentation comment.
    The comment must start with /** and end with */, which should be alone on their respective line.
    */

Note that the documentation you are reading right now has been generated in that mode, from the [std/reference/language](https://github.com/swag-lang/swag/tree/master/bin/reference/tests/language) module.
