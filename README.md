# JNI学习笔记
**前言**：JNI是在Java中调用本地方法的技术，说直白点就是用java调用C/C++程序，JVM这点功能肯定要有。

最近想着学习一下JNI这门技术，这篇文档算是一个学习总结，用以记录以免以后忘记了，毕竟JNI不太常用  
这篇文章也带有教程性质，我尽可能写的详细与基础，如果你也想学习JNI推荐先了解：  

1. JVM内存模型，不需要太深入，了解堆、栈、方法区、常量池等对于理解本地方法调用大有益处
2. C语言编译过程，预处理->编译->汇编->链接->运行，知道动态/静态链接库是什么
3. DOS或Shell命令，gcc编译器，JDK的使用

给出一个链接：https://docs.oracle.com/en/java/javase/11/docs/specs/jni/index.html ，这是Oracle关于JNI的官方文档

本文中的案例说明：  
    包名前缀为：com.scarike.jni.*  
    demo1：编程之源Hello World  
    demo2：普通类型参数传递  
    demo3：String类型参数传递  
    demo4：数组类型参数传递  
    因为全部使用javac命令手动编译，因此class文件和java源文件在同一目录下而不是target文件夹或out文件夹下

## JNI有啥用
JNI技术现在已经用的不多了，Java已经有了一套庞大的生态体系，足够应付许多任务。JNI主要用于：

1. 当你已经用C语言写了一套API但不想用Java再写一遍的时候
2. 当你会了一套C语言下的类库但该类库没有Java版（或者你不想学）的时候

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
Java部分的语法就说完了，现在是C语言部分

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

值得注意的是jchar对应的是unsigned short类型而不是char。详情请搜索JVM字符类型的内部表示方式。

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
## JNI中的字符串与数组
字符串本质是一个字符数组，所以这里把他们放到一个小节中。
### JNIEnv类型
在这之前见看JNIEnv这个类型，见名可知，这个参数表示JNI所处的运行环境。在jni.h文件中对它的定义如下（C语言中|部分）：
```c
typedef const struct JNINativeInterface_ *JNIEnv;
struct JNINativeInterface_ {
    jint (JNICALL *GetVersion)(JNIEnv *env);
    ...
}
```
可见JNIEnv是一个指向JNINativeInterface_的指针，在这个结构体中有一个void*指针，这个是不需要我们关注的，其他的都是各种函数指针以供我们调用，颇有面向对象编程的风格，不过结构体终究还是结构体，需要把env作为函数的第一个变量传入进去。在C++中对JNIEnv的定义是一个类，所以不需要这样。

在生成的头文件中的函数声明上，第一个参数是JNIEnv*类型，所以通过这个变量调用API的一般形式为（C语言中）：
```c
(*env)->MethodName(env,...);
```
### 字符串
JNI中有jstring类型用于对于Java中的String类，其定义如下：
```c
struct _jobject;

typedef struct _jobject *jobject;
typedef jobject jstring;
//jstring是jobject的别名，因为Java中String类是一个对象，jobject是指向结构体_jobject的指针，而_jobject是一个不透明的结构体
```
当本地方法的参数或返回值中有String类型时，在头文件中对应的声明为jstring类型，不过可见jstring并没有对应c语言中的字符串类型也就是char*，所以不能像使用jint一样使用jstring，需要用一个函数来转换。

