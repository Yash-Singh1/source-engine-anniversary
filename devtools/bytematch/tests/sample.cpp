// Self-test input: external and static calls, PIC/absolute data, a jump
// table, vtables, floating-point constants and an unexported function.
// No headers, so every toolchain in the self-test can build it.
#if defined(_WIN32)
#define EXPORT extern "C" __declspec(dllexport)
#else
#define EXPORT extern "C" __attribute__((visibility("default")))
#endif

static int g_counter;
int g_table[16] = { 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987, 1597 };
const char *g_names[] = { "zero", "one", "two" };

struct Base
{
	virtual ~Base() {}
	virtual int Get( int x ) const;
	int m;
};

struct Derived : Base
{
	virtual int Get( int x ) const;
};

int Base::Get( int x ) const { return m + x; }
int Derived::Get( int x ) const { return m * x + g_table[x & 15]; }

static Derived s_derived;

static __attribute__((noinline)) int helper( int a )
{
	return a * 7 + g_counter++;
}

__attribute__((noinline)) int not_exported( int a )
{
	return helper( a ) ^ 0x5a5a;
}

EXPORT int sample_switch( int v )
{
	switch ( v )
	{
	case 0: return helper( 1 );
	case 1: return 11;
	case 2: return g_table[3];
	case 3: return 37;
	case 4: return helper( v );
	case 5: return 99;
	case 6: return -4;
	case 7: return g_counter;
	default: return 0;
	}
}

EXPORT const char *sample_name( int i )
{
	return i >= 0 && i < 3 ? g_names[i] : "none";
}

EXPORT int sample_virtual( Base *b, int x )
{
	return b->Get( x ) + helper( x );
}

EXPORT Base *sample_make( int k )
{
	s_derived.m = k;
	return &s_derived;
}

EXPORT double sample_float( double a )
{
	return a * 1.5 + 0.25;
}

EXPORT int sample_loop( const int *p, int n )
{
	int s = 0;
	for ( int i = 0; i < n; ++i )
		s += p[i] * g_table[i & 15];
	return s + not_exported( n );
}
