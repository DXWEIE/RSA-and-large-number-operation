#include <memory.h>
#include <iostream>
#include <string>
#include <fstream>
#include <time.h>
#include <string>
#include <fstream>
using namespace std;
#define BITPERDIGIT 32//每个小块是uint32，32位顺序和人期望是一致的，当黑盒就好
#define BASE 0x100000000U//每个小块uint32，基数就是2^32,可以在模的时候用上
#define BNMAXDGT 32//最多32“位”个32位
#define ALLONE 0xffffffffU
#define ALLONEL 0xffffffffUL
typedef uint32_t BN[BNMAXDGT + 1];//BN是1024+32位的
#define BNMAXBIT BNMAXDGT<<5 //BN最大能处理1024位
#define BNDMAXBIT BNMAXBIT<<1 //BND最大能处理2048位
#define BASEDIV2  0x80000000U
typedef uint32_t BND[1 + (BNMAXDGT << 1)];//BND能处理2048位，长度是2048+32
#define BNSIZE sizeof(BN)//BN占用的字节数

//一些常量
BN ZERO_BN = { 0 };//0
BN ONE_BN = { 1,1 };//1
BN TWO_BN = { 1,2 }; //2,看上去第一位就是长度为，赋值的时候是[0]，uint32内的顺序就不用管了

//flag状态。。。后来没有用到
#define FLAG_OK 0 //没有溢出啊这些的，正常
#define FLAG_OF 1 //加法上溢了，最后一次[31]+[31]出现了进位，自动模除不用管
#define FLAG_UF 2//可能是减法下溢了
#define FLAG_DIVZERO 3//除0错误
#define FLAG_FILE_ERROR 4//读写文件错误
#define FLAG_NOINV 5//不互素没有逆元
#define FLAG_ERROR 6//出错了

//需要使用的一些宏函数

//比较大小
#define MIN(a,b) ((a)<(b)?(a):(b))

//获得位数,[0]里面存的是uint32格式的位数
#define DIGITS_B(n_b) ((uint32_t)*(n_b))

//设置位数，置[0]为想要的位数
#define SETDIGITS_B(n_b, b)  (*(n_b) = (uint32_t)(b))


//指向最高位,用指针加法
#define MSDPTR_B(n_b)  ((n_b) + DIGITS_B (n_b))


//指向最低位,[0]是位数，[1]才是数字
#define LSDPTR_B(n_b)    ((n_b)+1)

//置为全0，位数置为0就可以了，0位，以后可以覆盖
#define SETZERO_B(n_b)    (*(n_b) = 0)

//需要使用的一些函数
//增加1位位数
inline void INCDIGITS_B(BN n_b)
{
	*(n_b) = *(n_b)+1;
}

//减少1位位数
inline void DECDIGITS_B(BN n_b)
{
	if (DIGITS_B(n_b) > 0)
		*(n_b) = *(n_b)-1;
}

//先是32“位”的,消除前导0，只要有32为基数的位数，且那一位为0，就位数减去1位，获得实际“位”数
inline void RMLDZRS_B(BN n_b)
{
	while ((*MSDPTR_B(n_b) == 0) && (DIGITS_B(n_b) > 0))
	{
		DECDIGITS_B(n_b);
	}
}

//设置一个uint32数为大数，只占1“位”
inline void SETONEBIT_B(BN num, uint32_t u)
{
	*LSDPTR_B(num) = u;
	SETDIGITS_B(num, 1);
	RMLDZRS_B(num);
}

//拷贝src到dest中，可以复制BND，与结构无关
inline void cpy_b(BN dest, BN src)
{
	uint32_t * high = MSDPTR_B(src);//最高位
	uint32_t * current;
	uint32_t bits = DIGITS_B(src);

	while (*high == 0U && bits > 0U)//跳过前导0
	{
		high--;
		bits--;
	}

	if (bits == 0U)//位数为0就返回0
	{
		SETZERO_B(dest);
		return;
	}
	current = dest + bits;//从对应高位开始复制
	while (high > src)//还有位数
	{
		*current = *high;
		high--;
		current--;
	}
	SETDIGITS_B(dest, bits);
}

//比较大小两个大数大小，相等返回0，大于返回1，小于返回-1
inline int cmp_b(BN a, BN b)
{
	uint32_t *msdptra_l, *msdptrb_l;
	int la = (int)DIGITS_B(a);
	int lb = (int)DIGITS_B(b);

	if (la == 0 && lb == 0)//位数都是0，相等
	{
		return 0;
	}

	while (a[la] == 0 && la > 0)//消除a_l前导位的0
	{
		--la;
	}

	while (b[lb] == 0 && lb > 0)//消除b_l前导位的0
	{
		--lb;
	}

	if (la == 0 && lb == 0)//消除前导位的0以后都是0
	{
		return 0;
	}

	if (la > lb)//a的位数比b大
	{
		return 1;
	}

	if (la < lb)//a的位数比b小
	{
		return -1;
	}
	//位数一样时
	msdptra_l = a + la;//指向有效最高位
	msdptrb_l = b + lb;//指向有效最高位

	while ((*msdptra_l == *msdptrb_l) && (msdptra_l > a))//一位位比较，如果该位数字一样下一位
	{
		msdptra_l--;
		msdptrb_l--;
	}

	if (msdptra_l == a)//到末位都相同
	{
		return 0;
	}

	if (*msdptra_l > *msdptrb_l)//没到末位，此时比较出了a>b
	{
		return 1;
	}
	else//没到末位，此时比较出了a<b
	{
		return -1;
	}
}

