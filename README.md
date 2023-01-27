Stolen programming language
supports x64 only, windows and linux


Standard support for language
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
		printf("no need for {} for single call")

	i := 0
	while i != 100
	{
		printf("value %i\n",i)
		i += 1

		if i == 55
			break
	}

	for i : 100 // more on foreach cycle below
	{
		printf("value %i\n",i)
	}

	x = y->{void^} // cast type, there is no dynamic_cast
	a = 7 / b // operator "/" always result as float(or double)
	a = 7 div b // use div to divide as integer
	a = 7 mod b  // to take remainder
	
	x := int // create variable with type int
	y := int[3]
	z := int[] // pointer to ArraiView of type int, it could be pointer to Vector,ArraySpan,List,HybridQueue or anything else that inherits ArrayView
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
	//called like that ConstFunc().{int,27}
	printf("set %i\n",Value)
}
DefaultValues := !(int x = 13, void^ y = #uniq_call) -> void // TODO not implemented
{
	DefaultValues(1,null)
	DefaultValues(1)
	DefaultValues()

	//#uniq_call means caller will create "unique value"
	//designed for TGuard() 
}
ManyArgs := !(args...) -> void
{
	x := #best(args) // get type that could hold all value , TODO not implemented
	//x := #best(int,args)
	//int + float -> float
	//int + char -> int
	x = 0
	for it : args // variadic inputs can be iterated, cycle acts here as a macros
	{
		if it > x
			x = it
	}
}
```

Input variables can have static modifier, that means static variable will be created in place where it was called.
```java

Func := !(bool var = static true) -> void
{
	//all static variables always passed by reference
	var = false
}
Caller := !() -> void
{
	Func()

	//same as
	var := static true //TODO not implemented
	FuncStaticless(var)
}

```

Types:
u8 u16 u32 u64 s8 s16 s32 s64 float double void <br />
char is u8 <br />
short is s16 <br />
int is s32 <br />
long is s64 <br />
c_long is s32 on windows and s64 on linux <br />
size_t is u64 , x64 bit support only <br />


All files import each other, there is no need for #include or import like calls


Features:

Question operators:
```java
if pointer?.value != 0 //same as
//if pointer != null and pointer.value != 0
{
	do(pointer.value)
}

do(pointer?.value ?? 0) //pass 0 if pointer == null
do(pointer != null ?: pointer.value : 0) // can be used in trinary operator. half way implemented, only supported on left side of ?:

do(point1?.value ?? point2?.value ?? 0)
do(point1?.value ?? point2?.value) //function "do" will not be called at all if both pointers are null

Question operator works with references (reference could be null, for this case) // not implemented, it's need rework
Function call "[]" replaced with "[]?"

For example AVLMap uses change behavior

if Animals["lion"].type != "cat" // if "lion" does not exist, entry will be created
if Animals["lion"]?.type != "cat" // if "lion" does not exist, if statement evaluate to false, no entry created
```

Foreach cycle is overcomplicated
```java
for item, index : container
{
	//if container "for" function does not provide indexes, it will be 0,1,2,3...
}

for item : container
{
}
for container
{
	//variable "it" represent item
	it = 0
}

//to make use in cycle, class must support function "for", and return specific class
MyClass := class
{
	...
	"for" := !() -> MyIterator 
	{
		return MyIterator()
		return MyIterator(this,x,1) //constructor
	}
}
MyIterator := class
{
	"()" := !() -> ref int { return x } //Get value of object
	"()" := !(IndexType^& returnIndex) -> ref int { return x } //Get value of object and index

	//two ways to check for end of iterator
	//if both functions implemented, HaveValue is in priority
	HaveValue := !() -> bool { return x > 0} // called before operator "()"
	IsFinished := !() -> bool {return x > 0} //Is cycle ended, called after operator "()"
	
	//Optional
	Inc := !() -> void {} // called after cycle body ends

	// inherit from TGCObject to have c++ destructor like function
	Destroy := virtual !() -> void {}
}
//or it can return generator
MyClass2 := class
{
	...

	//All labmda functions support operator "()" and IsFinished
	"for" := !() -> !()& -> int
	{
		x := () ==> int
		{
			i := 0
			while i < 10
			{
				yield i
				i += 1
			}
		}
		return x.Capture()
	}
}

Cycle can also work as a macros for weird cases (there are only 2 right now)
Body of cycle will be copy-pasted for all elements (so don't put static variable here).

First use showed in variadic variables.

Second use is to iterate all elements of structure.
for field, filed_name : struct->AllFields // field_name have type of char^
{
	if field->Type == int
	{
		printf("Class have integer with name %s and value %i\n",field_name,field)
	}
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

//Map List and Vector support reverse iteration (TODO, vector does not support yes)

for item1 : container1.reverse
{
	
}

//openmp like launch, not implemented //TODO
//same as TAwaitWork
#awork //working in version 2
for i : 100
{
	...
	TGuard() // not implemented
	//CriticalSection
}
```

Operator [^]
it is actualy a macros
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

Classes have they own weirdness
```java
WayBiggerThenExpectedClass := class
{
	a := int
	b := int

	Method := !() -> void
	{
		aCalculated := field int
		//aCalculated is gonna be a field in class, it's designed for decorators
	}

	CalculateSome := @cache !(int x) -> int
	{
		//@cache decorator makes cache data...
		//so if function is called with same data twice or more, only first call is calculated, other call retrived from cache
		//for that to work decorator adds field Map.{Tuple.{int},int}
		return x**13 + x*3 - x + something else that makes calculation longer
	}
}

//Namespaces are way to have class with multiple identical functions, like operator ==
Namespaces := class
{
	SomeMethod := !() -> void
	{
		printf("hello\n")
	}

	ExtraSpace := fake // maybe in future will be renamed to namespace
	{
		//right now namespaces only works with methods
		SomeMethod := !() -> void
		{
			printf("world\n")
		}
	}
}

var := Namespaces
var.SomeMethod()
var.ExtraSpace.SomeMethods()

for example String have namespace End

file_name == ".bmp" //regular string equality check
file_name.End == ".bmp" // since ".bmp" is 4 letter long, last 4 letters of file_name will be compared to ".bmp"

Right now only first element can be used with namespaces, 2 lines below will fail to compile
".bmp" == file_name.End
file_name.End == file_name.Start

```



Tuple support
```java
x := Tuple.{int,float}(3,4.1)
y := Tuple.{int,int}
x.0 = 3
x.1 = 3.14
y = !{2,6}
x = y
```

Lambda support
```java
func := !() -> void
{
	a := 1 //lazy capture, at start capture by reference, when cloned, by value. Designed to be fast, nothing happens before capture.
	b := 2 //captured by value
	c := 3 //captured by reference

	lfunc := (int x,int y) ==> [b,c&,d = a + b]{ //variable d will be calculated here, not at caputure
		c := a + b + c + x + y
	}

	l := List.{int}()

	//dont need set type if passing to function that have description for lambda type
	l.DeleteIf(x ==> x < 0)
	
	//Capture creates copy of lambda on heap
	//Values captured by value are copied
	//If lambda is a generator, all stack objects copied, and execution state
	lfunc2 := lfunc.Capture()
	lfunc3 := lfunc2.Capture() //you can make as many copies as you want
	
	//Lambdas are reference counted (inherits TGCObject), so to release memory you must remove all references
	lfunc3 = null //this will release emmory
	//lfunc3 will be released anyway because life of the variable ends as we quit function
}
```

Generator support
```java
//only lambda functions can be generators
lambd := () ==> int {
{
	while true
	{
		yield 1
		yield 13
		yield 7
		yield 0
	}
	
	return 0 //return stops generator, either reaching the end of function, IsFinished will return true
	//it's a bug, returned value will not be used, in the future only "return void" will be allowed in generators
}

while true
{
	x := lambd()
	if lambd.IsFinished()
		break
	print(x)
}

```

Switch is different
```java
switch x
{
	case 1
		//at the end of case, code quits switch
		//you must call continue to move to the next case
	case 2 
	case y // case works if one of these three operators works x == y, x in y, x <=> y == 0
	case in 5..10
	case in ![9,0,123]
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
Implemented by longjmp (not a zero cost exception)
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
}
```

Defer tail function
```java
//this is outdated version
TGuard := !(void^ ptr) -> void //TODO, not implemented
{
	TLock(ptr)
} defer_tail (void^ ptr) {
	TUnlock(ptr)
}

main := !() -> void
{
	TGuard(variable&)

	//Sames as 
	TLock(variable&)
	defer TUnlock(variable&)
}

//right not it's just a concept
//defer_tail should act as continuation of a function, so all local variables are reacheble for bottom

OpenFile := !(char^ name) -> void^
{
	handle := fopen(name,"rb")
	return handle
} defer_tail {
	fclose(handle)
}

```

Threads
Inspired by javascript: one thread , but heavy computation passed to thread pool

```java

main := !(char^[] args) -> void
{
	TSpawnTask(() ==> {
		//Grean thread 1
		while true
		{
			//do some
			TSleep(1) // 1 second, async call

			x := @work HeavyComputation() //Heavy computation happens on different thread, this task is paused until function call is over
		}
	})
	TSpawnTask(() ==> {
		//Grean thread 2
		while true
		{
			TAwaitWork( // async call, waiting until lambda function is finished
				() ==> {
				// this is executed in separate thread
				TLock(ptr) // lock access to object
				defer TUnlock(ptr)

				//or
				TGuard(ptr)

				//or, using position in code as parameter (not realy)
				// same as openmp critical section
				TGuard()
			})
			//do some
		}
	})
}


```


Small features
```java
x := ![1,2,argc,pi] //creates array
//if all values are constant and primitive(numbers or "constant strings"), array will be created at compile time as global variable

//Standart type range
for i : 6..10 {}
y := Range
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
name := filename[0..4] // string is not copied, instead proxy class (StringSpan) is created 
//x..y 
//x - offset, negative means offset from end
//y - size , negative means how much left at the end, zero means to the end
switch fileName.End
{
	case ".txt"
	case ".webp"
	case ".c"
}

//centf
//centf is pair position,quaternion and scale(on everything,cant set x-axis only)
//act like matrix4x4, but perspective matrix not supported

```

ZipFS
Same as PhysicsFS, but worse
Support .zip only, deflate only, read only
```java
FSGetFile("README.md") // return file, null on error, exception in future

modFile := FSGetFile("Level1/box.ply")
modFile.IsVirtual() // true if file inside .zip file

fileMem := modFile.GetMap()

memcpy(myMem,fileMem.Get(),fileMem.Size())
```

