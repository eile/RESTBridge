Release Notes {#mainpage}
============

[TOC]

# Introduction {#Introduction}

Welcome to RESTBridge, a library that provides an simple way to expose a REST
interface in a C++ application.

Please file a [Bug Report](https://github.com/BlueBrain/RESTBridge/issues)
if you find any other issue with this release.

# Changes {#Changes}

## RESTBridge 0.2 (09-Nov-2015) {#version020}

* [43](https://github.com/BlueBrain/RESTBridge/pull/43):
Allow using existing publisher, remove zeroconf-based discovery
* [26](https://github.com/BlueBrain/RESTBridge/pull/26):
RESTBridge can parse command line parameters
* [23](https://github.com/BlueBrain/RESTBridge/pull/23):
Fix vocabulary to return all registered events, not only one

## RESTBridge 0.1 (09-Jul-2015) {#version010}

* Vocabulary registration
* Automatic and generic translation of REST requests into ZeroEQ event
* Heart-beat mechanism between RESTBridge and host application
* Support for PUT and GET REST verbs
* Standalone sample application

