# JNI学习笔记
**前言**：JNI是在Java中调用本地方法的技术，说直白点就是用java调用C/C++程序，毕竟JVM是用C语言实现的这点功能肯定要有。
## JNI有啥用
JNI技术现在已经用的不多了，Java已经有了一套庞大的生态体系，足够应付许多任务。JNI主要用于：

1. 当你已经用C语言写了一套API但不想用Java再写一遍的时候
2. 当你会了一套C语言下的类库但该类库没有Java版（或者你不想学）的时候
3. 想秀一波操作的时候

## JNI浅层原理
JVM说叫虚拟机，其实就是一个普通的程序，他运行时还是得调用底层的动态链接库接着进一步调用操作系统接口。他能用Sun公司开发出来的库，也能用操作系统里面的库，自然也能调用你自己写的库。

Java中有```System.loadLibrary(String)```方法，可以加载动态连接库，比如：
```java
static {//一般放在静态代码快里面，在虚拟机进行类加载的时候载入所需库文件
    System.loadLibrary("demo1");
    /*
        一行代码，参数传入库的名称
        在Linux系统，应该有一个叫libdemo1.so的文件可以被JVM找到并加载
        在Windows系统，应该有一个demo1.dll文件可以被找到并加载
        具体这个文件应该放在哪，与操作系统如何找库文件有关，一般是PATH环境变量所指的位置。
        或者在运行时使用-Djava.library.path=...指定路径
    */
}
```
而在类中，应当使用native关键字声明这个方法是一个本地方法，不需要实现，因为它将由C语言实现。下面是一个完整的例子：
```java
package com.scarike.jni.demo1;
public class Main{
    static{
        System.loadLibrary("demo1");
    }
    private native void sayHello();
    public static void main(String[] args){
        new Main().sayHello();
    }
}
```
在哪里调用，在哪里加载灵活多变，不要拘泥于此demo
## 如何编译动态库
Java部分的语法其实就说完了，现在来看C语言部分

JVM现在有一个class文件，需要的是一个动态链接库，那么JVM如何将库中的函数和Java方法对应上呢  
简单粗暴：用函数名  
对于一个Java方法
```java
void com.scarike.jni.demo1.Main.sayHello()
```
他在C语言中对应的方法是：
```c
JNIEXPORT void JNICALL Java_com_scarike_jni_demo1_Main_sayHello(JNIEnv*, jobject);
```
主要看函数名和参数，别看不认识的关键字，函数名是Java+全限定类名+方法名，用下划线分隔，参数JNIEnv*是指向JVM环境的指针，我们会用他来调用JNI给我们提供的方法，jobject是调用当前方法的实例对象

别担心，这些是不需要我们自己写的，编写好Java部分的代码后，使用以下命令(演示全部基于上面的代码实例，自己实践时注意包名类名方法名)
```shell
> cd src/ #首先注意切换到src目录，也就是包所在目录
> javac com/scarike/jni/demo1/Main.java #这一步是编译成class文件
> javah com.scarike.jni.demo1.Main #生成头文件
> ... #写好具体实现并编译后，比如把库放在src目录下
> java -Djava.library.path=. com.scarike.jni.demo1.Main #.表示当前目录，指定库文件位置并运行
```
顺带一提，如果使用JDK10以后，JDK取消了javah命令，和javac合并了，直接使用如下命令编译并生成头文件：
```shell
> javac -h ./ com/scarike/jni/demo1/Main.java #加入-h ./表示顺便生成头文件放在当前目录下，其他不变
```
这是上面的案例生成的头文件(删去了注释)：
```c

#include <jni.h>

#ifndef _Included_com_scarike_jni_demo1_Main
#define _Included_com_scarike_jni_demo1_Main
#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL Java_com_scarike_jni_demo1_Main_sayHello
  (JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif

```
有了头文件，下一步就是实现这个函数然后编译，注意这里包含了jni.h文件，这个文件内部还包含了jni_md.h，只需要在预处理阶段让编译器找到这个两个文件即可，我的做法是将这两个文件放在了/usr/include/目录下，也可以把这两个文件放到生成头文件的同级目录，然后把```#include <jni.h>```换成```#include "jni.h"```，他们的区别就不细说了,C语言编译过程也不细说了。

