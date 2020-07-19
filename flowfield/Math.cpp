#define TEMPLATE template <typename T>

TEMPLATE
struct vec2
{
	union
	{
		struct
		{
			T x;
			T y;
		};
		struct
		{
			T u;
			T v;
		};
		struct
		{
			T s;
			T t;
		};
		struct
		{
			T cos;
			T sin;
		};
	};
};

TEMPLATE
static vec2<T>
V2(const vec2<T>& vec) {
	vec2<T> temp;
	temp.x = vec.x;
	temp.y = vec.y;
	return temp;
}

TEMPLATE
static vec2<T>
V2(T x, T y) {
	vec2<T> temp;
	temp.x = x;
	temp.y = y;
	return temp;
}



float
InvSqRoot(float value)
{
	union
	{
		float f;
		unsigned int  i;
	}
	result = { value };
	result.i = 0x5F3759DF - (result.i >> 1);

	float halfValue = value * 0.5f;

	result.f = (result.f * (1.5f - (halfValue * result.f * result.f)));
	result.f = (result.f * (1.5f - (halfValue * result.f * result.f)));
	result.f = (result.f * (1.5f - (halfValue * result.f * result.f)));
	result.f = (result.f * (1.5f - (halfValue * result.f * result.f)));

	return result.f;
}

float SqRoot(float value)
{
	float result = 1.0f / InvSqRoot(value);

	return result;
}

TEMPLATE
vec2<T> operator + (vec2<T> left, vec2<T> right)
{
	return V2(left.x + right.x, left.y + right.y);
}


TEMPLATE
vec2<T> operator - (vec2<T> left, vec2<T> right)
{
	return V2(left.x - right.x, left.y - right.y);
}

TEMPLATE
float Dot(vec2<T> left, vec2<T> right)
{
	float dot = (left.x * right.x) + (left.y * right.y);

	return dot;
}

TEMPLATE
float LenSq(vec2<T> vec)
{
	float lenSq = Dot(vec, vec);

	return lenSq;
}

TEMPLATE
float Len(vec2<T> vec)
{
	float len = SqRoot(LenSq(vec));

	return len;
}

TEMPLATE
vec2<T> operator * (float constant, vec2<T> vec)
{
	return V2(constant * vec.x, constant * vec.y);
}

TEMPLATE
vec2<T> operator * (vec2<T> vec, float constant)
{
	return V2(constant * vec.x, constant * vec.y);
}

TEMPLATE
vec2<T> operator * (vec2<T> left, vec2<T> right) 
{
	return V2(left.x * right.x, left.y * right.y);
}

template <typename T>
vec2<T>
operator - (vec2<T> vec)
{
	vec2<T> temp = V2(-vec.x, -vec.y);

	return temp;
}

TEMPLATE
vec2<T>
operator / ( vec2<T> vec, float constant)
{
	vec2<T> vecDiv = vec * (1.0f / constant);

	return vecDiv;
}

TEMPLATE
vec2<T>
operator += (vec2<T>& left, vec2<T>  right)
{
	left.x += right.x;
	left.y += right.y;

	return left;
}

TEMPLATE
vec2<T>
operator -= (vec2<T>& left, vec2<T>  right)
{
	left.x -= right.x;
	left.y -= right.y;

	return left;
}

TEMPLATE
bool
operator == (vec2<T> left, vec2<T> right)
{
	return (left.x == right.x && left.y == right.y);
}

template <typename T>
vec2<T>
Normalize(vec2<T> vec)
{
	vec2<T> norm = vec * (1.0f / Len(vec));

	return norm;
}


template <typename T>
float
Cross(vec2<T> left, vec2<T> right)
{
	float vec = (left.x * right.y) - (left.y * right.x);

	return vec;
}


TEMPLATE
vec2<T>
Reflect(vec2<T> vec, vec2<T> normal)
{
	vec2<T> reflection;

	reflection = vec - 2.0f * Dot(vec, normal) * normal;

	return reflection;
}

/*******************VEC3*********************/
/*******************VEC3*********************/
/*******************VEC3*********************/
/*******************VEC3*********************/
/*******************VEC3*********************/
template <typename T>
struct vec3
{
	union
	{
		struct
		{
			T x;
			T y;
			T z;
		};
		struct
		{
			T width;
			T height;
			T depth;
		};
		struct
		{
			T r;
			T g;
			T b;
		};
	};
};

template <typename T>
vec3<T>
V3(T x, T y, T z)
{
	vec3<T> temp;
	temp.x = x;
	temp.y = y;
	temp.z = z;
	return temp;
}



/*******************VEC4*********************/
/*******************VEC4*********************/
/*******************VEC4*********************/
/*******************VEC4*********************/
/*******************VEC4*********************/

template <typename T>
struct vec4
{
	union
	{
		struct
		{
			T x;
			T y;
			T z;
			T w;
		};
		struct
		{
			T r;
			T g;
			T b;
			T a;
		};
	};
};

template <typename T>
vec4<T>
V4(T x, T y, T z, T a)
{
	vec4<T> temp;
	temp.x = x;
	temp.y = y;
	temp.z = z;
	temp.a = a;
	return temp;
}





/*******************MAT4*********************/
/*******************MAT4*********************/
/*******************MAT4*********************/
/*******************MAT4*********************/
/*******************MAT4*********************/



union mat4x4
{
	float arr[16];

	struct 
	{
		vec4<float> row1;
		vec4<float> row2;
		vec4<float> row3;
		vec4<float> row4;
	};

