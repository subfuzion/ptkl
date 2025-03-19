# ATTRIBUTION

This work incorporates C source code forked from various projects to implement
a lightweight, performant, and unified web runtime environment for JavaScript
apps with the following features:

- JavaScript engine and compiler
- Storage engine
- Database engine
- Messaging engine
- Unified command shell / TUI that includes
    - JavaScript REPL
    - Admin console for deployment and monitoring

This work is licensed under the [MIT License]. This license is considered to be
compatible with the licenses of the forked projects listed below.


## QuickJS

[QuickJS] is an embeddable JavaScript engine created by Fabrice Bellard.

The purpose of this fork is to implement a lightweight JavaScript runtime and
standard library for modern web apps. Changes in this fork include:

* Update for C23 and strip features that aren't needed, such as
  Windows and 32-bit support; remove support for non-standard bigfloat,
  bigdecimal, and qjscalc
* Implement standard library for web APIs (like `fetch`), web request handlers,
  and distributed app internal service messaging
* Add automatic type-stripping for JavaScript + types (a subset of TypeScript)

[QuickJS License]: MIT License

## SQLite

[SQLite] is an embeddable SQL data engine.

[SQLite License]: SQLite is in the public domain.

## ncurses

The ncurses (new curses) library is a free software emulation of curses in
System V Release 4.0, library of functions that manage an application's display
on character-cell terminals.

[ncurses License]: LGPL v2.1


[MIT License]: https://opensource.org/license/mit
[ncurses]: https://github.com/mirror/ncurses
[ncurses License]: https://github.com/mirror/ncurses/blob/master/COPYING
[QuickJS]: https://github.com/bellard/quickjs
[QuickJS License]: https://github.com/bellard/quickjs/blob/master/LICENSE
[SQLite]: https://github.com/sqlite/sqlite
[SQLite License]: https://github.com/sqlite/sqlite/blob/master/LICENSE.md
