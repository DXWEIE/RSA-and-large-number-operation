
#include "D:\study\����ѧ\my�ر���RSA and BN\bignum3.h"
#include<Windows.h>
using namespace std;


//�ӷ����еģ�ȡ1024λ
int add_b(BN a, BN b, BN sum)
{
	memset(sum, 0, BNSIZE);
	uint32_t *aptr, *bptr, *sptr = LSDPTR_B(sum);
	uint32_t *maptr, *mbptr;
	uint64_t carry = 0U;
	int flag = FLAG_OK;
	if (DIGITS_B(a) < DIGITS_B(b))//��a��ʾ�����Ǹ�
	{
		aptr = LSDPTR_B(b);
		maptr = MSDPTR_B(b);
		bptr = LSDPTR_B(a);
		mbptr = MSDPTR_B(a);
		SETDIGITS_B(sum, DIGITS_B(b));
	}
	else
	{
		aptr = LSDPTR_B(a);
		maptr = MSDPTR_B(a);
		bptr = LSDPTR_B(b);
		mbptr = MSDPTR_B(b);
		SETDIGITS_B(sum, DIGITS_B(a));
	}

	while (bptr <= mbptr)//�̵Ļ�û�м���
	{
		carry = (uint64_t)((uint64_t)(*aptr) + (uint64_t)(*bptr) + (uint64_t)(uint32_t)(carry >> BITPERDIGIT));
		*sptr = (uint32_t)carry;
		aptr++; bptr++; sptr++;
	}

	while (aptr <= maptr)//�͸ող����Ļ����еĽ�λ��ӣ�������Լ���
	{
		carry = (uint64_t)((uint64_t)(*aptr) + (uint64_t)(uint32_t)(carry >> BITPERDIGIT));
		*sptr = (uint32_t)carry;
		aptr++;  sptr++;
	}

	if (carry& BASE)//������н�λ1
	{
		*sptr = 1U;
		INCDIGITS_B(sum);
	}

	if (DIGITS_B(sum) > (uint32_t)BNMAXDGT)//������32��λ�����������
	{
		SETDIGITS_B(sum, BNMAXDGT);//������32��λ����������ǰ��0,ģ������32��λ����
		RMLDZRS_B(sum);
		flag = FLAG_OF;//�ӷ��������
	}
	return flag;
}

