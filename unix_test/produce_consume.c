/**
 * @describe: 测试单消费者，多生产者模型
 * @compile: gcc 01_test.c -o 01_test -lpthread
 * 
*/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define MAXNITEMS     1000000
#define MAXNTHREADS   100
 
int nitems = 0;

#ifdef __GNUC__

/**
 *  @describe:
 *  1. 如果使用 #define MIN(a, b) ((a) <= (b) ? (a) : (b))可能会导致一些问题
 *  如 MIN(i++, j++) 展开后为 ((i++)) <= (j++) ? (i++): (j++))，这样无论i和j
 *  谁小，都会导致i,j自增两次，因此就采用__GUNC__对c的扩展，它允许再一个表达式中执行
 *  多个语句。并将最后一个语句的值返回 : ({statement1 .. statementn; expresion})
 *  2.所以 #define MIN(a, b) ((a) <= (b) ? (a) : (b))就转变为：
 *  #define MIN(a,b) ({
 *      int _a = a; \
 *      int _b = b; \
 *      _a < _b ? _a : _b; \
 *   })
 *   但是这样就只能比较一种类型了，为了比较多种类型，就只能修改为
 *   #define MIN(type, a,b) ({
 *      type _a = a; \
 *      type _b = b; \
 *      _a < _b ? _a : _b; \
 *   })
 *   
 *  3. __GUN__扩展了一个关键字，可以获取数据的类型，即typeof,因此上面就可以修改为
 *   #define MIN(a,b) ({
 *      typeof(a) _a = a; \
 *      typeof(b) _b = b; \
 *      _a < _b ? _a : _b; \
 *   })
 *  4. (void)(&_x == &_y)作用:
 *  1) 如果两个数据进行比较运算的值没有使用，在一些编译器中会警告，但是这个警告是可以
 *      忽略的，使用(void)就可以忽略这种警告 
 *  2) 用来进行检查，如果两个变量的类型不一致，在比较时会给出一个警告以此告诉使用者
*/
#define MIN(x, y)    ({\
        typeof(x) _x = x;        \
        typeof(y) _y = y;        \
        (void)(&_x == &_y);      \
        _x < _y ? _x:_y;         \
})
#endif

extern void *produce(void *arg);
extern void *consume(void *arg);

struct {
    pthread_mutex_t   mutex;
    int               buff[MAXNITEMS];
    int               nindex;
    int               nval;

} shared = {
    //静态分配锁, 如果动态分配锁(如malloc)，则使用pthread_mutex_init()
    .mutex  = PTHREAD_MUTEX_INITIALIZER, 
    .nindex = 0,
    .nval   = 0,
};

int main(int argc, char *argv[]) 
{
    int ret = 0;

    int i = 0, nthreads = 0, count[MAXNTHREADS];  
    pthread_t  produce_pid[MAXNTHREADS], consume_pid;


    if  (argc != 3) {
        printf("usage: process <nitems> <nthreads>\n");
        return 0;
    }

    nitems   = MIN(atoi(argv[1]), MAXNITEMS);
    nthreads = MIN(atoi(argv[2]), MAXNTHREADS);
    
    /**
     * 此例子不对生产者与消费者进行同步，故先创建完生成者，然后再创建
     * 消费者
    */
    for (i = 0; i < nthreads; i++) {
        count[i] = 0;
        pthread_create(&produce_pid[i], NULL, produce, &count[i]);
    }

    for (i = 0; i < nthreads; i++) {
        pthread_join(produce_pid[i], NULL);
        printf("pthread %d count[%d] is %d \n", i, i, count[i]);
    }

    pthread_create(&consume_pid, NULL, consume, NULL);
    pthread_join(produce_pid[i], NULL);

    return ret;
}


void * produce(void *arg) 
{
    for (; ;) {
        pthread_mutex_lock(&shared.mutex); 
        //数组满了，退出
        if (shared.nindex >= nitems) {
            pthread_mutex_unlock(&shared.mutex);
            return NULL;
        }
        shared.buff[shared.nindex] = shared.nval;
        shared.nindex++;
        shared.nval++;

        pthread_mutex_unlock(&shared.mutex);

        *((int *)arg) += 1; 
    }
}

void * consume(void *arg)
{
    int     i;

    for (i = 0; i < nitems; i++) {
        if (shared.buff[i] != i) {
            printf("count[%d] is %d\n", i, shared.buff[i]);
        }
    }

    return NULL;

}