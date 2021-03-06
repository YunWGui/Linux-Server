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