//�ӷ�����������BND�ļӷ�
void add(BN a, BN b, BN sum)
{
	uint32_t *aptr, *bptr, *sptr = LSDPTR_B(sum);
	uint32_t *maptr, *mbptr;
	uint64_t carry = 0U;

	if (DIGITS_B(a) < DIGITS_B(b))//��a��ʾ�����Ǹ�
	{
		aptr = LSDPTR_B(b);
		maptr = MSDPTR_B(b);
		bptr = LSDPTR_B(a);
		mbptr = MSDPTR_B(a);
		SETDIGITS_B(sum, DIGITS_B(b));
	}
	else
	{
		aptr = LSDPTR_B(a);
		maptr = MSDPTR_B(a);
		bptr = LSDPTR_B(b);
		mbptr = MSDPTR_B(b);
		SETDIGITS_B(sum, DIGITS_B(a));
	}

	while (bptr <= mbptr)//�̵Ļ�û�м���
	{
		carry = (uint64_t)((uint64_t)(*aptr) + (uint64_t)(*bptr) + (uint64_t)(uint32_t)(carry >> BITPERDIGIT));
		*sptr = (uint32_t)carry;
		aptr++; bptr++; sptr++;
	}

	while (aptr <= maptr)//�͸ող����Ļ����еĽ�λ��ӣ�������Լ���
	{
		carry = (uint64_t)((uint64_t)(*aptr) + (uint64_t)(uint32_t)(carry >> BITPERDIGIT));
		*sptr = (uint32_t)carry;
		aptr++;  sptr++;
	}

	if (carry& BASE)//������н�λ1
	{
		*sptr = 1U;
		INCDIGITS_B(sum);
	}
	RMLDZRS_B(sum);
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
int adduint_b(BN a, uint32_t b, BN sum)
{
	BND temp = { 0 };
	temp[0] = 1; temp[1] = b;//����temp�Ǵ���
	add(a, temp, sum);
	return 0;
}

int adduint_b(uint32_t a, BN b, BN &sum)
{
	BN temp = { 0 };
	temp[0] = 1; temp[1] = a;//����temp�Ǵ���
	int flag = add_b(b, temp, sum);
	return flag;
}

//ģ1024λ��ȡģ��
int sub_b(BN a, BN b, BN result)
{
	memset(result, 0, BNSIZE);
	uint64_t carry = 0ULL;
	uint32_t *aptr, *bptr, *rptr, *maptr, *mbptr;

	int flag = FLAG_OK;//�����Ƿ�����������

	uint32_t a_t[BITPERDIGIT + 2];//����1λ
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

//����
void sub(BN a, BN b, BN result)
{
	uint32_t *aptr, *bptr, *rptr, *maptr, *mbptr;
	uint64_t carry = 0U;
	aptr = LSDPTR_B(a);
	bptr = LSDPTR_B(b);
	maptr = MSDPTR_B(a);
	mbptr = MSDPTR_B(b);
	rptr = LSDPTR_B(result);
	SETDIGITS_B(result, DIGITS_B(a));
	//���ai<bi��carry�ĸ�λ��ȫΪ1������0���жϵ�33λ
	//����2λ�����ƣ�����Ϊ4�������carry��4λ��01-1��caryy=1110;10-11=1111���жϽ�λ��ʱ���ж�33λ������λ�Ƿ�Ϊ0�Ϳ���
	while (bptr <= mbptr)
	{
		carry = (uint64_t)((uint64_t)(*aptr) - (uint64_t)(*bptr) - (uint64_t)((carry&BASE) >> BITPERDIGIT));
		*rptr = (uint32_t)carry;
		aptr++; bptr++; rptr++;
	}
	while (aptr <= maptr)//����������λ
	{
		carry = (uint64_t)((uint64_t)(*aptr) - (uint64_t)((carry&BASE) >> BITPERDIGIT));
		*rptr = (uint32_t)carry;
		aptr++;  rptr++;
	}
	RMLDZRS_B(result);
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
		sub(a_t, b_t, temp);
		//cout << bn2str(a_t) << " - " << bn2str(b_t) << " = " << bn2str(temp) << endl;

		modn_b(temp, n, rem);
		//cout << "zhe cuo le line 424!" << endl;
		//cout << bn2str(temp) << " % " << bn2str(n) << " = " << bn2str(rem) << endl;
		cpy_b(result, rem);
	}
	else //a<b...�Ǿ�b-a=t��Ȼ����n-t  12-19 mod 5  19-12 mod 5 =2  5-2=3
	{
		//sub_b(b_t, a_t, temp);
		sub(b_t, a_t, temp);
		modn_b(temp, n, rem);
		//cout << bn2str(temp) << " % " << bn2str(n) << " = " << bn2str(rem) << endl;
		//sub_b(n, rem, temp);
		sub(n, rem, temp);
		//cout << bn2str(n) << " - " << bn2str(rem) << " = " << bn2str(temp) << endl;
		cpy_b(result, temp);
	}
}

//�˷�ȡ1024λ
int mul_b(BN a, BN b, BN & result)
{
	BN a_t = { 0 }, b_t = { 0 };
	BND temp = { 0 };
	uint32_t *aptr, *bptr, *maptr, *mbptr, *pptr;
	uint64_t carry = 0;

	if (DIGITS_B(a) == 0 || DIGITS_B(b) == 0)
	{
		cpy_b(result, ZERO_BN);
		return FLAG_OK;
	}

	//if (DIGITS_B(a) < DIGITS_B(b))
	//{
	//	cpy_b(a_t, b);
	//	cpy_b(b_t, a);
	//}
	//else
	//{
	//	cpy_b(a_t, a);
	//	cpy_b(b_t, b);
	//}
	cpy_b(a_t, a);
	cpy_b(b_t, b);
	maptr = MSDPTR_B(a_t);
	mbptr = MSDPTR_B(b_t);
	int pos = 0;
	for (bptr = LSDPTR_B(b_t), pptr = LSDPTR_B(temp); bptr <= mbptr; bptr++)
	{
		carry = 0;//ǰһ�ν�λ�Ѿ����������λ
		for (aptr = LSDPTR_B(a_t); aptr <= maptr; aptr++, pptr++)
		{
			carry = (uint64_t)((uint64_t)(*aptr) * (uint64_t)(*bptr) + (uint64_t)(*pptr)
				+ (uint64_t)(uint32_t)(carry >> BITPERDIGIT));
			*pptr = (uint32_t)carry;//��32�Ǳ�λ��
		}
		pos++;//����һλ
		*pptr = (uint32_t)(carry >> BITPERDIGIT);//���һλ��λ
		pptr = LSDPTR_B(temp) + pos;//�ص���ͷ
	}
	SETDIGITS_B(temp, DIGITS_B(a) + DIGITS_B(b));
	RMLDZRS_B(temp);

	if (DIGITS_B(temp) > BNMAXDGT)//����1024ȡ1024λ
	{
		SETDIGITS_B(temp, BNMAXDGT);
		RMLDZRS_B(temp);
	}
	cpy_b(result, temp);
	return FLAG_OK;
}
//�˷�
int mul(BN a, BN b, BN result)
{
	BN a_t = { 0 }, b_t = { 0 };
	BND temp = { 0 };
	uint32_t *aptr, *bptr, *maptr, *mbptr, *pptr;
	uint64_t carry = 0U;

	if (DIGITS_B(a) == 0 || DIGITS_B(b) == 0)
	{
		cpy_b(result, ZERO_BN);
		return FLAG_OK;
	}
	//if (DIGITS_B(a) < DIGITS_B(b))
	//{
	//	cpy_b(a_t, b);
	//	cpy_b(b_t, a);
	//}
	//else
	//{
	//	cpy_b(a_t,a);
	//	cpy_b(b_t,b);
	//}
	cpy_b(a_t, a);
	cpy_b(b_t, b);
	maptr = MSDPTR_B(a_t);
	mbptr = MSDPTR_B(b_t);
	int pos = 0;//ÿ��p��ǰ����һλ
	for (bptr = LSDPTR_B(b_t), pptr = LSDPTR_B(temp); bptr <= mbptr; bptr++)
	{
		carry = 0;//ǰһ�ν�λ�Ѿ����������λ
		for (aptr = LSDPTR_B(a_t); aptr <= maptr; aptr++, pptr++)
		{
			carry = (uint64_t)((uint64_t)(*aptr) * (uint64_t)(*bptr) + (uint64_t)(*pptr)
				+ (uint64_t)(uint32_t)(carry >> BITPERDIGIT));
			*pptr = (uint32_t)carry;//��32�Ǳ�λ��
		}
		pos++;//����һλ
		*pptr = (uint32_t)(carry >> BITPERDIGIT);//���һλ��λ
		pptr = LSDPTR_B(temp) + pos;//�ص���ͷ	
	}
	/*cout << "bits of a is" << getbits_b(a_t) << endl;
	cout << "pos is " << pos << endl;
	cout << "bits of b is" << getbits_b(b_t) << endl;*/
	SETDIGITS_B(temp, DIGITS_B(a) + DIGITS_B(b));
	RMLDZRS_B(temp);
	//cout << "bits of temp is" << getbits_b(temp) << endl;
	cpy_b(result, temp);
	return FLAG_OK;
}

//ģ��
void modmul(BN a, BN b, BN n, BN & result)
{
	//cout << "line 369  aλ��Ϊ " << getbits_b(a) << endl;
	//cout << "line 369  bλ��Ϊ " << getbits_b(b) << endl;
	//cout << "line 369  nλ��Ϊ " << getbits_b(n) << endl;
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
	//cout << "line 384  �˻�λ��Ϊ " << getbits_b(temp) << endl;
	//cout << "lint652?������653������" << endl;
	modn_b(temp, n, result);

}


//�ɸ�����Լ��result=DT*R' (mod N )
void mont_redc(BN DT, BN N, BN Np, BN R,BN & result)
{
	BN temp1 = { 0 };
	BND temp2 = { 0 };
	BN m = { 0 }, t = { 0 };
	unsigned int R_bits = getbits_b(R);//ģ��R���Ǳ�����ô��λ
	int Bits = (R_bits + 31) / 32;//����ɴ��λ��
	int remain = R_bits - 32 * (Bits-1);//��ߵġ�λ���ĵ�[remain-1]λΪ1��ԭ�ⲻ������[remain-2]..[0]λ�Ϳ���
	//if(remain>1) ����R[0];����R[0]-1,��������R[0]-1"λ"
	//����R[Bits]������R[Bits][remain-2]..R[Bits][0]
	
	//if (Bits>1) ����R[Bits-1]..R[1]
	//����temp=T%R
	if (getbits_b(DT) >= R_bits)//ֻ�ж��ʱ���Ҫȡ��,R��0100��T��1101ҲҪȡ�࣬���繹��һ����100���101�Ϳ����ˣ����ǲ��������λ
	{
		for (unsigned int i = 1; i <R[0]; i++)//ȫ����������0��
		{
			temp1[i] = DT[i];
		}

		if (remain >1)//���λ������10,remain=2,���Ա���1λ
		{
			temp1[R[0]] = (DT[R[0]] & (uint32_t)(1U<<(remain-1))-1U );
			temp1[0] = R[0];
		}
		else {
			temp1[0] = R[0]-1;
		}
	}
	else {
		cpy_b(temp1, DT);
	}
	//(T%R)*N'�����г�N'�Ĳ���,���ܻᳬ��1024λ
	mul(temp1, Np, temp2);
	//m=temp1%R
	if (getbits_b(temp2) >= R_bits)//ֻ�ж��ʱ���Ҫȡ��,R��0100��T��1101ҲҪȡ�࣬���繹��һ����100���101�Ϳ����ˣ����ǲ��������λ
	{
		for (unsigned int i = 1; i < R[0]; i++)//ȫ����������0��
		{
			m[i] = temp2[i];
		}

		if (remain > 1)//���λ������10,remain=2,���Ա���1λ
		{
			m[R[0]] = (temp2[R[0]] & (uint32_t)(1U << (remain - 1)) - 1U);
			m[0] = R[0];
		}
		else {
			m[0] = R[0] - 1;
		}
	}
	else {
		cpy_b(m, temp2);
	}
	memset(temp2, 0, sizeof(temp2));

	mul(m, N, temp2);
	add(DT, temp2, temp2);
	//cout << "t*R= " << bn2str(temp2) <<endl;

	for (int i = getbits_b(R)-1;i>0;i--)
	{
		shr_b(temp2);
	}
	//cout << "t= " << bn2str(temp2) << endl << endl;
	cpy_b(t, temp2);

	if(cmp_b(t, N) >= 0)
	{
		sub(t, N, t);//t=t-N
	}
	cpy_b(result,t);
}
//�ɸ������㷨��ģ�� result=x*y (mod n)
void mont_modmul(BN x, BN y, BN N ,BN & result)
{	
	BND temp1 = { 0 }, temp2 = { 0 }, temp3 = { 0 };//���ܻᳬ��1024λ���϶�����ֻ��1024λ
	BN Xp = { 0 }, Yp = { 0 }, Zp = { 0 };

	int m = getbits_b(N);//ģ��������ϲ������Ҫģż��������ӽ��ܲ�̫���ܳ��֣���Ϊ��+1,R=2^mһ������n
	BN RRN = { 0 };//R*R (mod n)
	BN R = { 0 }, Rp = { 0 }, Np = { 0 };
	int Bits = (m + 31) / 32;//����ɴ��λ��
	int remain = m - 32 * (Bits - 1);//��ߵġ�λ���ĵ�[remain-1]λΪ1��ԭ�ⲻ������[remain-2]..[0]λ�Ϳ���
	R[0] = Bits;
	R[Bits] = (uint32_t)(1U <<remain);
	//inv_b(R, N, Rp);
	sub(R, N, temp1);//temp1=-N
	inv_b(temp1, R, Np);
	modmul(R, R, N, RRN);

	//cout << "R= " << bn2str(R) << endl;
	//cout << "N= " << bn2str(N) << endl;
	//cout << "temp1= " << bn2str(temp1)<< endl;
	//cout << "Np= " << bn2str(Np) << endl<<endl;
		mul(x, RRN, temp1);
		mul(y, RRN, temp2);
		
		mont_redc(temp1, N, Np, R, Xp);
		//cout << "Xp= " << bn2str(Xp) << endl << endl;
		mont_redc(temp2, N, Np, R, Yp);
		//cout << "Yp= " << bn2str(Yp) << endl << endl;
		mul(Xp, Yp, temp3);
		mont_redc(temp3, N, Np, R, Zp);
		//cout << "Zp= " << bn2str(Zp) << endl << endl;
		mont_redc(Zp, N, Np, R, result);
	
}

void mont_modmul(BN x, BN y, BN R,BN N,BN Np,BN RRN, BN & result)//����ģ�ݵ�ģ��
{
	BND temp1 = { 0 }, temp2 = { 0 }, temp3 = { 0 };//���ܻᳬ��1024λ���϶�����ֻ��1024λ
	BN Xp = { 0 }, Yp = { 0 }, Zp = { 0 };
	mul(x, RRN, temp1);
	mul(y, RRN, temp2);
	mont_redc(temp1, N, Np, R, Xp);
	mont_redc(temp2, N, Np, R, Yp);
	mul(Xp, Yp, temp3);
	mont_redc(temp3, N, Np, R, Zp);
	mont_redc(Zp, N, Np, R, result);
}

void mont_modexp(BN a, BN b, BN N, BN & result)//�ɸ�����ģ�� a^b mod N
{
	int m = getbits_b(N);//ģ��������ϲ������Ҫģż��������ӽ��ܲ�̫���ܳ��֣���Ϊ��+1,R=2^mһ������n
	BN RRN = { 0 };//R*R (mod n)
	BN R = { 0 }, Rp = { 0 }, Np = { 0 };
	BN a_t = { 1,1 }, b_t;//a=1;n���˶�����չ��
	BN temp1 = { 0 }, temp2 = { 0 };//������Ϊresult�и��������
	BN Xp = { 0 }, Yp = { 0 };
	int Bits = (m + 31) / 32;//����ɴ��λ��
	int remain = m - 32 * (Bits - 1);//��ߵġ�λ���ĵ�[remain-1]λΪ1��ԭ�ⲻ������[remain-2]..[0]λ�Ϳ���
	R[0] = Bits;
	R[Bits] = (uint32_t)(1U << (remain - 1));

	sub(R, N, temp1);//temp1=-N
	inv_b(temp1, R, Np);
	modmul(R, R, N, RRN);

	//b^n (mod m) --->a^b mod N
	memset(result, 0, sizeof(result));
	
	cpy_b(b_t, a);//b_t=b,��ʼ��
	uint32_t *nptr, *mnptr;
	nptr = LSDPTR_B(b);
	mnptr = MSDPTR_B(b);//��������������
	char binform[33];//ÿ��32bit��uint32ת��Ϊ�����Ƽ��ɣ�һ�δ�ȡ����
	int i = 0;
	while (nptr <= mnptr)//ûԽ��Ͷ�����
	{
		memset(binform, 0, sizeof(binform));
		_ultoa(*nptr, binform, 2);

		i = strlen(binform) - 1;//�������һλ
		for (int j = 31; j >= 0; j--)//��ʼģƽ��
		{
			if (i >= 0)//���¶�������ֻ��ƽ��bȡģ
			{

				if (binform[i] == '1')
				{
					mont_modmul(a_t,b_t, R, N, Np, RRN, a_t);
				}
				i--;
			}
			mont_modmul(b_t, b_t, R, N, Np, RRN, b_t);
		}
		nptr++;
	}

	cpy_b(result, a_t);
	RMLDZRS_B(result);
}

string bn2str(BN bignum)//����תΪ�ַ���
{
	if (DIGITS_B(bignum) == 0)//�����0��Ҫ��ʾ���0
		return string("0");
	//char strbignum[265] = { 0 };//(1024+32)/4
	//BN temp;
	char strbignum[520] = { 0 };//(1024+32)/4
	BND temp;
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
		//string finalstr(strbignum + zeros, &strbignum[264]);
		string finalstr(strbignum + zeros, &strbignum[519]);
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

	//cout << "in line 527 modn_b    a mod n =rem " << endl;
	//cout << "line 527 aλ��Ϊ " << getbits_b(a) << endl;
	//cout << "line 527 nλ��Ϊ " << getbits_b(n) << endl;
	//cout << "line 527 aΪ " << bn2str(a) << endl;
	//cout << "line 527 nΪ " << bn2str(n) << endl;
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
		flag = div_b(a, n, temp, result);
		//flag = remdiv_b(a, n, result);
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
	flag = div_b(a, b, q, temp);
	//flag = div_b(a, b, q, temp);
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

		div_b(g, v3, q, t3);

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

//��һ����������λ�ͼӼ���ʵ�ֵ����淽����������Ӧ�ø���
//���(a,n)=1,����ax = 1 (mod n)�������쳣û����Ԫ������0����Ȼ��Ԫ������Ϊ0  
int new_inv(BN a, BN n, BN & x)
{

	memset(x, 0, BNSIZE);
	BN   x1 = { 0 }, y1 = { 0 }, x2 = { 0 }, y2 = { 0 };
	BN temp1 = { 0 }, temp2 = { 0 };//�����м���


	if (cmp_b(a, ONE_BN) == 0)//�����1,��Ԫ����1
	{
		SETONEBIT_B(x, 1U);
		return FLAG_OK;
	}

	gcd_b(a, n, temp1);//�����ӣ��ж���û����Ԫ
	if (cmp_b(temp1, ONE_BN) != 0)//��������أ�û����Ԫ
	{
		SETZERO_B(x);
		return FLAG_NOINV;
	}

	//����Ԫ���Ϳ�ʼ�󡣷���x=y*a (mod n)��ƽ����(x1,y1)=(a,1)  (x2,y2)=(n,0)
	//��ʼ��x1=a,y1=1  x2=n,y2=0
	cpy_b(x1, a);
	cpy_b(x2, n);
	cpy_b(y1, ONE_BN);
	cpy_b(y2, ZERO_BN);
	int i = 0;
	do
	{
		while (uint32_t(x1[1] & 1U) == 0U)//C����Ҫע��ǿ�����ͣ�����x1[1] & 1U��Ȼ������0U��Ҳ������0,�ڱ�ϵͳĬ����0UL
		{
			shr_b(x1);
			if(uint32_t(y1[1] & 1U) == 0U)
				shr_b(y1);
			else {
				add(y1, n, y1);
				shr_b(y1);
			}
		}
		while (uint32_t(x2[1] & 1U) == 0U)
		{
			shr_b(x2);
			if (uint32_t(y2[1] & 1U) == 0U)
				shr_b(y2);
			else {
				add(y2, n, y2);
				shr_b(y2);
			}
		}
		if ((x1[0] == 1 && x1[1] == 1) || (x2[0] == 1 && x2[1] == 1))
			break;
		if (x1[0]>=x2[0] && cmp_b(x1, x2) > 0)//x1>x2ʱ,x1=x1-x2,y1=y1-y2
		{
			sub(x1, x2, x1);
			if (y1[0] <= y2[0] && cmp_b(y1, y2) < 0)//���y1<y2��y1=n-(y2-y1)
			{
				sub(y2, y1, temp1);
				sub(n, temp1, y1);
			}
			else {
				sub(y1, y2, y1);
			}
		}
		else//x1<x2,x2=x2-x1,y2=y2-y1
		{
			sub(x2, x1, x2);
			if (y2[0] <= y1[0] && cmp_b(y2, y1) < 0)//���y2<y1��y1=n-(y1-y2)
			{
				sub(y1, y2, temp1);
				sub(n, temp1, y2);
			}
			else {
				sub(y2, y1, y2);
			}
		}
		if ((x1[0] == 1 && x1[1] == 1) || (x2[0] == 1 && x2[1] == 1))
			break;

	} while (1);

	//��ʼ��
	if (x1[0] == 1 && x1[1] == 1)
	{
		cpy_b(x, y1);
	}
	else {
		cpy_b(x, y2);
	}
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
				/*cout <<endl<<31-j<< ": ai-1= " << bn2str(a_t) << "  bi-1= " << bn2str(b_t) << endl;*/
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
			/*cout << "b_tλ��Ϊ " << getbits_b(b_t) << endl;*/

			modmul(b_t, b_t, m, temp2);//b=b*b mod m	

			/*cout << "�������modmul!!!line 1776" << endl;*/
			cpy_b(b_t, temp2);
			/*cout <<endl<<31-j<< ": ai= " << bn2str(a_t) << "  bi= " << bn2str(b_t) << endl << endl;*/

		}
		nptr++;
	}
	cpy_b(result, a_t);
	RMLDZRS_B(result);
	/*cout << "int function result = " << bn2str(result) << endl;*/
	return FLAG_OK;
}

