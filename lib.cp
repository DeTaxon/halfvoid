printf :=  !(char^ Line,...) -> int declare
sscanf := !(char^ Buf,char^ Format,...) -> int declare
puts := !(char^ Line) -> int declare
malloc := !(int Size) -> void^ declare
free := !(void^ Point) -> void declare
strtof := !(char^ Line) -> float declare
sleep := !(s32 seconds) -> int declare

sinf := !(float In) -> float declare
cosf := !(float In) -> float declare
tanf := !(float In) -> float declare
sqrtf := !(float In) -> float declare

glXGetProcAdress := !(char^ name) -> void^ declare
