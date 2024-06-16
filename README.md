<h2>Introduction</h2>

My own programming language. You can think of it as C++ clone.

Compiler is at early stage, and not user freandly.
It only have features that i use.

Here is a simple program for demonstration.
```java
main := !(char[] args) -> int
{
        thisFile := FSGetFile("main2.hv")
        fileMap := thisFile.GetMap()
        fileText := StringSpan(fileMap.Get()->{char^},fileMap.Size())

        reg := #Regexp( "[a-zA-Z][0-9a-zA-Z]*",1
                        "[0-9]+",2)

        words := 0
        numbers := 0
        reg.RegExpReadText(fileText,(wrd,tokenValue) ==> {
                switch tokenValue
                {
                        case 1
                                words += 1
                        case 2
                                numbers += 1
                }
        })

        printf("File contain %i words and %i numbers\n",words,numbers)

	return 0
}
```

<h2>Why did i invent a wheel</h2>

With my own compiler i can add wacky features. Most of them would never be added to commercial compilers, some of them should not exist.

You can find these usefull:

<h4>C# partial class</h4>
Class implementation could be separated between multiple files.

<h4>File embedding</h4>
With this you can make a file as part of .exe file. Nothing new for popular compilers. Also i provide some middle step before embedding. It could be simple as special compression algorithm. I made it so i could compile vulkan api shaders before embedding them.

<h4>Reflection</h4>
It is not fully implemented and i have no idea what "fully implemented" is. Right now you can iterate over all field of class and compare types. Just enough for simple reference counting.

<h4>Vulkan api support</h4>
For easier work, compiler provides unique class. All of the fields are function pointers to Vulkan API commands. But fields are created on demand. Unused functions don't exist in the .exe file. 
And reflection still works for this class, you can iterate over all of its fields (only created ones), and initialize them with vkGetInstanceProcAddr.

Another modification is empbedding shaders. Deference from embedding it as file is that you select text file, and it is gonna be compiled into bytecode before embedding.

<h2>End goal</h2>

Allow compiler to be modified. So my exotic features could be a library. Right now i want to allow code to be launched at compile time. They could call some functions like RegisterClass,RegisterCommand.
At least it should be possible to open .xml file and make a new class out of it.
 

