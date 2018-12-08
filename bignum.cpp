#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <memory.h>
#include <iostream>
#include <string>
#include <fstream>
#include "bignum.h"
using namespace std;

#define BITPERDIGIT 32//ÿ��С����uint32��32λ˳�����������һ�µģ����ںоͺ�
#define BASE 0x100000000U//ÿ��С��uint32����������2^32,������ģ��ʱ������
#define BNMAXDGT 32//���32��λ����32λ
#define ALLONE 0xffffffffU
#define ALLONEL 0xffffffffUL
typedef uint32_t BN[BNMAXDGT + 1];//BN��1024+32λ��
#define BNMAXBIT BNMAXDGT<<5 //����ܴ���1024λ
typedef uint32_t BND[1 + (BNMAXDGT << 1)];//BND�ܴ���2048λ��������2048+32
#define LDBITPERDGT 5
#define BNSIZE sizeof(BN)//BNռ�õ��ֽ���

BN ZERO_BN = { 0 };//0
BN ONE_BN = { 1,1 };//1
BN TWO_BN = { 1,2 }; //2,����ȥ��һλ���ǳ���Ϊ����ֵ��ʱ����[0]��uint32�ڵ�˳��Ͳ��ù���
#define PURGEVARS_L(X) (void)0
#define ISPURGED_L(X) (void)0
#define FLAG_OK 0 //û���������Щ�ģ�����
#define FLAG_OF 1 //�ӷ������ˣ����һ��[31]+[31]�����˽�λ���Զ�ģ�����ù�
#define FLAG_UF 2//�����Ǽ���������
#define FLAG_DIVZERO 3//��0����
#define FLAG_FILE_ERROR 4//��д�ļ�����
#define FLAG_NOINV 5//������û����Ԫ


#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif
//���λ��,[0]��������uint32��ʽ��λ�����ǹ��õģ�̫����
#define digits_b(n_b)\
    ((uint32_t)*(n_b))
#define DIGITS_B(n_b)\
    ((uint32_t)*(n_b))

//����λ������[0]Ϊ��Ҫ��
#define SETDIGITS_B(n_b, b)\
    (*(n_b) = (uint32_t)(b))
#define setdigits_b(n_b, b)\
    (*(n_b) = (uint32_t)(b))

//ָ�����λ
#define MSDPTR_B(n_b)\
    ((n_b) + DIGITS_B (n_b))
#define msdptr_b(n_b)\
    ((n_b) + DIGITS_B (n_b))

//ָ�����λ,[0]��λ����[1]��������
#define LSDPTR_B(n_b)\
    ((n_b)+1)
#define lsdptr_b(n_b)\
    ((n_b)+1)

//��Ϊȫ0��λ����Ϊ0�Ϳ����ˣ�0λ���Ժ��õ��ڸ��ǣ����ܼ�����0
#define setzero_b(n_b)\
    (*(n_b) = 0)
#define SETZERO_B(n_b)\
    (*(n_b) = 0)

void SETONEBIT_B(BN num_l, uint32_t u)
{
	*LSDPTR_B(num_l) = u;
	SETDIGITS_B(num_l, 1);
	RMLDZRS_B(num_l);
}

//����1λ
#define INCDIGITS_B(n_b)\
    (++*(n_b))
#define incdigits_b(n_b)\
    (++*(n_b))

//����1λ
#define DECDIGITS_B(n_b)\
    (DIGITS_B (n_b) > 0);\
    (--*(n_b))

#define decdigits_b(n_b)\
    (DIGITS_B (n_b) > 0);\
    (--*(n_b))

//����32��λ����,����ǰ��0��ֻҪ��32Ϊ������λ��������һλΪ0����λ����ȥ1λ�����ʵ�ʡ�λ����
#define RMLDZRS_B(n_b) \
    while ((DIGITS_B (n_b) > 0) && (*MSDPTR_B (n_b) == 0)) {DECDIGITS_B (n_b);}
#define rmldzrs_b(n_b) \
    while ((DIGITS_B (n_b) > 0) && (*MSDPTR_B (n_b) == 0)) {DECDIGITS_B (n_b);}


//�ο��˿���
void cpy_b(BN dest_b, BN src_b)
{
	uint32_t *lastsrc_b = MSDPTR_B(src_b);
	*dest_b = *src_b;//����λ��

	while ((*lastsrc_b == 0) && (*dest_b > 0))//ȥ��ǰ��0
	{
		lastsrc_b = lastsrc_b - 1;
		*dest_b = *dest_b - 1;//λ��-1��ȥ��ǰ��0
	}

	while (src_b < lastsrc_b)//������ֵ��
	{
		dest_b++; src_b++;
		*dest_b = *src_b;
	}
}

//�ο��˱Ƚϴ�С����ȷ���0�����ڷ���1��С�ڷ���-1
int cmp_b(BN a_l, BN b_l)
{
	uint32_t *msdptra_l, *msdptrb_l;
	int la = (int)DIGITS_B(a_l);
	int lb = (int)DIGITS_B(b_l);

	if (la == 0 && lb == 0)
	{
		return 0;
	}

	while (a_l[la] == 0 && la > 0)
	{
		--la;
	}

	while (b_l[lb] == 0 && lb > 0)
	{
		--lb;
	}

	if (la == 0 && lb == 0)
	{
		return 0;
	}

	if (la > lb)
	{
		PURGEVARS_L((2, sizeof(la), &la,
			sizeof(lb), &lb));
		ISPURGED_L((2, sizeof(la), &la,
			sizeof(lb), &lb));
		return 1;
	}

	if (la < lb)
	{
		PURGEVARS_L((2, sizeof(la), &la,
			sizeof(lb), &lb));
		ISPURGED_L((2, sizeof(la), &la,
			sizeof(lb), &lb));
		return -1;
	}

	msdptra_l = a_l + la;
	msdptrb_l = b_l + lb;

	while ((*msdptra_l == *msdptrb_l) && (msdptra_l > a_l))
	{
		msdptra_l--;
		msdptrb_l--;
	}

	PURGEVARS_L((2, sizeof(la), &la,
		sizeof(lb), &lb));
	ISPURGED_L((2, sizeof(la), &la,
		sizeof(lb), &lb));

	if (msdptra_l == a_l)
	{
		return 0;
	}

	if (*msdptra_l > *msdptrb_l)
	{
		return 1;
	}
	else
	{
		return -1;
	}
}


//�������õ�������0xffffff��ʵ��С����ȡģ
void setmax_b(BN &a)
{
	uint32_t  *aptr = a;
	uint32_t *maptr = a + BNMAXDGT;//ָ�����[31]

	while (++aptr <= maptr)//ȫ��Ϊ0xffffffffȫ1
	{
		*aptr = ALLONE;//ȫ1���
	}
	SETDIGITS_B(a, BNMAXDGT);//32��λ��ȫ1
}

//Yeahhhhh�ӷ����е�
int add_b(BN a, BN b, BN &sum)//???�ܲ��ܴ����ô���??����ʺţ���������ָ�룿���Ǹ�����
{
	memset(sum, 0, BNSIZE);
	uint32_t temps[BNMAXDGT + 1];//�Ȳ��ż������������temps����
	memset(temps, 0, sizeof(temps));
	uint32_t *aptr, *bptr, *sptr = LSDPTR_B(temps);//ָ��a,b,temp��ĩλ
	uint32_t *maptr, *mbptr;//ָ��a��b�������Чλ��ָ��
	int flag = FLAG_OK;//��ʼ��ΪOK
	uint64_t carry = 0ULL;//��32λ��ʾ�м�ͣ���32λ��ʾ��λ
	if (DIGITS_B(a) < DIGITS_B(b))//���a��λ����bС����bΪ�ο�����һ��ѭ��
	{
		aptr = LSDPTR_B(b);//b�ǳ���
		maptr = MSDPTR_B(b);
		bptr = LSDPTR_B(a);//a�Ƕ̵�
		mbptr = MSDPTR_B(a);
		SETDIGITS_B(temps, DIGITS_B(b));//Ҫô��b��λ����Ҫô��b��λ����һλ����˵
	}
	else//�������aҪô��b����Ҫô��bһ����
	{
		aptr = LSDPTR_B(a);//a�ǳ���
		maptr = MSDPTR_B(a);
		bptr = LSDPTR_B(b);//b�Ƕ̵�
		mbptr = MSDPTR_B(b);
		SETDIGITS_B(temps, DIGITS_B(a));//Ҫô��a��λ����Ҫô��a��λ����һλ����˵
	}
	while (bptr <= mbptr)//b��λ����û�꣬������1λ��
	{
		carry = (uint64_t)*aptr + (uint64_t)*bptr + (uint64_t)(uint32_t)(carry >> BITPERDIGIT);//a+b+(ci-1)
		*sptr = (uint32_t)(carry);//��32λ���м��
		aptr++; bptr++; sptr++;//��һλ
	}
	while (aptr <= maptr)//a��λ������û����ᣬ������a����
	{
		carry = (uint64_t)*aptr + (uint64_t)(uint32_t)(carry >> BITPERDIGIT);//a+(ci-1)
		*sptr = (uint32_t)(carry);//��32λ���м��
		aptr++;  sptr++;//��һλ,����������λ
	}
	if (carry&BASE)//������carry��1λ��λ���͵ġ�λ������a��1��λ��
	{
		*sptr = 1;//��1������1
		SETDIGITS_B(temps, DIGITS_B(temps) + 1);//��֮ǰ��һλ
	}
	if (DIGITS_B(temps) > (uint32_t)BNMAXDGT)//������32��λ�����������
	{
		SETDIGITS_B(temps, BNMAXDGT);//������32��λ����������ǰ��0,ģ������32��λ����
		RMLDZRS_B(temps);
		flag = FLAG_OF;//�ӷ��������
	}
	cpy_b(sum, temps);
	return flag;
}
//add����һ����ʽ��������sum���ܱ�Ԥ�ϵĶ�һλ������ģ�ӵ�ʱ���õõ���ָ��ʽ��sum��sptrֱ��ָ��sum
void add(BN a, BN b, BN sum)
{
	uint32_t *aptr, *bptr, *sptr = LSDPTR_B(sum);//ָ��a,b,sum��ĩλ
	uint32_t *maptr, *mbptr;//ָ��a��b�������Чλ��ָ��
	int flag = FLAG_OK;//��ʼ��ΪOK
	uint64_t carry = 0ULL;//��32λ��ʾ�м�ͣ���32λ��ʾ��λ
	if (DIGITS_B(a) < DIGITS_B(b))//���a��λ����bС����bΪ�ο�����һ��ѭ��
	{
		aptr = LSDPTR_B(b);//b�ǳ���
		maptr = MSDPTR_B(b);
		bptr = LSDPTR_B(a);//a�Ƕ̵�
		mbptr = MSDPTR_B(a);
		SETDIGITS_B(sum, DIGITS_B(b));//Ҫô��b��λ����Ҫô��b��λ����һλ����˵
	}
	else//�������aҪô��b����Ҫô��bһ����
	{
		aptr = LSDPTR_B(a);//a�ǳ���
		maptr = MSDPTR_B(a);
		bptr = LSDPTR_B(b);//b�Ƕ̵�
		mbptr = MSDPTR_B(b);
		SETDIGITS_B(sum, DIGITS_B(a));//Ҫô��a��λ����Ҫô��a��λ����һλ����˵
	}
	while (bptr <= mbptr)//b��λ����û�꣬������1λ��
	{
		carry = (uint64_t)*aptr + (uint64_t)*bptr + (uint64_t)(uint32_t)(carry >> BITPERDIGIT);//a+b+(ci-1)
		*sptr = (uint32_t)(carry);//��32λ���м��
		aptr++; bptr++; sptr++;//��һλ
	}
	while (aptr <= maptr)//a��λ������û����ᣬ������a����
	{
		carry = (uint64_t)*aptr + (uint64_t)(uint32_t)(carry >> BITPERDIGIT);//a+(ci-1)
		*sptr = (uint32_t)(carry);//��32λ���м��
		aptr++;  sptr++;//��һλ,����������λ
	}
	if (carry&BASE)//������carry��1λ��λ���͵ġ�λ������a��1��λ��
	{
		*sptr = 1;//��1������1
		INCDIGITS_B(sum);
	}
}
//result=a+b(mod n)
int modadd_b(BN a, BN b, BN n, BN & result)
{
	int flag = FLAG_OK;
	uint32_t temp1[BNMAXDGT + 2];
	memset(temp1, 0, sizeof(temp1));
	BN temp2;
	memset(temp1, 0, BNSIZE);
	memset(temp2, 0, BNSIZE);
	if (cmp_b(n, ZERO_BN) == 0)//���ģ��Ϊ0
		add(a, b, result);
	else
	{
		add(a, b, temp1);
		flag = modn_b(temp1, n, temp2);
		cpy_b(result, temp2);
	}
	return flag;
}

