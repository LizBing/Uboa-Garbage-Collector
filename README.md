# Uboa-Garbage-Collector
A pauseless garbage collector in C.

## 都什么年代了，还有人用传统的malloc/free？
我不知道，但是我开发了一款无暂停的垃圾回收器，专门针对C/C++设计。

## 为什么要做这个？
单纯是对这门技术感兴趣，花自己空闲时间研究了八个月，做出来给众多C/C++同好开发使用。

## 为什么要用C做？
C是一门非常优秀的语言，它的语法简单，编译速度快，运行速度快，内存管理简单。

## 基本操作接口（v0.0.0dev2）

### 1. 垃圾回收器的初始化
void uboa_init(int nGCWorkers, size_t xmx, long timerInterval_min, bool enableRuleProactive, const char* logFilePath);

@nGCWorkers: 垃圾回收器的工作线程数。

@xmx: 垃圾回收器的最大堆内存，单位为字节。

@timerInterval_min: 垃圾回收器的定时器间隔，单位为分钟。值为0时表示不启用定时器。

@enableRuleProactive: 是否启用主动式回收。

@logFilePath: 日志文件的路径。

### 2. 安全区域
所有的内存操作都必须在安全区域内进行，否则会导致程序崩溃。

void uboa_enterSafeRegion();

void uboa_exitSafeRegion();

宏uboa_safeRegion()可以用来简化安全区域的使用。代码示例如下：

    uboa_safeRegion( /* 在这里进行内存操作 */ )

### 3. 应用线程注册以及引用类型
如果你想在应用线程中使用垃圾回收器，那么你必须先注册应用线程。如果你不能保证线程安全，那么每个线程都必须注册。此处的操作可以不在安全区域内进行。

AppThrdHandle uboa_createAppThrdHandle(void* stack);

@stack: 为引用类型提供栈空间。

返回值为应用线程的句柄，用于后续的内存操作。

void uboa_destroyAppThrdHandle(AppThrdHandle);

提供对应句柄，将应用线程从垃圾回收器中注销。

uboa_reference uboa_pushReference(AppThrdHandle);

将一个引用类型压入栈顶, 返回值为引用的句柄。

void uboa_popReferences(AppThrdHandle, size_t);

将栈顶的引用弹出指定的个数。

注销应用线程时，栈中剩余的引用会自动弹出。

### 4. 内存分配

void uboa_new_Oop(AppThrdHandle, uboa_reference, size_t, off_t*);

有一种特殊的对象OOP，用于描述对象内的引用位置。此函数就用于分配这种对象，并让其通过一个引用管理。本来吧我是想把OOP分出来让用户自己管理的，但是考虑到安全性以及方便性，我还是把它放在了垃圾回收器里。如果要用GC来配合反射的实现，这将会非常方便。

@AppThrdHandle: 应用线程的句柄。

@uboa_reference: 引用的句柄。

@off_t*: 一个元素为off_t类型对象的数组，用于描述对象内的引用位置，数组的长度由@size_t指定。将会被复制到OOP对象内。

void uboa_new(AppThrdHandle, uboa_reference, size_t, uboa_reference oopRef);

分配一个普通的对象，并让其通过一个引用管理。

@AppThrdHandle: 应用线程的句柄。

@uboa_reference: 引用的句柄。

@size_t: 对象的大小，单位为字节。

@oopRef: 一个引用，用于描述对象内的引用位置。如果不需要，可以传入'uboa_null()'。

void uboa_newArray(AppThrdHandle, uboa_reference, size_t, size_t count, uboa_reference oopRef);

分配一个数组，并让其通过一个引用管理。将会分配count个对象，每个对象的大小通过size_t传入。

@AppThrdHandle: 应用线程的句柄。

@uboa_reference: 引用的句柄。

@size_t: 对象的大小，单位为字节。

@count: 数组的长度。

### 5. 内存操作

在这个0.0.0dev1版本中，内存操作只提供了一些简单的必要的操作，后续版本会提供更多的操作。

uboa_reference uboa_null();

返回一个空引用。

void uboa_assign(uboa_reference dst, uboa_reference src);

将一个引用赋值给另一个引用。

bool uboa_isNull(uboa_reference);

判断一个引用是否为空。

intptr_t uboa_load(uboa_reference, off_t);

从引用的对象中加载一个64位数据。

void uboa_store(uboa_reference, off_t, intptr_t);

将一个64位数据存储到引用的对象中。

void uboa_loadReference(uboa_reference dst, uboa_reference src, off_t);

将引用的对象中的引用加载到栈中的一个有效引用。

@dst: 目标引用句柄。

@src: 对象引用句柄。

@off_t: 偏移量，表示对象内的引用的位置。

void uboa_storeReference(uboa_reference dst, off_t, uboa_reference src);

将栈中的一个有效引用存储到引用的对象中。

@dst: 对象引用句柄。

@off_t: 偏移量，表示对象内的引用的位置。

@src: 源引用句柄。
