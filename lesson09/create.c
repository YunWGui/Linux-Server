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