//��һ��uint32
int adduint_b(BN a, uint32_t b, BN &sum)
{
	BN temp = { 0 };
	temp[0] = 1; temp[1] = b;//����temp�Ǵ���
	int flag = add_b(a, temp, sum);
	return flag;
}

int adduint_b(uint32_t a, BN b, BN &sum)
{
	BN temp = { 0 };
	temp[0] = 1; temp[1] = a;//����temp�Ǵ���
	int flag = add_b(b, temp, sum);
	return flag;
}

//ODK����
int sub_b(BN a, BN b, BN &result)
{
	memset(result, 0, BNSIZE);
	uint64_t carry = 0ULL;
	uint32_t *aptr, *bptr, *rptr, *maptr, *mbptr;

	int flag = FLAG_OK;//�����Ƿ�����������

	uint32_t a_t[BITPERDIGIT + 1];//����1λ
	BN b_t;
	memset(b_t, 0, sizeof(b_t));
	cpy_b(a_t, a);
	cpy_b(b_t, b);
	aptr = LSDPTR_B(a_t);
	bptr = LSDPTR_B(b_t);
	rptr = LSDPTR_B(result);
	maptr = MSDPTR_B(a_t);
	mbptr = MSDPTR_B(b_t);

	if (cmp_b(a_t, b_t) == -1)//���a<b
	{
		setmax_b(a_t);
		maptr = a_t + BNMAXDGT;//ָ��[31]
		SETDIGITS_B(result, BNMAXDGT);//���ǽ��Ҳ����ô��λ��������ô���������
		flag = FLAG_UF;//������
	}
	else//û�з�������
	{
		SETDIGITS_B(result, DIGITS_B(a_t));//λ��Ӧ�ú�a��һ����
	}
	while (bptr <= mbptr)//b����λ������ȼӷ�
	{
		carry = (uint64_t)*aptr - (uint64_t)*bptr - ((carry&BASE) >> BITPERDIGIT);//a-b-ci(����֮ǰ��λ��)
		*rptr = (uint32_t)carry;
		aptr++; bptr++; rptr++;
	}
	while (aptr <= maptr)//a��û����,��ȼӷ�
	{
		carry = (uint64_t)*aptr - ((carry&BASE) >> BITPERDIGIT);//a-b-ci(����֮ǰ��λ��)
		*rptr = (uint32_t)carry;
		aptr++; rptr++;
	}
	RMLDZRS_B(result);//����ǰ��0
	if (flag == FLAG_UF)//��������ˣ�����һ��
	{
		add_b(result, a, result);
		add_b(result, ONE_BN, result);//(Nm-b+a)+1
	}
	return flag;
}
//��һ��uint32
int subuint_b(BN a, uint32_t b, BN &result)
{
	BN temp = { 0 };
	temp[0] = 1; temp[1] = b;//����temp�Ǵ���
	int flag = sub_b(a, temp, result);
	return flag;
}

//ͨ�õļ���
void sub(BN a, BN b, BN result)
{
	uint64_t carry = 0ULL;
	uint32_t *aptr, *bptr, *rptr, *maptr, *mbptr;

	int flag = FLAG_OK;//�����Ƿ�����������

	aptr = LSDPTR_B(a);
	bptr = LSDPTR_B(b);
	rptr = LSDPTR_B(result);
	maptr = MSDPTR_B(a);
	mbptr = MSDPTR_B(b);
	SETDIGITS_B(result, DIGITS_B(a));
	while (bptr <= mbptr)//b����λ������ȼӷ�
	{
		carry = (uint64_t)*aptr - (uint64_t)*bptr - ((carry&BASE) >> BITPERDIGIT);//a-b-ci(����֮ǰ��λ��)
		*rptr = (uint32_t)carry;
		aptr++; bptr++; rptr++;
	}
	while (aptr <= maptr)//a��û����,��ȼӷ�
	{
		carry = (uint64_t)*aptr - ((carry&BASE) >> BITPERDIGIT);//a-b-ci(����֮ǰ��λ��)
		*rptr = (uint32_t)carry;
		aptr++; rptr++;
	}
	RMLDZRS_B(result);//����ǰ��0
}

void modsub_b(BN a, BN b, BN n, BN &result)
{
	BN a_t, b_t, rem, temp;
	memset(rem, 0, sizeof(rem));
	memset(rem, 0, sizeof(temp));
	cpy_b(a_t, a);
	cpy_b(b_t, b);
	if (cmp_b(a_t, b_t) >= 0)//���a>=b
	{
		//sub_b(a_t, b_t, temp);
		sub_b(a_t, b_t, temp);
		//cout << bn2str(a_t) << " - " << bn2str(b_t) << " = " << bn2str(temp) << endl;

		modn_b(temp, n, rem);
		//cout << "zhe cuo le line 424!" << endl;
		//cout << bn2str(temp) << " % " << bn2str(n) << " = " << bn2str(rem) << endl;
		cpy_b(result, rem);
	}
	else //a<b...�Ǿ�b-a=t��Ȼ����n-t  12-19 mod 5  19-12 mod 5 =2  5-2=3
	{
		//sub_b(b_t, a_t, temp);
		sub_b(b_t, a_t, temp);
		modn_b(temp, n, rem);
		//cout << bn2str(temp) << " % " << bn2str(n) << " = " << bn2str(rem) << endl;
		//sub_b(n, rem, temp);
		sub_b(n, rem, temp);
		//cout << bn2str(n) << " - " << bn2str(rem) << " = " << bn2str(temp) << endl;
		cpy_b(result, temp);
	}
}

//�˷�hhh
int mul_b(BN a, BN b, BN &result)
{
	int flag = FLAG_OK;
	uint32_t * aptr, *bptr, *maptr, *mbptr, *bcir, *acir, *rptr, *cptr;
	uint32_t ta;//��ʱ���a
	uint64_t carry;//���汾λ�ͽ�λ
	BND tempr;//�洢���ս����������������BN���ȣ�2048λ����������ˣ�
	memset(tempr, 0, sizeof(tempr));
	BN aa, bb;
	memset(aa, 0, BNSIZE);
	memset(bb, 0, BNSIZE);
	memset(result, 0, BNSIZE);
	cpy_b(aa, a); cpy_b(bb, b);//����
	//RMLDZRS_B(aa);//ȥ��ǰ��0
	//RMLDZRS_B(bb);
	if (DIGITS_B(aa) == 0 || DIGITS_B(bb) == 0)//��û�г���Ϊ0��Ͷ��ȡ��
	{
		result[0] = 0; result[1] = 0;//���Ϊ0
		return flag;
	}
	if (DIGITS_B(aa) < DIGITS_B(bb))//���a�ĳ��ȱ�b�Ķ̣��ͼӼ�һ������
	{
		aptr = bb;
		bptr = aa;
	}
	else
	{
		aptr = aa;
		bptr = bb;
	}
	maptr = aptr + *aptr;//ָ�������Чλ
	mbptr = bptr + *bptr;//ָ�������Чλ

	carry = 0;
	ta = *LSDPTR_B(aptr);//ĩλ
	//���(bn-1bn-2...b1)��a0
	for (bcir = LSDPTR_B(bptr), rptr = LSDPTR_B(tempr); bcir <= mbptr;
		bcir++, rptr++)
	{
		carry = (uint64_t)ta * (uint64_t)*bcir + (uint64_t)(uint32_t)(carry >> BITPERDIGIT);
		*rptr = (uint32_t)carry;

	}
	*rptr = (uint32_t)(carry >> BITPERDIGIT);//���һ�εĽ�λ
	//ѭ��,a[0]�Ѿ�����
	for (cptr = LSDPTR_B(tempr) + 1, acir = LSDPTR_B(aptr) + 1;
		acir <= maptr; cptr++, acir++)
	{
		carry = 0;
		ta = *acir;

		for (bcir = LSDPTR_B(bptr), rptr = cptr;
			bcir <= mbptr; bcir++, rptr++)
		{
			carry = (uint64_t)ta * (uint64_t)*bcir + (uint64_t)*rptr + (uint64_t)(uint32_t)(carry >> BITPERDIGIT);
			*rptr = (uint32_t)carry;
		}
		*rptr = (uint32_t)(carry >> BITPERDIGIT);//���һ�εĽ�λ
	}
	SETDIGITS_B(tempr, DIGITS_B(aptr) + DIGITS_B(bptr));//������a+b��λ��
	RMLDZRS_B(tempr);//ȥ��ǰ��0
	if (DIGITS_B(tempr) > (uint32_t)BNMAXDGT)//�˷������������32��λ��
	{
		SETDIGITS_B(tempr, BNMAXDGT);//������32��λ����������ǰ��0,ģ������32��λ����
		RMLDZRS_B(tempr);
		flag = FLAG_OF;//�������
	}
	cpy_b(result, tempr);
	return flag;
}