先看Java代码
```java
package com.scarike.jni.demo3;

public class Main {

    static{
        System.loadLibrary("demo3");
    }

    private native String jniString(String str);

    public static void main(String[] args) {
        String str=new Main().jniString("中文&English");
        System.out.println("JVM recv=> "+str);
    }
}

```
再看C语言代码，函数的说明在注释中给出
```c
#include <stdio.h>
#include <stdlib.h>
#include "com_scarike_jni_demo3_Main.h"

JNIEXPORT jstring JNICALL Java_com_scarike_jni_demo3_Main_jniString
  (JNIEnv* env, jobject jobj, jstring jstr){
      const char* str=(*env)->GetStringUTFChars(env,jstr,0);
      /*
        GetStringUTFChars函数用于将jstring类型转换为char*类型，使用UTF编码。
        参数：
            JNIEnv*：表示运行环境，固定写法（说明这个函数运行时需要获取JVM的一些信息，怎么获取不用关心）
            jstring：用于转换的jstring
            jboolean*：最后一个参数是isCopy，传入一个指向jboolean的指针（如果传入的不是0，在该方法中会改变这个jboolean值）
                如果返回的是JNI_TRUE：表示返回的字符串指针指向的是String类型的深拷贝，此时在C中改变值不会影响Java中字符串的值。
                如果返回的是JNI_FALSE：表示返回的字符串指针指向的就是String字符串，此时在C中改变字符串的值Java中也会改变。
                一般来说，不应该在C中改变这个字符串的值，这里我传入了0，他不会有任何变化。
        返回：一个const char*的字符串类型，如果失败返回NULL，所以非空检查是有必要的。

      */
      if(str==NULL){
          printf("str is NULL");
          return NULL;
      }
      printf("C recv=> %s\n",str);
      /*
        ReleaseStringUTFChars方法用于销毁内存，在上一步得到的字符串不再使用的时候应该调用，否则会造成内存泄露，即使是GC也不会挽救。
        参数：
            JNIEnv：固定写法
            jstring：java中的jstring对象
            const char*：上面的jstring转换得到的字符串指针
      */
      (*env)->ReleaseStringUTFChars(env,jstr,str);

      const char* newStr="C语言中定义的String!";
      /*
        NewStringUTF，根据已有的字符串，创建一个jstring对象
        参数：
            JNIEnv：固定写法
            const char*：已有的字符串指针
        返回：jstring对象，同样如果失败返回NULL；
      */
      jstring newJstr=(*env)->NewStringUTF(env,newStr);
      return newJstr;
  }
```
运行结果
```shell
> java -Djava.library.path=. com.scarike.jni.demo3.Main 
C recv=> 中文&English
JVM recv=> C语言中定义的String!
```
这三个函数是常用的接口，更多的可以看官方文档
### 数组
数组对应c语言中的jarray类型，还有更具体的jintArray,jdoubleArray等等，其定义如下：
```c
JNI技术现在已经用的不多了，Java已经有了一套庞大的生态体系，足够应付许多任务。JNI主要用于：

当你已经用C语言写了一套API但不想用Java再写一遍的时候

当你会了一套C语言下的类库但该类库没有Java版（或者你不想学）的时候

JNI浅层原理
JVM说叫虚拟机，其实就是一个普通的程序，他运行时还是得调用底层的动态链接库接着进一步调用操作系统接口。他能用Sun公司开发出来的库，也能用操作系统里面的库，自然也能调用你自己写的库。

Java中有System.loadLibrary(String)方法，可以加载动态连接库，比如：struct _jobject;

typedef struct _jobject *jobject;
typedef jobject jstring;
typedef jobject jarray;
typedef jarray jbooleanArray;
typedef jarray jbyteArray;
typedef jarray jcharArray;
typedef jarray jshortArray;
typedef jarray jintArray;
typedef jarray jlongArray;
typedef jarray jfloatArray;
typedef jarray jdoubleArray;
typedef jarray jobjectArray;
```
从定义来看其实和jstring一样的，只是他的具体实现不同（颇有多态那味了）

与jstring一样，不能直接使用，需要一个函数来转换。

在本案例中，传入一个int数组，返回一个double数组表示int数组的总和与平均值，在调用结束后再输出int数组，看看是否有变化。  
C语言代码中对传入的数组进行求和求平均，然后对其进行排序并输出。此部分所以输出的内容使用=======native=======包含起来以便区分。

