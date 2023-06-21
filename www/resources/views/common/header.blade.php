<header class="sticky top-0 z-50 bg-black">
    <div class="lg:m-auto w-full lg:w-[75rem] lg:flex justify-between items-center p-2" x-data="{ open_menu: false }">
        <div class="flex justify-between items-center">
            <a href="{{ route('index') }}"><img src="/imgs/swag-logo.jpg" alt="SWAG" class="w-24 lg:w-50" /></a>
            <div class="lg:hidden">
                <img src="/imgs/burger.png" alt="" class="w-10 cursor-pointer" @click="open_menu = ! open_menu" />
            </div>
        </div>
        <ul class="text-white text-md mt-3 lg:mt-0" :class="open_menu ? '' : 'hidden lg:flex'">
            <li class="lg:mr-5 mb-1 lg:mb-0 ml-2 lg:ml-0"><a href="{{ route('index') }}" class="<?=$_SERVER['REQUEST_URI'] == '/' ? 'border-b' : ''?> hover:border-b">Home</a></li>
            <li class="lg:mr-5 mb-1 lg:mb-0 ml-2 lg:ml-0"><a href="{{ route('overview') }}" class="<?=$_SERVER['REQUEST_URI'] == '/overview' ? 'border-b' : ''?> hover:border-b">Overview</a></li>
            <li class="lg:mr-5 mb-1 lg:mb-0 ml-2 lg:ml-0"><a href="/std/swag.runtime.html" target="_blank" class="hover:border-b">Runtime</a></li>
            <li class="lg:mr-5 mb-1 lg:mb-0 ml-2 lg:ml-0"><a href="{{ route('std') }}" class="<?=$_SERVER['REQUEST_URI'] == '/std' ? 'border-b' : ''?> hover:border-b">Std</a></li>
            <li class="lg:mr-5 mb-1 lg:mb-0 ml-2 lg:ml-0"><a href="https://www.youtube.com/channel/UC9dkBu1nNfJDxUML7r7QH1Q" target="_blank" class="hover:border-b">Youtube</a></li>
            <li class="mb-1 lg:mb-0 ml-2 lg:ml-0"><a href="https://github.com/swag-lang/swag" target="_blank" class="hover:border-b">Github</a></li>
        </ul>
    </div>
</header>
