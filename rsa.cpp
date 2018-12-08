
#include "D:/homework and study/����ѧ/��������ҵ/bignum.h"
using namespace std;


//�ӷ����е�
int add_b(BN a, BN b, BN sum)
{
	memset(sum, 0, BNSIZE);
	uint32_t temps[BNMAXDGT + 2];//�Ȳ��ż������������temps����
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

//add����һ����ʽ��������sum���ܱ�Ԥ�ϵĶ�һλ������ģ�ӵ�ʱ���õõ���
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

//ODK����
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

//ͨ�õļ���
void sub(BN a, BN b, BN result)
{
	uint64_t carry = 0ULL;
	uint32_t *aptr, *bptr, *rptr, *maptr, *mbptr;

	int flag = FLAG_OK;

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
	//ѭ��,a[1]�Ѿ�����
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
	//cout<<"line580   tempr bits="<<getbits_b(tempr) << endl;
	cpy_b(result, tempr);
	//cout << "line580   result bits=" << getbits_b(result) << endl;
	return flag;
}

//ģ��
void modmul(BN a, BN b, BN n, BN & result)
{
	/*cout << "line 639  aλ��Ϊ " << getbits_b(a) << endl;
	cout << "line 639  bλ��Ϊ " << getbits_b(b) << endl;
	cout << "line 639  nλ��Ϊ " << getbits_b(n) << endl;*/
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
	/*cout << "line 653  �˻�λ��Ϊ " << getbits_b(temp) << endl;
	cout << "lint652?������653������" << endl;*/
	modn_b(temp, n, result);
	
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

	/*cout << "in line 1565 modn_b    a mod n =rem " << endl;
	cout << "line 1565 aλ��Ϊ " << getbits_b(a) << endl;
	cout << "line 1565 nλ��Ϊ " << getbits_b(n) << endl;*/
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
	flag= div_b(a, b, q, temp);
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
	
	sub_b(a, ONE_BN, temp1);//temp1=a-1
	
	//sub(a, ONE_BN, temp1);//temp1=a-1
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
	for (int i = 0; i < 30; i++)
	{
		SETONEBIT_B(temp2, prime[i]);
		
		mul_b(temp1, temp2, temp3);
		
		cpy_b(temp1, temp3);
		RMLDZRS_B(temp1);
		}
	writebn("30primefac.txt",temp1);
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
	int error =FLAG_OK;

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
			error =FLAG_OF;
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

	while ((high &bit32) == 0)
	{
		high = high << 1;
		bits--;
	}
	return bits;
}

//�ɿ��ĳ���,��Ȼ����
int div_b(BN a, BN b, BN q, BN rem)
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
	//uint32_t r_t[2 + (BNMAXDGT << 1)];//������2048λ��һ��ʲô�ģ�����Ҫ��1λ�泤�ȣ�һλd*a���ܱ�a��һλ
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

//����bits��������λ�Ĵ���result
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
	int times = (bits + 31 )/ 32;
	//cout << "times= "<<times << endl;
	int remain = bits % 32;
	BN a = { 0 }, temp = { 0 };
	SETDIGITS_B(a, times);
	uint32_t perdig=0;
	int i = 0;
	for (; i < times-1; i++)//���32λ�Ȳ���
	{
		writerand(randpath);		
		memset(digest, 0, sizeof(digest));
		mysha1(randpath, digest);//��40���ֽڣ�160λ��
		memcpy(onebit, digest, 8);
		perdig=stoul(onebit,NULL, 16);
		a[i + 1] = perdig;
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
	a[i + 1] = perdig;
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
		printf("write to  %s failed\n",addr);
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
	BN temp1 = { 1,1 }, temp2, temp3;
	int judge[30] = { 0 };
	SETONEBIT_B(temp3, prime[19]);//����Ƿ�Χ
	
	BN bignum, fac,gcd,linshi,adjnum,linshi1;
	SETONEBIT_B(linshi, 2U);
	int fermatresult=0;
	readbn(fac, "20primefac.txt");//ǰ30�������ĳ˻�
	int times = 0;

//redo:
	
	/*genBN(bignum,bits);
	gcd_b(bignum, fac, gcd);
	cout << "bignum= " << bn2str(bignum) << endl;
	cout << "gcd= " << bn2str(gcd) << endl;*/
	while (fermatresult != 1)
	{
		genBN(bignum, bits);
		gcd_b(bignum, fac, gcd);
		//cout << "redo a bignum" << endl;
		//cout << "bignum= " << bn2str(bignum) << endl;
		//cout << "gcd= " << bn2str(gcd) << endl;
		if (cmp_b(gcd, ONE_BN) == 0)//���ɸѡΪ����,������
		{
			//cout << "bignum= " << bn2str(bignum) << endl;
			//cout << "û�е�����ֱ�ӷ���" << endl;
			fermatresult = fermat_b(bignum);
			times++;
			//cout << "������Ϊ  " << fermatresult << endl<<endl;//Ϊ1����ȷ��
			cpy_b(a, bignum);
			SETONEBIT_B(linshi, 2U);
		}
		else if (cmp_b(gcd, temp3) > 0||cmp_b(gcd,TWO_BN)==0)//������һ��
		{		
		}
		else if(cmp_b(gcd, TWO_BN) != 0)//����,ǰ�᲻��ż���ŵ���
		{
			SETONEBIT_B(linshi, 2U);
			for (int i = 0; i < 17; i++)
			{
				add_b(bignum, linshi, adjnum);
				//cpy_b(bignum, adjnum);
				gcd_b(adjnum, fac, gcd);
				if (cmp_b(gcd, ONE_BN) == 0)
				{
					fermatresult = fermat_b(adjnum);
					times++;
					fermatresult = fermat_b(adjnum);
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
	cout << "���������� " << times << " ��" << endl;
	/*if (getbits_b(bignum) != bits)
		goto redo;*/
	RMLDZRS_B(a);
	return FLAG_OK;
}

void genpq(char * p_path,char * q_path)
{
	BN p, q;
	memset(p, 0, BNSIZE);
	memset(q, 0, BNSIZE);
	int realtime1 = clock();
	findprime(p, 524);
	printf("\nfind a %d bits prime process costs %d ms \n\n", getbits_b(p), clock() - realtime1);
	writebn(p_path, p);
	cout << "prime p=  " << bn2str(p) << endl;
	cout << "bits of p is    " << getbits_b(p) << endl << endl;


	int realtime2 = clock();
	findprime(q, 500);
	printf("\nfind a %d bits prime process costs %d ms \n\n", getbits_b(q), clock() - realtime2);

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
	cout << "���Ĵ����" <<cipher_path << endl;
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
		cout << "������p���·��"<<endl;
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


		cout << "�����Ժ�: " << endl<<endl;

		cout << "p �����" << p_path << endl;
		cout << "q �����" << q_path << endl;
		cout << "e �����" << e_path << endl;
		cout << "���Ĵ����" << plain_path << endl;
		cout << "���Ĵ����" << cipher_path << endl;
		cout << "�����ļ������" << decry_path << endl;

	}

	if (ifgen == 1)//Ϊ1�����
	{
        genpq(p_path, q_path);
		SETONEBIT_B(e, 10001U);
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

	subuint_b(p, 1U, p_1);
	cout << "p-1 is  " << bn2str(p_1) << endl;

	subuint_b(q, 1U, q_1);
	cout << "q-1 is  " << bn2str(q_1) << endl;

	mul(p_1, q_1, eula);
	cout << "��(n) is  " << bn2str(eula) << endl;
	writebn("fn.txt", eula);
	
	//һ�㹫Կe =65537
	//SETONEBIT_B(e, 10001U);

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
	inv_b(e,eula, d);
	cout << "d is  " << bn2str(d) << endl;
	writebn("d.txt", d);
	//��ȡ����
	readbn(plain, plain_path);
	cout << "������ is  " << bn2str(plain) << endl;
	//����
	int time1 = clock();
	modexp_b(plain, e,n, cry);
	printf("���ܺ�ʱ%d ms\n", clock() - time1);
	cout << "������ is  " << bn2str(cry) << endl;
	
	writebn(cipher_path, cry);

	//����
	int time2 = clock();
	//modexp_b(cry, d, n, dec);
	crt_b(cry, d, p, q, dec);
	printf("\n�����ܺ�ʱ%d ms\n", clock() - time2);
	cout << "����Ϊ is  " << bn2str(dec) << endl;
	writebn(decry_path, dec);



	readbn(temp1, plain_path);
	readbn(temp2, decry_path);

	cout << endl << "����Ϊ" << bn2str(temp1);
	cout  << endl<<"����Ϊ" << bn2str(temp2)<<endl;

	if (checkresult(plain_path, decry_path) == 1)
		cout << "\n�ӽ�����ȷ��" << endl;
	else
		cout << "\n�ӽ��ܳ��ִ���" << endl;
	system("pause");
	return 0;
}


