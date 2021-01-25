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