//设置每一位为0xffffff来实现小减大取模
inline void setmax_b(BN  a)
{
	uint32_t  *aptr = a;
	uint32_t *maptr = a + BNMAXDGT;//指向最高[31]

	while (++aptr <= maptr)//全部为0xffffffff全1
	{
		*aptr = ALLONE;//全1最大
	}
	SETDIGITS_B(a, BNMAXDGT);//32“位”全1
}

//加法
int add_b(BN a, BN b, BN sum);//a+b=sum，会模掉1024位
void add(BN a, BN b, BN sum);//add的另一个形式，区别是sum可能比预料的多一位，在求模加的时候用得到,没有模1024位
int adduint_b(BN a, uint32_t b, BN sum);//a加一个uint32形式的b,a+b=sum
int adduint_b(uint32_t a, BN b, BN &sum);

//减法
int sub_b(BN a, BN b, BN result);//result=a-b
int subuint_b(BN a, uint32_t b, BN &result);//减一个uint32
void sub(BN a, BN b, BN result);//没有模1024位的减法，会用的到


//乘法
int mul_b(BN a, BN b, BN &result);//result = a*b 留下1024位
int mul(BN a, BN b, BN result);//没有模2^32^32的乘法

//除法

int mydiv_b(BN a, BN b, BN q, BN rem);//可靠的移位除法,虽然不快
int div_b(BN a, BN b, BN q, BN rem);//a=qb+rem，商是q，余数是rem，a可以是BND，应付模幂运算，使用knuth方法
int remdiv_b(BN a, BN b, BN & rem);//取余数除法，a/b的余数是rem


//取模
int modn_b(BN a, BN n, BN & rem);//rem=a mod n
int modadd_b(BN a, BN b, BN n, BN & result);//result=a+b(mod n)
void modsub_b(BN a, BN b, BN n, BN &result);//result = a - b(mod n)
void modmul(BN a, BN b, BN n, BN & result);//模乘，result=a*b (mod n )

//信安数学中的函数
int gcd_b(BN a, BN b, BN & result);//求公因子，result=(a,b)
int inv_b(BN a, BN n, BN & x);//如果(a,n)=1,则有ax = 1 (mod n)；否则异常没有逆元，返回0，显然逆元不可能为0  
int modexp_b(BN b, BN n, BN m, BN & result);//模幂运算，用的模平方,result= b^n (mod m)
int fermat_b(BN a);//用费马小定理检测a是不是素数，选的是2/3/5/7，200位的a会出现误报，500位还没有发现误报
void crt_b(BN a, BN b, BN p, BN q, BN & result);//用中国剩余定理求模幂，a^b mod(p*q)

//位运算函数
int shl_b(BN a);//左移一位
int shr_b(BN a);//右移一位
uint32_t getbits_b(BN a);//获取a的二进制位数，a可以是BND形式,最大2048


//特殊操作的函数
void exclu();//求前几十个素数的乘积，存在文件中，便于后面找素数的时候利用乘积求gcd，多求gcd，少进行费马检测
int genBN_t(BN result, int bits);//产生bits个二进制位的大数result，每32位哈希一次，很慢
int genBN(BN result, int bits);//利用sha-1结果产生bits个二进制位的大数result,每5个32位哈希一次，快了一点点
void writerand(char * addr);//把随机数写到addr中，后面对它求哈希
int findprime(BN a, int bits);//寻找bits位素数a，会调用genBN产生一个大数，然后利用exclu()结果进行调整才去费马
void genpq(char * p_path, char * q_path);//产生私钥p和q存在p_path和q_path文件中

//SHA-1的函数和常数
uint32_t H0 = 0x67452301;
uint32_t H1 = 0xEFCDAB89;
uint32_t H2 = 0x98BADCFE;
uint32_t H3 = 0x10325476;
uint32_t H4 = 0xC3D2E1F0;

const uint32_t K0 = 0x5A827999;
const uint32_t K1 = 0x6ED9EBA1;
const uint32_t K2 = 0x8F1BBCDC;
const uint32_t K3 = 0xCA62C1D6;
void subround(uint32_t & A, uint32_t & B, uint32_t & C, uint32_t & D, uint32_t & E, uint32_t &W, uint32_t K, int mode);
long long msgsize(char*plainaddr);
inline uint32_t cirleft(uint32_t word, int bit);
int mysha1(char *inputfileaddr, char *output);//每次调用用的是全局的，相当于一个状态向量
int SHA1(char *inputfileaddr, char *output);//每次调用里面会重新初始化向量
int checkresult(char * plainpath, char * decrypath);//检查明文和解密以后是否相同，相同返回1


//字符串和文件处理
string bn2str(BN bignum);//bignum转化为字符串形式返回，返回的不显示前导0，和正常预期是一样的，可以吞下BND
int str2bn(BN & bignum, string strbn);//字符串形式的转化为大数，只能转化为BN
int readbn(BN &bignum, string filename);//从文件filename中读取大数到bignum中，字符串是16进制的不能是0x开头
int writebn(string filename, BN bignum);//把大数bignum写入到文件filename中，不带前导0和前缀0x