//û��ģ2^32^32�ĳ˷�
int mul(BN a, BN b, BN result)
{
	int flag = FLAG_OK;
	uint32_t * aptr, *bptr, *maptr, *mbptr, *bcir, *acir, *rptr, *cptr;
	uint32_t ta;//��ʱ���a
	uint64_t carry;//���汾λ�ͽ�λ
	BND tempr;//�洢���ս����������������BN���ȣ�2048λ����������ˣ�
	memset(tempr, 0, sizeof(tempr));
	memset(result, 0, sizeof(result));
	if (DIGITS_B(a) == 0 || DIGITS_B(b) == 0)//��û�г���Ϊ0��Ͷ��ȡ��
	{
		result[0] = 0; result[1] = 0;//���Ϊ0
		return flag;
	}
	if (DIGITS_B(a) < DIGITS_B(b))//���a�ĳ��ȱ�b�Ķ̣��ͼӼ�һ������
	{
		aptr = b;
		bptr = a;
	}
	else
	{
		aptr = a;
		bptr = b;
	}
	maptr = aptr + *aptr;//ָ�������Чλ
	mbptr = bptr + *bptr;//ָ�������Чλ

	carry = 0;
	ta = *LSDPTR_B(aptr);//ĩλ
	//���(bn-1bn-2...b1)��a0
	for (bcir = LSDPTR_B(bptr), rptr = LSDPTR_B(tempr); bcir <= mbptr;
		bcir++, rptr++)
	{
		carry = (uint64_t)ta * (uint64_t)*bcir + (uint64_t)(uint32_t)(carry >> BITPERDIGIT);
		*rptr = (uint32_t)carry;

	}
	*rptr = (uint32_t)(carry >> BITPERDIGIT);//���һ�εĽ�λ
	//ѭ��,a[0]�Ѿ�����
	for (cptr = LSDPTR_B(tempr) + 1, acir = LSDPTR_B(aptr) + 1;
		acir <= maptr; cptr++, acir++)
	{
		carry = 0;
		ta = *acir;

		for (bcir = LSDPTR_B(bptr), rptr = cptr;
			bcir <= mbptr; bcir++, rptr++)
		{
			carry = (uint64_t)ta * (uint64_t)*bcir + (uint64_t)*rptr + (uint64_t)(uint32_t)(carry >> BITPERDIGIT);
			*rptr = (uint32_t)carry;
		}
		*rptr = (uint32_t)(carry >> BITPERDIGIT);//���һ�εĽ�λ
	}
	SETDIGITS_B(tempr, DIGITS_B(aptr) + DIGITS_B(bptr));//������a+b��λ��
	RMLDZRS_B(tempr);//ȥ��ǰ��0
	cpy_b(result, tempr);
	return flag;
}

void mult(BN aa_l, BN bb_l, BN p_l) /* Allow for double length result    */
{
	uint32_t *cptr_l, *bptr_l;
	uint32_t *a_l, *b_l, *aptr_l, *csptr_l, *msdptra_l, *msdptrb_l;
	uint32_t av;
	uint64_t carry;

	if (aa_l[0] == 0 || bb_l[0] == 0)
	{
		SETZERO_B(p_l);
		return;
	}

	if (DIGITS_B(aa_l) < DIGITS_B(bb_l))
	{
		a_l = bb_l;
		b_l = aa_l;
	}
	else
	{
		a_l = aa_l;
		b_l = bb_l;
	}

	msdptra_l = MSDPTR_B(a_l);
	msdptrb_l = MSDPTR_B(b_l);

	carry = 0;
	av = *LSDPTR_B(a_l);
	for (bptr_l = LSDPTR_B(b_l), cptr_l = LSDPTR_B(p_l); bptr_l <= msdptrb_l; bptr_l++, cptr_l++)
	{
		*cptr_l = (uint32_t)(carry = (uint64_t)av * (uint64_t)*bptr_l +
			(uint64_t)(uint32_t)(carry >> BITPERDIGIT));
	}
	*cptr_l = (uint32_t)(carry >> BITPERDIGIT);

	for (csptr_l = LSDPTR_B(p_l) + 1, aptr_l = LSDPTR_B(a_l) + 1; aptr_l <= msdptra_l; csptr_l++, aptr_l++)
	{
		carry = 0;
		av = *aptr_l;
		for (bptr_l = LSDPTR_B(b_l), cptr_l = csptr_l; bptr_l <= msdptrb_l; bptr_l++, cptr_l++)
		{
			*cptr_l = (uint32_t)(carry = (uint64_t)av * (uint64_t)*bptr_l +
				(uint64_t)*cptr_l + (uint64_t)(uint32_t)(carry >> BITPERDIGIT));
		}
		*cptr_l = (uint32_t)(carry >> BITPERDIGIT);
	}

	SETDIGITS_B(p_l, DIGITS_B(a_l) + DIGITS_B(b_l));
	RMLDZRS_B(p_l);
}

//ģ��
void modmul(BN a, BN b, BN n, BN & result)
{
	//cout << "modmul��n = " << bn2str(n) << endl;
	memset(result, 0, sizeof(result));
	BN a_t = { 0 }, b_t = { 0 };
	BND temp = { 0 };
	if (cmp_b(a, ZERO_BN) == 0 || cmp_b(b, ZERO_BN) == 0)
	{
		cmp_b(result, ZERO_BN);
		return;
	}
	cpy_b(a_t, a);
	cpy_b(b_t, b);

	mul(a_t, b_t, temp);

	modn_b(temp, n, result);

}

int div_ini(BN a, BN b, BN  q, BN & rem)
{
	//cout << "okd" << endl;
	memset(rem, 0, BNSIZE);
	memset(q, 0, BNSIZE);
	uint32_t *rptr, *bptr;
	BN b_t;//��ʱ���b
	uint32_t r_t[2 + (BNMAXDGT << 1)];//������2048λ��һ��ʲô�ģ�����Ҫ��1λ�泤�ȣ�һλd*a���ܱ�a��һλ
	uint32_t *qptr, *mbptr, *mrptr, *lrptr;
	uint32_t bv, rv, qh, ri, ri_1, ri_2;//q^,ri,ri-1,ri-2
	uint32_t bn_1, bn_2;//bn-1  bn-2
	uint64_t right, left, rh, borrow, carry, sminus;

	uint32_t d = 0;//��Ҫ��λ����λ��2^d�η���ʾ����d
	cpy_b(r_t, a);
	cpy_b(b_t, b);
	if (DIGITS_B(b) == 0)//���b��0,��0����
		return FLAG_DIVZERO;
	else if (DIGITS_B(r_t) == 0)//���a=0
	{
		SETZERO_B(q);
		SETZERO_B(rem);
		return FLAG_OK;
	}
	else if (cmp_b(r_t, b_t) == -1)//���a<b,����a�ͺ���
	{
		cpy_b(rem, r_t);
		SETZERO_B(q);//��Ϊ0
		return FLAG_OK;
	}
	else if (cmp_b(r_t, b_t) == 0)//���a=b,����1�ͺ���
	{
		q[0] = 1; q[1] = 1;//��Ϊ1
		SETZERO_B(rem);//����Ϊ0
		return FLAG_OK;
	}
	else if (DIGITS_B(r_t) == 0)//���a=0���ǳ���
	{
		SETZERO_B(q);
		SETZERO_B(rem);
		return FLAG_OK;
	}
	//���������a>b...������,����Knuth���м�Сqi�ķ�������qi��Ȼ��ƴ����Q��ʣ�µ�����r
	if (DIGITS_B(b_t) == 1)//�������ֻ��һλ���Ͳ�����ô������
	{

		rv = 0;
		bv = *LSDPTR_B(b_t);
		for (rptr = MSDPTR_B(r_t), qptr = q + DIGITS_B(r_t);
			rptr >= LSDPTR_B(r_t); rptr--, qptr--)
		{
			*qptr = (uint32_t)((rh = ((((uint64_t)rv) << BITPERDIGIT) + (uint64_t)*rptr)) / bv);
			rv = (uint32_t)(rh - (uint64_t)bv * (uint64_t)*qptr);
		}
		SETDIGITS_B(q, DIGITS_B(r_t));
		RMLDZRS_B(q);

		SETDIGITS_B(rem, 1);
		rem[1] = rv;
		RMLDZRS_B(rem);

		return FLAG_OK;
	}

	mbptr = MSDPTR_B(b_t);//�������λ
	bn_1 = *mbptr;
	while (bn_1 < (BASE / 2))
	{
		d++;
		bn_1 = bn_1 << 1;
	}
	sminus = (int)(BITPERDIGIT - d);
	if (d > 0)
	{
		bn_1 = bn_1 + (*(mbptr - 1) >> sminus);
		if (DIGITS_B(b_t) > 2)//b����2λ
		{
			bn_2 = (uint32_t)(*(mbptr - 1) << d) + (*(mbptr - 2) >> sminus);
		}
		else
		{
			bn_2 = (uint32_t)(*(mbptr - 1) << d);
		}
	}
	else
	{
		bn_2 = (uint32_t)(*(mbptr - 1));
	}

	mbptr = MSDPTR_B(b_t);
	mrptr = MSDPTR_B(r_t) + 1;
	lrptr = MSDPTR_B(r_t) - DIGITS_B(b_t) + 1; //���뵽ak���λ��,���֮ǰ�Ķ���Ҫ����������
	*mrptr = 0;//����a^��a����һλ�����԰���λ����Ϊ0

	qptr = q + DIGITS_B(r_t) - DIGITS_B(b_t) + 1;//�����̵����λ

	while (lrptr >= LSDPTR_B(r_t))
	{
		ri = (uint32_t)((*mrptr << d) + (*(mrptr - 1) >> sminus));
		ri_1 = (uint32_t)((*mrptr - 1) << d) + (*(mrptr - 2) >> sminus);
		if (mrptr - 3 > r_t)//�������������3λ
		{
			ri_2 = (uint32_t)((*(mrptr - 2) << d) + (*(mrptr - 3) >> sminus));
		}
		else //������ֻ��3λ
		{
			ri_2 = (uint32_t)(*(mrptr - 2) << d);
		}

		if (ri != bn_1)//�Ƿ����
		{
			qh = (uint32_t)((rh = ((uint64_t)ri << BITPERDIGIT) + (uint64_t)ri_1) / bn_1);
			right = ((rh = (rh - (uint64_t)bn_1*qh)) << BITPERDIGIT) + ri_2;

			if ((left = (uint64_t)bn_2*qh) > right)
			{
				qh--;
				if ((rh + bn_1) < BASE)
				{
					if ((left - bn_2) > (right + ((uint64_t)bn_1 << BITPERDIGIT)))
					{
						qh--;
					}
				}
			}
		}
		else //ri==bn_1
		{
			qh = ALLONE;
			right = ((uint64_t)(rh = (uint64_t)bn_1 + (uint64_t)ri_1) << BITPERDIGIT) + ri_2;
			if (rh < BASE)
			{
				if ((left = (uint64_t)bn_2*qh) > right)
				{
					qh--;
					if ((rh + bn_1) < BASE)
					{
						if ((left - bn_2) > (right + ((uint64_t)bn_1 << BITPERDIGIT)))
						{
							qh--;
						}
					}
				}
			}
		}
		borrow = BASE;
		carry = 0;
		for (bptr = LSDPTR_B(b_t), rptr = lrptr;
			bptr <= mbptr; bptr++, rptr++)
		{
			if (borrow >= BASE)
			{
				*rptr = (uint32_t)(borrow = ((uint32_t)*rptr + BASE - (uint64_t)(uint32_t)(carry =
					(uint64_t)*bptr * qh + (uint64_t)(uint32_t)(carry >> BITPERDIGIT))));
			}
			else
			{
				*rptr = (uint32_t)(borrow = ((uint64_t)*rptr + ALLONE -
					(uint64_t)(uint32_t)(carry = (uint64_t)*bptr * qh + (uint64_t)(uint32_t)(carry >> BITPERDIGIT))));
			}
		}
		if (borrow >= BASE)
		{
			*rptr = (uint32_t)(borrow = ((uint64_t)*rptr + BASE - (uint64_t)(uint32_t)(carry >> BITPERDIGIT)));
		}
		else
		{
			*rptr = (uint32_t)(borrow = ((uint64_t)*rptr + ALLONE - (uint64_t)(uint32_t)(carry >> BITPERDIGIT)));
		}
		*qptr = qh;//���̴�����

		if (borrow < BASE)
		{
			carry = 0;
			for (bptr = LSDPTR_B(b_t), rptr = lrptr; bptr <= mbptr; bptr++, rptr++)
			{
				*rptr = (uint32_t)(carry = ((uint64_t)*rptr + (uint64_t)(*bptr) + (uint64_t)(uint32_t)(carry >> BITPERDIGIT)));
			}
			*rptr = *rptr + (uint32_t)(carry >> BITPERDIGIT);
			*qptr = *qptr - 1;
		}
		mrptr--;
		lrptr--;
		qptr--;
	}
	SETDIGITS_B(q, DIGITS_B(r_t) - DIGITS_B(b_t) + 1);
	RMLDZRS_B(q);

	SETDIGITS_B(r_t, DIGITS_B(b_t));
	cpy_b(rem, r_t);
	//cout << "before rml digits rem=" << DIGITS_B(rem) << endl;
	RMLDZRS_B(rem);
	//cout << "after digits rem=" << DIGITS_B(rem) << endl;
	return FLAG_OK;
}

