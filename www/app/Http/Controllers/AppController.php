<?php

namespace App\Http\Controllers;

use Illuminate\Http\Request;

class AppController extends Controller
{

    public function home()
    {
        return view('pages.home');
    }

    public function std()
    {
        return view('pages.libraries');
    }

    public function overview()
    {
        return view('pages.overview');
    }

    public function getting_started()
    {
        return view('pages.getting-started');
    }

    public function swag_as_script()
    {
        return view('pages.swag-as-script');
    }
}
