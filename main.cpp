#include <iostream>
#include <cstring>
#include <string>
#include <map>
#include "BigInteger.h"
typedef unsigned char byte;

const short BASE_NUMBER_SIZE = 100;
const short BASE = 256;
const int STRING_MAX = 10000;
short compare(byte*a1, byte*b1)
{
	for (int i = 99; i >= 2; --i)
	{
		if (a1[i] > b1[i])
			return 1;
		else
			if (a1[i] < b1[i])
				return -1;
	}
	return 0;
}
void  subtract(byte *a1, byte *b1, byte*c)
{
	byte*a = nullptr;
	byte*b = nullptr;
	size_t size_mass = 0;

	if (a1[0] >= b1[0])
	{
		size_mass = a1[0];
		a = a1;
		b = b1;
	}
	else
	{
		size_mass = b1[0];
		a = b1;
		b = a1;
	}
	for (int i = 0; i <= int(a[0] + 1); i++)
	{
		c[i] = a[i];
	}
	a = c;

	__asm {
		// адреса a и b отправляем в esi и edi
		lea esi, a;
		lea edi, b;
		// сейчас в esi лежит адрес адреса массива a
		// так как a и b - указатели, а в esi и edi были указатели на указатель, то достаем ещё раз
		mov esi, [esi];
		mov edi, [edi];
		mov bh, 0;

		// длину массива закидываем в ecx
		mov ecx, size_mass;
		add esi, 2;
		add edi, 2;

	L:
		// достаем очередное число из *a и перемещаем его в *b
		mov al, [esi];
		mov bl, [edi];
		mov bh, 0;
		mov ah, 0;

		cmp al, bl
			jb L1;    // если a1<b1

		jmp L2;
	L1:
		mov ah, 256;     // al=al+(256-bl)
		mov bh, 1;       // вычтем в конце из след разряда
	L2:
		sub ah, bl;
		add al, ah;
		mov[esi], al;

		add esi, 1;
		add edi, 1;

		mov edx, ecx; 	 //будем хранить значение регистра 

		cmp[esi], bh;
		jae L3;		 // [esi]>=dl

	Loo:			 // проходим по циклу уменьшая старшые биты пока разряд меньше 1 
		sub[esi], 1;
		add esi, 1;
		cmp[esi], 1;
		jae L5;		 // [esi]>=dl выход из цикла 

		loop Loo;

	L5:
		sub[esi], 1;
		mov ebx, ecx;
		sub ebx, edx;
		mov ecx, edx;
		sub esi, ebx;
		jmp L4;
	L3:
		sub[esi], bh;
	L4:
		loop L;
	}
}
void  sum(byte *a1, byte *b1, byte*c)
{
	byte*a = nullptr;
	byte*b = nullptr;
	size_t size_mass = 0;
	if (a1[1] == b1[1])
	{
		if (a1[0] >= b1[0])
		{
			size_mass = a1[0];
			a = a1;
			b = b1;
		}
		else
		{
			size_mass = b1[0];
			a = b1;
			b = a1;
		}


		for (int i = 0; i <= int(a[0] + 1); i++)
		{
			c[i] = a[i];
		}
		a = c;
		__asm {
			
			lea esi, a;
			lea edi, b;
		
			mov esi, [esi];
			mov edi, [edi];
			mov dl, 0;
		
			mov ecx, size_mass;
			add esi, 2;
			add edi, 2;

		L:
	
			mov al, [esi];
			mov bl, [edi];

			mov ah, al;       //сохраним al в ah

			add al, bl;
			


			cmp ah, al;
			ja L1;

			mov dl, 0;
			mov[esi], al;

			jmp L2;

		L1:
			mov dl, 1;
			mov[esi], al;

		L2:
			add esi, 1;
			add edi, 1;

			add[esi], dl;

			cmp ecx, 1
				je L3;
			jmp L4;
		L3:
			sub esi, size_mass;
			sub esi, 2;

			add[esi], dl;
		L4:
			loop L;
		}
	}
	else // разные знаки 
	{
		if (compare(a1, b1) == 1) //  a1>b1
		{
			subtract(a1, b1, c);
			c[1] = a1[1];
		}
		else if (compare(a1, b1) == -1) // a1<b1
		{
			subtract(b1, a1, c);
			c[1] = b1[1];
		}
		else if (compare(a1, b1) == 0)
		{
			return;
		}
	}

}
void  sub(byte *a1, byte *b1, byte *c)
{
	b1[1] = (!b1[1]);
	sum(a1, b1, c);
	b1[1] = (!b1[1]);
}

string convertToNumber(byte* base256Number)
{
	BigInteger number(0);
	BigInteger multiplier(1);

	for (int i = 1; i < base256Number[0]; i++) {
		multiplier = 1;
		for (int j = 1; j < i ; j++) {
			multiplier *= 256;
		}

		number += multiplier * base256Number[i + 1];
	}
	if (base256Number[1] == 1)number.setSign(1);
	return (string)number;
}

pair<string, short> divideOn256(const string number) {
	long long rem = 0;
	string result; 
	result.resize(STRING_MAX);

	for (int indx = 0, len = number.length(); indx < len; ++indx)
	{
		rem = (rem * 10) + (number[indx] - '0');
		result[indx] = rem / BASE + '0';
		rem %= BASE;
	}
	result.resize(number.length());

	while (result[0] == '0' && result.length() != 1)
		result.erase(0, 1);

	if (result.length() == 0)
		result = "0";

	return make_pair(result, rem);
}

byte* convertToBase256(string number)
{
	byte *base256Number = new byte[BASE_NUMBER_SIZE];
	std::fill_n(base256Number, BASE_NUMBER_SIZE, 0);

	short i = 2;
	bool sign=0;
	if (number[0] == '-')
	{
		number.erase(0, 1);
		sign = 1;
	}
	while (true) 
	{
		
		if (number.length() <= 3 && std::stoi(number) < 256)
			break;

		auto qutRemPair = divideOn256(number);
		number = qutRemPair.first;

		base256Number[i++] = qutRemPair.second;
	}

	base256Number[i] = std::stoi(number);

	base256Number[0] = i;
	base256Number[1] = sign;

	return base256Number;
}
void show_arr(byte*arr)
{
	for (int i = 0; i <= int(arr[0]); i++)
		cout << int(arr[i]) << ' ';
	cout << endl;
}
int main()
{
	auto base256number1 = convertToBase256("-255");
	cout << "byte array Num1: " << endl;
	show_arr(base256number1);

	auto base256number2 = convertToBase256("65987");
	cout << "byte array Num2: " << endl;
	show_arr(base256number2);
	cout << "\n\n";

	byte *result = new byte[BASE_NUMBER_SIZE];
	fill_n(result, BASE_NUMBER_SIZE, 0);

	sum(base256number1, base256number2, result);
	
	cout << "Num1+Num2= "<<convertToNumber(result) << endl;
	cout << "byte array Nm1+Num2: " << endl;
	show_arr(result);
	cout << "\n\n";

	sub(base256number1, base256number2, result);
	cout << "Num1-Num2= "<<convertToNumber(result) << endl;
	cout << "byte array Nm1-Num2: " << endl;
	show_arr(result);
	
	system("pause");
	return 0;
}




