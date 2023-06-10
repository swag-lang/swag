@extends('layouts.layout')

@section('app')

@include('common.breadcrumb', ['text' => 'Std'])

<div class="text-2xl">Standard Modules</div>
<ul class="mt-5 ml-5 list-disc">

    <?php
    $files = [];
    $dir = public_path('std/');
    $handle = opendir($dir);

    while ($file = readdir($handle))
        if (is_file($dir . $file) && $file != 'swag.runtime.html')
            $files[] = $file;

    sort($files);

    foreach ($files as $file) {
        ?>

        <li>
            <a href="/std/{{ $file }}" class="hover:underline" target="_blank">{{ str_replace('.html', '', $file) }}</a>
        </li>

        <?php
    }
    ?>

</ul>

@endsection
