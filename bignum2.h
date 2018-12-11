#include <memory.h>
#include <iostream>
#include <string>
#include <fstream>
#include <time.h>
#include <string>
#include <fstream>
using namespace std;
#define BITPERDIGIT 32//ÿ��С����uint32��32λ˳�����������һ�µģ����ںоͺ�
#define BASE 0x100000000U//ÿ��С��uint32����������2^32,������ģ��ʱ������
#define BNMAXDGT 32//���32��λ����32λ
#define ALLONE 0xffffffffU
#define ALLONEL 0xffffffffUL
typedef uint32_t BN[BNMAXDGT + 1];//BN��1024+32λ��
#define BNMAXBIT BNMAXDGT<<5 //BN����ܴ���1024λ
#define BNDMAXBIT BNMAXBIT<<1 //BND����ܴ���2048λ
#define BASEDIV2  0x80000000U
typedef uint32_t BND[1 + (BNMAXDGT << 1)];//BND�ܴ���2048λ��������2048+32
#define BNSIZE sizeof(BN)//BNռ�õ��ֽ���

//һЩ����
BN ZERO_BN = { 0 };//0
BN ONE_BN = { 1,1 };//1
BN TWO_BN = { 1,2 }; //2,����ȥ��һλ���ǳ���Ϊ����ֵ��ʱ����[0]��uint32�ڵ�˳��Ͳ��ù���

//flag״̬����������û���õ�
#define FLAG_OK 0 //û���������Щ�ģ�����
#define FLAG_OF 1 //�ӷ������ˣ����һ��[31]+[31]�����˽�λ���Զ�ģ�����ù�
#define FLAG_UF 2//�����Ǽ���������
#define FLAG_DIVZERO 3//��0����
#define FLAG_FILE_ERROR 4//��д�ļ�����
#define FLAG_NOINV 5//������û����Ԫ
#define FLAG_ERROR 6//������

//��Ҫʹ�õ�һЩ�꺯��

//�Ƚϴ�С
#define MIN(a,b) ((a)<(b)?(a):(b))

//���λ��,[0]��������uint32��ʽ��λ��
#define DIGITS_B(n_b) ((uint32_t)*(n_b))

//����λ������[0]Ϊ��Ҫ��λ��
#define SETDIGITS_B(n_b, b)  (*(n_b) = (uint32_t)(b))


//ָ�����λ,��ָ��ӷ�
#define MSDPTR_B(n_b)  ((n_b) + DIGITS_B (n_b))


//ָ�����λ,[0]��λ����[1]��������
#define LSDPTR_B(n_b)    ((n_b)+1)

//��Ϊȫ0��λ����Ϊ0�Ϳ����ˣ�0λ���Ժ���Ը���
#define SETZERO_B(n_b)    (*(n_b) = 0)

//��Ҫʹ�õ�һЩ����
//����1λλ��
inline void INCDIGITS_B(BN n_b)
{
	*(n_b) = *(n_b)+1;
}

//����1λλ��
inline void DECDIGITS_B(BN n_b)
{
	if (DIGITS_B(n_b) > 0)
		*(n_b) = *(n_b)-1;
}

//����32��λ����,����ǰ��0��ֻҪ��32Ϊ������λ��������һλΪ0����λ����ȥ1λ�����ʵ�ʡ�λ����
inline void RMLDZRS_B(BN n_b)
{
	while ((*MSDPTR_B(n_b) == 0) && (DIGITS_B(n_b) > 0))
	{
		DECDIGITS_B(n_b);
	}
}

//����һ��uint32��Ϊ������ֻռ1��λ��
inline void SETONEBIT_B(BN num, uint32_t u)
{
	*LSDPTR_B(num) = u;
	SETDIGITS_B(num, 1);
	RMLDZRS_B(num);
}

//����src��dest�У����Ը���BND����ṹ�޹�
inline void cpy_b(BN dest, BN src)
{
	uint32_t * high = MSDPTR_B(src);//���λ
	uint32_t * current;
	uint32_t bits = DIGITS_B(src);

	while (*high == 0U && bits > 0U)//����ǰ��0
	{
		high--;
		bits--;
	}

	if (bits == 0U)//λ��Ϊ0�ͷ���0
	{
		SETZERO_B(dest);
		return;
	}
	current = dest + bits;//�Ӷ�Ӧ��λ��ʼ����
	while (high > src)//����λ��
	{
		*current = *high;
		high--;
		current--;
	}
	SETDIGITS_B(dest, bits);
}

//�Ƚϴ�С����������С����ȷ���0�����ڷ���1��С�ڷ���-1
inline int cmp_b(BN a, BN b)
{
	uint32_t *msdptra_l, *msdptrb_l;
	int la = (int)DIGITS_B(a);
	int lb = (int)DIGITS_B(b);

	if (la == 0 && lb == 0)//λ������0�����
	{
		return 0;
	}

	while (a[la] == 0 && la > 0)//����a_lǰ��λ��0
	{
		--la;
	}

	while (b[lb] == 0 && lb > 0)//����b_lǰ��λ��0
	{
		--lb;
	}

	if (la == 0 && lb == 0)//����ǰ��λ��0�Ժ���0
	{
		return 0;
	}

	if (la > lb)//a��λ����b��
	{
		return 1;
	}

	if (la < lb)//a��λ����bС
	{
		return -1;
	}
	//λ��һ��ʱ
	msdptra_l = a + la;//ָ����Ч���λ
	msdptrb_l = b + lb;//ָ����Ч���λ

	while ((*msdptra_l == *msdptrb_l) && (msdptra_l > a))//һλλ�Ƚϣ������λ����һ����һλ
	{
		msdptra_l--;
		msdptrb_l--;
	}

	if (msdptra_l == a)//��ĩλ����ͬ
	{
		return 0;
	}

	if (*msdptra_l > *msdptrb_l)//û��ĩλ����ʱ�Ƚϳ���a>b
	{
		return 1;
	}
	else//û��ĩλ����ʱ�Ƚϳ���a<b
	{
		return -1;
	}
}

