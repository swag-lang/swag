<header class="sticky top-0 z-50">
    <div class="xl:m-auto w-full xl:w-[75rem] xl:flex justify-between items-center p-2" x-data="{ open_menu: false }">
        <div class="flex justify-between items-center">
            <a href="index.php"><img src="imgs/swag_logo.png" alt="SWAG" class="w-24 xl:w-50" /></a>
            <div class="xl:hidden">
                <img src="imgs/burger.png" alt="" class="w-10 cursor-pointer" @click="open_menu = ! open_menu" />
            </div>
        </div>
        <ul class="text-white text-md mt-3 xl:mt-0" :class="open_menu ? '' : 'hidden xl:flex'">
            <li class="xl:mr-5 mb-1 xl:mb-0 ml-2 xl:ml-0"><a href="index.php" class="<?=$_SERVER['REQUEST_URI'] == '/index.php' ? 'border-b' : ''?> hover:border-b">Home</a></li>
            <li class="xl:mr-5 mb-1 xl:mb-0 ml-2 xl:ml-0"><a href="language.php" class="<?=$_SERVER['REQUEST_URI'] == '/language.php' ? 'border-b' : ''?> hover:border-b">Overview</a></li>
            <li class="xl:mr-5 mb-1 xl:mb-0 ml-2 xl:ml-0"><a href="swag.runtime.php" class="<?=$_SERVER['REQUEST_URI'] == '/swag.runtime.php' ? 'border-b' : ''?> hover:border-b">Runtime</a></li>
            <li class="xl:mr-5 mb-1 xl:mb-0 ml-2 xl:ml-0"><a href="std.php" class="<?=$_SERVER['REQUEST_URI'] == '/std.php' ? 'border-b' : ''?> hover:border-b">Std</a></li>
            <li class="xl:mr-5 mb-1 xl:mb-0 ml-2 xl:ml-0"><a href="https://www.youtube.com/channel/UC9dkBu1nNfJDxUML7r7QH1Q" target="_blank" class="hover:border-b">YouTube</a></li>
            <li class="mb-1 xl:mb-0 ml-2 xl:ml-0"><a href="https://github.com/swag-lang/swag" target="_blank" class="hover:border-b">Github</a></li>
        </ul>
    </div>
</header>
