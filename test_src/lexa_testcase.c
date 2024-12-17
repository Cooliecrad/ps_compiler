/**
 * @warning 这个文件是一个测试文件！
 */

int fibnacci ( int num )
{
    if ( num < 2 ) return 1;
    else return fibnacci ( num - 1 ) + fibnacci ( num - 2 );
}

int main ( void )
{
    printf ( "Helloworld\n" );
}