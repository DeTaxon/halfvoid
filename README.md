Stolen programming language
supports x64 only, windows and linux


Standard support for compiler

//create function, name := !(arguments) -> return type
main := !(int argc,char^^ argv) -> int
{
	if true
	{
		//do on true
	}else{
		//do on false
	}

	if true
		printf("no need for {} for one call)

	i := 0
	while i != 100
	{
		printf("value %i\n",i)
		i += 1
	}

	for i : 100
	{
		printf("value %i\n",i)
	}

	return 0
}
//T - is a template type
FuncTemplate := !(@T value) -> T
{
	return value + 1
}

StrZero := !(char[@Size] chr) -> void
{
	memset(chr,0,Size)
}
ConstFunc := !() .{@T,@Value} -> void
{
	//pass type and integer as constants
	//call ConstFunc().{int,27}
	printf("set %i\n",Value)
}


Types:
u8 u16 u32 u64 s8 s16 s32 s64 float double void
char is u8
short is s16
int is s32
long is s64
c_lang is s32 on windows and s64 on linux
size_t is u64 , x64 bit support only


Features:

Question operators:

if pointer?.value != 0
{
	do(pointer.value)
}

do(pointer?.value ?? 0) //pass 0 if pointer == null
do(pointer != null ?: pointer.value : 0) // trinary operator

do(point1?.value ?? point2?.value ?? 0)

Question operator works with references (reference could be null)
Function call "[]" replaced with "[]?"

For example AVLMap uses change behavior

if Animals["lion"].type != "cat" // if "lion" does not exist, entry will be created
if Animals["lion"]?.type != "cat" // if "lion" does not exist, if statement evaluate to false, no entry created

For operator work in different name

for item, index : container
{
	//if container does not have indexes, it will be 0,1,2,3 ...
}

for item : container
{
}
for container
{
	//variable "it" represent item
	it = 0
}

Operator  can iterate in parallel (not like threads or OpenMP)
Every iteration you get i-n element from container 1, and i-n element from container 2 and etc

container1 := AVLMap.{char^,Object^}()
container2 := List.{Object^}()

for item1,ind1 : container1 , item2 : container2
{
	item1 = item2
}

Use attributes to iterate in reverse (require iterator support)
for item1 : container1 ; $reverse
{
	
}

Operator [^]
it is actualy macros

//this lines is same 
container1[^] = 0
container2[^ind] = ind
//as this lines
for container2
{
	it = 0
}
for it, ind : container2
{
	it = ind
}

It can get worse
This will be cycle inside a cycle
sum += container1[^] * container2[^]
Multiply matrices in two lines
matr1[^i][^j] = 0
matr1[^i][^j] += matr2[i][^k]*matr3[k][j] // probably not correct way to multiply


Tuple support

x := Tuple.{int,float}(3,4.1)
y := Tuple.{int,int}
y = !{2,6}
x = y


Lambda support

func := !() -> void
{
	a := 1 //lazy capture, at start capture by reference, when cloned, by value. no preparetio happens
	b := 2 //captured by value
	c := 3 //captured by reference

	lfunc := (int x,int y) ==> [b,c&]{
		c := a + b + c + x + y
	}

	l := List.{int}()

	//dont need set type if passing to function
	l.DeleteIf(x ==> x < 0)
	
	//boost.lambda
	//in tree all operators united to lambda
	//possible to use not all of the input variables, but you can't use non existing , like _9999
	l.DeleteIf(_1 < 0)
}