int div_d(BN d1_l, BN d2_l, BN quot_l, BN rem_l)
{
	register uint32_t *rptr_l, *bptr_l;
	BN b_l;
	uint32_t r_l[2 + (BNMAXDGT << 1)]; /* Provide space for double long */
										 /* dividend + 1 digit */
	uint32_t *qptr_l, *msdptrb_l, *msdptrr_l, *lsdptrr_l;
	uint32_t bv, rv, qhat, ri, ri_1, ri_2, bn_1, bn_2;
	uint64_t right, left, rhat, borrow, carry, sbitsminusd;
	unsigned int d = 0;
	int i;

	cpy_b(r_l, d1_l);
	cpy_b(b_l, d2_l);

	if (cmp_b(b_l,ZERO_BN)==0)
	{
		PURGEVARS_L((1, sizeof(r_l), r_l));
		ISPURGED_L((1, sizeof(r_l), r_l));

		return FLAG_DIVZERO;          /* Division by Zero */
	}

	if (cmp_b(r_l, ZERO_BN) == 0)
	{
		SETZERO_B(quot_l);
		SETZERO_B(rem_l);

		PURGEVARS_L((1, sizeof(b_l), b_l));
		ISPURGED_L((1, sizeof(b_l), b_l));

		return FLAG_OK;
	}

	i = cmp_b(r_l, b_l);

	if (i == -1)
	{
		cpy_b(rem_l, r_l);
		SETZERO_B(quot_l);

		PURGEVARS_L((2, sizeof(b_l), b_l,
			sizeof(r_l), r_l));
		ISPURGED_L((2, sizeof(b_l), b_l,
			sizeof(r_l), r_l));
		return FLAG_OK;
	}
	else if (i == 0)
	{
		SETONEBIT_B(quot_l,1U);
		SETZERO_B(rem_l);

		PURGEVARS_L((2, sizeof(b_l), b_l,
			sizeof(r_l), r_l));
		ISPURGED_L((2, sizeof(b_l), b_l,
			sizeof(r_l), r_l));
		return FLAG_OK;
	}

	if (DIGITS_B(b_l) == 1)
	{
		goto shortdiv;
	}

	/* Step 1 */
	msdptrb_l = MSDPTR_B(b_l);

	bn_1 = *msdptrb_l;
	while (bn_1 < BASE/2)
	{
		d++;
		bn_1 <<= 1;
	}

	sbitsminusd = (int)BITPERDIGIT - d;

	if (d > 0)
	{
		bn_1 += *(msdptrb_l - 1) >> sbitsminusd;

		if (DIGITS_B(b_l) > 2)
		{
			bn_2 = (uint32_t)((*(msdptrb_l - 1) << d) + (*(msdptrb_l - 2) >> sbitsminusd));
		}
		else
		{
			bn_2 = (uint32_t)(*(msdptrb_l - 1) << d);
		}
	}
	else
	{
		bn_2 = (uint32_t)(*(msdptrb_l - 1));
	}

	/* Steps 2 and 3 */
	msdptrr_l = MSDPTR_B(r_l) + 1;
	lsdptrr_l = MSDPTR_B(r_l) - DIGITS_B(b_l) + 1;
	*msdptrr_l = 0;

	qptr_l = quot_l + DIGITS_B(r_l) - DIGITS_B(b_l) + 1;

	/* Step 4 */
	while (lsdptrr_l >= LSDPTR_B(r_l))
	{
		ri = (uint32_t)((*msdptrr_l << d) + (*(msdptrr_l - 1) >> sbitsminusd));

		ri_1 = (uint32_t)((*(msdptrr_l - 1) << d) + (*(msdptrr_l - 2) >> sbitsminusd));

		if (msdptrr_l - 3 > r_l)
		{
			ri_2 = (uint32_t)((*(msdptrr_l - 2) << d) + (*(msdptrr_l - 3) >> sbitsminusd));
		}
		else
		{
			ri_2 = (uint32_t)(*(msdptrr_l - 2) << d);
		}

		if (ri != bn_1)               /* almost always */
		{
			qhat = (uint32_t)((rhat = ((uint64_t)ri << BITPERDIGIT) + (uint64_t)ri_1) / bn_1);
			right = ((rhat = (rhat - (uint64_t)bn_1 * qhat)) << BITPERDIGIT) + ri_2;

			/* test qhat */

			if ((left = (uint64_t)bn_2 * qhat) > right)
			{
				qhat--;
				if ((rhat + bn_1) < BASE)
					/* else bn_2 * qhat < rhat * b_l */
				{
					if ((left - bn_2) > (right + ((uint64_t)bn_1 << BITPERDIGIT)))
					{
						qhat--;
					}
				}
			}
		}
		else                        /* ri == bn_1, almost never */
		{
			qhat = ALLONE;
			right = ((uint64_t)(rhat = (uint64_t)bn_1 + (uint64_t)ri_1) << BITPERDIGIT) + ri_2;
			if (rhat < BASE)       /* else bn_2 * qhat < rhat * b_l */
			{
				/* test qhat */

				if ((left = (uint64_t)bn_2 * qhat) > right)
				{
					qhat--;
					if ((rhat + bn_1) < BASE)
						/* else bn_2 * qhat < rhat * b_l */
					{
						if ((left - bn_2) > (right + ((uint64_t)bn_1 << BITPERDIGIT)))
						{
							qhat--;
						}
					}
				}
			}
		}

		/* Step 5 */
		borrow = BASE;
		carry = 0;
		for (bptr_l = LSDPTR_B(b_l), rptr_l = lsdptrr_l; bptr_l <= msdptrb_l; bptr_l++, rptr_l++)
		{
			if (borrow >= BASE)
			{
				*rptr_l = (uint32_t)(borrow = ((uint64_t)(*rptr_l) + BASE -
					(uint64_t)(uint32_t)(carry = (uint64_t)(*bptr_l) *
						qhat + (uint64_t)(uint32_t)(carry >> BITPERDIGIT))));
			}
			else
			{
				*rptr_l = (uint32_t)(borrow = ((uint64_t)(*rptr_l) + ALLONEL -
					(uint64_t)(uint32_t)(carry = (uint64_t)(*bptr_l) *
						qhat + (uint64_t)(uint32_t)(carry >> BITPERDIGIT))));
			}
		}

		if (borrow >= BASE)
		{
			*rptr_l = (uint32_t)(borrow = ((uint64_t)(*rptr_l) + BASE -
				(uint64_t)(uint32_t)(carry >> BITPERDIGIT)));
		}
		else
		{
			*rptr_l = (uint32_t)(borrow = ((uint64_t)(*rptr_l) + ALLONEL -
				(uint64_t)(uint32_t)(carry >> BITPERDIGIT)));
		}

		/* Step 6 */
		*qptr_l = qhat;

		if (borrow < BASE)
		{
			carry = 0;
			for (bptr_l = LSDPTR_B(b_l), rptr_l = lsdptrr_l; bptr_l <= msdptrb_l; bptr_l++, rptr_l++)
			{
				*rptr_l = (uint32_t)(carry = ((uint64_t)(*rptr_l) + (uint64_t)(*bptr_l) +
					(uint64_t)(uint32_t)(carry >> BITPERDIGIT)));
			}
			*rptr_l += (uint32_t)(carry >> BITPERDIGIT);
			(*qptr_l)--;
		}

		/* Step 7 */
		msdptrr_l--;
		lsdptrr_l--;
		qptr_l--;
	}

	/* Step 8 */
	SETDIGITS_B(quot_l, DIGITS_B(r_l) - DIGITS_B(b_l) + 1);
	RMLDZRS_B(quot_l);

	SETDIGITS_B(r_l, DIGITS_B(b_l));
	cpy_b(rem_l, r_l);

	/* Purging of variables */
	PURGEVARS_L((17, sizeof(bv), &bv,
		sizeof(rv), &rv,
		sizeof(qhat), &qhat,
		sizeof(ri), &ri,
		sizeof(ri_1), &ri_1,
		sizeof(ri_2), &ri_2,
		sizeof(bn_1), &bn_1,
		sizeof(bn_2), &bn_2,
		sizeof(right), &right,
		sizeof(left), &left,
		sizeof(rhat), &rhat,
		sizeof(borrow), &borrow,
		sizeof(carry), &carry,
		sizeof(sbitsminusd), &sbitsminusd,
		sizeof(d), &d,
		sizeof(b_l), b_l,
		sizeof(r_l), r_l));

	ISPURGED_L((17, sizeof(bv), &bv,
		sizeof(rv), &rv,
		sizeof(qhat), &qhat,
		sizeof(ri), &ri,
		sizeof(ri_1), &ri_1,
		sizeof(ri_2), &ri_2,
		sizeof(bn_1), &bn_1,
		sizeof(bn_2), &bn_2,
		sizeof(right), &right,
		sizeof(left), &left,
		sizeof(rhat), &rhat,
		sizeof(borrow), &borrow,
		sizeof(carry), &carry,
		sizeof(sbitsminusd), &sbitsminusd,
		sizeof(d), &d,
		sizeof(b_l), b_l,
		sizeof(r_l), r_l));

	return FLAG_OK;

	/* Division by divisor with one-digit */
shortdiv:

	rv = 0;
	bv = *LSDPTR_B(b_l);
	for (rptr_l = MSDPTR_B(r_l), qptr_l = quot_l + DIGITS_B(r_l); rptr_l >= LSDPTR_B(r_l); rptr_l--, qptr_l--)
	{
		*qptr_l = (uint32_t)((rhat = ((((uint64_t)rv) << BITPERDIGIT) +
			(uint64_t)*rptr_l)) / bv);
		rv = (uint32_t)(rhat - (uint64_t)bv * (uint64_t)*qptr_l);
	}

	SETDIGITS_B(quot_l, DIGITS_B(r_l));

	RMLDZRS_B(quot_l);
	SETONEBIT_B(rem_l, rv);

	/* Purging of variables */
	PURGEVARS_L((4, sizeof(rv), &rv,
		sizeof(bv), &bv,
		sizeof(b_l), b_l,
		sizeof(r_l), r_l));

	ISPURGED_L((4, sizeof(rv), &rv,
		sizeof(bv), &bv,
		sizeof(b_l), b_l,
		sizeof(r_l), r_l));

	return FLAG_OK;
}