//������
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

	//sub_b(a, ONE_BN, temp1);//temp1=a-1
	sub(a, ONE_BN, temp1);//temp1=a-1
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

int miller_b(BN n,int t=3)
{
	if ((n[1] & 1U) == 0)
		return 0;
	srand((unsigned)time(NULL));
	BN n_1 = { 0 }, q = { 0 }, temp = { 0 };
	BN x = { 0 };//x�������õ�
	BN y = { 0 };
	int flag = 1;//����������
	cpy_b(n_1, n);
	n_1[1] = n_1[1] - 1U;//n-1
	
	int bits = getbits_b(n_1);
	int k = 0;

	cpy_b(q, n_1);
	for (int i = 1; i < bits; i++)
	{
		if (uint32_t(q[1] & 1U) == 0U)
		{
			k++;
			shr_b(q);
		}
		else
			break;
	}

	for (int times = 0; times < t; times++)//Ĭ����3�μ���
	{
		memset(x, 0U, sizeof(x));
		randBN(x, getbits_b(n));
		if (cmp_b(x, n))
			shr_b(x);
		//cout << "x = " << bn2str(x)<<endl;
		//cout << "x �� " << getbits_b(x) << "λ" << endl;
		//cout << "q= " << bn2str(q) << endl;
		//cout << "k= " << k << endl << endl;
		modexp_b(x, q, n, y);//y=x^q(mod n)
		if ((y[0] == 1U && y[1] == 1U) || !cmp_b(y, n_1))
			continue;

		for (int j = 1; j < k; j++)
		{
			modmul(y, y, n, temp);
			if (temp[0] == 1U && temp[1] == 1U)
				break;
			else if (!cmp_b(temp, n_1))
				goto prime;
			cpy_b(y, temp);
		}
		flag=0;//�Ǻ���
		break;
	prime://����
		;
	}
	return flag;
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
	char pre[5][20] = {
		"20primefac.txt","25primefac.txt","30primefac.txt","35primefac.txt",
		"40primefac.txt"
	};
	int prenum[5] = { 20,25,30,35,40 };
	//ǰ50������
	unsigned int prime[50] =
	{
		2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,113,
		127,131,137,139,149,151,157,163,167,173,179,181,191,193,197,199,211,223,227,229
	};
	BN temp1, temp2, temp3;
	for (int j = 0; j < 5; j++)
	{
		SETONEBIT_B(temp1, 1U);
		for (int i = 0; i < prenum[j]; i++)
		{
			SETONEBIT_B(temp2, prime[i]);

			mul_b(temp1, temp2, temp3);

			cpy_b(temp1, temp3);
			RMLDZRS_B(temp1);
		}
		writebn(pre[j], temp1);
	}
	//uint64_t fac = 1;
	//for (int i = 0; i < 50; i++)
	//{
	//	fac = fac * prime[i];
	//}
	//cout << fac << endl;
}

//����1λ
int shr_b(BN a)
{
	uint32_t *aptr;
	uint32_t current, undercarry = 0;

	if (DIGITS_B(a) == 0)//Ϊ0�Ͳ����ƶ���
	{
		return FLAG_UF;
	}

	for (aptr = MSDPTR_B(a); aptr > a; aptr--)
	{
		current = (uint32_t)((uint32_t)(*aptr >> 1) | (uint32_t)(undercarry << (BITPERDIGIT - 1)));

		undercarry = (uint32_t)(*aptr & 1U);//�Ӹߡ�λ������͡�λ���е���λ��������һλ���λ

		*aptr = current;//����Ϊ��λ����1λ����λ���λΪ��ǰһλ��������
	}

	RMLDZRS_B(a);
	return FLAG_OK;
}
//����1λ
int shl_b(BN a)
{
	uint32_t *aptr, *maptr;
	uint64_t carry = 0L;
	int error = FLAG_OK;

	RMLDZRS_B(a);
	if ((uint32_t)getbits_b(a) >= (uint32_t)BNDMAXBIT)
	{
		SETDIGITS_B(a, BNMAXDGT);
		error = FLAG_OF;
	}

	maptr = MSDPTR_B(a);
	for (aptr = LSDPTR_B(a); aptr <= maptr; aptr++)
	{
		carry = ((uint64_t)(*aptr) << 1) | (carry >> BITPERDIGIT);
		*aptr = (uint32_t)carry;
	}

	if (carry >> BITPERDIGIT)//���carry��󻹽���1λ���ͼӷ�һ��������һλ1��ͬʱ��1��λ��
	{
		if (DIGITS_B(a) < BNDMAXBIT)//�����BND��Χ��
		{
			*aptr = 1U;
			INCDIGITS_B(a);
			error = FLAG_OK;
		}
		else
		{
			error = FLAG_OF;
		}
	}

	RMLDZRS_B(a);
	return error;
}

//��ȡ������λ����������BND a
uint32_t getbits_b(BN a)
{
	uint32_t bits = DIGITS_B(a);
	uint32_t high;

	while (a[bits] == 0 && bits > 0)//����ǰ���0 
	{
		bits--;
	}

	high = a[bits];//highָ�������Чλ
	if (high == 0 && bits == 0)//0
		return 0;

	bits = bits << 5;//ÿλ32��������λ
	uint32_t bit32 = 0x80000000U;

	while ((high &bit32) == 0)//���û����ô��λ��λ������-1
	{
		high = high << 1;
		bits--;
	}
	return bits;
}