//����ÿһλΪ0xffffff��ʵ��С����ȡģ
inline void setmax_b(BN  a)
{
	uint32_t  *aptr = a;
	uint32_t *maptr = a + BNMAXDGT;//ָ�����[31]

	while (++aptr <= maptr)//ȫ��Ϊ0xffffffffȫ1
	{
		*aptr = ALLONE;//ȫ1���
	}
	SETDIGITS_B(a, BNMAXDGT);//32��λ��ȫ1
}

//�ӷ�
int add_b(BN a, BN b, BN sum);//a+b=sum����ģ��1024λ
void add(BN a, BN b, BN sum);//add����һ����ʽ��������sum���ܱ�Ԥ�ϵĶ�һλ������ģ�ӵ�ʱ���õõ�,û��ģ1024λ
int adduint_b(BN a, uint32_t b, BN sum);//a��һ��uint32��ʽ��b,a+b=sum
int adduint_b(uint32_t a, BN b, BN &sum);

//����
int sub_b(BN a, BN b, BN result);//result=a-b
int subuint_b(BN a, uint32_t b, BN &result);//��һ��uint32
void sub(BN a, BN b, BN result);//û��ģ1024λ�ļ��������õĵ�


//�˷�
int mul_b(BN a, BN b, BN &result);//result = a*b ����1024λ
int mul(BN a, BN b, BN result);//û��ģ2^32^32�ĳ˷�

//����

int mydiv_b(BN a, BN b, BN q, BN rem);//�ɿ�����λ����,��Ȼ����
int div_b(BN a, BN b, BN q, BN rem);//a=qb+rem������q��������rem��a������BND��Ӧ��ģ�����㣬ʹ��knuth����
int remdiv_b(BN a, BN b, BN & rem);//ȡ����������a/b��������rem


//ȡģ
int modn_b(BN a, BN n, BN & rem);//rem=a mod n
int modadd_b(BN a, BN b, BN n, BN & result);//result=a+b(mod n)
void modsub_b(BN a, BN b, BN n, BN &result);//result = a - b(mod n)
void modmul(BN a, BN b, BN n, BN & result);//ģ�ˣ�result=a*b (mod n )

//�Ű���ѧ�еĺ���
int gcd_b(BN a, BN b, BN & result);//�����ӣ�result=(a,b)
int inv_b(BN a, BN n, BN & x);//���(a,n)=1,����ax = 1 (mod n)�������쳣û����Ԫ������0����Ȼ��Ԫ������Ϊ0  
int modexp_b(BN b, BN n, BN m, BN & result);//ģ�����㣬�õ�ģƽ��,result= b^n (mod m)
int fermat_b(BN a);//�÷���С������a�ǲ���������ѡ����2/3/5/7��200λ��a������󱨣�500λ��û�з�����
void crt_b(BN a, BN b, BN p, BN q, BN & result);//���й�ʣ�ඨ����ģ�ݣ�a^b mod(p*q)

//λ���㺯��
int shl_b(BN a);//����һλ
int shr_b(BN a);//����һλ
uint32_t getbits_b(BN a);//��ȡa�Ķ�����λ����a������BND��ʽ,���2048


//��������ĺ���
void exclu();//��ǰ��ʮ�������ĳ˻��������ļ��У����ں�����������ʱ�����ó˻���gcd������gcd���ٽ��з�����
int genBN_t(BN result, int bits);//����bits��������λ�Ĵ���result��ÿ32λ��ϣһ�Σ�����
int genBN(BN result, int bits);//����sha-1�������bits��������λ�Ĵ���result,ÿ5��32λ��ϣһ�Σ�����һ���
void writerand(char * addr);//�������д��addr�У�����������ϣ
int findprime(BN a, int bits);//Ѱ��bitsλ����a�������genBN����һ��������Ȼ������exclu()������е�����ȥ����
void genpq(char * p_path, char * q_path);//����˽Կp��q����p_path��q_path�ļ���

//SHA-1�ĺ����ͳ���
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
int mysha1(char *inputfileaddr, char *output);//ÿ�ε����õ���ȫ�ֵģ��൱��һ��״̬����
int SHA1(char *inputfileaddr, char *output);//ÿ�ε�����������³�ʼ������
int checkresult(char * plainpath, char * decrypath);//������ĺͽ����Ժ��Ƿ���ͬ����ͬ����1


//�ַ������ļ�����
string bn2str(BN bignum);//bignumת��Ϊ�ַ�����ʽ���أ����صĲ���ʾǰ��0��������Ԥ����һ���ģ���������BND
int str2bn(BN & bignum, string strbn);//�ַ�����ʽ��ת��Ϊ������ֻ��ת��ΪBN
int readbn(BN &bignum, string filename);//���ļ�filename�ж�ȡ������bignum�У��ַ�����16���ƵĲ�����0x��ͷ
int writebn(string filename, BN bignum);//�Ѵ���bignumд�뵽�ļ�filename�У�����ǰ��0��ǰ׺0x

