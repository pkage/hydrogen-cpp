# Hydrogen Project
A "fully-featured" programming language
---

copy `hydrogen/` into `/usr/local` and `bootstrapper/hydrogen` onto your `$PATH` to install

---

note that while the execution server can run on either python 2.7+ or python 3.3+ it requires flask to be installed

install flask with `sudo apt-get install flask`

---

`hydrogen-runtime/` contains the C++ runtime client

`hydrogen-ios/` contains the ios client

`hydrogen-web/` contains the web client

`hydrogen/server` contains the execution server

---

Both the hydrogen-ios and hydrogen-web projects rely on an execution server to run
