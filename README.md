### 牛客 C++ 项目 —— [Linux 高并发服务器开发](https://www.nowcoder.com/courses/cover/live/504)

[toc]

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