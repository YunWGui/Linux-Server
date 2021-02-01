### 牛客 C++ 项目 —— [Linux 高并发服务器开发](https://www.nowcoder.com/courses/cover/live/504)

[TOC]

-----------

# 第1章 Linux 系统编程入门

## 1.19 文件描述符

![](https://yunwgui-image.oss-cn-beijing.aliyuncs.com/86f5978d9b/20210120211207.png)

## 1.20 open 打开文件

```cpp
int open(const char *pathname, int flags);
```

## 1.21 open 创建新文件

```cpp
int open(const char *pathname, int flags, mode_t mode);
```

```cpp
// create.c
/**
 * #include <sys/types.h>
 * #include <sys/stat.h>
 * #include <fcntl.h>
 * 
 * int open(const char *pathname, int flags, mode_t mode);
 *   参数：
 *      - pathname : 要创建的文件的路径
 *      - flags : 对文件的操作权限和其他的设置
 *        - 必选项：O_RDONLY, O_WRONLY, O_RDWR （三者之间互斥）
 *        - 可选项：O_CREAT 文件不存在时，创建新文件
 *        - flags 参数是一个 int 类型的数据，占 4 个字节，32 位
 *          flags 32 个位，每一位就是一个标志位
 *      - mode : 八进制的数，表示用户对创建出的新的文件的操作权限
 *        最终的权限是：mode & ~umask （umask 的作用是「抹去某些权限」）
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>  // close
#include <stdio.h>  // perror

int main()
{
    // 创建一个新文件
    int fd = open( "create.txt", O_RDWR | O_CREAT, 0777 );

    if ( fd == -1 ) {
        perror( "OPEN");
    }

    // 关闭
    close( fd );

    return 0;
}
```

## 1.22 read、write 函数

```cpp
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);
```

```cpp
/*******************************************************************************  
    #include <unistd.h>
    ssize_t read(int fd, void *buf, size_t count);
        参数：
            - fd：文件描述符，通过 open 得到的，通过这个文件描述符操作某个文件
            - buf：需要读取数据存放的地方，数组的地址（传出参数）
            - count：指定的数组的大小
        返回值：
            - 成功：
                >0: 返回实际的读取到的字节数
                =0：文件已经读取完了
            - 失败：-1 ，并且设置errno

    #include <unistd.h>
    ssize_t write(int fd, const void *buf, size_t count);
        参数：
            - fd：文件描述符，open得到的，通过这个文件描述符操作某个文件
            - buf：要往磁盘写入的数据，数据
            - count：要写的数据的实际的大小
        返回值：
            成功：实际写入的字节数
            失败：返回-1，并设置errno
*******************************************************************************/

#include <unistd.h>  // read, write
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main() {

    // 1.通过open打开english.txt文件
    int srcfd = open("english.txt", O_RDONLY);
    if(srcfd == -1) {
        perror("open");
        return -1;
    }

    // 2.创建一个新的文件（拷贝文件）
    int destfd = open("cpy.txt", O_WRONLY | O_CREAT, 0664);
    if(destfd == -1) {
        perror("open");
        return -1;
    }

    // 3.频繁的读写操作
    char buf[1024] = {0};
    int len = 0;
    while((len = read(srcfd, buf, sizeof(buf))) > 0) {
        write(destfd, buf, len);
    }

    // 4.关闭文件
    close(destfd);
    close(srcfd);


    return 0;
}
```

## 1.23 lseek 函数

```cpp
off_t lseek(int fd, off_t offset, int whence);
```

```cpp
/*******************************************************************************
 * 标准 C 库函数
 * #include <stdio.h>
 * int fseek( FILE* stream, long offset, int whence );
 * 
 * Linux 系统函数
 * #include <sys/types.h>
 * #include <unistd.h>
 * off_t lseek( int fd, off_t offset, int whence );
 *   参数：
 *      - fd : 文件描述符，通过 open 得到的，通过这个 fd 操作某个文件
 *      - offset : 偏移量
 *      - whence : 
 *          SEEK_SET : 设置文件指针的偏移量
 *          SEEK_CUR : 设置偏移量：当前位置 + 第二个参数 offset 的值
 *          SEEK_END : 设置偏移量：文件大小 + 第二个参数 offset 的值
 *   返回值：返回文件指针的位置
 * 
 * 作用：
 *   1. 移动文件指针到文件头
 *      lseek( fd, 0, SEEK_SET );
 *   2. 获取当前文件指针的位置
 *      lseek( fd, 0, SEEK_CUR );
 *   3. 获取文件长度
 *      lseek( fd, 0, SEEK_END );
 *   4. 拓展文件的长度，当前文件 10b -> 110b，增加了 100 个字节
 *      lseek( fd, 100, SEEK_END );  // 注意：需要写一次数据
*******************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>  // lseek
#include <fcntl.h>
#include <stdio.h>  // perror

int main()
{
    int fd = open( "hello.txt", O_RDWR );

    if ( fd == -1 ) {
        perror( "OPEN" );
        return -1;
    }

    // 扩展文件的长度
    int ret = lseek( fd, 100, SEEK_END );

    if ( ret == -1 ) {
        perror( "lseek" );
        return -1;
    }

    // 写入一个空数据
    write( fd, " ", 1 );

    // 关闭文件
    close( fd );

    return 0;
}
```

## 1.24 stat、lstat 函数

```cpp
int stat(const char *pathname, struct stat *statbuf);
int lstat(const char *pathname, struct stat *statbuf);

struct stat {
    dev_t st_dev; // 文件的设备编号
    ino_t st_ino; // 节点
    mode_t st_mode; // 文件的类型和存取的权限
    nlink_t st_nlink; // 连到该文件的硬连接数目
    uid_t st_uid; // 用户ID
    gid_t st_gid; // 组ID
    dev_t st_rdev; // 设备文件的设备编号
    off_t st_size; // 文件字节数(文件大小)
    blksize_t st_blksize; // 块大小
    blkcnt_t st_blocks; // 块数
    time_t st_atime; // 最后一次访问时间
    time_t st_mtime; // 最后一次修改时间
    time_t st_ctime; // 最后一次改变时间(指属性)
};
```

```cpp
/*******************************************************************************
 * #include <sys/types.h>
 * #include <sys/stat.h>
 * #include <unistd.h>
 * 
 * int stat( const char* pathname, struct stat* statbuf );
 *   作用：获取一个文件相关的一些信息
 *   参数；
 *     - pathname : 操作的文件的路径
 *     - statbuf : 结构体变量，传出参数，用于保存获取到的文件信息
 *   返回值：
 *     成功：返回 0
 *     失败：返回 -1，设置 errno
 * 
 * int lstat( const char* pathname, struct stat* statbuf );
 *   作用：存在软链接的情况下，直接获取软链接的 stat
 *   参数；
 *     - pathname : 操作的文件的路径
 *     - statbuf : 结构体变量，传出参数，用于保存获取到的文件信息
 *   返回值：
 *     成功：返回 0
 *     失败：返回 -1，设置 errno
*******************************************************************************/

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

int main()
{
    struct stat statbuf;

    int ret = stat( "a.txt", &statbuf );

    if ( ret == -1 ) {
        perror( "stat" );
        return -1;
    }

    printf("size: %ld\n", statbuf.st_size );

    return 0;
}
```

## 1.25 模拟实现 ls -l 命令

```cpp

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>

// 模拟实现 ls -l 指令
// -rw-rw-r-- 1 nowcoder nowcoder 12 12月  3 15:48 a.txt
int main(int argc, char * argv[]) {

    // 判断输入的参数是否正确
    if(argc < 2) {
        printf("%s filename\n", argv[0]);
        return -1;
    }

    // 通过stat函数获取用户传入的文件的信息
    struct stat st;
    int ret = stat(argv[1], &st);
    if(ret == -1) {
        perror("stat");
        return -1;
    }

    // 获取文件类型和文件权限
    char perms[11] = {0};   // 用于保存文件类型和文件权限的字符串

    switch(st.st_mode & S_IFMT) {
        case S_IFLNK:
            perms[0] = 'l';
            break;
        case S_IFDIR:
            perms[0] = 'd';
            break;
        case S_IFREG:
            perms[0] = '-';
            break; 
        case S_IFBLK:
            perms[0] = 'b';
            break; 
        case S_IFCHR:
            perms[0] = 'c';
            break; 
        case S_IFSOCK:
            perms[0] = 's';
            break;
        case S_IFIFO:
            perms[0] = 'p';
            break;
        default:
            perms[0] = '?';
            break;
    }

    // 判断文件的访问权限

    // 文件所有者
    perms[1] = (st.st_mode & S_IRUSR) ? 'r' : '-';
    perms[2] = (st.st_mode & S_IWUSR) ? 'w' : '-';
    perms[3] = (st.st_mode & S_IXUSR) ? 'x' : '-';

    // 文件所在组
    perms[4] = (st.st_mode & S_IRGRP) ? 'r' : '-';
    perms[5] = (st.st_mode & S_IWGRP) ? 'w' : '-';
    perms[6] = (st.st_mode & S_IXGRP) ? 'x' : '-';

    // 其他人
    perms[7] = (st.st_mode & S_IROTH) ? 'r' : '-';
    perms[8] = (st.st_mode & S_IWOTH) ? 'w' : '-';
    perms[9] = (st.st_mode & S_IXOTH) ? 'x' : '-';

    // 硬连接数
    int linkNum = st.st_nlink;

    // 文件所有者
    char * fileUser = getpwuid(st.st_uid)->pw_name;

    // 文件所在组
    char * fileGrp = getgrgid(st.st_gid)->gr_name;

    // 文件大小
    long int fileSize = st.st_size;

    // 获取修改的时间
    char * time = ctime(&st.st_mtime);

    char mtime[512] = {0};
    strncpy(mtime, time, strlen(time) - 1);

    char buf[1024];
    sprintf(buf, "%s %d %s %s %ld %s %s", perms, linkNum, fileUser, fileGrp, fileSize, mtime, argv[1]);

    printf("%s\n", buf);

    return 0;
}
```

## 1.26 文件属性操作函数

```cpp
int access(const char *pathname, int mode);  // 判断文件的权限
int chmod(const char *filename, int mode);  // 修改文件的权限
int chown(const char *path, uid_t owner, gid_t group);
int truncate(const char *path, off_t length);  // 增减文件大小
```

```cpp
/*******************************************************************************
 * #include <unistd.h>
 * int access(const char *pathname, int mode);
 *   作用：判断某个文件是否有某个权限，或者判断文件是否存在
 *   参数：
 *     - pathname : 判断文件路径
 *     - mode : 
 *         R_OK : 判断是否有读权限
 *         W_OK : 判断是否有写权限
 *         X_OK : 判断是否有执行权限
 *         F_OK : 判断文件是否存在
 *   返回值：成功返回 0，失败返回 -1
*******************************************************************************/

#include <stdio.h>
#include <unistd.h>

int main()
{
    int ret = access( "a.txt", F_OK );
    if ( ret == -1 ) {
        perror( "access" );
    }

    printf( "文件存在!! \n" );


    return 0;
}
```

```cpp
/*******************************************************************************
 * #include <unistd.h>
 * #include <sys/types.h>
 * int truncate( const char* path, off_t length );
 *   作用：缩减或扩展文件的尺寸到指定的大小
 *   参数：
 *     - path : 需要修改的文件的路径
 *     - length : 需要最终文件变成的大小
 *   返回值：成功返回 0，失败返回 -1
*******************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main()
{
    int ret = truncate( "b.txt", 5 );
    if ( ret == -1 ) {
        perror( "Truncate\n" );
        return -1;
    }

    return 0;
}
```

## 1.27 目录操作函数

```cpp
int mkdir(const char *pathname, mode_t mode);  // 创建目录
int rmdir(const char *pathname);  // 删除空目录

int rename(const char *oldpath, const char *newpath);  // 重命名
int chdir(const char *path);  // 改变当前路径
char *getcwd(char *buf, size_t size);  // 获取当前路径
```

```cpp
// mkdir.c
/*******************************************************************************
 * #include <sys/stat.h>
 * #include <sys/types.h>
 * int mkdir( const char* pathname, mode_t mode );
 *   作用：创建一个目录
 *   参数：
 *     - pathname : 创建 的目录的路径
 *     - mode : 权限(八进行的数)
 *   返回值：成功返回 0，失败返回 -1
*******************************************************************************/

#include <stdio.h>  // perror
#include <sys/stat.h>
#include <sys/types.h>

int main()
{
    int ret = mkdir( "aaa", 0777 );

    if ( ret == -1 ) {
        perror( "Mkdir" );
        return -1;
    }

    return 0;
}
```

```cpp
// rename.c 
/*******************************************************************************
 * #include <stdio.h>
 * int rename(const char *oldpath, const char *newpath);
*******************************************************************************/

#include <stdio.h>

int main()
{
    int ret = rename( "aaa", "bbb" );

    if ( ret == -1 ) {
        perror( "Rename" );
        return -1;
    }

    return 0;
}
```

```cpp
// chdir.c
/*******************************************************************************
    #include <unistd.h>
    int chdir(const char *path);
        作用：修改进程的工作目录
            比如在/home/nowcoder 启动了一个可执行程序a.out, 进程的工作目录 /home/nowcoder
        参数：
            path : 需要修改的工作目录

    #include <unistd.h>
    char *getcwd(char *buf, size_t size);
        作用：获取当前工作目录
        参数：
            - buf : 存储的路径，指向的是一个数组（传出参数）
            - size: 数组的大小
        返回值：
            返回的指向的一块内存，这个数据就是第一个参数
*******************************************************************************/

#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

int main() {

    // 获取当前的工作目录
    char buf[128];
    getcwd(buf, sizeof(buf));
    printf("当前的工作目录是：%s\n", buf);

    // 修改工作目录
    int ret = chdir("/home/nowcoder/Linux/lesson13");
    if(ret == -1) {
        perror("chdir");
        return -1;
    } 

    // 创建一个新的文件
    int fd = open("chdir.txt", O_CREAT | O_RDWR, 0664);
    if(fd == -1) {
        perror("open");
        return -1;
    }

    close(fd);

    // 获取当前的工作目录
    char buf1[128];
    getcwd(buf1, sizeof(buf1));
    printf("当前的工作目录是：%s\n", buf1);
    
    return 0;
}
```

## 1.28 目录遍历函数

```cpp
DIR *opendir(const char *name);  // 打开目录
struct dirent *readdir(DIR *dirp);
int closedir(DIR *dirp);  // 关闭目录
```

## 1.29 dup、dup2 函数

```cpp
int dup(int oldfd);  // 复制文件描述符
int dup2(int oldfd, int newfd);  // 重定向文件描述符
```

## 1.30 fcntl 函数

```cpp
int fcntl(int fd, int cmd, ... /* arg */ );
// 作用：
//   1. 复制文件描述符
//   2. 设置/获取文件的状态标志
```

# 第2章 Linux 多进程开发

## 2.1 进程概述

进程是正在运行的程序的实例

## 2.2 进程的状态转换

## 2.3 进程创建

```cpp
/*******************************************************************************
pid_t fork(void)
  函数的作用：用于创建子进程
  返回值：
    fork() 的返回值会返回两次。一次是在父进程中，一次是在子进程中。
    在父进程中返回创建的子进程的ID
    在子进程中返回0
    如何区分父进程和子进程：通过 fork 的返回值
    在父进程中返回 -1，表示创建子进程失败，并且设置 errno ( error number )
*******************************************************************************/
```

## 2.4 父子进程虚拟地址空间情况

## 2.5 父子进程关系及GDB多进程调试

## 2.6 exec 函数族

```c
int execl(const char *path, const char *arg, .../* (char *) NULL */);
int execlp(const char *file, const char *arg, ... /* (char *) NULL */);
int execle(const char *path, const char *arg, .../*, (char *) NULL, char * const envp[] */);
int execv(const char *path, char *const argv[]);
int execvp(const char *file, char *const argv[]);
int execvpe(const char *file, char *const argv[], char *const envp[]);
int execve(const char *filename, char *const argv[], char *const envp[]);
```

- l(list) 参数地址列表，以空指针结尾
- v(vector) 存有各参数地址的指针数组的地址
- p(path) 按 PATH 环境变量指定的目录搜索可执行文件
- e(environment) 存有环境变量字符串地址的指针数组的地址

# 第 3 章 Linux 多线程开发

## 3.1 线程概述

查看指定的进程的 LWP 号：

```shell
ps -LF pid
```

## 3.2 创建线程

```cpp
#include <pthread.h>

int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*start_routine) (void *), void *arg);
```

```cpp
// pthread_create.c
/*******************************************************************************
 * 一般情况下，main 函数所在的线程，我们称之为主线程 ( main 线程)，其余创建的线程称
 * 为子线程。
 * 程序中默认只有一个进程，fork() 函数调用，-> 2 个进程
 * 程序中默认只有一个线程，pthread_create() 函数调用 -> 2 个线程
 * 
 * #include <pthread.h>
 * int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
 *                    void *(*start_routine) (void *), void *arg);
 * - 功能：创建一个子线程
 * - 参数：
 *   - thread : 传出参数。线程创建成功后，子线程的线程 ID 被写到该变量中。
 *   - attr : 设置线程的属性，一般使用默认值，NULL
 *   - start_routine : 函数指针，这个函数是子线程需要处理的逻辑代码
 *   - arg : 给第三个参数使用，传参
 * - 返回值：
 *     成功：返回 0
 *     失败：返回错误号。这个错误号和之前的 errno 不太一样
 *           获取错误号的信息，char* strerror( int errnum );
*******************************************************************************/

// 线程库不是标准库，是第三方库，因此需要用额外的参数引入库
// gcc pthread_create.c -o create -pthread
// gcc pthread_create.c -o create -lpthread

#include <stdio.h>
#include <pthread.h>  // pthread_create
#include <string.h>
#include <unistd.h>  // sleep

void* callback( void* arg ) {
    printf( "child thread...\n" );
    printf("arg value: %d\n", *(int*) arg );
    return NULL;
}


int main()
{
    pthread_t tid;  // 线程 ID
    
    int num = 10;
     
    // 创建一个子线程
    int ret = pthread_create( &tid, NULL, callback, (void*)&num );

    if ( ret != 0 ) {
        char* errstr = strerror( ret );
        printf( "error : %s\n", errstr );
    }

    for ( int i = 0; i < 5; ++i ) {
        printf( "%d\n", i );
    }

    sleep( 1 );

    return 0;
}
```

## 3.3 终止线程

```cpp
/*******************************************************************************
 * #include <pthread.h>
 * void pthread_exit(void *retval);
 * - 功能：终止一个线程，在哪个线程中调用，就表示终止哪个线程
 * - 参数：
 *     retval : 需要传递一个指针，作为一个返回值，可以在 pthread_join() 中获取到
 * 
 * #include <pthread.h>
 * pthread_t pthread_self(void);
 * - 功能：获取当前的线程的线程ID
 *    
 * #include <pthread.h>
 * int pthread_equal(pthread_t t1, pthread_t t2); 
 * - 功能：比较两个线程 ID 是否相等
 *         不同的操作系统，pthread_t 类型实现不一样，有的是无符号的长整型，有的是
 *         是用结构体去实现的。
*******************************************************************************/

#include <stdio.h>
#include <pthread.h>
#include <string.h>  // strerror
#include <unistd.h>  // sleep

void* callback( void* arg ) {
    // 打印子线程 ID
    printf( "child thread id : %ld\n", pthread_self() );
    return NULL;
}

int main()
{
    // 创建一个子线程
    pthread_t tid;
    int ret = pthread_create( &tid, NULL, callback, NULL );

    if ( ret != 0 ) {
        char* errstr = strerror( ret );
        printf( "error : %s \n", errstr );
    }

    for ( int i = 0; i < 5; ++i ) {
        printf( "%d\n", i );
    }

    // sleep( 1 );

    printf( "tid : %ld, main thread id : %ld\n", tid, pthread_self() );

    // 让主线程退出，当主线程退出时，不会影响其他正常运行的线程
    pthread_exit( NULL );

    printf( "main thread exit.\n" );

    return 0;
}
```

## 3.4 连接已终止的线程

```cpp
// pthread_join.c
/*******************************************************************************
 * #include <pthread.h>
 * int pthread_join(pthread_t thread, void **retval);
 * - 功能：和一个已终止的线程进行连接
 *         回收子线程的资源
 *         这个函数是阻塞函数，调用一次只能回收一个子线程
 *         一般在主线程中使用
 * - 参数：
 *   - thread : 需要回收的子线程的 ID
 *   - retval : 接收子线程退出时的返回值
 * - 返回值：
 *     成功：返回 0
 *     失败，非 0，返回错误号
*******************************************************************************/

#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

int value = 10;

void * callback(void * arg) {
    printf("child thread id : %ld\n", pthread_self());
    // sleep(3);
    // return NULL; 
    // int value = 10; // 局部变量
    pthread_exit((void *)&value);   // return (void *)&value;
} 

int main() {

    // 创建一个子线程
    pthread_t tid;
    int ret = pthread_create(&tid, NULL, callback, NULL);

    if(ret != 0) {
        char * errstr = strerror(ret);
        printf("error : %s\n", errstr);
    }

    // 主线程
    for(int i = 0; i < 5; i++) {
        printf("%d\n", i);
    }

    printf("tid : %ld, main thread id : %ld\n", tid ,pthread_self());

    // 主线程调用pthread_join()回收子线程的资源
    int * thread_retval;
    ret = pthread_join(tid, (void **)&thread_retval);

    if(ret != 0) {
        char * errstr = strerror(ret);
        printf("error : %s\n", errstr);
    }

    printf("exit data : %d\n", *thread_retval);

    printf("回收子线程资源成功！\n");

    // 让主线程退出,当主线程退出时，不会影响其他正常运行的线程。
    pthread_exit(NULL);

    return 0; 
}
```

## 3.5 线程的分离

```cpp
// pthread_detach.c
/*******************************************************************************
 * #include <pthread.h>
 * int pthread_detach(pthread_t thread);
 * - 功能：分离一个线程。被分离的线程在终止的时候，会自动释放资源返回给系统。
 *         1. 不能多次分离，会产生不可预料的行为
 *         2. 不能去连接一个已经分离的线程，因为会报错。
 * - 参数：需要分离的线程的 ID
 * - 返回值：
 *     成功：0
 *     失败：返回错误号
*******************************************************************************/

#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

void * callback(void * arg) {
    printf("chid thread id : %ld\n", pthread_self());
    return NULL;
}

int main() {

    // 创建一个子线程
    pthread_t tid;

    int ret = pthread_create(&tid, NULL, callback, NULL);
    if(ret != 0) {
        char * errstr = strerror(ret);
        printf("error1 : %s\n", errstr);
    }

    // 输出主线程和子线程的id
    printf("tid : %ld, main thread id : %ld\n", tid, pthread_self());

    // 设置子线程分离,子线程分离后，子线程结束时对应的资源就不需要主线程释放
    ret = pthread_detach(tid);
    if(ret != 0) {
        char * errstr = strerror(ret);
        printf("error2 : %s\n", errstr);
    }

    // 设置分离后，对分离的子线程进行连接 pthread_join()
    // ret = pthread_join(tid, NULL);
    // if(ret != 0) {
    //     char * errstr = strerror(ret);
    //     printf("error3 : %s\n", errstr);
    // }

    pthread_exit(NULL);

    return 0;
}
```

## 3.6 线程取消

```cpp
/*******************************************************************************
 * #include <pthread.h>
 * int pthread_cancel(pthread_t thread);
 * - 功能：取消线程（让线程终止）
 *     取消某个线程，可以终止某个线程的运行，
 *     但是并不是立马终止，而是当子线程执行到一个取消点，线程才会终止。
 *     取消点：系统规定好的一些系统调用。我们可以粗略的理解为从用户区到内核区的切换，
 *             这个位置称之为取消点。
*******************************************************************************/

#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

void * callback(void * arg) {
    printf("chid thread id : %ld\n", pthread_self());
    for(int i = 0; i < 5; i++) {
        printf("child : %d\n", i);
    }
    return NULL;
}

int main() {
    
    // 创建一个子线程
    pthread_t tid;

    int ret = pthread_create(&tid, NULL, callback, NULL);
    if(ret != 0) {
        char * errstr = strerror(ret);
        printf("error1 : %s\n", errstr);
    }

    // 取消线程
    pthread_cancel(tid);

    for(int i = 0; i < 5; i++) {
        printf("%d\n", i);
    }

    // 输出主线程和子线程的id
    printf("tid : %ld, main thread id : %ld\n", tid, pthread_self());
    
    pthread_exit(NULL);

    return 0;
}
```

## 3.7 线程属性

```cpp
/*******************************************************************************
 * int pthread_attr_init(pthread_attr_t *attr);
 * - 初始化线程变量属性
 * int pthread_attr_destroy(pthread_attr_t *attr);
 * - 释放线程属性的资源
 * int pthread_attr_getdetachstate(const pthread_attr_t *attr, int detachstate);
 * - 获取线程分离的状态属性
 * int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);
 * - 设置线程分离的状态属性
*******************************************************************************/

#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

void * callback(void * arg) {
    printf("chid thread id : %ld\n", pthread_self());
    return NULL;
}

int main() {

    // 创建一个线程属性变量
    pthread_attr_t attr;

    // 初始化属性变量
    pthread_attr_init(&attr);

    // 设置属性
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    // 创建一个子线程
    pthread_t tid;

    int ret = pthread_create(&tid, &attr, callback, NULL);
    if(ret != 0) {
        char * errstr = strerror(ret);
        printf("error1 : %s\n", errstr);
    }

    // 获取线程的栈的大小
    size_t size;
    pthread_attr_getstacksize(& attr, &size);
    printf("thread stack size : %ld\n", size);

    // 输出主线程和子线程的id
    printf("tid : %ld, main thread id : %ld\n", tid, pthread_self());

    // 释放线程属性资源
    pthread_attr_destroy(&attr);

    pthread_exit(NULL);

    return 0;
}
```

## 3.8 线程同步

```cpp
/*******************************************************************************
 * 使用多线程实现买票的案例
 * 有 3 个窗口，一共 100 张票。
*******************************************************************************/

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>  // usleep

// 全局变量，所有的线程都共享这一份资源。
int tickets = 100;  

void* sell_ticket( void* arg ) {
    // 卖票
    while ( tickets > 0 ) {
        usleep( 8000 );  // 睡眠 X 微秒
        printf( "%ld 正在卖第 %d 张门票\n", pthread_self(), tickets );
        --tickets;
    }
    return NULL;
}

int main()
{
    // 创建三个子线程
    pthread_t tid1, tid2, tid3;  // 线程一、二、三
    pthread_create( &tid1, NULL, sell_ticket, NULL );
    pthread_create( &tid2, NULL, sell_ticket, NULL );
    pthread_create( &tid3, NULL, sell_ticket, NULL );

    // 回收子线程的资源
    pthread_join( tid1, NULL );
    pthread_join( tid2, NULL );
    pthread_join( tid3, NULL );

    // 设置线程分离
    pthread_detach( tid1 );
    pthread_detach( tid2 );
    pthread_detach( tid3 );

    pthread_exit( NULL );  // 退出主线程

    return 0;
}
```

## 3.9 互斥锁

```cpp
// mutex.c
/*******************************************************************************
 * 互斥量的类型 pthread_mutex_t
 * int pthread_mutex_init(pthread_mutex_t *restrict mutex, 
 *                        const pthread_mutexattr_t *restrict attr);
 *   - 初始化互斥量
 *   - 参数：
 *       - mutex : 需要初始化的互斥量变量
 *       - attr : 互斥量相关的属性，NULL
 *   - restrict : C 语言的修饰符。被修饰的指针，不能由另外的一个指针进行操作。
 * int pthread_mutex_destroy(pthread_mutex_t *mutex);
 *   - 释放互斥量的资源
 * int pthread_mutex_lock(pthread_mutex_t *mutex);
 *   - 加锁，阻塞的。如果有一个线程加锁了，那么其他的线程只能阻塞等待
 * int pthread_mutex_trylock(pthread_mutex_t *mutex);
 *   - 尝试加锁，如果加锁失败，不会阻塞，会直接返回。
 * int pthread_mutex_unlock(pthread_mutex_t *mutex);
 *   - 解锁。
*******************************************************************************/

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>  // usleep

// 全局变量，所有的线程都共享这一份资源。
int tickets = 1000;  

// 创建一个互斥量
pthread_mutex_t mutex;

void* sell_ticket( void* arg ) {

    // 卖票
    while ( 1 ) {
        pthread_mutex_lock( &mutex );  // 加锁

        if ( tickets > 0 ) {
            usleep( 60000 );  // 睡眠 X 微秒
            printf( "%ld 正在卖第 %d 张门票\n", pthread_self(), tickets );
            --tickets;
        }
        else {
            pthread_mutex_unlock( &mutex );  // 解锁
            break;
        }

        pthread_mutex_unlock( &mutex );  // 解锁
    }
    

    return NULL;
}

int main()
{
    // 初始化互斥量
    pthread_mutex_init( &mutex, NULL );

    // 创建三个子线程
    pthread_t tid1, tid2, tid3;  // 线程一、二、三
    pthread_create( &tid1, NULL, sell_ticket, NULL );
    pthread_create( &tid2, NULL, sell_ticket, NULL );
    pthread_create( &tid3, NULL, sell_ticket, NULL );

    // 回收子线程的资源
    pthread_join( tid1, NULL );
    pthread_join( tid2, NULL );
    pthread_join( tid3, NULL );

    // 设置线程分离
    pthread_detach( tid1 );
    pthread_detach( tid2 );
    pthread_detach( tid3 );

    pthread_exit( NULL );  // 退出主线程

    // 释放互斥量资源
    pthread_mutex_destroy( &mutex );

    return 0;
}
```

## 3.10 死锁

有时，一个线程需要同时访问两个或更多不同的共享资源，而每个资源又都由不同的互斥量管理。当超过一个线程加锁同一组互斥量时，就有可能发生死锁。

两个或两个以上的进程在执行过程中，因争夺共享资源而造成的一种互相等待的现象，若无外力作用，它们都将无法推进下去。此时称系统处于死锁状态或系统产生了死锁。

死锁的几种场景：

- 忘记释放锁
- 重复加锁
- 多线程加锁，抢占锁资源

```cpp
// dead_lock.c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>  // usleep

// 全局变量，所有的线程都共享这一份资源。
int tickets = 1000;  

// 创建一个互斥量
pthread_mutex_t mutex;

void* sell_ticket( void* arg ) {

    // 卖票
    while ( 1 ) {
        pthread_mutex_lock( &mutex );  // 加锁
        pthread_mutex_lock( &mutex );  // 加锁

        if ( tickets > 0 ) {
            usleep( 60000 );  // 睡眠 X 微秒
            printf( "%ld 正在卖第 %d 张门票\n", pthread_self(), tickets );
            --tickets;
        }
        else {
            pthread_mutex_unlock( &mutex );  // 解锁
            break;
        }

        pthread_mutex_unlock( &mutex );  // 解锁
    }
    

    return NULL;
}

int main()
{
    // 初始化互斥量
    pthread_mutex_init( &mutex, NULL );

    // 创建三个子线程
    pthread_t tid1, tid2, tid3;  // 线程一、二、三
    pthread_create( &tid1, NULL, sell_ticket, NULL );
    pthread_create( &tid2, NULL, sell_ticket, NULL );
    pthread_create( &tid3, NULL, sell_ticket, NULL );

    // 回收子线程的资源
    pthread_join( tid1, NULL );
    pthread_join( tid2, NULL );
    pthread_join( tid3, NULL );

    // 设置线程分离
    pthread_detach( tid1 );
    pthread_detach( tid2 );
    pthread_detach( tid3 );

    pthread_exit( NULL );  // 退出主线程

    // 释放互斥量资源
    pthread_mutex_destroy( &mutex );

    return 0;
}
```

```cpp
// dead_lock1.c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// 创建 2 个互斥量
pthread_mutex_t mutex1, mutex2;

void* workA( void* arg ) {
    pthread_mutex_lock( &mutex1 );
    sleep( 1 );
    pthread_mutex_lock( &mutex2 );

    printf( "workA...\n" );
    pthread_mutex_unlock( &mutex2 );
    pthread_mutex_unlock( &mutex1 );

    return NULL;
}

void* workB( void* arg ) {
    pthread_mutex_lock( &mutex2 );
    sleep( 1 );
    pthread_mutex_lock( &mutex1 );

    printf( "workB...\n" );
    pthread_mutex_unlock( &mutex1 );
    pthread_mutex_unlock( &mutex2 );

    return NULL;
}

int main()
{
    // 初始化互斥量
    pthread_mutex_init( & mutex1, NULL );
    pthread_mutex_init( & mutex2, NULL );

    // 创建 2 个子线程
    pthread_t tid1, tid2;
    pthread_create( &tid1, NULL, workA, NULL );
    pthread_create( &tid2, NULL, workB, NULL );

    // 回收子线程资源
    pthread_join( tid1, NULL );
    pthread_join( tid2, NULL );

    // 释放互斥量资源
    pthread_mutex_destroy( &mutex1 );
    pthread_mutex_destroy( &mutex2 );

    return 0;
}
```

## 3.11 读写锁

在对数据的读写操作中，更多的是读操作，写操作较少。例如对数据库数据的读写应用。为了满足当前能够允许多个读出，但只允许一个写入的需求，线程提供了读写锁来实现。

读写锁的特点：

- 如果有其他线程读数据，则允许其它线程执行读操作，但不允许写操作
- 如果有其它线程写数据，则其它线程都不允许读、写操作
- 写是独占的，写的优先级高

```cpp
// rwlock.c
/*******************************************************************************
 * 读写锁类型：pthread_rwlock_t
 * int pthread_rwlock_init(pthread_rwlock_t *restrict rwlock, 
 *                         const pthread_rwlockattr_t *restrict attr);
 * int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);
 * int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
 * int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);
 * int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);
 * int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock);
 * int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);
 * 
 * 案例：8 个线程操作同一个全局变量
 * 3 个线程不定时写这个全局变量，5 个线程不定时读这个全局变量
*******************************************************************************/

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// 创建一个共享数据
int num = 1;
// pthread_mutex_t mutex;  // 创建互斥锁
pthread_rwlock_t rwlock;  // 创建读写锁

void * writeNum(void * arg) {
    while(1) {
        pthread_rwlock_wrlock(&rwlock);
        num++;
        printf("++write, tid : %ld, num : %d\n", pthread_self(), num);
        pthread_rwlock_unlock(&rwlock);
        usleep(100);
    }

    return NULL;
}

void * readNum(void * arg) {
    while(1) {
        pthread_rwlock_rdlock(&rwlock);
        printf("===read, tid : %ld, num : %d\n", pthread_self(), num);
        pthread_rwlock_unlock(&rwlock);
        usleep(100);
    }

    return NULL;
}

int main() {

   pthread_rwlock_init(&rwlock, NULL);

    // 创建3个写线程，5个读线程
    pthread_t wtids[3], rtids[5];
    for(int i = 0; i < 3; i++) {
        pthread_create(&wtids[i], NULL, writeNum, NULL);
    }

    for(int i = 0; i < 5; i++) {
        pthread_create(&rtids[i], NULL, readNum, NULL);
    }

    // 设置线程分离
    for(int i = 0; i < 3; i++) {
       pthread_detach(wtids[i]);
    }

    for(int i = 0; i < 5; i++) {
         pthread_detach(rtids[i]);
    }

    pthread_exit(NULL);

    pthread_rwlock_destroy(&rwlock);

    return 0;
}
```

## 3.12 生产者和消费者模型

```cpp
/*******************************************************************************
 * 生产者 - 消费者模型（粗略版）
*******************************************************************************/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>  // malloc
#include <unistd.h>

// 创建一个互斥量
pthread_mutex_t mutex;

struct Node{
    int num;
    struct Node *next;
};

// 头结点
struct Node * head = NULL;

void * producer(void * arg) {
    // 不断的创建新的节点，添加到链表中
    while(1) {
        pthread_mutex_lock(&mutex);
        struct Node * newNode = (struct Node *)malloc(sizeof(struct Node));
        
        newNode->next = head;
        head = newNode;
        newNode->num = rand() % 1000;
        
        printf("add node, num : %d, tid : %ld\n", newNode->num, pthread_self());
        pthread_mutex_unlock(&mutex);
        usleep(100);
    }

    return NULL;
}

void * customer(void * arg) {

    while(1) {
        pthread_mutex_lock(&mutex);
        // 保存头结点的指针
        struct Node * tmp = head;

        // 判断是否有数据
        if(head != NULL) {
            // 有数据
            head = head->next;
            printf("del node, num : %d, tid : %ld\n", tmp->num, pthread_self());
            free(tmp);
            pthread_mutex_unlock(&mutex);
            usleep(100);
        } else {
            // 没有数据
            pthread_mutex_unlock(&mutex);
        }
    }
    return  NULL;
}

int main() {

    pthread_mutex_init(&mutex, NULL);

    // 创建5个生产者线程，和5个消费者线程
    pthread_t ptids[5], ctids[5];

    for(int i = 0; i < 5; i++) {
        pthread_create(&ptids[i], NULL, producer, NULL);
        pthread_create(&ctids[i], NULL, customer, NULL);
    }

    // 线程分离 
    for(int i = 0; i < 5; i++) {
        pthread_detach(ptids[i]);
        pthread_detach(ctids[i]);
    }

    while(1) {
        sleep(10);
    }

    pthread_mutex_destroy(&mutex);

    pthread_exit(NULL);

    return 0;
}
```





