
#include "D:/homework and study/密码学/第六次作业/bignum.h"
using namespace std;


//加法可行的
int add_b(BN a, BN b, BN sum)
{
	memset(sum, 0, BNSIZE);
	uint32_t temps[BNMAXDGT + 2];//先不着急传结果，存在temps里面
	memset(temps, 0, sizeof(temps));
	uint32_t *aptr, *bptr, *sptr = LSDPTR_B(temps);//指向a,b,temp的末位
	uint32_t *maptr, *mbptr;//指向a和b的最高有效位的指针
	int flag = FLAG_OK;//初始化为OK
	uint64_t carry = 0ULL;//低32位表示中间和，高32位表示进位
	if (DIGITS_B(a) < DIGITS_B(b))//如果a的位数比b小，以b为参考做第一次循环
	{
		aptr = LSDPTR_B(b);//b是长的
		maptr = MSDPTR_B(b);
		bptr = LSDPTR_B(a);//a是短的
		mbptr = MSDPTR_B(a);
		SETDIGITS_B(temps, DIGITS_B(b));//要么是b的位数，要么比b的位数多一位，再说
	}
	else//其他情况a要么比b长，要么和b一样长
	{
		aptr = LSDPTR_B(a);//a是长的
		maptr = MSDPTR_B(a);
		bptr = LSDPTR_B(b);//b是短的
		mbptr = MSDPTR_B(b);
		SETDIGITS_B(temps, DIGITS_B(a));//要么是a的位数，要么比a的位数多一位，再说
	}
	while (bptr <= mbptr)//b的位数还没完，至少有1位吧
	{
		carry = (uint64_t)*aptr + (uint64_t)*bptr + (uint64_t)(uint32_t)(carry >> BITPERDIGIT);//a+b+(ci-1)
		*sptr = (uint32_t)(carry);//低32位是中间和
		aptr++; bptr++; sptr++;//下一位
	}
	while (aptr <= maptr)//a的位数或许还没有完结，继续把a算完
	{
		carry = (uint64_t)*aptr + (uint64_t)(uint32_t)(carry >> BITPERDIGIT);//a+(ci-1)
		*sptr = (uint32_t)(carry);//低32位是中间和
		aptr++;  sptr++;//下一位,可能连续进位
	}
	if (carry&BASE)//如果最后carry有1位进位，和的“位”数比a多1“位”
	{
		*sptr = 1;//置1，就是1
		SETDIGITS_B(temps, DIGITS_B(temps) + 1);//比之前多一位
	}
	if (DIGITS_B(temps) > (uint32_t)BNMAXDGT)//超过了32“位”，上溢出了
	{
		SETDIGITS_B(temps, BNMAXDGT);//先设置32“位”，可能有前导0,模掉大于32“位”的
		RMLDZRS_B(temps);
		flag = FLAG_OF;//加法上溢出了
	}
	cpy_b(sum, temps);
	return flag;
}