//�ɿ��ĳ���,��Ȼ����
int mydiv_b(BN a, BN b, BN q, BN rem)
{

	/*cout << "\nin line 2179  a=qb+rem \n " << endl;
	cout << "line 2179 aλ��Ϊ " << getbits_b(a) << endl;
	cout << "line 2179 bλ��Ϊ " << getbits_b(b) << endl;*/
	memset(rem, 0, sizeof(rem));
	memset(q, 0, sizeof(q));
	//uint32_t *rptr, *bptr,*mbptr,*mrptr;
	
	BND b_t;//��ʱ���b
	BND tempq = { 0 };
	BND tempsub = { 0 };//����̺���ʱ��
	BND q_t = { 0 };//ÿ����
	BND r_t;

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
	//cout << "shiftnum= "<<shiftnum << endl;
	int subtimes = abit - bbit + 1;
	//cout << "line 2263��λ��ǰ  b_t  " << bn2str(b_t) << endl;

	for (int i = 0; i < shiftnum; i++)
		shl_b(b_t);
	//shift_b(b_t, shiftnum);//���ƶ������λ,�Ժ�ÿ������һλ,�ƶ��Ժ��b_t
	//cout << "line 2263��λ�Ժ�  b_t  " << bn2str(b_t) << endl;
	//cout << "line 2263  r_t λ��Ϊ  " << getbits_b(r_t) << endl;
	//cout << "line 2263��λ�Ժ�  b_t λ��Ϊ  " << getbits_b(b_t)<< endl;
	//cout << "a��λ����:" << abit << endl;
	//cout << "b��λ����:" << bbit << endl;
	//cout <<"��������" <<subtimes << endl;
	for (int i = 0; i < subtimes; i++) {
		//cout << "line 2252 subtimes"<<i << endl;
		if (cmp_b(r_t, b_t) >= 0)//�����еȺţ���������
		{
			//sub_b(r_t, b_t, tempsub);

			sub(r_t, b_t, tempsub);

			cpy_b(r_t, tempsub);
			//if (*(q_t) > 1)
			shl_b(q_t);

			adduint_b(q_t, 1U, tempq);//��1
		/*	if (i == 1025)cout << "line 2252 here" << i << endl;*/
			cpy_b(q_t, tempq);
			//cout <<"1:  "<< getbits_b(q_t) << endl;
			shr_b(b_t);
			//cout << i<<"  1 :" << bn2str(q_t) << endl;
			//cout << "line 2263" << bn2str(q_t) << endl;
		}
		else
		{
			shl_b(q_t);//��0
			//cout << "0:  "<<getbits_b(q_t) << endl;

			shr_b(b_t);
			//cout << i << " 0 :" << bn2str(q_t) << endl;
		}
	}

	//cout<< bn2str(q_t) << endl;

	cpy_b(q, q_t);

	RMLDZRS_B(q);



	cpy_b(rem, r_t);
	RMLDZRS_B(rem);

	return FLAG_OK;
}

//�Ҹ���Knuth����ʵ�ֵĳ���
int div_b(BN a, BN b, BN q, BN rem)
{

	memset(rem, 0, sizeof(rem));
	memset(q, 0, sizeof(q));

	BND b_t;//��ʱ���b
	BND q_t = { 0 };//ÿ����
	uint32_t r_t[2 + (BNMAXDGT << 1)];

	uint32_t *bptr, *mbptr, *rptr, *rcir, *mrptr, *qptr;
	uint32_t d = 0, qh = 0, qh1 = 0;
	uint64_t kuo, left, right, borrow, carry;
	uint32_t bn_1 = 0, bn_2 = 0;
	uint32_t ri = 0, ri_1 = 0, ri_2 = 0;//����r'������d�Ժ��
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
	//�������λ��Ϊ1��ʹ����λ����������Ҫ��������ڵ�����λ
	if (DIGITS_B(b_t) == 1)
	{
		mydiv_b(r_t, b_t, q, rem);
		return FLAG_OK;
	}


	mbptr = MSDPTR_B(b_t);
	bn_1 = *mbptr;

	while (bn_1 < BASEDIV2)
	{
		bn_1 = bn_1 << 1;
		d++;
	}
	uint64_t shiftr = (int)(BITPERDIGIT - d);//����ʱ��ϵ����ƴ���

	if (d > 0)
	{
		bn_1 = bn_1 + (uint32_t)((uint64_t)*(mbptr - 1) >> shiftr);

		if (DIGITS_B(b_t) > 2)
			bn_2 = (uint32_t)((uint32_t)((uint64_t)(*(mbptr - 1)) << d) + (uint32_t)((uint64_t)(*(mbptr - 2)) >> shiftr));
		else//������λ��ֱ�Ӹ�ֵ
		{
			bn_2 = (uint32_t)((uint64_t)(*(mbptr - 1)) << d);
		}
	}
	else//û���ƶ���ԭ�ⲻ��
	{
		bn_2 = (uint32_t)(*(mbptr - 1));
	}

	mbptr = MSDPTR_B(b_t);
	mrptr = MSDPTR_B(r_t) + 1;//ָ�򻬶��������λ������һλ��֮�������0���ܲ��ǣ��ɺ�����λ����
	rptr = MSDPTR_B(r_t) - DIGITS_B(b_t) + 1;//ָ�򻬶��������λ
	qptr = q + DIGITS_B(r_t) - DIGITS_B(b_t) + 1;//��߼�������Ϊ0���ܲ��ǣ��ɺ�����λ����
	*mrptr = 0;//��ʼ��Ϊ0



	while (rptr >= LSDPTR_B(r_t))//��ʼ�����飬�±��Ӧ��r(m+n)��Ӧa(m)��r(m+n-1)��Ӧa(m-1)
	{

		ri = (uint32_t)((uint32_t)((uint64_t)(*mrptr) << d) + (uint32_t)((uint64_t)(*(mrptr - 1)) >> shiftr));
		ri_1 = (uint32_t)(((uint32_t)((uint64_t)(*(mrptr - 1)) << d) + (uint32_t)((uint64_t)(*(mrptr - 2)) >> shiftr)));

		if (mrptr - 3 > r_t)//��ֹ��3λ
		{
			ri_2 = (uint32_t)(((uint32_t)((uint64_t)(*(mrptr - 2)) << d) + (uint32_t)((uint64_t)(*(mrptr - 3)) >> shiftr)));
		}
		else//ֻ��2λ
		{
			ri_2 = (uint32_t)((uint64_t)(*(mrptr - 2)) << d);
		}


		//����q���ƣ�kuo��64λ�ģ�ȡ��������
		kuo = (uint64_t)((((uint64_t)ri << BITPERDIGIT) + (uint64_t)ri_1) / bn_1);

		if (kuo < ALLONE)//ѡ��С���Ǹ��������Ƶ�q���ᳬ��B-1�����ᳬ��B����
		{
			qh = (uint32_t)kuo;

		}
		else
			qh = ALLONE;

		kuo = ((uint64_t)ri << BITPERDIGIT) + (uint64_t)ri_1 - (uint64_t)bn_1*(uint64_t)qh;
		if (kuo < BASE)//��������еĶ���B���ˣ���߿϶�b[n-2]*qС��B*B,С�ڲűȽϣ����ڻ������Ҳ�Ƚϲ���
		{
			right = (uint64_t)(kuo << BITPERDIGIT) + (uint64_t)ri_2;
			left = (uint64_t)bn_2 *(uint64_t)qh;
			if (left > right)//û���������Ҫ�Ƚ�
			{
				qh--;
				//�ظ���һ����
				kuo = ((uint64_t)ri << BITPERDIGIT) + (uint64_t)ri_1 - (uint64_t)bn_1*(uint64_t)qh;
				if (kuo < BASE)//��������еĶ���B���ˣ���߿϶�b[n-2]*qС��B*B,С�ڲűȽϣ����ڻ������Ҳ�Ƚϲ���
				{
					right = (uint64_t)(kuo << BITPERDIGIT) + (uint64_t)ri_2;
					left = (uint64_t)bn_2 *(uint64_t)qh;
					if (left > right)//û���������Ҫ�Ƚ�
						qh--;
				}
				else
				{
					//printf("rh = %llX\n", rh);
					//printf("rh*B = %llX\n",rh <<BITPERDIGIT);
					//printf("rh*B+ri-2 = %llX\n", (rh << BITPERDIGIT) + (uint64_t)ri_2);
				}

			}
		}

		borrow = BASE;//��λĬ��
		carry = 0;
		int i = 0;
		for (bptr = LSDPTR_B(b_t), rcir = rptr; bptr <= mbptr; bptr++, rcir++)
		{
			if (borrow >= BASE)// û�з�����λ���������ߴ�����BASE
			{
				carry = (uint64_t)qh * (uint64_t)*bptr + (uint64_t)(uint32_t)(carry >> BITPERDIGIT);
				borrow = (uint64_t)*rcir + BASE - (uint64_t)(uint32_t)carry;
				*rcir = (uint32_t)(borrow);
			}
			else//�����˽�λ,�ָ�
			{
				carry = (uint64_t)qh * (uint64_t)*bptr + (uint64_t)(uint32_t)(carry >> BITPERDIGIT);
				borrow = (uint64_t)*rcir + (uint64_t)BASE - (uint64_t)(uint32_t)carry - 1ULL;//��λ��
				*rcir = (uint32_t)(borrow);
			}
		}

		if (borrow >= BASE) //û�з�����λ���������ߴ�����BASE
		{
			borrow = (uint64_t)*rcir + BASE - (uint64_t)(uint32_t)(carry >> BITPERDIGIT);
			*rcir = (uint32_t)(borrow);
		}
		else//�����˽�λ
		{
			borrow = (uint64_t)*rcir + BASE - (uint64_t)(uint32_t)(carry >> BITPERDIGIT) - 1ULL;//��λ��
			*rcir = (uint32_t)(borrow);
		}
		*qptr = qh;

		if (borrow < BASE)//borrow������λ��û�л�����,q����һλ��b�ӻᵽa�У��ͼӷ�����һ��
		{
			carry = 0;
			for (bptr = LSDPTR_B(b_t), rcir = rptr; bptr <= mbptr; bptr++, rcir++)
			{
				carry = ((uint64_t)*rcir + (uint64_t)*bptr + (uint64_t)(uint32_t)(carry >> BITPERDIGIT));
				*rcir = (uint32_t)carry;
			}
			*rcir = *rcir + (uint32_t)(carry >> BITPERDIGIT);
			*qptr = *qptr - 1;
		}
		qptr--; mrptr--; rptr--;
	}
	SETDIGITS_B(q, DIGITS_B(r_t) - DIGITS_B(b_t) + 1);
	RMLDZRS_B(q);
	SETDIGITS_B(r_t, DIGITS_B(b_t));
	cpy_b(rem, r_t);
	return FLAG_OK;

}



