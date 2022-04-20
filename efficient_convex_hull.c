#include <stdlib.h> // atoi, rand, qsort, malloc
#include <stdio.h>
#include <assert.h> // assert
#include <time.h> //time

#define RANGE 10000

typedef struct
{
	int x;
	int y;
} t_point;

typedef struct
{
	t_point from;
	t_point to;
} t_line;

////////////////////////////////////////////////////////////////////////////////
// function declaration
// 점들의 집합(points; 점의 수 num_point)에서 점 p1과 점 pn을 잇는 직선의 upper hull을 구하는 함수 (재귀호출)
// [output] lines: convex hull을 이루는 선들의 집합
// [output] num_line: 선의 수
// [output] capacity: lines에 할당된 메모리의 용량 (할당 가능한 선의 수)
// return value: 선들의 집합(lines)에 대한 포인터
t_line *upper_hull( t_point *points, int num_point, t_point p1, t_point pn, t_line *lines, int *num_line, int *capacity);

// 직선(ax+by-c=0)과 주어진 점 p(x1, y1) 간의 거리
// distance = |ax1+by1-c| / sqrt(a^2 + b^2)
// 실제로는 sqrt는 계산하지 않음
// return value: 직선과 점 사이의 거리 (분모 제외)
float distance( float a, float b, float c, t_point p);

// 두 점(from, to)을 연결하는 직선(ax + by - c = 0)으로 n개의 점들의 집합 s(점의 수 num_point)를 s1(점의 수 n1)과 s2(점의 수 n2)로 분리하는 함수
// [output] s1 : 직선의 upper(left)에 속한 점들의 집합 (ax+by-c < 0)
// [output] s2 : lower(right)에 속한 점들의 집합 (ax+by-c > 0)
// [output] n1 : s1 집합에 속한 점의 수
// [output] n2 : s2 집합에 속한 점의 수
void separate_points( t_point *points, int num_point, t_point from, t_point to, t_point *s1, t_point *s2, int *n1, int *n2);

////////////////////////////////////////////////////////////////////////////////
void print_header(char *filename)
{
	printf( "#! /usr/bin/env Rscript\n");
	printf( "png(\"%s\", width=700, height=700)\n", filename);
	
	printf( "plot(1:%d, 1:%d, type=\"n\")\n", RANGE, RANGE);
}

////////////////////////////////////////////////////////////////////////////////
void print_footer(void)
{
	printf( "dev.off()\n");
}

////////////////////////////////////////////////////////////////////////////////
// qsort를 위한 비교 함수
int cmp_x( const void *p1, const void *p2)
{
	t_point *p = (t_point *)p1;
	t_point *q = (t_point *)p2;
	
	float diff = p->x - q->x;
	
    return ((diff >= 0.0) ? ((diff > 0.0) ? +1 : 0) : -1);
}

////////////////////////////////////////////////////////////////////////////////
void print_points( t_point *points, int num_point)
{
	int i;
	printf( "\n#points\n");
	
	for (i = 0; i < num_point; i++)
		printf( "points(%d,%d)\n", points[i].x, points[i].y);
}

////////////////////////////////////////////////////////////////////////////////
void print_line_segments( t_line *lines, int num_line)
{
	int i;

	printf( "\n#line segments\n");
	
	for (i = 0; i < num_line; i++)
		printf( "segments(%d,%d,%d,%d)\n", lines[i].from.x, lines[i].from.y, lines[i].to.x, lines[i].to.y);
}