//add的另一个形式，区别是sum可能比预料的多一位，在求模加的时候用得到。
void add(BN a, BN b, BN sum)
{
	uint32_t *aptr, *bptr, *sptr = LSDPTR_B(sum);//指向a,b,sum的末位
	uint32_t *maptr, *mbptr;//指向a和b的最高有效位的指针
	int flag = FLAG_OK;//初始化为OK
	uint64_t carry = 0ULL;//低32位表示中间和，高32位表示进位
	if (DIGITS_B(a) < DIGITS_B(b))//如果a的位数比b小，以b为参考做第一次循环
	{
		aptr = LSDPTR_B(b);//b是长的
		maptr = MSDPTR_B(b);
		bptr = LSDPTR_B(a);//a是短的
		mbptr = MSDPTR_B(a);
		SETDIGITS_B(sum, DIGITS_B(b));//要么是b的位数，要么比b的位数多一位，再说
	}
	else//其他情况a要么比b长，要么和b一样长
	{
		aptr = LSDPTR_B(a);//a是长的
		maptr = MSDPTR_B(a);
		bptr = LSDPTR_B(b);//b是短的
		mbptr = MSDPTR_B(b);
		SETDIGITS_B(sum, DIGITS_B(a));//要么是a的位数，要么比a的位数多一位，再说
	}
	while (bptr <= mbptr)//b的位数还没完，至少有1位吧
	{
		carry = (uint64_t)*aptr + (uint64_t)*bptr + (uint64_t)(uint32_t)(carry >> BITPERDIGIT);//a+b+(ci-1)
		*sptr = (uint32_t)(carry);//低32位是中间和
		aptr++; bptr++; sptr++;//下一位
	}
	while (aptr <= maptr)//a的位数或许还没有完结，继续把a算完
	{
		carry = (uint64_t)*aptr + (uint64_t)(uint32_t)(carry >> BITPERDIGIT);//a+(ci-1)
		*sptr = (uint32_t)(carry);//低32位是中间和
		aptr++;  sptr++;//下一位,可能连续进位
	}
	if (carry&BASE)//如果最后carry有1位进位，和的“位”数比a多1“位”
	{
		*sptr = 1;//置1，就是1
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
	if (cmp_b(n, ZERO_BN) == 0)//如果模数为0
		add(a, b, result);
	else
	{
		add(a, b, temp1);
		flag = modn_b(temp1, n, temp2);
		cpy_b(result, temp2);
	}
	return flag;
}

//加一个uint32
int adduint_b(BN a, uint32_t b, BN sum)
{
	BND temp = { 0 };
	temp[0] = 1; temp[1] = b;//现在temp是大数
	add(a, temp, sum);
	return 0;
}

int adduint_b(uint32_t a, BN b, BN &sum)
{
	BN temp = { 0 };
	temp[0] = 1; temp[1] = a;//现在temp是大数
	int flag = add_b(b, temp, sum);
	return flag;
}

//ODK减法
int sub_b(BN a, BN b, BN result)
{
	memset(result, 0, BNSIZE);
	uint64_t carry = 0ULL;
	uint32_t *aptr, *bptr, *rptr, *maptr, *mbptr;

	int flag = FLAG_OK;//检验是否发生意外下溢

	uint32_t a_t[BITPERDIGIT + 2];//多了1位
	BN b_t;
	memset(b_t, 0, sizeof(b_t));
	cpy_b(a_t, a);
	cpy_b(b_t, b);
	aptr = LSDPTR_B(a_t);
	bptr = LSDPTR_B(b_t);
	rptr = LSDPTR_B(result);
	maptr = MSDPTR_B(a_t);
	mbptr = MSDPTR_B(b_t);

	if (cmp_b(a_t, b_t) == -1)//如果a<b
	{
		setmax_b(a_t);
		maptr = a_t + BNMAXDGT;//指向[31]
		SETDIGITS_B(result, BNMAXDGT);//怕是结果也有这么多位，先设这么多最后消除
		flag = FLAG_UF;//下溢了
	}
	else//没有发生下溢
	{
		SETDIGITS_B(result, DIGITS_B(a_t));//位数应该和a是一样的
	}
	while (bptr <= mbptr)//b还有位数，类比加法
	{
		carry = (uint64_t)*aptr - (uint64_t)*bptr - ((carry&BASE) >> BITPERDIGIT);//a-b-ci(可能之前借位了)
		*rptr = (uint32_t)carry;
		aptr++; bptr++; rptr++;
	}
	while (aptr <= maptr)//a还没有完,类比加法
	{
		carry = (uint64_t)*aptr - ((carry&BASE) >> BITPERDIGIT);//a-b-ci(可能之前借位了)
		*rptr = (uint32_t)carry;
		aptr++; rptr++;
	}
	RMLDZRS_B(result);//消除前导0
	if (flag == FLAG_UF)//如果下溢了，更正一下
	{
		add_b(result, a, result);
		add_b(result, ONE_BN, result);//(Nm-b+a)+1
	}
	return flag;
}
//减一个uint32
int subuint_b(BN a, uint32_t b, BN &result)
{
	BN temp = { 0 };
	temp[0] = 1; temp[1] = b;//现在temp是大数
	int flag = sub_b(a, temp, result);
	return flag;
}

//通用的减法
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
	while (bptr <= mbptr)//b还有位数，类比加法
	{
		carry = (uint64_t)*aptr - (uint64_t)*bptr - ((carry&BASE) >> BITPERDIGIT);//a-b-ci(可能之前借位了)
		*rptr = (uint32_t)carry;
		aptr++; bptr++; rptr++;
	}
	while (aptr <= maptr)//a还没有完,类比加法
	{
		carry = (uint64_t)*aptr - ((carry&BASE) >> BITPERDIGIT);//a-b-ci(可能之前借位了)
		*rptr = (uint32_t)carry;
		aptr++; rptr++;
	}
	RMLDZRS_B(result);//消除前导0
}