java代码：
```java
package com.scarike.jni.demo4;

import java.util.Arrays;

public class Main {
    static{
        System.loadLibrary("demo4");
    }

    private native double[] jniArray(int[] intArray);

    public static void main(String[] args) {
        int[] intArray={4,2,5,7,8,9,0,3,1,6,7};
        double[] res=new Main().jniArray(intArray);
        System.out.println("sum="+res[0]);
        System.out.println("avg="+res[1]);
        System.out.println("Array=> "+Arrays.toString(intArray));
    }
}

```
c语言代码：
```c
#include <stdlib.h>
#include <stdio.h>
#include "com_scarike_jni_demo4_Main.h"

int comp(jint* o1,jint* o2){
    return *o1-*o2;
}

JNIEXPORT jdoubleArray JNICALL Java_com_scarike_jni_demo4_Main_jniArray
  (JNIEnv * env, jobject jobj, jintArray jints){
      printf("=============native===========\n");
      jboolean mark=JNI_FALSE;
      /*
        GetIntArrayElements：将jintArray对象转换为jint数组。
            参数：
                JNIEnv：固定写法
                jintArray：待转换的jintArray对象
                jboolean*：isCopy标记，其作用与String中的相同，一般情况下都会返回JNI_TRUE，这是JNI的保护策略。
            返回：jint*指针指向jint数组地址。失败返回NULL
      */
      jint* cintArray=(*env)->GetIntArrayElements(env,jints,&mark);
      if(cintArray==NULL){
          printf("array is NULL\n");
          return NULL;
      }
      printf("mark is %s\n",mark?"JNI_TRUE":"JNI_FALSE");
      /*
        GetArrayLength：获取数组长度
        参数：
            JNIEnv：固定写法
            jintArray：intArray对象
        返回：数组长度
      */
      int size=(*env)->GetArrayLength(env,jints);
      /*输出信息，遍历，排序，再输出排序结果*/
      printf("size is %d\n",size);
      double avg,sum=0;
      printf("recv array=> [");
      for(int i=0;i<size;++i){
          printf("%d%c",cintArray[i],",]"[i==size-1]);
          sum+=cintArray[i];
      }
      printf("\n");
      avg=sum/size;
      qsort(cintArray,size,sizeof(jint),comp);
      printf("sorted array=> [");
      for(int i=0;i<size;++i){
          printf("%d%c",cintArray[i],",]"[i==size-1]);
          sum+=cintArray[i];
      }
      printf("\n");
      /*
        ReleaseIntArrayElements：释放内存
        参数：
            JNIEnv：固定写法
            jintArray：intArray对象
            jint*：上面的intArray对象转换得到的jint数组
            int：释放的模式，对此有三种可选：
            （首先，isCopy为JNI_TRUE时，对jint数组的修改是不会同步到intArray对象的，因为jint数组是深拷贝过来的）
                0：释放内存，并将对jint数组的修改同步到intArray对象中，
                JNI_COMMIT：将对jint数组的修改同步到intArray对象中，但不会释放内存
                JNI_ABORT：释放内存，而且不会将修改同步到intArray对象中。
            可见当isCopy返回JNI_FALSE时，三种模式不会有什么效果。
        案例中最后一个参数传入了0，所以会有一种现象：isCopy为JNI_TRUE，但是Java中输出的是已排序后的结果。
      */
      (*env)->ReleaseIntArrayElements(env,jints,cintArray,0);
      
      jdouble res[2];
      res[0]=sum,res[1]=avg;
      //NewDoubleArray新建一个长度为2的jdoubleArray数组
      jdoubleArray jdoubles=(*env)->NewDoubleArray(env,2);
      if(jdoubles==NULL){
          printf("jdoubles is NULL\n");
          return NULL;
      }
      //SetDoubleArrayRegion将一个double数组拷贝到jdoubleArray对象中，从index为0开始拷贝2个。
      (*env)->SetDoubleArrayRegion(env,jdoubles,0,2,res);
      printf("=============native===========\n");
      return jdoubles;
  }

```
运行结果：
```shell
# 注，由于c代码中有两个函数（还有一个比较函数），gcc编译时加上-fPIC参数
> java -Djava.library.path=. com.scarike.jni.demo4.Main 
=============native===========
mark is JNI_TRUE
size is 11
recv array=> [4,2,5,7,8,9,0,3,1,6,7]
sorted array=> [0,1,2,3,4,5,6,7,7,8,9]
=============native===========
sum=104.0
avg=4.7272727272727275
Array=> [0, 1, 2, 3, 4, 5, 6, 7, 7, 8, 9]
# 可以看到此时对数组的修改同步了过来，如果使用JNI_ABORT，最后一行就会输出：Array=> [4, 2, 5, 7, 8, 9, 0, 3, 1, 6, 7]
```
还有其他的short数组，long数组以至Object数组等都有相应的转换函数，可参考帮助文档



## 对象类型