//#include "D:\homework and study\����ѧ\��������ҵ\bignum.h"
int div_laji (BN d1_b, BN d2_b, BN quot_b, BN rem_b)
{
	register uint32_t *rptr_b, *bptr_b;
	BN b_b;
	uint32_t r_b[2 + (BNMAXDGT << 1)]; /* Provide space for double long */
										 /* dividend + 1 digit */
	uint32_t *qptr_b, *msdptrb_b, *msdptrr_b, *lsdptrr_b;
	uint32_t bv, rv, qhat, ri, ri_1, ri_2, bn_1, bn_2;
	uint64_t right, left, rhat, borrow, carry, sbitsminusd;
	unsigned int d = 0;
	int i;

	cpy_b(r_b, d1_b);
	cpy_b(b_b, d2_b);

	if (cmp_b(b_b, ZERO_BN) == 0)
	{
		PURGEVARS_L((1, sizeof(r_b), r_b));
		ISPURGED_L((1, sizeof(r_b), r_b));

		return FLAG_DIVZERO;          /* Division by Zero */
	}

	if (cmp_b(r_b, ZERO_BN) == 0)
	{
		SETZERO_B(quot_b);
		SETZERO_B(rem_b);

		PURGEVARS_L((1, sizeof(b_b), b_b));
		ISPURGED_L((1, sizeof(b_b), b_b));

		return FLAG_OK;
	}

	i = cmp_b(r_b, b_b);

	if (i == -1)
	{
		cpy_b(rem_b, r_b);
		SETZERO_B(quot_b);

		PURGEVARS_L((2, sizeof(b_b), b_b,
			sizeof(r_b), r_b));
		ISPURGED_L((2, sizeof(b_b), b_b,
			sizeof(r_b), r_b));
		return FLAG_OK;
	}
	else if (i == 0)
	{
		SETONE_B(quot_b);
		SETZERO_B(rem_b);
		return FLAG_OK;
	}

	if (DIGITS_B(b_b) == 1)
	{
		cout<<"fukc"<<endl;
		goto shortdiv;
	}

	/* Step 1 */
	msdptrb_b = MSDPTR_B(b_b);

	bn_1 = *msdptrb_b;
	while (bn_1 < 80000000U)
	{
		d++;
		bn_1 <<= 1;
	}

	sbitsminusd = (int)BITPERDIGIT - d;

	if (d > 0)
	{
		bn_1 += *(msdptrb_b - 1) >> sbitsminusd;

		if (DIGITS_B(b_b) > 2)
		{
			bn_2 = (uint32_t)((*(msdptrb_b - 1) << d) + (*(msdptrb_b - 2) >> sbitsminusd));
		}
		else
		{
			bn_2 = (uint32_t)(*(msdptrb_b - 1) << d);
		}
	}
	else
	{
		bn_2 = (uint32_t)(*(msdptrb_b - 1));
	}

	/* Steps 2 and 3 */
	msdptrr_b = MSDPTR_B(r_b) + 1;
	lsdptrr_b = MSDPTR_B(r_b) - DIGITS_B(b_b) + 1;
	*msdptrr_b = 0;

	qptr_b = quot_b + DIGITS_B(r_b) - DIGITS_B(b_b) + 1;

	/* Step 4 */
	while (lsdptrr_b >= LSDPTR_B(r_b))
	{
		ri = (uint32_t)((*msdptrr_b << d) + (*(msdptrr_b - 1) >> sbitsminusd));

		ri_1 = (uint32_t)((*(msdptrr_b - 1) << d) + (*(msdptrr_b - 2) >> sbitsminusd));

		if (msdptrr_b - 3 > r_b)
		{
			ri_2 = (uint32_t)((*(msdptrr_b - 2) << d) + (*(msdptrr_b - 3) >> sbitsminusd));
		}
		else
		{
			ri_2 = (uint32_t)(*(msdptrr_b - 2) << d);
		}

		if (ri != bn_1)               /* almost always */
		{
			qhat = (uint32_t)((rhat = ((uint64_t)ri << BITPERDIGIT) + (uint64_t)ri_1) / bn_1);
			right = ((rhat = (rhat - (uint64_t)bn_1 * qhat)) << BITPERDIGIT) + ri_2;

			/* test qhat */

			if ((left = (uint64_t)bn_2 * qhat) > right)
			{
				qhat--;
				if ((rhat + bn_1) < BASE)
					/* else bn_2 * qhat < rhat * b_b */
				{
					if ((left - bn_2) > (right + ((uint64_t)bn_1 << BITPERDIGIT)))
					{
						qhat--;
					}
				}
			}
		}
		else                        /* ri == bn_1, almost never */
		{
			qhat = ALLONE;
			right = ((uint64_t)(rhat = (uint64_t)bn_1 + (uint64_t)ri_1) << BITPERDIGIT) + ri_2;
			if (rhat < BASE)       /* else bn_2 * qhat < rhat * b_b */
			{
				/* test qhat */

				if ((left = (uint64_t)bn_2 * qhat) > right)
				{
					qhat--;
					if ((rhat + bn_1) < BASE)
						/* else bn_2 * qhat < rhat * b_b */
					{
						if ((left - bn_2) > (right + ((uint64_t)bn_1 << BITPERDIGIT)))
						{
							qhat--;
						}
					}
				}
			}
		}

		/* Step 5 */
		borrow = BASE;
		carry = 0;
		for (bptr_b = LSDPTR_B(b_b), rptr_b = lsdptrr_b; bptr_b <= msdptrb_b; bptr_b++, rptr_b++)
		{
			if (borrow >= BASE)
			{
				*rptr_b = (uint32_t)(borrow = ((uint64_t)(*rptr_b) + BASE -
					(uint64_t)(uint32_t)(carry = (uint64_t)(*bptr_b) *
						qhat + (uint64_t)(uint32_t)(carry >> BITPERDIGIT))));
			}
			else
			{
				*rptr_b = (uint32_t)(borrow = ((uint64_t)(*rptr_b) + ALLONEL -
					(uint64_t)(uint32_t)(carry = (uint64_t)(*bptr_b) *
						qhat + (uint64_t)(uint32_t)(carry >> BITPERDIGIT))));
			}
		}

		if (borrow >= BASE)
		{
			*rptr_b = (uint32_t)(borrow = ((uint64_t)(*rptr_b) + BASE -
				(uint64_t)(uint32_t)(carry >> BITPERDIGIT)));
		}
		else
		{
			*rptr_b = (uint32_t)(borrow = ((uint64_t)(*rptr_b) + ALLONEL -
				(uint64_t)(uint32_t)(carry >> BITPERDIGIT)));
		}

		/* Step 6 */
		*qptr_b = qhat;

		if (borrow < BASE)
		{
			carry = 0;
			for (bptr_b = LSDPTR_B(b_b), rptr_b = lsdptrr_b; bptr_b <= msdptrb_b; bptr_b++, rptr_b++)
			{
				*rptr_b = (uint32_t)(carry = ((uint64_t)(*rptr_b) + (uint64_t)(*bptr_b) +
					(uint64_t)(uint32_t)(carry >> BITPERDIGIT)));
			}
			*rptr_b += (uint32_t)(carry >> BITPERDIGIT);
			(*qptr_b)--;
		}

		/* Step 7 */
		msdptrr_b--;
		lsdptrr_b--;
		qptr_b--;
	}

	/* Step 8 */
	SETDIGITS_B(quot_b, DIGITS_B(r_b) - DIGITS_B(b_b) + 1);
	RMLDZRS_B(quot_b);

	SETDIGITS_B(r_b, DIGITS_B(b_b));
	cpy_b(rem_b, r_b);

	/* Purging of variables */
	PURGEVARS_L((17, sizeof(bv), &bv,
		sizeof(rv), &rv,
		sizeof(qhat), &qhat,
		sizeof(ri), &ri,
		sizeof(ri_1), &ri_1,
		sizeof(ri_2), &ri_2,
		sizeof(bn_1), &bn_1,
		sizeof(bn_2), &bn_2,
		sizeof(right), &right,
		sizeof(left), &left,
		sizeof(rhat), &rhat,
		sizeof(borrow), &borrow,
		sizeof(carry), &carry,
		sizeof(sbitsminusd), &sbitsminusd,
		sizeof(d), &d,
		sizeof(b_b), b_b,
		sizeof(r_b), r_b));

	ISPURGED_L((17, sizeof(bv), &bv,
		sizeof(rv), &rv,
		sizeof(qhat), &qhat,
		sizeof(ri), &ri,
		sizeof(ri_1), &ri_1,
		sizeof(ri_2), &ri_2,
		sizeof(bn_1), &bn_1,
		sizeof(bn_2), &bn_2,
		sizeof(right), &right,
		sizeof(left), &left,
		sizeof(rhat), &rhat,
		sizeof(borrow), &borrow,
		sizeof(carry), &carry,
		sizeof(sbitsminusd), &sbitsminusd,
		sizeof(d), &d,
		sizeof(b_b), b_b,
		sizeof(r_b), r_b));

	return FLAG_OK;

	/* Division by divisor with one-digit */
shortdiv:

	rv = 0;
	bv = *LSDPTR_B(b_b);
	for (rptr_b = MSDPTR_B(r_b), qptr_b = quot_b + DIGITS_B(r_b); rptr_b >= LSDPTR_B(r_b); rptr_b--, qptr_b--)
	{
		*qptr_b = (uint32_t)((rhat = ((((uint64_t)rv) << BITPERDIGIT) +
			(uint64_t)*rptr_b)) / bv);
		rv = (uint32_t)(rhat - (uint64_t)bv * (uint64_t)*qptr_b);
	}

	SETDIGITS_B(quot_b, DIGITS_B(r_b));

	RMLDZRS_B(quot_b);
	SETONEBIT_B(rem_b, rv);

	/* Purging of variables */
	PURGEVARS_L((4, sizeof(rv), &rv,
		sizeof(bv), &bv,
		sizeof(b_b), b_b,
		sizeof(r_b), r_b));

	ISPURGED_L((4, sizeof(rv), &rv,
		sizeof(bv), &bv,
		sizeof(b_b), b_b,
		sizeof(r_b), r_b));

	return FLAG_OK;
}


