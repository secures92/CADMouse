#ifndef MOUSE_H_
#define MOUSE_H_


#define XAxis 0
#define YAxis 1
#define ZAxis 2

int xAxisValue;
int yAxisValue;
int zAxisValue;

int getXAxis(void);
int getYAxis(void);
int getZAxis(void);

void mouseInit(void);




#endif /* MOUSE_H_ */