void modsub_b(BN a, BN b, BN n, BN &result)
{
	BN a_t, b_t, rem, temp;
	memset(rem, 0, sizeof(rem));
	memset(rem, 0, sizeof(temp));
	cpy_b(a_t, a);
	cpy_b(b_t, b);
	if (cmp_b(a_t, b_t) >= 0)//如果a>=b
	{
		//sub_b(a_t, b_t, temp);
		sub(a_t, b_t, temp);
		//cout << bn2str(a_t) << " - " << bn2str(b_t) << " = " << bn2str(temp) << endl;

		modn_b(temp, n, rem);
		//cout << "zhe cuo le line 424!" << endl;
		//cout << bn2str(temp) << " % " << bn2str(n) << " = " << bn2str(rem) << endl;
		cpy_b(result, rem);
	}
	else //a<b...那就b-a=t，然后用n-t  12-19 mod 5  19-12 mod 5 =2  5-2=3
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

//乘法hhh
int mul_b(BN a, BN b, BN &result)
{
	int flag = FLAG_OK;
	uint32_t * aptr, *bptr, *maptr, *mbptr, *bcir, *acir, *rptr, *cptr;
	uint32_t ta;//暂时存放a
	uint64_t carry;//保存本位和进位
	BND tempr;//存储最终结果，可能是两倍的BN长度，2048位，不会更长了！
	memset(tempr, 0, sizeof(tempr));
	BN aa, bb;
	memset(aa, 0, BNSIZE);
	memset(bb, 0, BNSIZE);
	memset(result, 0, BNSIZE);
	cpy_b(aa, a); cpy_b(bb, b);//复制
	//RMLDZRS_B(aa);//去掉前导0
	//RMLDZRS_B(bb);
	if (DIGITS_B(aa) == 0 || DIGITS_B(bb) == 0)//有没有乘数为0的投机取巧
	{
		result[0] = 0; result[1] = 0;//结果为0
		return flag;
	}
	if (DIGITS_B(aa) < DIGITS_B(bb))//如果a的长度比b的短，和加减一样处理
	{
		aptr = bb;
		bptr = aa;
	}
	else
	{
		aptr = aa;
		bptr = bb;
	}
	maptr = aptr + *aptr;//指向最高有效位
	mbptr = bptr + *bptr;//指向最高有效位

	carry = 0;
	ta = *LSDPTR_B(aptr);//末位
	//完成(bn-1bn-2...b1)·a0
	for (bcir = LSDPTR_B(bptr), rptr = LSDPTR_B(tempr); bcir <= mbptr;
		bcir++, rptr++)
	{
		carry = (uint64_t)ta * (uint64_t)*bcir + (uint64_t)(uint32_t)(carry >> BITPERDIGIT);
		*rptr = (uint32_t)carry;

	}
	*rptr = (uint32_t)(carry >> BITPERDIGIT);//最后一次的进位
	//循环,a[1]已经做了
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
		*rptr = (uint32_t)(carry >> BITPERDIGIT);//最后一次的进位
	}
	SETDIGITS_B(tempr, DIGITS_B(aptr) + DIGITS_B(bptr));//可能是a+b“位”
	RMLDZRS_B(tempr);//去掉前导0
	if (DIGITS_B(tempr) > (uint32_t)BNMAXDGT)//乘法溢出，超过了32“位”
	{
		SETDIGITS_B(tempr, BNMAXDGT);//先设置32“位”，可能有前导0,模掉大于32“位”的
		RMLDZRS_B(tempr);
		flag = FLAG_OF;//上溢出了
	}
	cpy_b(result, tempr);
	return flag;
}

//没有模2^32^32的乘法
int mul(BN a, BN b, BN result)
{
	int flag = FLAG_OK;
	uint32_t * aptr, *bptr, *maptr, *mbptr, *bcir, *acir, *rptr, *cptr;
	uint32_t ta;//暂时存放a
	uint64_t carry;//保存本位和进位
	BND tempr;//存储最终结果，可能是两倍的BN长度，2048位，不会更长了！
	memset(tempr, 0, sizeof(tempr));
	memset(result, 0, sizeof(result));
	if (DIGITS_B(a) == 0 || DIGITS_B(b) == 0)//有没有乘数为0的投机取巧
	{
		result[0] = 0; result[1] = 0;//结果为0
		return flag;
	}
	if (DIGITS_B(a) < DIGITS_B(b))//如果a的长度比b的短，和加减一样处理
	{
		aptr = b;
		bptr = a;
	}
	else
	{
		aptr = a;
		bptr = b;
	}
	maptr = aptr + *aptr;//指向最高有效位
	mbptr = bptr + *bptr;//指向最高有效位

	carry = 0;
	ta = *LSDPTR_B(aptr);//末位
	//完成(bn-1bn-2...b1)·a0
	for (bcir = LSDPTR_B(bptr), rptr = LSDPTR_B(tempr); bcir <= mbptr;
		bcir++, rptr++)
	{
		carry = (uint64_t)ta * (uint64_t)*bcir + (uint64_t)(uint32_t)(carry >> BITPERDIGIT);
		*rptr = (uint32_t)carry;

	}
	*rptr = (uint32_t)(carry >> BITPERDIGIT);//最后一次的进位
	//循环,a[0]已经做了
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
		*rptr = (uint32_t)(carry >> BITPERDIGIT);//最后一次的进位
	}
	SETDIGITS_B(tempr, DIGITS_B(aptr) + DIGITS_B(bptr));//可能是a+b“位”
	RMLDZRS_B(tempr);//去掉前导0
	//cout<<"line580   tempr bits="<<getbits_b(tempr) << endl;
	cpy_b(result, tempr);
	//cout << "line580   result bits=" << getbits_b(result) << endl;
	return flag;
}

//模乘
void modmul(BN a, BN b, BN n, BN & result)
{
	/*cout << "line 639  a位数为 " << getbits_b(a) << endl;
	cout << "line 639  b位数为 " << getbits_b(b) << endl;
	cout << "line 639  n位数为 " << getbits_b(n) << endl;*/
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
	/*cout << "line 653  乘积位数为 " << getbits_b(temp) << endl;
	cout << "lint652?问题在653！！！" << endl;*/
	modn_b(temp, n, result);
	
}