//����bits��������λ�Ĵ���result��ÿ5��32λ��ϣһ�Σ����˲�ֹһ��
int genBN(BN result, int bits)
{

	if (bits < 0 || bits>BNMAXBIT)//�޷���������λ����
		return FLAG_ERROR;
	if (bits == 0)
	{
		SETZERO_B(result);
		return FLAG_OK;
	}
	//���������������

	char randpath[11] = "rand.txt";
	char digest[80];
	char onebit[9] = { 0 };
	int times = (bits + 31) / 32;
	int t5 = ((times + 4) / 5) - 1;//һ��ȡ��5��������ж��ٴ�
	int t5r = times % 5;//����5λ��
	if (t5r >= 1) t5r--;//�������һ��

	//cout << "times= "<<times << endl;
	int remain = bits % 32;//���һλû��
	BN a = { 0 }, temp = { 0 };
	SETDIGITS_B(a, times);
	uint32_t perdig = 0;
	int i = 0, j = 0;
	for (; i < t5; i++)//���32λ�Ȳ���
	{
		writerand(randpath);
		memset(digest, 0, sizeof(digest));
		mysha1(randpath, digest);//��40���ֽڣ�160λ��		
		for (j = 0; j < 5; j++)
		{
			memcpy(onebit, &digest[8 * j], 8);
			perdig = stoul(onebit, NULL, 16);
			a[5 * i + j + 1] = perdig;
		}
	}
	j = 0;
	writerand(randpath);
	memset(digest, 0, sizeof(digest));
	mysha1(randpath, digest);//��40���ֽڣ�160λ��
	for (; j < t5r; j++)//����5λ��
	{
		memcpy(onebit, &digest[8 * j], 8);
		perdig = stoul(onebit, NULL, 16);
		a[5 * i + j + 1] = perdig;
	}

	int shiftright = 0;
	uint32_t bit32 = 0x80000000U;
	if (remain == 0)//��32������Ҫ�ճ�32λ��
	{
		while (true)
		{
			writerand(randpath);
			memset(digest, 0, sizeof(digest));
			mysha1(randpath, digest);//��40���ֽڣ�160λ��
			memcpy(onebit, digest, 8);
			perdig = stoul(onebit, NULL, 16);
			if (perdig >= bit32)//Ҫ�չ�
				break;
		}
	}
	else
	{
		writerand(randpath);
		memset(digest, 0, sizeof(digest));
		mysha1(randpath, digest);//��40���ֽڣ�160λ��
		memcpy(onebit, digest, 8);
		perdig = stoul(onebit, NULL, 16);

		SETONEBIT_B(temp, perdig);
		shiftright = getbits_b(temp) - remain;
		//cout << "shiftright = " << shiftright << endl;
		if (shiftright >= 0)//���ڲ����ƶ�����Ҫ����
			perdig = perdig >> shiftright;
		else if (shiftright < 0)
			perdig = perdig << abs(shiftright);
	}
	a[5 * i + j + 1] = perdig;
	cpy_b(result, a);
	RMLDZRS_B(result);
	return FLAG_OK;
}
int randBN(BN result, int bits)
{
	if (bits < 0 || bits>BNMAXBIT)//�޷���������λ����
		return FLAG_ERROR;
	if (bits == 0)
	{
		SETZERO_B(result);
		return FLAG_OK;
	}
	//���������������
	srand((unsigned)time(NULL));
	int times = (bits + 31) / 32;

	//cout << "times= "<<times << endl;
	int remain = bits % 32;//���һλû����Ҫ��remainλ
	BN a = { 0 }, temp = { 0 };
	SETDIGITS_B(a, times);
	uint32_t perdig = 0;
	int i = 1, j = 0;
	for (; i < times; i++)//���32λ�Ȳ���
	{
		a[i] = rand()*rand();
	}
	j = 0;

	int shiftright = 0;
	uint32_t bit32 = 0x80000000U;
	if (remain == 0)//��32������Ҫ�ճ�32λ��
	{
		;
	}
	else
	{
		a[times] = rand()*rand();
		for (uint32_t t = a[times];;)
		{
			t = t >> 1;
			shiftright++;
			if (t == 0U)
				break;		
		}
		shiftright = shiftright - remain;
		//cout << "shiftright = " << shiftright << endl;
		if (shiftright >= 0)//���ڲ����ƶ�����Ҫ����
			a[times] = a[times] >> shiftright;
		else if (shiftright < 0)
			a[times] = a[times] << abs(shiftright);
	}
	cpy_b(result, a);
	RMLDZRS_B(result);
	return FLAG_OK;
}
//�������д��addr��
void writerand(char * addr)
{
	FILE *fp = NULL;
	char buffer[80] = { 0 };
	if ((fp = fopen(addr, "wb")) == NULL)
	{
		printf("write to  %s failed\n", addr);
		system("pause");
		exit(0);
	}
	srand((unsigned)time(NULL));
	int randnum = rand()*rand();
	_itoa(randnum, buffer, 2);
	int len = strlen(buffer);
	fwrite(buffer, len, 1, fp);
	fclose(fp);
}


void subround(uint32_t & A, uint32_t & B, uint32_t & C, uint32_t & D, uint32_t & E, uint32_t &W, uint32_t K, int mode)
{
	uint32_t t;
	switch (mode)
	{
	case 1:
		t = A;
		A = ((B&C) | ((~B)&D)) + ((A << 5) | (A >> 27)) + E + W + K;
		E = D;
		D = C;
		C = (B << 30) | (B >> 2);
		B = t;
		break;
	case 2:
		t = A;
		A = (B^C^D) + ((A << 5) | (A >> 27)) + E + W + K;
		E = D;
		D = C;
		C = (B << 30) | (B >> 2);
		B = t;
		break;
	case 3:
		t = A;
		A = ((B&C) | (B&D) | (C&D)) + ((A << 5) | (A >> 27)) + E + W + K;
		E = D;
		D = C;
		C = (B << 30) | (B >> 2);
		B = t;
		break;
	case 4:
		t = A;
		A = (B^C^D) + ((A << 5) | (A >> 27)) + E + W + K;
		E = D;
		D = C;
		C = (B << 30) | (B >> 2);
		B = t;
		break;
	default:
		break;
	}

}

long long msgsize(char*plainaddr)
{
	long long size = 0;
	FILE *fp = NULL;
	if ((fp = fopen(plainaddr, "rb")) == NULL)
	{

		printf("open %s faied!", plainaddr);
		exit(0);
	}

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	fclose(fp);
	return size;
}

inline uint32_t cirleft(uint32_t word, int bit)//wordѭ������bitλ
{
	return(word << bit) | (word >> (32 - bit));
}

