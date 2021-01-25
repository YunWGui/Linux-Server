/*******************************************************************************
 * #include <sys/stat.h>
 * int chmod( const char* pathname, mode_t mode );
 *   作用：修改文件的权限
 *   参数：
 *     - pathname : 需要修改的文件的路径
 *     - mode : 需要修改的权限值，八进制的数
 *   返回值：成功返回 0，失败返回 -1
*******************************************************************************/

#include <stdio.h>
#include <sys/stat.h>

int main()
{
    int ret = chmod( "a.txt", 0777 );

    if ( ret == -1 ) {
        perror( "CHMOD\n" );
        return -1;
    }

    return 0;
}