////////////////////////////////////////////////////////////////////////////////
// [input] points : set of points
// [input] num_point : number of points
// [output] num_line : number of lines
// return value : pointer of set of line segments that forms the convex hull
t_line *convex_hull( t_point *points, int num_point, int *num_line)
{
	int capacity = 10;

	t_line *lines = (t_line *) malloc( capacity * sizeof(t_line));
	*num_line = 0;

	// s1: set of points
	t_point *s1 = (t_point *)malloc(sizeof(t_point) * num_point);
	assert( s1 != NULL);

	// s2: set of points
	t_point *s2 = (t_point *)malloc(sizeof(t_point) * num_point);
	assert( s2 != NULL);

	int n1, n2; // number of points in s1, s2, respectively

	// x 좌표에 따라 정렬된 점들의 집합이 입력된 경우
	// points[0] : leftmost point (p1)
	// points[num_point-1] : rightmost point (pn)
	
	// 점들을 분리
	separate_points( points, num_point, points[0], points[num_point-1], s1, s2, &n1, &n2);

	// upper hull을 구한다.
	lines = upper_hull( s1, n1, points[0], points[num_point-1], lines, num_line, &capacity);
	lines = upper_hull( s2, n2, points[num_point-1], points[0], lines, num_line, &capacity);
	
	free( s1);
	free( s2);

	return lines;
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	float x, y;
	int num_point; // number of points
	
	if (argc != 2)
	{
		printf( "%s number_of_points\n", argv[0]);
		return 0;
	}

	num_point = atoi( argv[1]);
	if (num_point <= 0)
	{
		printf( "The number of points should be a positive integer!\n");
		return 0;
	}

	t_point *points;
	points = (t_point *)malloc( sizeof(t_point) * num_point);
	assert( points != NULL);
	
	// making points
	srand( time(NULL));
	for (int i = 0; i < num_point; i++)
	{
		x = rand() % RANGE + 1; // 1 ~ RANGE random number
		y = rand() % RANGE + 1;
	
		points[i].x = x;
		points[i].y = y;
 	}

	fprintf( stderr, "%d points created!\n", num_point);
	
	// sort the points by their x coordinate
	qsort( points, num_point, sizeof(t_point), cmp_x);

	print_header( "convex.png");
	
	print_points( points, num_point);
	
	// convex hull algorithm
	int num_line;
	t_line *lines = convex_hull( points, num_point, &num_line);
	
	fprintf( stderr, "%d lines created!\n", num_line);

	print_line_segments( lines, num_line);
	
	print_footer();
	
    
	free( points);
	free( lines);
	
	return 0;
}

t_line *upper_hull( t_point *points, int num_point, t_point p1, t_point pn, t_line *lines, int *num_line, int *capacity){
    int i, n, max;
    float a, b, c;
    float preResult, postResult;
    t_point pmax;
    int n11=0, n12=0 , n11_temp=0, n12_temp=0;
    
    if(num_point == 0){
        lines[*num_line].from = p1;
        lines[*num_line].to = pn;
        (*num_line)++;
        if(*num_line == *capacity){
            *capacity *= 2;
            lines = (t_line*)realloc(lines, (*capacity) * sizeof(t_line));
        }
        return lines;
    }else{
        a = pn.y - p1.y;
        b = p1.x - pn.x;
        c = (p1.x)*(pn.y) - (p1.y)*(pn.x);
        preResult = -1;
        for(i=0; i<num_point; i++){
            postResult = distance( a, b, c, points[i]);
            if(postResult > preResult){
                preResult = postResult;
                pmax = points[i];
            }
        }
        t_point *s11 = (t_point *)malloc(sizeof(t_point) * num_point);
        t_point *s11_temp = (t_point *)malloc(sizeof(t_point) * num_point);
        t_point *s12 = (t_point *)malloc(sizeof(t_point) * num_point);
        t_point *s12_temp = (t_point *)malloc(sizeof(t_point) * num_point);
        separate_points( points, num_point, p1, pmax, s11, s11_temp, &n11, &n11_temp);
        lines = upper_hull( s11, n11, p1, pmax, lines, num_line, capacity);
        separate_points( points, num_point, pmax, pn, s12, s12_temp, &n12, &n12_temp);
        lines = upper_hull( s12, n12, pmax, pn, lines, num_line, capacity);
        free( s11_temp);
        free( s12_temp);
        free( s11);
        free( s12);
    }
    return lines;
}

float distance( float a, float b, float c, t_point p){
    float result;
    result = a*(p.x)+b*(p.y)-c;
    if(result < 0)
        result = -result;
    return result;
}

void separate_points( t_point *points, int num_point, t_point from, t_point to, t_point *s1, t_point *s2, int *n1, int *n2){
    int i, j, k;
    int a, b, c;
    j = 0;
    k = 0;
    a = to.y - from.y;
    b = from.x - to.x;
    c = (from.x)*(to.y) - (from.y)*(to.x);
    for(i=0; i<num_point; i++){
        if(a*(points[i].x)+b*(points[i].y)-c < 0){
            if(((points[i].x != from.x) | (points[i].y != from.y)) && ((points[i].x != to.x) || (points[i].y != to.y))){
                s1[j].x = points[i].x;
                s1[j].y = points[i].y;
                j++;
            }
        }else if(a*(points[i].x)+b*(points[i].y)-c > 0){
            if(((points[i].x != from.x) | (points[i].y != from.y)) && ((points[i].x != to.x) || (points[i].y != to.y))){
                s2[k].x = points[i].x;
                s2[k].y = points[i].y;
                k++;
            }
        }
    }
    *n1 = j;
    *n2 = k;
}