string bn2str(BN bignum)//大数转为字符串
{
	if (DIGITS_B(bignum) == 0)//如果是0需要显示这个0
		return string("0");
	//char strbignum[265] = { 0 };//(1024+32)/4
	//BN temp;
	char strbignum[520] = { 0 };//(1024+32)/4
	BND temp;
	cpy_b(temp, bignum);

	for (int i = DIGITS_B(temp), j = 0; i > 0; i = i - 1, j++)//第几位索引就是几！！！
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
	//int index=rmzero.rfind("0");//去掉看得见的前导0
	if (zeros >= 1)
	{
		//string finalstr(strbignum + zeros, &strbignum[264]);
		string finalstr(strbignum + zeros, &strbignum[519]);
		char * cstr = new char[finalstr.length() + 1];
		std::strcpy(cstr, finalstr.c_str());
		finalstr = string(cstr);//去掉NULL 
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

int str2bn(BN & bignum, string strbn)//字符串转换为大数
{
	BN t = { 0 };
	char *perbit = new char[9];
	memset(perbit, 0, 9);
	int digits = 0;
	digits = (strbn.length() + 7) / 8;//位数
	//cout << "digits=" << digits << endl;
	int i = 0;
	int pos = strbn.length() - 8;
	for (i = 0; i < digits - 1; i++)//不能是digits-1!!!!!
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

int readbn(BN &bignum, string filename)//从文本中读取大数
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
	//cout << "大数是 " << bn2str(temp)<< endl;
	fin.close();
	return FLAG_OK;
}

int writebn(string filename, BN bignum)//写入大数到文本
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
	cout << "line 1565 a位数为 " << getbits_b(a) << endl;
	cout << "line 1565 n位数为 " << getbits_b(n) << endl;*/
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

	BN a_t, b_t, rn_1, rn;//存放两个余数用
	memset(a_t, 0, BNSIZE);
	memset(b_t, 0, BNSIZE);
	memset(rn, 0, BNSIZE);
	cpy_b(a_t, a);
	cpy_b(b_t, b);
	cpy_b(rn_1, b);//万一rn=0
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

//如果(a,n)=1,则有ax = 1 (mod n)；否则异常没有逆元，返回0，显然逆元不可能为0  
int inv_b(BN a, BN n, BN & x)
{
	
	memset(x, 0, BNSIZE);
	BN u = { 0 }, g = { 0 }, v1 = { 0 }, v3 = { 0 }, q = { 0 }, t3 = { 0 }, t1 = { 0 };
	BN temp;//保存中间结果
	gcd_b(a, n, g);
	

	if (cmp_b(g, ONE_BN) != 0)//如果不互素，没有逆元
	{
		SETZERO_B(x);
		return FLAG_NOINV;
	}
	memset(g, 0, sizeof(g));
	//初始化
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
	BN a_t = { 1,1 }, b_t;//a=1;n做了二进制展开
	BN temp1 = { 0 }, temp2 = { 0 };//计算作为result有个清零操作
	cpy_b(b_t, b);//b_t=b,初始化
	uint32_t *nptr, *mnptr;
	nptr = LSDPTR_B(n);
	mnptr = MSDPTR_B(n);//！！！！！！！
	char binform[33];//每个32bit的uint32转化为二进制即可，一次次取出来
	int i = 0;
	//cout << "\na= " << bn2str(a_t) << "  b= " << bn2str(b_t) << endl << endl;
	while (nptr <= mnptr)//没越界就都来做
	{

		memset(binform, 0, sizeof(binform));
		_ultoa(*nptr, binform, 2);
		//printf("binform=%s\n", binform);
		i = strlen(binform) - 1;//到达最后一位
		for (int j = 31; j >= 0; j--)//开始模平方
		{
			//cout << "\nai-1= " << bn2str(a_t) << "  bi-1= " << bn2str(b_t) << endl;
			if (i >= 0)//正事儿，否则只是平方b取模
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

				//其它情况下不用动a_t
				i--;
				
			}
			//cout << "j=" << j << endl;
			/*cout << "b_t位数为 " << getbits_b(b_t) << endl;*/
			
			modmul(b_t, b_t, m, temp2);//b=b*b mod m	
			
			/*cout << "问题出在modmul!!!line 1776" << endl;*/
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

//费马检测
int fermat_b(BN a)
{
	BN n2 = { 1,2 };
	BN n3 = { 1,3 };
	BN n5 = { 1,5 };
	BN n7 = { 1,7 };

	BN temp1, temp2;

	if (a[1] % 2 == 0 || a[1] % 5 == 0)
		return 0;//否
	
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
	modexp_b(n2, temp1, a, temp2);// n2^(a-1) mod a!=1就出错
	if (temp2[0] > 1 || temp2[1] != 1)
		return 0;
	//cout << "here" << endl;
	
	modexp_b(n3, temp1, a, temp2);// n2^(a-1) mod a!=1就出错
	if (temp2[0] > 1 || temp2[1] != 1)
		return 0;

	modexp_b(n5, temp1, a, temp2);// n2^(a-1) mod a!=1就出错
	if (temp2[0] > 1 || temp2[1] != 1)
		return 0;

	modexp_b(n7, temp1, a, temp2);// n2^(a-1) mod a!=1就出错
	if (temp2[0] > 1 || temp2[1] != 1)
		return 0;

	return 1;//是素数
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
void crt_b(BN a, BN b, BN p, BN q, BN & result)//计算a^b mod (p*q)
{
	BN b1, b2, M1p, M2p, m;
	BN tb1, tb2, pdec1, qdec1;//模掉欧拉函数以后的分别的指数
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
H=前50个素数乘积，如果gcd(p,50)=x,x>1，那么，如果x=7，p+2*3*5肯定不是2 3 5的，然后再从头来弄  
如果 p+2*3*5返回的gcd有2/3/5，那回退一步，之前的加法不要2/3/5中的一个或者两个
*/

void exclu()
{
	//前50个素数
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

//右移1位
int shr_b(BN a)
{
	uint32_t *aptr;
	uint32_t current, undercarry = 0;

	if (DIGITS_B(a) == 0)//为0就不用移动了
	{
		return FLAG_UF;        
	}

	for (aptr = MSDPTR_B(a); aptr > a; aptr--)
	{
		current = (uint32_t)((uint32_t)(*aptr >> 1) | (uint32_t)(undercarry << (BITPERDIGIT - 1)));

		undercarry = (uint32_t)(*aptr & 1U);//从高“位”移入低“位”中的那位，将在下一位最高位

		*aptr = current;//设置为本位左移1位，本位最高位为从前一位移下来的
	}

	RMLDZRS_B(a);
	return FLAG_OK;
}
//左移1位
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

	if (carry >> BITPERDIGIT)//如果carry最后还进了1位，和加法一样处理，加一位1，同时加1“位”
	{
		if (DIGITS_B(a) < BNDMAXBIT)//如果在BND范围内
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

//获取二进制位数，可以是BND a
uint32_t getbits_b(BN a)
{
	uint32_t bits = DIGITS_B(a);
	uint32_t high;

	while (a[bits] == 0 && bits > 0)//消除前面的0 
	{
		bits--;
	}

	high = a[bits];//high指向最高有效位
	if (high == 0 && bits == 0)//0
		return 0;

	bits = bits << 5;//每位32个二进制位
	uint32_t bit32 = 0x80000000U;

	while ((high &bit32) == 0)
	{
		high = high << 1;
		bits--;
	}
	return bits;
}

//可靠的除法,虽然不快
int div_b(BN a, BN b, BN q, BN rem)
{
	
	/*cout << "\nin line 2179  a=qb+rem \n " << endl;
	cout << "line 2179 a位数为 " << getbits_b(a) << endl;
	cout << "line 2179 b位数为 " << getbits_b(b) << endl;*/
	memset(rem, 0, sizeof(rem));
	memset(q, 0, sizeof(q));
	//uint32_t *rptr, *bptr,*mbptr,*mrptr;
	BND b_t;//临时存放b
	BND tempq = { 0 };
	BND tempsub = { 0 };//存放商和临时差
	BND q_t = { 0 };//每个商
	//uint32_t r_t[2 + (BNMAXDGT << 1)];//可能是2048位除一个什么的，还需要多1位存长度，一位d*a可能比a多一位
	BND r_t;

	cpy_b(r_t, a);
	cpy_b(b_t, b);
	if (DIGITS_B(b) == 0)//如果b是0,除0错误
		return FLAG_DIVZERO;
	else if (DIGITS_B(r_t) == 0)//如果a=0
	{
		SETZERO_B(q);
		SETZERO_B(rem);
		return FLAG_OK;
	}
	else if (cmp_b(r_t, b_t) == -1)//如果a<b,返回a就好了
	{
		cpy_b(rem, r_t);
		SETZERO_B(q);//商为0
		return FLAG_OK;
	}
	else if (cmp_b(r_t, b_t) == 0)//如果a=b,返回1就好了
	{
		q[0] = 1; q[1] = 1;//商为1
		SETZERO_B(rem);//余数为0
		return FLAG_OK;
	}
	else if (DIGITS_B(r_t) == 0)//如果a=0，非常好
	{
		SETZERO_B(q);
		SETZERO_B(rem);
		return FLAG_OK;
	}
	//其它情况下
	SETDIGITS_B(q_t, DIGITS_B(r_t) - DIGITS_B(b_t) + 1);
	int abit = getbits_b(r_t);
	int bbit = getbits_b(b);

	int shiftnum = abit - bbit;
	//cout << "shiftnum= "<<shiftnum << endl;
	int subtimes = abit - bbit + 1;
	//cout << "line 2263移位以前  b_t  " << bn2str(b_t) << endl;

	for (int i = 0; i < shiftnum; i++)
		shl_b(b_t);
	//shift_b(b_t, shiftnum);//左移对齐最高位,以后每次右移一位,移动以后的b_t
	//cout << "line 2263移位以后  b_t  " << bn2str(b_t) << endl;
	//cout << "line 2263  r_t 位数为  " << getbits_b(r_t) << endl;
	//cout << "line 2263移位以后  b_t 位数为  " << getbits_b(b_t)<< endl;
	//cout << "a的位数是:" << abit << endl;
	//cout << "b的位数是:" << bbit << endl;
	//cout <<"减法次数" <<subtimes << endl;
	for (int i = 0; i < subtimes; i++) {
		//cout << "line 2252 subtimes"<<i << endl;
		if (cmp_b(r_t, b_t) >= 0)//必须有等号！！！！！
		{
			//sub_b(r_t, b_t, tempsub);
			
			sub(r_t, b_t, tempsub);
			
			cpy_b(r_t, tempsub);
			//if (*(q_t) > 1)
			shl_b(q_t);
			
			adduint_b(q_t, 1U, tempq);//上1
		/*	if (i == 1025)cout << "line 2252 here" << i << endl;*/
			cpy_b(q_t, tempq);
			//cout <<"1:  "<< getbits_b(q_t) << endl;
			shr_b(b_t);
			//cout << i<<"  1 :" << bn2str(q_t) << endl;
			//cout << "line 2263" << bn2str(q_t) << endl;
		}
		else
		{
			shl_b(q_t);//商0
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

//产生bits个二进制位的大数result
int genBN(BN result, int bits)
{
	if (bits < 0 || bits>BNMAXBIT)//无法产生更大位数的
		return FLAG_ERROR;
	if (bits == 0)
	{
		SETZERO_B(result);
		return FLAG_OK;
	}
	//其它情况正常处理

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
	for (; i < times-1; i++)//最高32位先不动
	{
		writerand(randpath);		
		memset(digest, 0, sizeof(digest));
		mysha1(randpath, digest);//有40个字节，160位数
		memcpy(onebit, digest, 8);
		perdig=stoul(onebit,NULL, 16);
		a[i + 1] = perdig;
	}
	int shiftright = 0;
	uint32_t bit32 = 0x80000000U;
	if (remain == 0)//是32倍数，要凑出32位！
	{
		while (true)
		{
			writerand(randpath);
			memset(digest, 0, sizeof(digest));
			mysha1(randpath, digest);//有40个字节，160位数
			memcpy(onebit, digest, 8);
			perdig = stoul(onebit, NULL, 16);
			if (perdig >= bit32)//要凑够
				break;
		}
	}
	else
	{
		writerand(randpath);
		memset(digest, 0, sizeof(digest));
		mysha1(randpath, digest);//有40个字节，160位数
		memcpy(onebit, digest, 8);
		perdig = stoul(onebit, NULL, 16);

		SETONEBIT_B(temp, perdig);
		shiftright = getbits_b(temp) - remain;
		//cout << "shiftright = " << shiftright << endl;
		if (shiftright >= 0)//等于不用移动，还要左移
			perdig = perdig >> shiftright;
		else if (shiftright < 0)
			perdig = perdig << abs(shiftright);
	}
	a[i + 1] = perdig;
	cpy_b(result, a);
	RMLDZRS_B(result);
	return FLAG_OK;
}

//把随机数写到addr中
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

inline uint32_t cirleft(uint32_t word, int bit)//word循环左移bit位
{
	return(word << bit) | (word >> (32 - bit));
}

int mysha1(char *inputfileaddr, char *output)
{
	//char inputfileaddr[81] = "input.file";//输入
	//char outputfile[81] = "digest.file";//输出
	uint32_t W[80];//用空间换时间，W块有80个
	memset(W, 0, sizeof(W));//原始的为0就好啦
	uint32_t A, B, C, D, E;
	long long msglen = msgsize(inputfileaddr) * 8;//获得原文位数，二进制位单位，不是字节！
	FILE *fp = NULL;
	if ((fp = fopen(inputfileaddr, "rb")) == NULL)
	{
		printf("open %s faied!\n", inputfileaddr);
		system("PAUSE");
		return 1;
	}
	long long counter = 1, times = 0;
	int flag = 0;
	int bytes;//最后一次到底读了多少字节
	if (msglen % 512 > 440)//大于440的意思就是到了448那就用了W[13]，W[14]和W[15]是长度，那就放不下0x80了
	{
		times = (msglen + 512 - 1) / 512;//向上取整
		flag = 1;
	}
	else if (msglen % 512 == 0)
	{
		times = (msglen + 512 - 1) / 512 + 1;//多做一次,因为造了一个块，后面加了一个块
		flag = 2;
	}
	else times = (msglen + 512 - 1) / 512;//至少要做那么多次，可能会多一次

	A = H0; B = H1; C = H2; D = H3; E = H4;
	while (counter < times)//一般情况少做最后一次，特殊情况少做最后两次
	{
		fread(W, sizeof(char), 64, fp);//读出一个消息块512bits,读到W里面
		for (int i = 0; i < 16; i++)//把顺序转过来,得到想要的存储顺序
		{
			W[i] = (W[i] >> 24) + (W[i] >> 8 & 0xff00) + (W[i] << 8 & 0xff0000) + (W[i] << 24);
		}
		for (int i = 0; i < 20; i++)
		{
			if (i >= 16)//W[i]生成以后还要左移1位
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
		A = H0 = H0 + A;//加了以后继续CVi
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

		bytes = fread(W, sizeof(char), 64, fp);//读出一个消息块512bits,读到W里面	
		p = (unsigned char*)&W[0];
		p = p + bytes;
		*p = 0x80;
		p = (unsigned char*)&msglen;
		if (flag == 0)
		{
			memcpy(&W[14], p + 4, 4);//复制长度
			memcpy(&W[15], p, 4);//复制长度，要变一下存储顺序
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
		memcpy(&W[14], p + 4, 4);//复制长度
		memcpy(&W[15], p, 4);//复制长度，要变一下存储顺序
		W[14] = (W[14] >> 24) + (W[14] >> 8 & 0xff00) + (W[14] << 8 & 0xff0000) + (W[14] << 24);
		W[15] = (W[15] >> 24) + (W[15] >> 8 & 0xff00) + (W[15] << 8 & 0xff0000) + (W[15] << 24);
	}
	for (int i = 0; i < 16; i++)//顺序永久转过来
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
		memcpy(&W[14], p + 4, 4);//复制长度
		memcpy(&W[15], p, 4);//复制长度
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
	//char inputfileaddr[81] = "input.file";//输入
	//char outputfile[81] = "digest.file";//输出
	uint32_t W[80];//用空间换时间，W块有80个
	memset(W, 0, sizeof(W));//原始的为0就好啦
	uint32_t A, B, C, D, E;
	long long msglen = msgsize(inputfileaddr) * 8;//获得原文位数，二进制位单位，不是字节！
	FILE *fp = NULL;
	if ((fp = fopen(inputfileaddr, "rb")) == NULL)
	{
		printf("open %s faied!\n", inputfileaddr);
		system("PAUSE");
		return 1;
	}
	long long counter = 1, times = 0;
	int flag = 0;
	int bytes;//最后一次到底读了多少字节
	if (msglen % 512 > 440)//大于440的意思就是到了448那就用了W[13]，W[14]和W[15]是长度，那就放不下0x80了
	{
		times = (msglen + 512 - 1) / 512;//向上取整
		flag = 1;
	}
	else if (msglen % 512 == 0)
	{
		times = (msglen + 512 - 1) / 512 + 1;//多做一次,因为造了一个块，后面加了一个块
		flag = 2;
	}
	else times = (msglen + 512 - 1) / 512;//至少要做那么多次，可能会多一次

	A = H0; B = H1; C = H2; D = H3; E = H4;
	while (counter < times)//一般情况少做最后一次，特殊情况少做最后两次
	{
		fread(W, sizeof(char), 64, fp);//读出一个消息块512bits,读到W里面
		for (int i = 0; i < 16; i++)//把顺序转过来,得到想要的存储顺序
		{
			W[i] = (W[i] >> 24) + (W[i] >> 8 & 0xff00) + (W[i] << 8 & 0xff0000) + (W[i] << 24);
		}
		for (int i = 0; i < 20; i++)
		{
			if (i >= 16)//W[i]生成以后还要左移1位
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
		A = H0 = H0 + A;//加了以后继续CVi
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

		bytes = fread(W, sizeof(char), 64, fp);//读出一个消息块512bits,读到W里面	
		p = (unsigned char*)&W[0];
		p = p + bytes;
		*p = 0x80;
		p = (unsigned char*)&msglen;
		if (flag == 0)
		{
			memcpy(&W[14], p + 4, 4);//复制长度
			memcpy(&W[15], p, 4);//复制长度，要变一下存储顺序
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
		memcpy(&W[14], p + 4, 4);//复制长度
		memcpy(&W[15], p, 4);//复制长度，要变一下存储顺序
		W[14] = (W[14] >> 24) + (W[14] >> 8 & 0xff00) + (W[14] << 8 & 0xff0000) + (W[14] << 24);
		W[15] = (W[15] >> 24) + (W[15] >> 8 & 0xff00) + (W[15] << 8 & 0xff0000) + (W[15] << 24);
	}
	for (int i = 0; i < 16; i++)//顺序永久转过来
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
		memcpy(&W[14], p + 4, 4);//复制长度
		memcpy(&W[15], p, 4);//复制长度
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

int findprime(BN a, int bits)//寻找一个bits位的素数
{
	unsigned int prime[50] =
	{
		2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,113,
		127,131,137,139,149,151,157,163,167,173,179,181,191,193,197,199,211,223,227,229
	};
	BN temp1 = { 1,1 }, temp2, temp3;
	int judge[30] = { 0 };
	SETONEBIT_B(temp3, prime[19]);//最大考虑范围
	
	BN bignum, fac,gcd,linshi,adjnum,linshi1;
	SETONEBIT_B(linshi, 2U);
	int fermatresult=0;
	readbn(fac, "20primefac.txt");//前30个素数的乘积
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
		if (cmp_b(gcd, ONE_BN) == 0)//如果筛选为素数,费马检测
		{
			//cout << "bignum= " << bn2str(bignum) << endl;
			//cout << "没有调整，直接费马" << endl;
			fermatresult = fermat_b(bignum);
			times++;
			//cout << "费马结果为  " << fermatresult << endl<<endl;//为1就正确了
			cpy_b(a, bignum);
			SETONEBIT_B(linshi, 2U);
		}
		else if (cmp_b(gcd, temp3) > 0||cmp_b(gcd,TWO_BN)==0)//重新来一个
		{		
		}
		else if(cmp_b(gcd, TWO_BN) != 0)//调整,前提不是偶数才调整
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
					//cout << " 已经调整了的大数是  " << bn2str(adjnum) << endl;//为1就正确了
					//cout << "调整以后费马结果为  " << fermatresult << endl;//为1就正确了
					cpy_b(a, adjnum);
					//SETONEBIT_B(linshi, 2U);
					break;//重新来，或者正确
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
	cout << "费马检测做了 " << times << " 次" << endl;
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
	BN plain;//明文
	BN cry;//加密以后
	BN dec;//解密以后

	BN temp1 = { 0 }, temp2 = { 0 }, temp3 = { 0 };//用来测试

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
	
	//产生p和q，一般情况下是有了的不用自己产生
	char p_path[81] = "myp4.txt";
	char q_path[81] = "myq4.txt";
	char e_path[81] = "e.txt";
	char plain_path[81] = "plain.txt";
	char cipher_path[81] = "cipher.txt";
	char decry_path[81] = "decry.txt";


	int ifgen = 0;//是否产生私钥
	int ifset = 0;
	cout << "是否要产生私钥p q? 输入1产生，输入0不产生\n";
	cin >> ifgen;
	cout << "是否要改变默认文件路径? 输入1改变，输入0不改变\n";
	cout << "p 存放在" << p_path << endl;
	cout << "q 存放在" << q_path << endl;
	cout << "公钥e存放在" << e_path << endl;
	cout << "明文存放在" << plain_path << endl;
	cout << "密文存放在" <<cipher_path << endl;
	cout << "解密文件存放在" << decry_path << endl;
	cin >> ifset;

	if (ifset == 1)
	{
		memset(p_path, 0, sizeof(p_path));
		memset(q_path, 0, sizeof(q_path));
		memset(e_path, 0, sizeof(e_path));
		memset(plain_path, 0, sizeof(plain_path));
		memset(cipher_path, 0, sizeof(cipher_path));
		memset(decry_path, 0, sizeof(decry_path));
		cout << "请输入p存放路径"<<endl;
		cin >> p_path;
		cout << "请输入q存放路径" << endl;
		cin >> q_path;
		cout << "请输入e存放路径" << endl;
		cin >> e_path;
		cout << "请输入明文存放路径" << endl;
		cin >> plain_path;
		cout << "请输入密文存放路径" << endl;
		cin >> cipher_path;
		cout << "请输入解密后存放路径" << endl;
		cin >> decry_path;


		cout << "设置以后: " << endl<<endl;

		cout << "p 存放在" << p_path << endl;
		cout << "q 存放在" << q_path << endl;
		cout << "e 存放在" << e_path << endl;
		cout << "明文存放在" << plain_path << endl;
		cout << "密文存放在" << cipher_path << endl;
		cout << "解密文件存放在" << decry_path << endl;

	}

	if (ifgen == 1)//为1则产生
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
		cout << "初始e is  " << bn2str(e) << endl;
	}


	//获取p/q
	

	//计算n
	mul(p, q, n);
	cout << "n is  " << bn2str(n) << endl;
	writebn("n.txt", eula);

	subuint_b(p, 1U, p_1);
	cout << "p-1 is  " << bn2str(p_1) << endl;

	subuint_b(q, 1U, q_1);
	cout << "q-1 is  " << bn2str(q_1) << endl;

	mul(p_1, q_1, eula);
	cout << "φ(n) is  " << bn2str(eula) << endl;
	writebn("fn.txt", eula);
	
	//一般公钥e =65537
	//SETONEBIT_B(e, 10001U);

	gcd_b(e, eula, temp3);
	if (cmp_b(temp3, ONE_BN) != 0) {
		while (cmp_b(temp3, ONE_BN) != 0)//如果不互素！！！悲剧了
		{
			findprime(e, 17);
			gcd_b(e, eula, temp3);
		}
		cout << "之前公钥e与φ(n)不互素，修改以后的e is  " << bn2str(e) << endl;
		writebn("e.txt", e);
	}
	


	//私钥d  ed =1 mod φ(n)
	inv_b(e,eula, d);
	cout << "d is  " << bn2str(d) << endl;
	writebn("d.txt", d);
	//获取明文
	readbn(plain, plain_path);
	cout << "明文是 is  " << bn2str(plain) << endl;
	//加密
	int time1 = clock();
	modexp_b(plain, e,n, cry);
	printf("加密耗时%d ms\n", clock() - time1);
	cout << "密文是 is  " << bn2str(cry) << endl;
	
	writebn(cipher_path, cry);

	//解密
	int time2 = clock();
	//modexp_b(cry, d, n, dec);
	crt_b(cry, d, p, q, dec);
	printf("\n解密密耗时%d ms\n", clock() - time2);
	cout << "解密为 is  " << bn2str(dec) << endl;
	writebn(decry_path, dec);



	readbn(temp1, plain_path);
	readbn(temp2, decry_path);

	cout << endl << "明文为" << bn2str(temp1);
	cout  << endl<<"解密为" << bn2str(temp2)<<endl;

	if (checkresult(plain_path, decry_path) == 1)
		cout << "\n加解密正确！" << endl;
	else
		cout << "\n加解密出现错误！" << endl;
	system("pause");
	return 0;
}