string bn2str(BN bignum)//����תΪ�ַ���
{
	if (DIGITS_B(bignum) == 0)//�����0��Ҫ��ʾ���0
		return string("0");

	char strbignum[265] = { 0 };//(1024+32)/4
	BN temp;
	cpy_b(temp, bignum);

	for (int i = DIGITS_B(temp), j = 0; i > 0; i = i - 1, j++)//�ڼ�λ�������Ǽ�������
	{
		sprintf(&strbignum[8 * j], "%08X", temp[i]);
	}
	//printf("bignum[i]=%08X", temp[DIGITS_B(temp)]);
   // printf("bignum[i]=%08X", temp[DIGITS_B(temp)]);
	//string rmzero(strbignum, strbignum + 7);
	int zeros = 0;
	for (int i = 0; i < 7; i++)
	{
		if (strbignum[i] == '0')
			zeros++;
		else break;
	}
	//cout << "zeros =" << zeros << endl;
	//int index=rmzero.rfind("0");//ȥ�����ü���ǰ��0
	if (zeros >= 1)
	{
		string finalstr(strbignum + zeros, &strbignum[264]);
		char * cstr = new char[finalstr.length() + 1];
		std::strcpy(cstr, finalstr.c_str());
		finalstr = string(cstr);//ȥ��NULL 
		//printf("length of bn2str is %d\n", finalstr.length());
		delete[] cstr;
		return finalstr;
	}
	else
	{
		string finalstr(strbignum);
		char * cstr = new char[finalstr.length() + 1];
		std::strcpy(cstr, finalstr.c_str());
		finalstr = string(cstr);
		//printf("length of bn2str is %d\n", finalstr.length());
		delete[] cstr;
		return finalstr;
		return string(strbignum);
	}
}

int str2bn(BN & bignum, string strbn)//�ַ���ת��Ϊ����
{
	BN t = { 0 };
	char *perbit = new char[9];
	memset(perbit, 0, 9);
	int digits = 0;
	digits = (strbn.length() + 7) / 8;//λ��
	//cout << "digits=" << digits << endl;
	int i = 0;
	int pos = strbn.length() - 8;
	for (i = 0; i < digits - 1; i++)//������digits-1!!!!!
	{
		//printf("i=%d\n", i);
		//temp.insert(0, strbn, pos,8);
		strbn.copy(perbit, 8, pos);
		//printf("perbit=%s\n", perbit);
		t[i + 1] = strtoul(perbit, NULL, 16);
		//temp.erase();
		pos = pos - 8;
		memset(perbit, 0, 9);
	}
	//printf("i=%d\n", i);
	if (strbn.length() % 8 == 0)
		strbn.copy(perbit, 8, 0);
	else
		strbn.copy(perbit, strbn.length() % 8, 0);
	//printf("perbit=%s\n", perbit);
	t[i + 1] = strtoul(perbit, NULL, 16);

	SETDIGITS_B(t, 32);
	cpy_b(bignum, t);
	delete[]perbit;
	return FLAG_OK;
}

int readbn(BN &bignum, string filename)//���ı��ж�ȡ����
{
	BN temp;
	ifstream fin;
	fin.open(filename);
	if (fin.bad())
	{
		cout << "open" << filename << " failed!\n";
		return FLAG_FILE_ERROR;
	}
	string strbignum;
	fin >> strbignum;
	//cout << "lengh is" << strbignum.length() << endl;
	SETDIGITS_B(temp, 32);
	str2bn(temp, strbignum);
	cpy_b(bignum, temp);
	//cout << "������ " << bn2str(temp)<< endl;
	fin.close();
	return FLAG_OK;
}

int writebn(string filename, BN bignum)//д��������ı�
{
	ofstream fout;
	fout.open(filename);
	if (fout.bad())
	{
		cout << "write to " << filename << " failed!\n";
		return FLAG_FILE_ERROR;
	}
	string strbignum = bn2str(bignum);
	int index = strbignum.find(" ");
	//printf("index=%d\n", index);
	if (index >= 0)
	{
		string finalstr;
		finalstr.insert(0, strbignum, 0, index);
		//strbignum[index + 1], &strbignum + strbignum.length() - 1);
		fout << finalstr;
	}
	else
	{
		fout << strbignum;
	}
	fout.close();
	return FLAG_OK;
}

//rem=a mod n
int modn_b(BN a, BN n, BN & rem)
{
	//cout << "OKD" << endl;
	int flag = FLAG_OK;
	BND temp;
	BN result;
	memset(rem, 0, BNSIZE);
	memset(temp, 0, sizeof(temp));
	memset(result, 0, sizeof(result));
	if (cmp_b(a, ZERO_BN) == 0)
		cpy_b(rem, ZERO_BN);
	else
	{

		//flag = div_b(a, n, temp, result);
		flag = div_l(a, n, temp, result);
		cpy_b(rem, result);
		//cout << "in modn_b:\na = " << bn2str(a) << endl; 
		//cout << "n = " << bn2str(n) << endl<<endl;
		//cout << "rem = " << bn2str(result) << endl <<"end of modn"<< endl;

	}

	return flag;
}


//a= bq + rem
int remdiv_b(BN a, BN b, BN & rem)
{
	memset(rem, 0, BNSIZE);
	int flag = FLAG_OK;
	BN q, temp;
	memset(q, 0, BNSIZE);
	memset(temp, 0, BNSIZE);
	//flag= div_b(a, b, q, temp);
	flag = div_l(a, b, q, temp);
	cpy_b(rem, temp);
	return flag;

}

//result=(a,b)
int gcd_b(BN a, BN b, BN & result)
{
	memset(result, 0, BNSIZE);
	int flag = FLAG_OK;

	BN a_t, b_t, rn_1, rn;//�������������
	memset(a_t, 0, BNSIZE);
	memset(b_t, 0, BNSIZE);
	memset(rn, 0, BNSIZE);
	cpy_b(a_t, a);
	cpy_b(b_t, b);
	cpy_b(rn_1, b);//��һrn=0
	if (DIGITS_B(a_t) == 0)
	{
		cpy_b(result, b_t);
		return flag;
	}
	if (DIGITS_B(b_t) == 0)
	{
		cpy_b(result, a_t);
		return flag;
	}

	remdiv_b(a_t, b_t, rn);
	//cout << "rem= " << bn2str(rn) << endl;
	while (DIGITS_B(rn) != 0)
	{
		cpy_b(a_t, b_t);
		cpy_b(b_t, rn);
		//memset(rn_1, 0, BNSIZE);
		cpy_b(rn_1, rn);
		remdiv_b(a_t, b_t, rn);
		RMLDZRS_B(rn);
		//cout<< endl;
		/*cout << "a= " << bn2str(a_t) << endl;
		cout << "b= " << bn2str(b_t)<<endl;
		cout << "rem= " << bn2str(rn) << endl;*/
	}
	cpy_b(result, rn_1);
	return flag;
}

//���(a,n)=1,����ax = 1 (mod n)�������쳣û����Ԫ������0����Ȼ��Ԫ������Ϊ0  
int inv_b(BN a, BN n, BN & x)
{
	memset(x, 0, BNSIZE);
	BN u = { 0 }, g = { 0 }, v1 = { 0 }, v3 = { 0 }, q = { 0 }, t3 = { 0 }, t1 = { 0 };
	BN temp;//�����м���
	gcd_b(a, n, g);
	if (cmp_b(g, ONE_BN) != 0)//��������أ�û����Ԫ
	{
		SETZERO_B(x);
		return FLAG_NOINV;
	}
	memset(g, 0, sizeof(g));
	//��ʼ��
	cpy_b(u, ONE_BN);//u=1
	cpy_b(g, a);//g=a
	SETZERO_B(v1);//v1=0
	cpy_b(v3, n);
	
	do
	{
		//div_b(g, v3, q, t3);
		
		div_l(g, v3, q, t3);
		modmul(q, v1, n, temp);
		modsub_b(u, temp, n, t1);	
		cpy_b(u, v1);
		cpy_b(g, v3);
		cpy_b(v1, t1);
		cpy_b(v3, t3);
		/*cout << "g= " << bn2str(g) << endl;
		cout << "v3= " << bn2str(v3) << endl;
		cout << "q= " << bn2str(q) << endl;
		cout << "t3= " << bn2str(t3) << endl;
		cout << "u= " << bn2str(u) << endl;
		cout << endl;*/
	} while (cmp_b(v3, ZERO_BN) != 0);
	cpy_b(x, u);
	return FLAG_OK;
}

int inverse(BN a, BN n, BN & x)
{
	BN u, v1, v3, t1, t3, q,g;
	memset(u, 0, BNSIZE);
	memset(v1, 0, BNSIZE);
	memset(v3, 0, BNSIZE);
	memset(t1, 0, BNSIZE);
	memset(t3, 0, BNSIZE);
	memset(q, 0, BNSIZE);
	memset(g, 0, BNSIZE);
	BN temp1,temp2;
	if (cmp_b(a, ZERO_BN) == 0|| cmp_b(n, ZERO_BN) == 0)
	{
		SETZERO_B(x);
		return FLAG_NOINV;
	}
	SETONEBIT_B(u, 1);
	cpy_b(g, a);
	SETZERO_B(v1);
	cpy_b(v3, n);
	
	do
	{
		/*cout << "t3= " << bn2str(t3) << endl;
		cout << "u= " << bn2str(u) << endl << endl;*/
		div_l(g, v3, q, t3);
		///*cout << "g= " << bn2str(g) << endl;
		//cout << "v3= " << bn2str(v3) << endl;
		//cout << "q= " << bn2str(q) << endl;*/
		modmul(q, v1, n, temp1);
		modsub_b(u, temp1, n, t1);
		cpy_b(u, v1);
		cpy_b(g, v3);
		cpy_b(v1, t1);
		cpy_b(v3, t3);

		
	} while(cmp_b(v3,ZERO_BN)!=0);
	cpy_b(x, u);
	return FLAG_OK;
}

