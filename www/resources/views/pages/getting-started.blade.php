@extends('layouts.layout')

@section('app')

@include('common.breadcrumb', ['text' => 'Getting started'])
@include('pages.g.getting-started')

@endsection