int mysha1(char *inputfileaddr, char *output)
{
	//char inputfileaddr[81] = "input.file";//����
	//char outputfile[81] = "digest.file";//���
	uint32_t W[80];//�ÿռ任ʱ�䣬W����80��
	memset(W, 0, sizeof(W));//ԭʼ��Ϊ0�ͺ���
	uint32_t A, B, C, D, E;
	long long msglen = msgsize(inputfileaddr) * 8;//���ԭ��λ����������λ��λ�������ֽڣ�
	FILE *fp = NULL;
	if ((fp = fopen(inputfileaddr, "rb")) == NULL)
	{
		printf("open %s faied!\n", inputfileaddr);
		system("PAUSE");
		return 1;
	}
	long long counter = 1, times = 0;
	int flag = 0;
	int bytes;//���һ�ε��׶��˶����ֽ�
	if (msglen % 512 > 440)//����440����˼���ǵ���448�Ǿ�����W[13]��W[14]��W[15]�ǳ��ȣ��ǾͷŲ���0x80��
	{
		times = (msglen + 512 - 1) / 512;//����ȡ��
		flag = 1;
	}
	else if (msglen % 512 == 0)
	{
		times = (msglen + 512 - 1) / 512 + 1;//����һ��,��Ϊ����һ���飬�������һ����
		flag = 2;
	}
	else times = (msglen + 512 - 1) / 512;//����Ҫ����ô��Σ����ܻ��һ��

	A = H0; B = H1; C = H2; D = H3; E = H4;
	while (counter < times)//һ������������һ�Σ�������������������
	{
		fread(W, sizeof(char), 64, fp);//����һ����Ϣ��512bits,����W����
		for (int i = 0; i < 16; i++)//��˳��ת����,�õ���Ҫ�Ĵ洢˳��
		{
			W[i] = (W[i] >> 24) + (W[i] >> 8 & 0xff00) + (W[i] << 8 & 0xff0000) + (W[i] << 24);
		}
		for (int i = 0; i < 20; i++)
		{
			if (i >= 16)//W[i]�����Ժ�Ҫ����1λ
				W[i] = ((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]) << 1) |
				((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]) >> 31);
			subround(A, B, C, D, E, W[i], K0, 1);
		}

		for (int i = 20; i < 40; i++)
		{
			W[i] = ((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]) << 1) |
				((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]) >> 31);
			subround(A, B, C, D, E, W[i], K1, 2);
		}

		for (int i = 40; i < 60; i++)
		{
			W[i] = ((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]) << 1) |
				((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]) >> 31);
			subround(A, B, C, D, E, W[i], K2, 3);
		}

		for (int i = 60; i < 80; i++)
		{
			W[i] = ((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]) << 1) |
				((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]) >> 31);
			subround(A, B, C, D, E, W[i], K3, 4);
		}
		A = H0 = H0 + A;//�����Ժ����CVi
		B = H1 = H1 + B;
		C = H2 = H2 + C;
		D = H3 = H3 + D;
		E = H4 = H4 + E;
		counter++;
	}
	memset(W, 0, 64);
	unsigned char*p;
	if (flag == 0 || flag == 1)
	{

		bytes = fread(W, sizeof(char), 64, fp);//����һ����Ϣ��512bits,����W����	
		p = (unsigned char*)&W[0];
		p = p + bytes;
		*p = 0x80;
		p = (unsigned char*)&msglen;
		if (flag == 0)
		{
			memcpy(&W[14], p + 4, 4);//���Ƴ���
			memcpy(&W[15], p, 4);//���Ƴ��ȣ�Ҫ��һ�´洢˳��
			W[14] = (W[14] >> 24) + (W[14] >> 8 & 0xff00) + (W[14] << 8 & 0xff0000) + (W[14] << 24);
			W[15] = (W[15] >> 24) + (W[15] >> 8 & 0xff00) + (W[15] << 8 & 0xff0000) + (W[15] << 24);
		}
	}
	else if (flag == 2)
	{
		memset(W, 0, 64);
		p = (unsigned char*)&W[0];
		*p = 0x80;
		p = (unsigned char*)&msglen;
		memcpy(&W[14], p + 4, 4);//���Ƴ���
		memcpy(&W[15], p, 4);//���Ƴ��ȣ�Ҫ��һ�´洢˳��
		W[14] = (W[14] >> 24) + (W[14] >> 8 & 0xff00) + (W[14] << 8 & 0xff0000) + (W[14] << 24);
		W[15] = (W[15] >> 24) + (W[15] >> 8 & 0xff00) + (W[15] << 8 & 0xff0000) + (W[15] << 24);
	}
	for (int i = 0; i < 16; i++)//˳������ת����
	{
		W[i] = (W[i] >> 24) + (W[i] >> 8 & 0xff00) + (W[i] << 8 & 0xff0000) + (W[i] << 24);
	}
	A = H0; B = H1; C = H2; D = H3; E = H4;
	for (int i = 0; i < 20; i++)
	{
		if (i >= 16)
			W[i] = cirleft(W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16], 1);
		subround(A, B, C, D, E, W[i], K0, 1);
	}
	for (int i = 20; i < 40; i++)
	{
		W[i] = ((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]) << 1) |
			((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]) >> 31);
		subround(A, B, C, D, E, W[i], K1, 2);
	}
	for (int i = 40; i < 60; i++)
	{
		W[i] = ((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]) << 1) |
			((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]) >> 31);
		subround(A, B, C, D, E, W[i], K2, 3);
	}

	for (int i = 60; i < 80; i++)
	{
		W[i] = ((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]) << 1) |
			((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]) >> 31);
		subround(A, B, C, D, E, W[i], K3, 4);
	}
	A = H0 = H0 + A;
	B = H1 = H1 + B;
	C = H2 = H2 + C;
	D = H3 = H3 + D;
	E = H4 = H4 + E;
	if (flag == 1)
	{
		memset(W, 0, 64);
		p = (unsigned char*)&msglen;
		memcpy(&W[14], p + 4, 4);//���Ƴ���
		memcpy(&W[15], p, 4);//���Ƴ���
		//A = H0; B = H1; C = H2; D = H3; E = H4;
		for (int i = 0; i < 20; i++)
		{
			if (i >= 16)
				W[i] = ((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]) << 1) |
				((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]) >> 31);
			subround(A, B, C, D, E, W[i], K0, 1);
		}

		for (int i = 20; i < 40; i++)
		{
			W[i] = ((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]) << 1) |
				((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]) >> 31);
			subround(A, B, C, D, E, W[i], K1, 2);
		}

		for (int i = 40; i < 60; i++)
		{
			W[i] = ((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]) << 1) |
				((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]) >> 31);
			subround(A, B, C, D, E, W[i], K2, 3);
		}

		for (int i = 60; i < 80; i++)
		{
			W[i] = ((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]) << 1) |
				((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]) >> 31);
			subround(A, B, C, D, E, W[i], K3, 4);
		}
		A = H0 = H0 + A;
		B = H1 = H1 + B;
		C = H2 = H2 + C;
		D = H3 = H3 + D;
		E = H4 = H4 + E;
	}
	/*FILE*output;
	if ((output = fopen(outputfile, "wb")) == NULL)
	{
		printf("open %s faied!\n", outputfile);
		system("PAUSE");
		return 1;
	}*/
	sprintf(output, "%08X%08X%08X%08X%08X", A, B, C, D, E);
	//printf("%08X%08X%08X%08X%08X\n", A, B, C, D, E);
	fclose(fp);
	return 0;
}

int SHA1(char *inputfileaddr, char *output)
{
	uint32_t H0 = 0x67452301;
	uint32_t H1 = 0xEFCDAB89;
	uint32_t H2 = 0x98BADCFE;
	uint32_t H3 = 0x10325476;
	uint32_t H4 = 0xC3D2E1F0;
	//char inputfileaddr[81] = "input.file";//����
	//char outputfile[81] = "digest.file";//���
	uint32_t W[80];//�ÿռ任ʱ�䣬W����80��
	memset(W, 0, sizeof(W));//ԭʼ��Ϊ0�ͺ���
	uint32_t A, B, C, D, E;
	long long msglen = msgsize(inputfileaddr) * 8;//���ԭ��λ����������λ��λ�������ֽڣ�
	FILE *fp = NULL;
	if ((fp = fopen(inputfileaddr, "rb")) == NULL)
	{
		printf("open %s faied!\n", inputfileaddr);
		system("PAUSE");
		return 1;
	}
	long long counter = 1, times = 0;
	int flag = 0;
	int bytes;//���һ�ε��׶��˶����ֽ�
	if (msglen % 512 > 440)//����440����˼���ǵ���448�Ǿ�����W[13]��W[14]��W[15]�ǳ��ȣ��ǾͷŲ���0x80��
	{
		times = (msglen + 512 - 1) / 512;//����ȡ��
		flag = 1;
	}
	else if (msglen % 512 == 0)
	{
		times = (msglen + 512 - 1) / 512 + 1;//����һ��,��Ϊ����һ���飬�������һ����
		flag = 2;
	}
	else times = (msglen + 512 - 1) / 512;//����Ҫ����ô��Σ����ܻ��һ��

	A = H0; B = H1; C = H2; D = H3; E = H4;
	while (counter < times)//һ������������һ�Σ�������������������
	{
		fread(W, sizeof(char), 64, fp);//����һ����Ϣ��512bits,����W����
		for (int i = 0; i < 16; i++)//��˳��ת����,�õ���Ҫ�Ĵ洢˳��
		{
			W[i] = (W[i] >> 24) + (W[i] >> 8 & 0xff00) + (W[i] << 8 & 0xff0000) + (W[i] << 24);
		}
		for (int i = 0; i < 20; i++)
		{
			if (i >= 16)//W[i]�����Ժ�Ҫ����1λ
				W[i] = ((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]) << 1) |
				((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]) >> 31);
			subround(A, B, C, D, E, W[i], K0, 1);
		}

		for (int i = 20; i < 40; i++)
		{
			W[i] = ((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]) << 1) |
				((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]) >> 31);
			subround(A, B, C, D, E, W[i], K1, 2);
		}

		for (int i = 40; i < 60; i++)
		{
			W[i] = ((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]) << 1) |
				((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]) >> 31);
			subround(A, B, C, D, E, W[i], K2, 3);
		}

		for (int i = 60; i < 80; i++)
		{
			W[i] = ((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]) << 1) |
				((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]) >> 31);
			subround(A, B, C, D, E, W[i], K3, 4);
		}
		A = H0 = H0 + A;//�����Ժ����CVi
		B = H1 = H1 + B;
		C = H2 = H2 + C;
		D = H3 = H3 + D;
		E = H4 = H4 + E;
		counter++;
	}
	memset(W, 0, 64);
	unsigned char*p;
	if (flag == 0 || flag == 1)
	{

		bytes = fread(W, sizeof(char), 64, fp);//����һ����Ϣ��512bits,����W����	
		p = (unsigned char*)&W[0];
		p = p + bytes;
		*p = 0x80;
		p = (unsigned char*)&msglen;
		if (flag == 0)
		{
			memcpy(&W[14], p + 4, 4);//���Ƴ���
			memcpy(&W[15], p, 4);//���Ƴ��ȣ�Ҫ��һ�´洢˳��
			W[14] = (W[14] >> 24) + (W[14] >> 8 & 0xff00) + (W[14] << 8 & 0xff0000) + (W[14] << 24);
			W[15] = (W[15] >> 24) + (W[15] >> 8 & 0xff00) + (W[15] << 8 & 0xff0000) + (W[15] << 24);
		}
	}
	else if (flag == 2)
	{
		memset(W, 0, 64);
		p = (unsigned char*)&W[0];
		*p = 0x80;
		p = (unsigned char*)&msglen;
		memcpy(&W[14], p + 4, 4);//���Ƴ���
		memcpy(&W[15], p, 4);//���Ƴ��ȣ�Ҫ��һ�´洢˳��
		W[14] = (W[14] >> 24) + (W[14] >> 8 & 0xff00) + (W[14] << 8 & 0xff0000) + (W[14] << 24);
		W[15] = (W[15] >> 24) + (W[15] >> 8 & 0xff00) + (W[15] << 8 & 0xff0000) + (W[15] << 24);
	}
	for (int i = 0; i < 16; i++)//˳������ת����
	{
		W[i] = (W[i] >> 24) + (W[i] >> 8 & 0xff00) + (W[i] << 8 & 0xff0000) + (W[i] << 24);
	}
	A = H0; B = H1; C = H2; D = H3; E = H4;
	for (int i = 0; i < 20; i++)
	{
		if (i >= 16)
			W[i] = cirleft(W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16], 1);
		subround(A, B, C, D, E, W[i], K0, 1);
	}
	for (int i = 20; i < 40; i++)
	{
		W[i] = ((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]) << 1) |
			((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]) >> 31);
		subround(A, B, C, D, E, W[i], K1, 2);
	}
	for (int i = 40; i < 60; i++)
	{
		W[i] = ((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]) << 1) |
			((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]) >> 31);
		subround(A, B, C, D, E, W[i], K2, 3);
	}

	for (int i = 60; i < 80; i++)
	{
		W[i] = ((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]) << 1) |
			((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]) >> 31);
		subround(A, B, C, D, E, W[i], K3, 4);
	}
	A = H0 = H0 + A;
	B = H1 = H1 + B;
	C = H2 = H2 + C;
	D = H3 = H3 + D;
	E = H4 = H4 + E;
	if (flag == 1)
	{
		memset(W, 0, 64);
		p = (unsigned char*)&msglen;
		memcpy(&W[14], p + 4, 4);//���Ƴ���
		memcpy(&W[15], p, 4);//���Ƴ���
		//A = H0; B = H1; C = H2; D = H3; E = H4;
		for (int i = 0; i < 20; i++)
		{
			if (i >= 16)
				W[i] = ((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]) << 1) |
				((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]) >> 31);
			subround(A, B, C, D, E, W[i], K0, 1);
		}

		for (int i = 20; i < 40; i++)
		{
			W[i] = ((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]) << 1) |
				((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]) >> 31);
			subround(A, B, C, D, E, W[i], K1, 2);
		}

		for (int i = 40; i < 60; i++)
		{
			W[i] = ((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]) << 1) |
				((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]) >> 31);
			subround(A, B, C, D, E, W[i], K2, 3);
		}

		for (int i = 60; i < 80; i++)
		{
			W[i] = ((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]) << 1) |
				((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]) >> 31);
			subround(A, B, C, D, E, W[i], K3, 4);
		}
		A = H0 = H0 + A;
		B = H1 = H1 + B;
		C = H2 = H2 + C;
		D = H3 = H3 + D;
		E = H4 = H4 + E;
	}
	/*FILE*output;
	if ((output = fopen(outputfile, "wb")) == NULL)
	{
		printf("open %s faied!\n", outputfile);
		system("PAUSE");
		return 1;
	}*/
	sprintf(output, "%08X%08X%08X%08X%08X", A, B, C, D, E);
	//printf("%08X%08X%08X%08X%08X\n", A, B, C, D, E);
	fclose(fp);
	return 0;
}

