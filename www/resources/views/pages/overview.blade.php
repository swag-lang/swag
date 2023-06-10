@extends('layouts.layout-large')

@section('app')

@include('common.breadcrumb', ['text' => 'Overview'])
@include('pages.g.overview')

@endsection
