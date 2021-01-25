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