int findprime(BN a, int bits)//Ѱ��һ��bitsλ������
{
	unsigned int prime[50] =
	{
		2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,113,
		127,131,137,139,149,151,157,163,167,173,179,181,191,193,197,199,211,223,227,229
	};
	BN temp1 = { 1,1 }, temp3;
	int judge[30] = { 0 };
	SETONEBIT_B(temp3, prime[34]);//����Ƿ�Χ������524���أ�34�ȽϺã�Ҳ���ǵ�35����

	BN bignum, fac, gcd, linshi, adjnum, linshi1;
	SETONEBIT_B(linshi, 2U);
	int fermatresult = 0;
	readbn(fac, "35primefac.txt");//ǰ35�������ĳ˻�������524���رȽϺ�
	int times = 0;

	//redo:

		/*genBN(bignum,bits);
		gcd_b(bignum, fac, gcd);
		cout << "bignum= " << bn2str(bignum) << endl;
		cout << "gcd= " << bn2str(gcd) << endl;*/
	while (fermatresult != 1)
	{
		genBN(bignum, bits);
		//cout << "line 1823 :" << bn2str(bignum) << endl;
		//system("pause");
		gcd_b(bignum, fac, gcd);
		//cout << "redo a bignum" << endl;
		//cout << "bignum= " << bn2str(bignum) << endl;
		//cout << "gcd= " << bn2str(gcd) << endl;
		if (cmp_b(gcd, ONE_BN) == 0)//���ɸѡΪ����,������
		{
			//cout << "bignum= " << bn2str(bignum) << endl;
			//cout << "û�е�����ֱ�ӷ���" << endl;
			//fermatresult = fermat_b(bignum);
			fermatresult = miller_b(bignum);
			times++;
			//cout << "������Ϊ  " << fermatresult << endl<<endl;//Ϊ1����ȷ��
			cpy_b(a, bignum);
			SETONEBIT_B(linshi, 2U);
		}
		else if (cmp_b(gcd, temp3) > 0 || cmp_b(gcd, TWO_BN) == 0)//������һ��
		{
		}
		else if (cmp_b(gcd, TWO_BN) != 0)//����,ǰ�᲻��ż���ŵ���
		{
			SETONEBIT_B(linshi, 2U);
			for (int i = 0; i < 30; i++)//����524���أ�30�ȽϺ�
			{
				add_b(bignum, linshi, adjnum);
				//cpy_b(bignum, adjnum);
				gcd_b(adjnum, fac, gcd);
				if (cmp_b(gcd, ONE_BN) == 0)
				{
					//fermatresult = fermat_b(adjnum);
					fermatresult = miller_b(adjnum);
					times++;
					//fermatresult = fermat_b(adjnum);
					fermatresult = miller_b(adjnum);
					//cout << " �Ѿ������˵Ĵ�����  " << bn2str(adjnum) << endl;//Ϊ1����ȷ��
					//cout << "�����Ժ������Ϊ  " << fermatresult << endl;//Ϊ1����ȷ��
					cpy_b(a, adjnum);
					//SETONEBIT_B(linshi, 2U);
					break;//��������������ȷ
				}
				else if (cmp_b(gcd, temp3) > 0)
				{
					break;
				}
				else
				{
					mul_b(gcd, linshi, linshi1);
					cpy_b(linshi, linshi1);
				}
			}
		}
	}
	cout << "���Լ�ⷢ���� " << times << " ��" << endl;
	/*if (getbits_b(bignum) != bits)
		goto redo;*/
	RMLDZRS_B(a);
	return FLAG_OK;
}

void genpq(char * p_path, char * q_path)
{
	LARGE_INTEGER start, finish, frequency;
	QueryPerformanceFrequency(&frequency);
	double quadpart = (double)frequency.QuadPart;
	double elapsed = 0;

	BN p, q;
	memset(p, 0, BNSIZE);
	memset(q, 0, BNSIZE);

	QueryPerformanceCounter(&start);
	findprime(p, 524);
	QueryPerformanceCounter(&finish);
	elapsed += (finish.QuadPart - start.QuadPart) / quadpart;
	printf("\nfind a %d bits prime process costs %f ms \n\n", getbits_b(p), elapsed * 1000);
	elapsed = 0;
	writebn(p_path, p);
	cout << "prime p=  " << bn2str(p) << endl;
	cout << "bits of p is    " << getbits_b(p) << endl << endl;


	//int realtime2 = clock();
	QueryPerformanceCounter(&start);
	findprime(q, 500);
	QueryPerformanceCounter(&finish);
	elapsed += (finish.QuadPart - start.QuadPart) / quadpart;
	printf("\nfind a %d bits prime process costs %f ms \n\n", getbits_b(q), elapsed * 1000);

	writebn(q_path, q);

	cout << "prime q=  " << bn2str(q) << endl;
	cout << "bits of q is    " << getbits_b(q) << endl << endl;
}

int checkresult(char * plainpath, char * decrypath)
{
	char plainhash[81] = { 0 };
	char decryhash[81] = { 0 };

	SHA1(plainpath, plainhash);
	SHA1(decrypath, decryhash);

	if (strcmp(plainhash, decryhash) != 0)
		return 0;
	else
		return 1;
}