//result= b^n (mod m)
int modexp_b(BN b, BN n, BN m, BN & result)
{
	//cout << "a = " << bn2str(b) << endl;
	//cout << "b = " << bn2str(n) << endl;
	//cout << "m = " << bn2str(m) << endl;
	memset(result, 0, sizeof(result));
	BN a_t = { 1,1 }, b_t;//a=1;n���˶�����չ��
	BN temp1 = { 0 }, temp2 = { 0 };//������Ϊresult�и��������
	cpy_b(b_t, b);//b_t=b,��ʼ��
	uint32_t *nptr, *mnptr;
	nptr = LSDPTR_B(n);
	mnptr = MSDPTR_B(n);//��������������
	char binform[33];//ÿ��32bit��uint32ת��Ϊ�����Ƽ��ɣ�һ�δ�ȡ����
	int i = 0;
	//cout << "\na= " << bn2str(a_t) << "  b= " << bn2str(b_t) << endl << endl;
	while (nptr <= mnptr)//ûԽ��Ͷ�����
	{

		memset(binform, 0, sizeof(binform));
		_ultoa(*nptr, binform, 2);
		//printf("binform=%s\n", binform);
		i = strlen(binform) - 1;//�������һλ
		for (int j = 31; j >= 0; j--)//��ʼģƽ��
		{
			//cout << "\nai-1= " << bn2str(a_t) << "  bi-1= " << bn2str(b_t) << endl;
			if (i >= 0)//���¶�������ֻ��ƽ��bȡģ
			{
				//cout <<endl<<31-j<< ": ai-1= " << bn2str(a_t) << "  bi-1= " << bn2str(b_t) << endl;
				//printf("i=%d : %c\n", i, binform[i]);
				if (binform[i] == '1')
				{


					modmul(a_t, b_t, m, temp1);//a=a*b mod m
					//modmul(b_t, b_t, m, temp2);//b=b*b mod m

					cpy_b(a_t, temp1);
					//cpy_b(b_t, temp2);
					//cout << "i=" << i << endl;
				}
				//printf("i=%d : %c\n", i, binform[i]);

				//��������²��ö�a_t
				i--;
			}
			//cout << "j=" << j << endl;

			modmul(b_t, b_t, m, temp2);//b=b*b mod m		
			cpy_b(b_t, temp2);
			//cout <<endl<<31-j<< ": ai= " << bn2str(a_t) << "  bi= " << bn2str(b_t) << endl << endl;

		}
		nptr++;
	}
	cpy_b(result, a_t);
	return FLAG_OK;
}

int fermat_b(BN a)
{
	BN n2 = { 1,2 };
	BN n3 = { 1,3 };
	BN n5 = { 1,5 };
	BN n7 = { 1,7 };

	BN temp1, temp2;

	if (a[1] % 2 == 0 || a[1] % 5 == 0)
		return 0;//��

	gcd_b(a, n2, temp1);
	if (temp1[0] > 1 || temp1[1] != 1)
		return 0;


	gcd_b(a, n3, temp1);
	if (temp1[0] > 1 || temp1[1] != 1)
		return 0;

	gcd_b(a, n5, temp1);
	if (temp1[0] > 1 || temp1[1] != 1)
		return 0;
	gcd_b(a, n7, temp1);

	if (temp1[0] > 1 || temp1[1] != 1)
		return 0;

	sub_b(a, ONE_BN, temp1);//temp1=a-1

	modexp_b(n2, temp1, a, temp2);// n2^(a-1) mod a!=1�ͳ���
	if (temp2[0] > 1 || temp2[1] != 1)
		return 0;
	//cout << "here" << endl;
	modexp_b(n3, temp1, a, temp2);// n2^(a-1) mod a!=1�ͳ���
	if (temp2[0] > 1 || temp2[1] != 1)
		return 0;

	modexp_b(n5, temp1, a, temp2);// n2^(a-1) mod a!=1�ͳ���
	if (temp2[0] > 1 || temp2[1] != 1)
		return 0;

	modexp_b(n7, temp1, a, temp2);// n2^(a-1) mod a!=1�ͳ���
	if (temp2[0] > 1 || temp2[1] != 1)
		return 0;

	return 1;//������
}

/*
a^b = b1 mod p;   a^b = b2 mod q
x=b1 mod p
x=b2 mod q
m1=p;m2=q  m=m1*m2;
M1=m2=q   M2=m1=p
M1*M1'=1 mod p    M2*M2'=1  mod q
result= b1*M1'*M1 + b2* M2'*M2  mod(m)
*/
void crt_b(BN a, BN b, BN p, BN q, BN & result)//����a^b mod (p*q)
{
	BN b1, b2, M1p, M2p, m;
	BN tb1, tb2, pdec1, qdec1;//ģ��ŷ�������Ժ�ķֱ��ָ��
	subuint_b(p, 1, pdec1);
	subuint_b(q, 1, qdec1);

	modn_b(b, pdec1, tb1);
	modn_b(b, qdec1, tb2);

	mul_b(p, q, m);
	modexp_b(a, tb1, p, b1);
	modexp_b(a, tb2, q, b2);
	inv_b(q, p, M1p);
	inv_b(p, q, M2p);
	BN temp1, temp2, temp3;
	modmul(b1, M1p, m, temp1);
	modmul(temp1, q, m, temp2);

	modmul(b2, M2p, m, temp1);
	modmul(temp1, p, m, temp3);

	modadd_b(temp2, temp3, m, result);
	RMLDZRS_B(result);

}

/*
H=ǰ50�������˻������gcd(p,50)=x,x>1����ô�����x=7��p+2*3*5�϶�����2 3 5�ģ�Ȼ���ٴ�ͷ��Ū  
��� p+2*3*5���ص�gcd��2/3/5���ǻ���һ����֮ǰ�ļӷ���Ҫ2/3/5�е�һ����������
*/

void exclu()
{
	//ǰ50������
	unsigned int prime[50] =
	{
		2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,113,
		127,131,137,139,149,151,157,163,167,173,179,181,191,193,197,199,211,223,227,229
	};
	BN temp1 = { 1,1 },temp2,temp3;
	for (int i = 0; i < 50; i++)
	{
		SETONEBIT_B(temp2, prime[i]);
		
		mul_b(temp1, temp2, temp3);
		
		cpy_b(temp1, temp3);
		RMLDZRS_B(temp1);
		}
	writebn("50primefac.txt",temp1);
	//uint64_t fac = 1;
	//for (int i = 0; i < 50; i++)
	//{
	//	fac = fac * prime[i];
	//}
	//cout << fac << endl;
}

uint32_t ld_l(BN n_l)
{
	unsigned int l;
	uint32_t test;

	l = (unsigned int)DIGITS_B(n_l);
	while (n_l[l] == 0 && l > 0)
	{
		--l;
	}

	if (l == 0)
	{
		return 0;
	}

	test = n_l[l];
	l <<= LDBITPERDGT;

	while ((test & BASE/2) == 0)
	{
		test <<= 1;
		--l;
	}

	/* Purging of variables */
	PURGEVARS_L((1, sizeof(test), &test));
	ISPURGED_L((1, sizeof(test), &test));

	return l;
}

//����1λ
int shr_l(BN a_l)
{
	uint32_t *ap_l;
	uint32_t help, carry = 0;

	if (DIGITS_B(a_l) == 0)
	{
		return FLAG_UF;          /* Underflow */
	}

	for (ap_l = MSDPTR_B(a_l); ap_l > a_l; ap_l--)
	{
		help = (uint32_t)((uint32_t)(*ap_l >> 1) | (uint32_t)(carry << (BITPERDIGIT - 1)));
		carry = (uint32_t)(*ap_l & 1U);
		*ap_l = help;
	}

	RMLDZRS_B(a_l);

	/* Purging of variables */
	PURGEVARS_L((2, sizeof(help), &help,
		sizeof(carry), &carry));

	ISPURGED_L((2, sizeof(help), &help,
		sizeof(carry), &carry));

	return FLAG_OK;
}
//����1λ
int shl_l(BN a_l)
{
	uint32_t *ap_l, *msdptra_l;
	uint64_t carry = 0L;
	int error =FLAG_OK;

	RMLDZRS_B(a_l);
	if (ld_l(a_l) >= (uint32_t)BNMAXBIT)
	{
		SETDIGITS_B(a_l, BNMAXDGT);
		error = FLAG_OF;         /* Overflow */
	}

	msdptra_l = MSDPTR_B(a_l);
	for (ap_l = LSDPTR_B(a_l); ap_l <= msdptra_l; ap_l++)
	{
		*ap_l = (uint32_t)(carry = ((uint64_t)(*ap_l) << 1) | (carry >> BITPERDIGIT));
	}

	if (carry >> BITPERDIGIT)
	{
		if (DIGITS_B(a_l) <BNMAXDGT)
		{
			*ap_l = 1;
			INCDIGITS_B(a_l);
			error = FLAG_OK;
		}
		else
		{
			error =FLAG_OF;
		}
	}

	RMLDZRS_B(a_l);

	/* Purging of variables */
	PURGEVARS_L((1, sizeof(carry), &carry));
	ISPURGED_L((1, sizeof(carry), &carry));

	return error;
}
//������λ
int shift_b(BN n_l, long int noofbits)
{
	uint32_t shorts = (uint32_t)((uint64_t)(noofbits < 0 ? -noofbits : noofbits) / BITPERDIGIT);
	uint32_t bits = (uint32_t)((uint64_t)(noofbits < 0 ? -noofbits : noofbits) % BITPERDIGIT);
	long int resl;
	uint32_t i;
	int error = FLAG_OK;

	uint32_t *nptr_l;
	uint32_t *msdptrn_l;

	RMLDZRS_B(n_l);
	resl = (int)ld_l(n_l) + noofbits;

	if (DIGITS_B(n_l) == 0)
	{
		shorts = bits = 0;
		return ((resl < 0) ? FLAG_UF : FLAG_OK);
	}

	if (noofbits == 0)
	{
		return FLAG_OK;
	}

	if ((resl < 0) || (resl > (long)BNMAXBIT))
	{
		error = ((resl < 0) ? FLAG_OF :FLAG_OK);   /* Under-/Overflow */
	}

	SETDIGITS_B(n_l, MIN(DIGITS_B(n_l), BNMAXDGT));

	if (noofbits < 0)
	{

		/* Shift Right */

		shorts = (uint32_t)MIN(DIGITS_B(n_l), shorts);
		msdptrn_l = MSDPTR_B(n_l) - shorts;
		for (nptr_l = LSDPTR_B(n_l); nptr_l <= msdptrn_l; nptr_l++)
		{
			*nptr_l = *(nptr_l + shorts);
		}
		SETDIGITS_B(n_l, DIGITS_B(n_l) - shorts);

		for (i = 0; i < bits; i++)
		{
			shr_l(n_l);
		}
	}
	else
	{

		/* Shift Left   */

		if (shorts < BNMAXDGT)
		{
			SETDIGITS_B(n_l, MIN((uint32_t)(DIGITS_B(n_l) + shorts), BNMAXDGT));
			nptr_l = n_l + DIGITS_B(n_l);
			msdptrn_l = n_l + shorts;
			while (nptr_l > msdptrn_l)
			{
				*nptr_l = *(nptr_l - shorts);
				--nptr_l;
			}

			while (nptr_l > n_l)
			{
				*nptr_l-- = 0;
			}

			RMLDZRS_B(n_l);
			for (i = 0; i < bits; i++)
			{
				shl_l(n_l);
			}
		}
		else
		{
			SETZERO_B(n_l);
		}
	}

	/* Purging of variables */
	PURGEVARS_L((3, sizeof(resl), &resl,
		sizeof(shorts), &shorts,
		sizeof(bits), &bits));

	ISPURGED_L((3, sizeof(resl), &resl,
		sizeof(shorts), &shorts,
		sizeof(bits), &bits));

	return error;
}

