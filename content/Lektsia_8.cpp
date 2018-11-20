������ 8.

// ====================================================================

1) ����������� ������ ������� (��. ������ 7)

-- public|private, class|struct
-- ������������/�����������
-- explicit ������������
-- ����������� (������� ����)
-- initializer_list, {}
-- ��������� this
-- static ����� ������

// ====================================================================

2) ������ �������������� �������

-- �������� ������������ (��.������ 7)
-- ����������� �������-�����
---- ����������� �� ��������� (��.������ 7)
---- ���������� (��.������ 7)
---- ���������� ��������
---- ������������ ��������
-- �������-����� / ������-����� (��.������ 7)
-- ��������� 

// ====================================================================

3) ���������

x+y*z

class complex
{
	complex operator+(complex);
};

1) ��������� ����� ������ a+b
2) �������������� ����� ������ a.operator+(b)

->[]() -- ���� ����� ����������� ��� ������ ������
:: . .* -- ������ �����������
, & -- �� ������������� �����������

// ===================================================================

class X
{
	void operator+(int);
	X(int); // non explicit
};

void operator+(X, X);
void operator+(X, double);

X a; a + 1; 1.0f + a; 1 + 1; a + 1.0;
������ 1 ���������������� ��������������
���������� ����� ����� ��������� ���������� ��������������

// ===================================================================

��� �� ������� ���������
+ -- ��������� �������� + ������
void operator+(X, X, X);
% -- �� ����� ���� ������� ����������
:? -- ������ �����������

// ===================================================================

+ += ++

++ -- +=1
+ -- +=

class X
{
	X & operator+=(X) // op=
	{
		return *this;
	}
};

void operator+(X, X)
{
	// +=
	// x+=y
}

1 + x
x + 1
1 += x; // �����������
x += 1;
(x += 1).f();

x++;
++x;

// ===================================================================

class X
{
	X(int);
	operator int() { return ...; }
};

void operator+(X, X)

X x;
int result = 1 + x;

// ===================================================================

lvalue | rvalue � references

lvalue -- ������, ������� �������� ���������������� ����� � ������
rvalue -- ��� ���������

int x = 10;
int x = y;

1) ������������������(i)
2) ��������������(m)

lvalue(i & !m) --������������� gvalue
rvalue(m)
gvalue(i) // generalized
xvalue(i & m) // extended | expert only -- ������������� gvalue|rvalue
prvalue(!i & m) // pure -- ������ -- ������������� rvalue

// ===================================================================

references

int x = 10;
int & ref = x;

string && rvr { "hello" }; // ��� ��������� �����������
const string & lvr = "hello"; // const !

������������: destructive read;

int x = 10;
int&& rvr = std::move(x); // move ��� x ������ ��� int&&

using rr_i = int&&;
using lr_i = int&;
using rr_rr_i = rr_i && ; // int &&
using lr_rr_i = rr_i & ; // int &
using rr_lr_i = lr_i && ; // int &
using lr_lr_i = lr_i & ; // int &

1) ����������
2) auto && x = ... // int & x = ...
3) �������

// ===================================================================

10;

void f(const int & x) // ��������
{

}
f(10); // ������ ������
int x = 10;
f(x);

// ���� �����������
int && rvr = 5;

int & ref = x;

// ===================================================================

copy VS move

int x = 0, y = 10;
x = y; // copy

{
	X x;
	// ...
	f(x); // ����� �����������
	// ... -- �� ������������ x
}

class X
{
	X(const X& other);
	X& operator=(const X& other);

	X(X&& other);
	X& operator=(X&& other);
};

// X && 
// const X && -- �� �������� �� ������������

�������� �����������:
1) ���������������
2) �������������

���������� ����� ������������ ����. �-�����

class X
{
	int * ptr;
	int length;
	X()
	{
		ptr = new int[length];
	}
	// X& operator=(const X &);
	X& operator=(X&& other)
	{
		this->ptr = other.ptr;
		this->length = other.length;
		other.ptr = nullptr;
		other.length = 0;
	}
};

X x1, x2;
// ... x2
x1 = std::move(x2);
x1.operator=(std::move(x2));

X x1, x2;
x1 = x2; // shallow copy -- ������������� �����������
(*x2.ptr) = ...

����� class ����� ��������� (��� ��� ���-�� ���������)
����� ������ �������� ��������������

���������������� �������� ��������� deep copy

// ===================================================================

void swap(X & a, X & b)
{
	const X tmp = a; // ����� ����� ������ �� �����������
	a = b;
	b = tmp;
} // ��������� exchange

void swap(X & a, X & b)
{
	X tmp = std::move(a); // static_cast < X&& > (a)
	a = std::move(b);
	b = std::move(tmp);
}

// ===================================================================

Matrix operator+(...)
{
	Matrix result;
	// ...
	return result; // ����������� !!!
}

Matrix & operator+(...)
{
	Matrix & result = *new Matrix(...);
	// ...
	return result;
}

// delete 