int main()
{
	LARGE_INTEGER start, finish, frequency;
	QueryPerformanceFrequency(&frequency);
	double quadpart = (double)frequency.QuadPart;
	double elapsed = 0;

	BN p, q, n, eula, e, d;
	BN p_1, q_1;
	BN plain;//����
	BN cry;//�����Ժ�
	BN dec;//�����Ժ�

	BN temp1 = { 0 }, temp2 = { 0 }, temp3 = { 0 };//��������

	memset(p, 0, BNSIZE);
	memset(q, 0, BNSIZE);
	memset(p_1, 0, BNSIZE);
	memset(q_1, 0, BNSIZE);
	memset(n, 0, BNSIZE);
	memset(eula, 0, BNSIZE);
	memset(e, 0, BNSIZE);
	memset(d, 0, BNSIZE);
	memset(plain, 0, BNSIZE);
	memset(cry, 0, BNSIZE);
	memset(dec, 0, BNSIZE);

	int debug = 0;
	cout << "debug or not? 1 or 0" << endl;
	cin >> debug;
	if (debug == 1)
	{
		exclu();
		int bits = 0;
		cout << "how many bits prime do you want?" << endl;
		cin >> bits;
		//int realtime1 = clock();
		QueryPerformanceCounter(&start);
		findprime(p, bits);
		QueryPerformanceCounter(&finish);
		elapsed += (finish.QuadPart - start.QuadPart) / quadpart;
		printf("\nfind a %d bits prime process costs %f ms \n\n", getbits_b(p), elapsed * 1000);
		elapsed = 0;
		//printf("the time is %f seconds\n", );
		cout << "prime p=  " << bn2str(p) << endl;
		cout << "bits of p is    " << getbits_b(p) << endl << endl;
		system("pause");
		return 0;
	}

	if (debug == 2)
	{
		//char pstr[128] = "557615999999792";
		//char qstr[128] = "33825171222171";
		//char nstr[128] = "322331751";
		BN a, b,n,result;
		readbn(a, "D:/study/����ѧ/my�ر���RSA and BN/myp4.txt");
		readbn(b, "D:/study/����ѧ/my�ر���RSA and BN/myp4.txt");
		readbn(n, "D:/study/����ѧ/my�ر���RSA and BN/myq4.txt");
		//str2bn(a, pstr);
		//str2bn(b, qstr);
		//str2bn(n, nstr);
		//int judge = 0;
		QueryPerformanceCounter(&start);
		findprime(a, 512);
		QueryPerformanceCounter(&finish);
	//	if (judge)
	//		cout << "a��������\n" << endl;
	//	else
	//		cout << "a�Ǻ���\n" << endl;
		//cout << "��ģ��a^a (mod n)��" << endl;
		//cout << "a�� " << bn2str(a) << endl;
		//cout << "n�� " << bn2str(n) << endl;
		//cout << "n�� " << bn2str(n) << endl;
		
		//for (int i = 0; i < 1; i++)
	//	{
	//		modmul(a, a, n, result);
	//		add(a, a, a);
	//	}
		//modmul(a, a, n, result);
		//QueryPerformanceCounter(&finish);
		elapsed += (finish.QuadPart - start.QuadPart) / quadpart;
		printf("\n��%d��������λ�����֣����� %f ms \n\n", getbits_b(a), elapsed * 1000);
		//elapsed = 0;
		//cout << "��ȷ���ǣ�" << bn2str(result) << endl;

		//memset(result, 0, sizeof(result));
		//
		//int m = getbits_b(n);//ģ��������ϲ������Ҫģż��������ӽ��ܲ�̫���ܳ��֣���Ϊ��+1,R=2^mһ������n
		//BN RRN = { 0 };//R*R (mod n)
		//BN R = { 0 }, Rp = { 0 }, Np = { 0 };
		//BN a_t = { 1,1 }, b_t;//a=1;n���˶�����չ��
		//BN temp1 = { 0 }, temp2 = { 0 };//������Ϊresult�и��������
		//BN Xp = { 0 }, Yp = { 0 };
		//int Bits = (m + 31) / 32;//����ɴ��λ��
		//int remain = m - 32 * (Bits - 1);//��ߵġ�λ���ĵ�[remain-1]λΪ1��ԭ�ⲻ������[remain-2]..[0]λ�Ϳ���
		//R[0] = Bits;
		//R[Bits] = (uint32_t)(1U << (remain - 1));

		//sub(R, n, temp1);//temp1=-N
		//inv_b(temp1, R, Np);
		//modmul(R, R, n, RRN);
		////mont_modexp(a, a, n, result);
		//QueryPerformanceCounter(&start);
		//for (int i = 0; i < 1; i++)
		//{
		//	mont_modmul(a,a,R,n,Np,RRN,result);
		//	add(a, a, a);
		//}
		////modmul(a, a, n, result);
		//QueryPerformanceCounter(&finish);
		//elapsed += (finish.QuadPart - start.QuadPart) / quadpart;
		//printf("\n���ɸ���������ģ�ݻ��� %f ms \n\n", elapsed * 1000);
		//cout << "�ɸ���������ǣ�" << bn2str(result) << endl;
		////str2bn(n, pstr);
		////str2bn(a,qstr);
		//cout << "��a���棺" << endl;
		//cout << "a�� " << bn2str(a) << endl;
		//cout << "ģ��n�� " << bn2str(n) << endl;
		//elapsed = 0;
		//QueryPerformanceCounter(&start);
		//inv_b(a, n, result);
		//QueryPerformanceCounter(&finish);
		//elapsed += (finish.QuadPart - start.QuadPart) / quadpart;
		//printf("\nŷ����ó������滨�� %f ms \n\n", elapsed * 1000);
		//cout << "��ȷ��Ԫ�� " << bn2str(result) << endl;
		//elapsed = 0;
		//QueryPerformanceCounter(&start);
		//new_inv(a, n, result);
		//QueryPerformanceCounter(&finish);
		//elapsed += (finish.QuadPart - start.QuadPart) / quadpart;
		//printf("\n��һ�ַ������滨�� %f ms \n\n",  elapsed * 1000);
		//
		//cout << "��һ�ַ�����Ԫ�� " << bn2str(result) << endl;
		
		//str2bn(n, pstr);
		//str2bn(eula, qstr);
		//cout << "����" << endl;
		//readbn(p, "myp4.txt");
		//readbn(q, "myq4.txt");
		//readbn(plain, "plain.txt");
		//readbn(e, "e.txt");
		//uint64_t a = 1UL;
		//printf("%llX\n", a);
		//printf("%llX\n", a<<28);
		//printf("%llX\n", a << 32);
		/*subuint_b(p, 1U, p_1);
		subuint_b(q, 1U, q_1);*/
		//mul(p, q, n);
		//modexp_b(plain, e,n, cry);

		//cout << "p is " << bn2str(p) << endl;
		//cout << "q is " << bn2str(q) << endl;
		//cout << "cryo is " << bn2str(cry) << endl;

		//memset(p, 0, sizeof(p));
		//mul(d, d, p);

		//div_b(p, n, d, eula);
		//cout << "������  is " << bn2str(p) << endl;
		//cout << "����  is " << bn2str(n) << endl;
		//cout << "quo is " << bn2str(d) << endl;
		//cout << "remain is " << bn2str(eula) << endl;
		//unsigned char a = 0x1;
		//unsigned char b = 0x3;
		//unsigned char c = a - b;
		//printf("%X\n", c);
		system("pause");
		return 0;
	}
	//����p��q��һ������������˵Ĳ����Լ�����
	char p_path[81] = "myp4.txt";
	char q_path[81] = "myq4.txt";
	char e_path[81] = "e.txt";
	char plain_path[81] = "plain.txt";
	char cipher_path[81] = "cipher.txt";
	char decry_path[81] = "decry.txt";


	int ifgen = 0;//�Ƿ����˽Կ
	int ifset = 0;
	cout << "�Ƿ�Ҫ����˽Կp q? ����1����������0������\n";
	cin >> ifgen;
	cout << "�Ƿ�Ҫ�ı�Ĭ���ļ�·��? ����1�ı䣬����0���ı�\n";
	cout << "p �����" << p_path << endl;
	cout << "q �����" << q_path << endl;
	cout << "��Կe�����" << e_path << endl;
	cout << "���Ĵ����" << plain_path << endl;
	cout << "���Ĵ����" << cipher_path << endl;
	cout << "�����ļ������" << decry_path << endl;
	cin >> ifset;

	if (ifset == 1)
	{
		memset(p_path, 0, sizeof(p_path));
		memset(q_path, 0, sizeof(q_path));
		memset(e_path, 0, sizeof(e_path));
		memset(plain_path, 0, sizeof(plain_path));
		memset(cipher_path, 0, sizeof(cipher_path));
		memset(decry_path, 0, sizeof(decry_path));
		cout << "������p���·��" << endl;
		cin >> p_path;
		cout << "������q���·��" << endl;
		cin >> q_path;
		cout << "������e���·��" << endl;
		cin >> e_path;
		cout << "���������Ĵ��·��" << endl;
		cin >> plain_path;
		cout << "���������Ĵ��·��" << endl;
		cin >> cipher_path;
		cout << "��������ܺ���·��" << endl;
		cin >> decry_path;


		cout << "�����Ժ�: " << endl << endl;

		cout << "p �����" << p_path << endl;
		cout << "q �����" << q_path << endl;
		cout << "e �����" << e_path << endl;
		cout << "���Ĵ����" << plain_path << endl;
		cout << "���Ĵ����" << cipher_path << endl;
		cout << "�����ļ������" << decry_path << endl;

	}

	if (ifgen == 1)//Ϊ1�����
	{
		exclu();
		genpq(p_path, q_path);
		SETONEBIT_B(e, 10001u);
		writebn(e_path, e);
		readbn(p, p_path);
		readbn(q, q_path);
		readbn(e, e_path);
	}
	else
	{
		readbn(p, p_path);
		readbn(q, q_path);
		readbn(e, e_path);
		cout << "p is  " << bn2str(p) << endl;
		cout << "q is  " << bn2str(q) << endl;
		cout << "��ʼe is  " << bn2str(e) << endl;
	}


	//��ȡp/q


	//����n
	mul(p, q, n);
	cout << "n is  " << bn2str(n) << endl;
	writebn("n.txt", eula);

	subuint_b(p, 1u, p_1);
	cout << "p-1 is  " << bn2str(p_1) << endl;

	subuint_b(q, 1u, q_1);
	cout << "q-1 is  " << bn2str(q_1) << endl;

	mul(p_1, q_1, eula);
	cout << "��(n) is  " << bn2str(eula) << endl;
	writebn("fn.txt", eula);

	//һ�㹫Կe =65537

	gcd_b(e, eula, temp3);
	if (cmp_b(temp3, ONE_BN) != 0) {
		while (cmp_b(temp3, ONE_BN) != 0)//��������أ�����������
		{
			findprime(e, 17);
			gcd_b(e, eula, temp3);
		}
		cout << "֮ǰ��Կe���(n)�����أ��޸��Ժ��e is  " << bn2str(e) << endl;
		writebn("e.txt", e);
	}



	//˽Կd  ed =1 mod ��(n)
	inv_b(e, eula, d);
	cout << "d is  " << bn2str(d) << endl;
	writebn("d.txt", d);
	//��ȡ����
	readbn(plain, plain_path);
	cout << "������ is  " << bn2str(plain) << endl;
	//����
	//int time1 = clock();
	elapsed = 0;
	QueryPerformanceCounter(&start);
	modexp_b(plain, e, n, cry);
	QueryPerformanceCounter(&finish);
	elapsed += (finish.QuadPart - start.QuadPart) / quadpart;
	printf("���ܺ�ʱ%f ms\n", elapsed * 1000);
	cout << "������   " << bn2str(cry) << endl;

	writebn(cipher_path, cry);

	//����
	//int time2 = clock();
	//modexp_b(cry, d, n, dec);
	elapsed = 0;
	QueryPerformanceCounter(&start);
	crt_b(cry, d, p, q, dec);
	QueryPerformanceCounter(&finish);
	elapsed += (finish.QuadPart - start.QuadPart) / quadpart;
	printf("\n�����ܺ�ʱ%f ms\n", elapsed * 1000);
	cout << "����Ϊ  " << bn2str(dec) << endl;
	writebn(decry_path, dec);



	readbn(temp1, plain_path);
	readbn(temp2, decry_path);

	cout << endl << "����Ϊ" << bn2str(temp1);
	cout << endl << "����Ϊ" << bn2str(temp2) << endl;

	if (checkresult(plain_path, decry_path) == 1)
		cout << "\n�ӽ�����ȷ��" << endl;
	else
		cout << "\n�ӽ��ܳ��ִ���" << endl;
	system("pause");
	return 0;
}