	struct
	{
		float xx;
		float xy;
		float xz;
		float xw;

		float yx;
		float yy;
		float yz;
		float yw;

		float zx;
		float zy;
		float zz;
		float zw;

		float wx;
		float wy;
		float wz;
		float ww;
	};
};

static mat4x4 
setOrthoFrustum(float l, float r, float b, float t, float n, float f)
{
	mat4x4 mat = {};
	mat.arr[0] = 2 / (r - l);
	mat.arr[5] = 2 / (t - b);
	mat.arr[10] = -2 / (f - n);
	mat.arr[12] = -(r + l) / (r - l);
	mat.arr[13] = -(t + b) / (t - b);
	mat.arr[14] = -(f + n) / (f - n);
	mat.arr[15] = 1;
	return mat;
}








const float Pi_ = 3.14159265359f;

float 
RadiansToDegrees(float radians)
{
	return radians * (180.0 / Pi_);
}

float
DegreesToRadians(float degrees)
{
	float result = (Pi_ * degrees) / 180.0f;

	return result;
}

float
CosRadians(float radians)
{
	float radians2 = radians * radians;
	float radians4 = radians2 * radians2;
	float radians6 = radians4 * radians2;
	float radians8 = radians6 * radians2;
	float radians10 = radians8 * radians2;
	float radians12 = radians10 * radians2;
	float radians14 = radians12 * radians2;

	float invFac2 = 1.0f / 2.0f;
	float invFac4 = 1.0f / 24.0f;
	float invFac6 = 1.0f / 720.0f;
	float invFac8 = 1.0f / 40320.0f;
	float invFac10 = 1.0f / 3628800.0f;
	float invFac12 = 1.0f / 479001600.0f;
	float invFac14 = 1.0f / 87178291200.0f;


	float result = 1 - (radians2 * invFac2) + (radians4 * invFac4) - (radians6 * invFac6) + (radians8 * invFac8) - (radians10 * invFac10) + (radians12 * invFac12) - (radians14 * invFac14);

	return result;
}

float
SinRadians(float radians)
{
	float radians2 = radians * radians;
	float radians3 = radians * radians2;
	float radians5 = radians3 * radians2;
	float radians7 = radians5 * radians2;
	float radians9 = radians7 * radians2;
	float radians11 = radians9 * radians2;
	float radians13 = radians11 * radians2;
	float radians15 = radians13 * radians2;

	float invFac3 = 1.0f / 6.0f;
	float invFac5 = 1.0f / 120.0f;
	float invFac7 = 1.0f / 5040.0f;
	float invFac9 = 1.0f / 362880.0f;
	float invFac11 = 1.0f / 39916800.0f;
	float invFac13 = 1.0f / 6227020800.0f;
	float invFac15 = 1.0f / 130767436800.0f;


	float result = radians - (radians3 * invFac3) + (radians5 * invFac5) - (radians7 * invFac7) + (radians9 * invFac9) - (radians11 * invFac11) + (radians13 * invFac13) - (radians15 * invFac15);

	return result;
}

float
TanRadians(float radians)
{
	float radians2 = radians * radians;
	float radians3 = radians * radians2;
	float radians5 = radians3 * radians2;
	float radians7 = radians5 * radians2;
	float radians9 = radians7 * radians2;
	float radians11 = radians9 * radians2;
	float radians13 = radians11 * radians2;
	float radians15 = radians13 * radians2;

	float invFac3 = 1.0f / 3.0f;
	float invFac5 = 2.0f / 15.0f;
	float invFac7 = 17.0f / 315.0f;
	float invFac9 = 62.0f / 2835.0f;
	float invFac11 = 1382.0f / 155925.0f;
	float invFac13 = 43688.0f / 2027025.0f;
	float invFac15 = 155347.0f / 106706000.0f;


	float result = radians + (radians3 * invFac3) + (radians5 * invFac5) + (radians7 * invFac7) + (radians9 * invFac9) + (radians11 * invFac11) + (radians13 * invFac13) + (radians15 * invFac15);

	return result;
}

float
CosDegrees(float degrees)
{
	float result = CosRadians(DegreesToRadians(degrees));

	return result;
}

float
SinDegrees(float degrees)
{
	float result = SinRadians(DegreesToRadians(degrees));

	return result;
}

float
TanDegrees(float degrees)
{
	float result = TanRadians(DegreesToRadians(degrees));

	return result;
}

vec2<float> RotateVec(vec2<float> original, float degrees)
{
	//float radians = degrees * (180.0 / 3.141592653589793238463);
	float radians = degrees * (3.141592653589793238463 / 180.0);
	/*vec2<float> result;
	result.x = original.x * cos(radians) - original.y * sin(radians);
	result.y = original.x * sin(radians) + original.y * cos(radians);
	return result;*/

	float sinT = sinf(radians); float cosT = cosf(radians);
	return V2(original.x*cosT - original.y*sinT,
		original.x*sinT + original.y*cosT);
}

//FLOATS


float
Abs(float value)
{
	union
	{
		float result;
		unsigned int i;
	}
	data = { value };
	data.i &= 0x7FFFFFFF;

	return data.result;
}


float
Square(float value)
{
	float square = value * value;

	return square;
}


float
SQRT(float num)
{
	float guess, e, upperbound;
	guess = 1;
	e = 0.001f;
	do
	{
		upperbound = num / guess;
		guess = (upperbound + guess) / 2;
	} while (!(guess * guess >= num - e &&
		guess * guess <= num + e));
	return guess;
}