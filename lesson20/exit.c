/*******************************************************************************
 *  #include <stdlib.h>
 *  void exit( int status );
 * 
 *  #include <unistd.h>
 *  void _exit( int status );
 * 
 *  status 参数：是进程退出时的一个状态信息。父进程
 * 
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    printf("hello\n");
    printf("world");

    // exit( 0 );
    _exit( 0 );

    return 0;
}