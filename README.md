Stolen programming language
supports x64 only, windows and linux


Standard support for compiler
```java
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
		printf("no need for {} for single call)

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

	x = y->{void^} // cast type, there is no dynamic_cast
	a = 7 / b // operator "/" always result as float(or double)
	a = 7 div b // use div to divide as integer
	a = 7 % b 
	// look at Priority.pr for more operators, you can add your own operator in file, but must recompile project
	
	x := int // create variable winth type int
	y := int[3]
	z := int[] // pointer to array that have array size (z->len) , this is a mistake, those arrays shold be allocated on temporary array only
	w := int^ //pointer to int
	xyzw := int^[3] // array of 3 poiters to int
	
	x := 3 + sqrtf(4) //create variable x with value evaluated by line
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
ManyArgs := !(args...) -> void
{
	x := #best(args) // get type that could hold all value
	//x := #best(int,args)
	//int + float -> float
	//int + char -> char
	x = 0
	(x += args)... //operator ... make copy of line, where args replaced with paramter
}
```

Types:
u8 u16 u32 u64 s8 s16 s32 s64 float double void
char is u8
short is s16
int is s32
long is s64
c_long is s32 on windows and s64 on linux
size_t is u64 , x64 bit support only

There is a import, but should not be used 
All files import each other


Features:

Question operators:
```java
if pointer?.value != 0 //same as if pointer != null and pointer.value != 0
{
	do(pointer.value)
}

do(pointer?.value ?? 0) //pass 0 if pointer == null
do(pointer != null ?: pointer.value : 0) // trinary operator

do(point1?.value ?? point2?.value ?? 0)
do(point1?.value ?? point2?.value) //function "do" will not be called at all if both pointers are null

Question operator works with references (reference could be null, for this case)
Function call "[]" replaced with "[]?"

For example AVLMap uses change behavior

if Animals["lion"].type != "cat" // if "lion" does not exist, entry will be created
if Animals["lion"]?.type != "cat" // if "lion" does not exist, if statement evaluate to false, no entry created
```

For cycle works in different way
```java
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
```
Operator  can iterate in parallel (not like threads or OpenMP)
Every iteration you get i-n element from container 1, and i-n element from container 2 and etc

```java
container1 := AVLMap.{char^,Object^}()
container2 := List.{Object^}()

for item1,ind1 : container1 , item2 : container2
{
	item1 = item2
}
//Use attributes to iterate in reverse (require iterator support)

for item1 : container1 ; $reverse
{
	
}
```

Operator [^]
it is actualy macros
```java
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
//It can get worse
//This will be cycle inside a cycle
sum += container1[^] * container2[^]
//Multiply matrices in two lines
matr1[^i][^j] = 0
matr1[^i][^j] += matr2[i][^k]*matr3[k][j] // probably not correct way to multiply
```


Tuple support
```java
x := Tuple.{int,float}(3,4.1)
y := Tuple.{int,int}
y = !{2,6}
x = y
```

Generator support
```java
GetRandom := !() -> int //state value is global variable
{
	while true
	{
		yield 1
		yield 13
		yield 7
		yield 0
	}
	
	return 0 //return call reset generator
}
SomeClass := class
{
	funct := !() -> int //state value is part of class
	{
		yield 1
		return 6
	}
}

lambd := () ==> int {
	yield 3 //state value is connected with lambda objects
	// when lambda is captured, state value is copied
}

```

Lambda support
```java
func := !() -> void
{
	a := 1 //lazy capture, at start capture by reference, when cloned, by value. no preperations happens
	b := 2 //captured by value
	c := 3 //captured by reference

	lfunc := (int x,int y) ==> [b,c&]{
		c := a + b + c + x + y
	}

	l := List.{int}()

	//dont need set type if passing to function
	l.DeleteIf(x ==> x < 0)
	
	//boost.lambda
	//in syntax tree all operators united to lambda from _X variable
	//it is possible to use not all of the input variables, but you can't use non existing , like _9999
	l.DeleteIf(_1 < 0)
	
	//Capture creates copy of lambda on heap, affected by memory poll
	//Values captured by value are copied
	//If lambda is a generator, all stack objects copied
	lfunc2 := lfunc.Capture()
	lfunc3 := lfunc2.Capture() //you can make as many copies as you want
	
	//Release memory
	lfunc2.Destroy()
}
```

Switch is different
```java
switch x
{
	case 1
		//at the end of case, code quits switch
		//you must call continue to move to next case
	case 2 
	case y // case works if one of three operators works x == y, x in y, x <=> y == 0
	case 5..10
	case ![9,0,123]
	case void // if none of them worked
}

//switch is short version of
if x == 1 ...
else if x == 2 ...
else if x == y ...
//if all of the case values are constant and primitives (numbers and char^)
//switch optimized to binary search
```

Exceptions
Implemented by longjmp (not a zero cost)
```java
try
{
	throw new Exception("Failed something") // will be created on temporary memory
}catch(IException^ e)
{
	printf("Exception %s\n",e.Msg())
}
```

Defer
```java
func := !() -> int^
{
	file := fopen()
	defer fclose(file) //line will be called before function exits, even when exception is thrown
	
	if x
	{
		defer printf("") // will be called when exiting if.true statement
	}
	
	values := new int[1024]
	on_exception delete values //same as defer, but only get called when exception is thrown
	
	return values
}
```

Small features
```java
x := ![1,2,argc,pi] //create array
//if all values are constant and primitive(numbers or char^), array will be created at compile time

//Standart type range
for i : 6..10 {}
y := range //and rangef for float
if x in y {}

//3D vectors
pos := vec3f()
x := pos.x
pos2D := pos.xy
posBack := pos2D.xy0 // posBack.z will be 0
pos1 <+> pos2 // scalar multip.
pos1 <*> pos2 // vector multip.

angle := 37deg // 37 will be transformed to radians
```

Library features
```java
//StringSpan
filename := "file.txt"
name := filename[0..4] // string is not copied, proxy class created
//x..y 
//x - offset, negative means offset from end
//y - size , negative means how much left at the end, zero means to the end
name.Str() // make copy
//if passed to function that accepts char^, StringSpan make a copy in temporary memory
switch StringEnd(fileName) //proxy object, cant describe
{
	case ".txt"
	case ".webp"
	case ".c"
}

//centf
//centf is pair position,quaternion and scale
//act like matrix4x4, but perspective not supported

```



