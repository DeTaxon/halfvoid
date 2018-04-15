printf := !(char^ str, ...) -> int declare
calloc := !(int size,int sizet) -> void^ declare
malloc := !(int size) -> void^ declare
free := !(void^ point) -> void declare

dlopen := !(char^ name,int pri) -> void^ declare
dlsym := !(void^ hndl, char^ name) -> void^ declare
dlclose := !(void^ hndl) -> int declare