int getbits_b(BN a)
{
	BND a_t = { 0 };
	cpy_b(a_t, a);
	int bits = 0;
	uint32_t mb = 0;
	uint32_t bit[32] = { 0 };
	bit[0] = 0x0U;
	bit[1] = 0x2U;
	bit[2] = 0x4U;
	bit[3] = 0x8U;
	bit[4] = 0x10U;
	bit[5] = 0x20U;
	bit[6] = 0x40U;
	bit[7] = 0x80U;
	bit[8] = 0x100U;
	bit[9] = 0x200U;
	bit[10] = 0x400U;
	bit[11] = 0x800U;
	bit[12] = 0x1000U;
	bit[13] = 0x2000U;
	bit[14] = 0x4000U;
	bit[15] = 0x8000U;
	bit[16] = 0x10000U;
	bit[17] = 0x20000U;
	bit[18] = 0x40000U;
	bit[19] = 0x80000U;
	bit[20] = 0x100000U;
	bit[21] = 0x200000U;
	bit[22] = 0x400000U;
	bit[23] = 0x800000U;
	bit[24] = 0x1000000U;
	bit[25] = 0x2000000U;
	bit[26] = 0x4000000U;
	bit[27] = 0x8000000U;
	bit[28] = 0x10000000U;
	bit[29] = 0x20000000U;
	bit[30] = 0x40000000U;
	bit[31] = 0x80000000U;
	if (DIGITS_B(a_t) == 0)
		bits = 0;
	else
	{
		bits = (DIGITS_B(a_t) - 1) * 32;
		mb = *MSDPTR_B(a_t);//������λ��ֵ
		for (int i = 0; i < 32; i++)
		{
			if (mb >= bit[i])
			{
				bits++;
			}
			else break;
		}
	}
	
	return bits;
}

int div_l(BN a, BN b, BN q, BN rem)
{
	memset(rem, 0, BNSIZE);
	memset(q, 0, BNSIZE);
	uint32_t *rptr, *bptr,*mbptr,*mrptr;
	BN b_t;//��ʱ���b
	BN tempq = { 0 }, tempsub = { 0 };//����̺���ʱ��
	BN q_t = { 0 };//ÿ����
	uint32_t r_t[2 + (BNMAXDGT << 1)];//������2048λ��һ��ʲô�ģ�����Ҫ��1λ�泤�ȣ�һλd*a���ܱ�a��һλ


	cpy_b(r_t, a);
	cpy_b(b_t, b);
	if (DIGITS_B(b) == 0)//���b��0,��0����
		return FLAG_DIVZERO;
	else if (DIGITS_B(r_t) == 0)//���a=0
	{
		SETZERO_B(q);
		SETZERO_B(rem);
		return FLAG_OK;
	}
	else if (cmp_b(r_t, b_t) == -1)//���a<b,����a�ͺ���
	{
		cpy_b(rem, r_t);
		SETZERO_B(q);//��Ϊ0
		return FLAG_OK;
	}
	else if (cmp_b(r_t, b_t) == 0)//���a=b,����1�ͺ���
	{
		q[0] = 1; q[1] = 1;//��Ϊ1
		SETZERO_B(rem);//����Ϊ0
		return FLAG_OK;
	}
	else if (DIGITS_B(r_t) == 0)//���a=0���ǳ���
	{
		SETZERO_B(q);
		SETZERO_B(rem);
		return FLAG_OK;
	}
	//���������
	SETDIGITS_B(q_t, DIGITS_B(r_t) - DIGITS_B(b_t) + 1);
	int abit = getbits_b(r_t);
	int bbit = getbits_b(b);

	int shiftnum = abit - bbit;
	int subtimes = abit - bbit + 1;
	shift_b(b_t, shiftnum);//���ƶ������λ,�Ժ�ÿ������һλ,�ƶ��Ժ��b_t
	//cout << "a��λ����:" << abit << endl;
	//cout << "b��λ����:" << bbit << endl;
	//cout <<"��������" <<subtimes << endl;
	for (int i = 0; i < subtimes; i++) {

		if (cmp_b(r_t, b_t) >= 0)//�����еȺţ���������
		{
			sub_b(r_t, b_t, tempsub);
			cpy_b(r_t, tempsub);
			//if (*(q_t) > 1)
			shl_l(q_t);
			adduint_b(q_t, 1U, tempq);//��1
			
			cpy_b(q_t, tempq);
			//cout <<"1:  "<< getbits_b(q_t) << endl;
			shr_l(b_t);
			//cout << i<<"  1 :" << bn2str(q_t) << endl;
		}
		else
		{
			shl_l(q_t);//��0
			//cout << "0:  "<<getbits_b(q_t) << endl;
			shr_l(b_t);
			//cout << i << " 0 :" << bn2str(q_t) << endl;
		}
	}
	
	//cout<< bn2str(q_t) << endl;
	cpy_b(q, q_t);
		
	RMLDZRS_B(q);
	//cout << bn2str(q) << endl;

	cpy_b(rem, r_t);
	RMLDZRS_B(rem);

	return FLAG_OK;
}


int main()
{
	BN a, b, sum, result, p,q, rem, temp,n;

	memset(a, 0, BNSIZE);//33*4
	memset(b, 0, BNSIZE);
	memset(sum, 0, BNSIZE);
	memset(result, 0, BNSIZE);
	memset(p, 0, BNSIZE);
	memset(q, 0, BNSIZE);
	memset(rem, 0, BNSIZE);
	//readbn(a, "a.txt");
	//readbn(b, "b.txt");
	//readbn(q, "e.txt");
	//readbn("50primefac.txt", temp1);
	string astr1 = "D7A7E5CF1092C0BC99D72B75663236E8B5D2602E854501ADEE5FFDA15B9A6BE23E068340BBDAE5DEC3764812277752C697A29A4451447A439281F01E4A5ED223ABEAAB9D13A31081CA41CC83705E9A46A56E6EC2F0E20417078850C37D270FF971AAE4CCAE2BE3DD3268A30F54F5A6906ACF364815463D9BC2317A5DD78D62E9";
	string astr2 = "75AA1DB4";
	string astr3 = "B590";
	string bstr1 = "99FF";
	string cstr1 = "9876543210";
	string cstr2 = "100000000000000001";
	string cstr3 = "B";
	string pr = "E3695C708CE57B22281D18A7F61C2C485CF537CD42E4C219377561E3BEEEA3C6E5278707DFE77F60C119187C8126CEE4103596F65213175B7A4CB89D2F4607C61F3D7E108BAA0E12B875C7";
	string sushu = "1063102404746B1B6BB2F34AA0AC43D7866CBE651E0CCC88D51F227763B669739FA5D96DFD54D2DE7E566D5475A66D78876AFAC5DBE1E6CEBEF7191E7FFF98CF";
	string qr = "F2C435C2D744CA2140DE28B2E8C17D80619B2319D39151B041A68110F0D3C1EFE6418B4FCCBE090A021D6DFC9C7E1493F16B0CA1CF";
	//str2bn(q, qstr);
	string pqinv = "99AF08A78B715FBB18FA9D28E8CF5D9A63229C50AD3D7F047BB088102291FF7742AAACD9D276A378F8750817EF417FD2EFE8ED869C";
	
	string socallinv = "E0F012FE565D522FF7CDDABE2344E24970E1B32A2D7A56A8119C6D3D4BAB986BE0ED084E584973AD554F543A22690D65EC6484F81726C49C607F34EF45D346DE0756A6A8BABE686B3C9B51";
	string socalmodmul = "99B94624FF273162D";

	string a1 = "16F53E48B8D69793B";
	string b1 = "B535D4C7E524F875";
	string a2 = "A567";
	string b2 = "32";

	str2bn(a, a1);
	str2bn(b, b1);
	str2bn(p, pr);
    str2bn(q, qr);
	str2bn(temp, cstr3);
	str2bn(n, astr1);
	//gcd_b(a, b, result);
    //inv_l(p, q, sum,result);
	//modmul(p, p, q, result);
	//modmul(p, a, q, result);
	//inv_b(p,a,result);
	
	inv_b(a, temp, result);
	cout <<"��Ԫd��:" <<bn2str(result) << endl;
	//modmul(p, result, a, sum);
	//cout << "ģ����:" << bn2str(sum) << endl;
	//div_l(result, a,q, sum);
	//cout << "����:" << bn2str(q) << endl;
	//gcd_b(n, p, result);
	//cout << "gcd��:" << bn2str(result) << endl;

	//modsub_b(p, n, q, result);
	//cout << "ģ����:" << bn2str(result) << endl;
	//modmul(b, result, a, sum);
	//cout << "e=" << bn2str(b) << endl;
	//cout << "n=" << bn2str(a) << endl;
	//cout << "e*d mod n=" << bn2str(sum) << endl;
	//exclu();
	//div_l(n, p,result,rem);
	//div_d(a, b, result, rem);
	/*cout << "������:" << bn2str(n) << endl;
	cout << "������:" << bn2str(p) << endl;
	cout << "����:" << bn2str(result) << endl;
	cout << "������:" << bn2str(rem) << endl;
	cout << "a ��λ����" << getbits_b(n) << endl;*/
	//mul_b(b, result, sum);
	//add_b(sum, rem, q);
	//cout <<endl<< "����1:" << bn2str(b) << endl;
	//cout << "����2:" << bn2str(result) << endl;
	//cout << "����(����1):" << bn2str(sum) << endl;
	//cout << "����2��:" << bn2str(rem) << endl;
	//cout << "�Ӻ���:" << bn2str(q) << endl;
	//crt_b(a, b, q, temp, result);
	//gcd_b(a, b, result);
	//mul_b(a, a, rem);

	//writebn("sum.txt", sum);
	//readbn(rem, "r1.txt");
	//add_b(result, rem, sum);
	//div_l(q, a, result, rem);
	//cout << "a=qb+r,q=" << endl << bn2str(q) << "\nr=  " << bn2str(rem) << endl;
	//cout << "digits r=" << DIGITS_B(rem) << endl;
	//modsub_b(a, b, q, result);
	//writebn("rem.txt", rem)
	;
	//writebn("quo.txt", q);
	//modmul(a,b,q,result);
	//modexp_b(a, b, q, result);
	//modn_b(q, a, result);
	//inv_b(a, b, result);
	//int t = fermat_b(q);
	/*cout << "a is " << bn2str(a) << endl;
	cout << "b is " << bn2str(b) << endl;
	mul_b(q, temp, sum);
	cout << "(mod n) is " << bn2str(sum) << endl;
	cout << "\na^b mod n result is " << bn2str(result) << endl;*/
	//cout << "\na^b mod n rem is " << bn2str(rem) << endl;
	//int t = fermat_b(a);
	//printf("t = %d\n", t);
	//writebn("tr.txt", rem);

	//readbn(q, "q.txt");
	//readbn(rem, "r.txt");

	//mul_b(q, b, result);
	//add_b(result, rem, result);
	//writebn("sum.txt", result);
	//string t1 = "abcdeabcdeabcdefab";
	//string t2 = "12345678000123";
	//BN d = { 0 };
	//str2bn(d, t1);
	//cout <<"t1 is " << bn2str(d) << endl;
	//str2bn(d, t2);
	//cout << "t2 is " << bn2str(d) << endl;
	//writebn("test.txt", d);
	system("pause");
	return 0;
}