下面是实现的com_scarike_jni_demo1_Main.c文件：
```c
#include <stdlib.h>
#include <stdio.h>
#include "com_scarike_jni_demo1_Main.h"

JNIEXPORT void JNICALL Java_com_scarike_jni_demo1_Main_sayHello(JNIEnv * env, jobject obj){
	printf("Hello JNI!");
}
```
然后使用下面的命令编译（我的系统是Ubuntu）
```shell
> gcc --shared com_scarike_jni_demo1_Main.c -o libdemo1.so
```
动态库我们准备好了，发布程序带着这个库就行了，最后放一下此时的目录结构
```
src
├── com
│   └── scarike
│       └── jni
│           └── demo1
│               ├── Main.class
│               └── Main.java
├── com_scarike_jni_demo1_Main.c
├── com_scarike_jni_demo1_Main.h
└── libdemo1.so
```
还有需要的所有指令:
```shell
#我的环境：Ubuntu 20.04/JDK 11.0.12/gcc 9.3.0
> cd src/
> javac -h ./ com/scarike/jni/demo1/Main.java
> vim com_scarike_jni_demo1_Main.c
> gcc --shared com_scarike_jni_demo1_Main.c -o libdemo1.so
> java com.scarike.jni.demo1.Main
```
## JNI类型映射
进行JNI开发时遇到的第一个问题就是如何将java类型转换为c语言类型。其实这也会是最后一个问题

java的类型有：```byte```,```short```,```int```,```long```,```float```,```double```,```boolean```,```char```以及对象类型```Object```。  
c语言的基本类型有：```char```，```short```，```int```，```long```，```double```，```float```以及几个特殊的```unsigned```关键字，```void*```指针类型，其实还有如__INT64等等。

他们之间的对应关系已经在jni.h中定义好了：
```c
typedef unsigned char   jboolean;
typedef unsigned short  jchar;
typedef short           jshort;
typedef float           jfloat;
typedef double          jdouble;
//下面三个在jni_md.h中
typedef int             jint;
typedef long            jlong;
typedef signed char     jbyte;
//注意不同的系统由于其不同的宏定义，可能映射关系不一样，请自己查看（这两个文件在jdk安装目录的include目录下，最好在IDE中查看，IDE会检查当前的宏定义然后高亮出在你的系统上的定义），我的是64位系统
```
所以看见这些陌生的关键字不要怂，都是我们熟悉的数据类型换了个名字。

值得注意的是jchar对应的是unsigned short类型而不是char。详情请搜索JVM字符类型的表示方式。

有了这些定义，我们直接使用就好了，当作java类型来使用。看下面的案例：

java源文件
```java
package com.scarike.jni.demo2;

public class Main {
    static {
        System.loadLibrary("demo2");
    }

    private native double eleType(byte b,short s,int i,long l,float f,double d,boolean bo,char c);

    public static void main(String[] args) {
        double d = new Main().eleType((byte) 123, (short) 345, 31415926, 999999999999999999l, 3.14f, 3.14159265, true, 'c');
        System.out.println("JVM revc=>"+d);
    }
}
```
头文件
```c
/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_scarike_jni_demo2_Main */

#ifndef _Included_com_scarike_jni_demo2_Main
#define _Included_com_scarike_jni_demo2_Main
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_scarike_jni_demo2_Main
 * Method:    eleType
 * Signature: (BSIJFDZC)D
 */
JNIEXPORT jdouble JNICALL Java_com_scarike_jni_demo2_Main_eleType
  (JNIEnv *, jobject, jbyte, jshort, jint, jlong, jfloat, jdouble, jboolean, jchar);

#ifdef __cplusplus
}
#endif
#endif
```
c语言源文件
```c
#include <stdlib.h>
#include <stdio.h>
#include "com_scarike_jni_demo2_Main.h"

JNIEXPORT jdouble JNICALL Java_com_scarike_jni_demo2_Main_eleType
  (JNIEnv * env, jobject obj, jbyte b, jshort s, jint i, jlong l, jfloat f, jdouble d, jboolean bo, jchar c){
	  printf("b=%d\ns=%d\ni=%ld\nl=%lld\nf=%f\nd=%lf\nbo=%dc=%c\n",b,s,i,l,f,d,bo,c);
	  return 114.514;
  }
```
编译运行后输出
```
> java -Djava.library.path=. com.scarike.jni.demo2.Main 
b=123
s=345
i=31415926
l=999999999999999999
f=3.140000
d=3.141593
bo=1c=c
JVM revc=>114.514
```