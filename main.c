#include <GL/glut.h>
//#include <GL/glu.h>
//#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.1415926535
#define P2 PI/2
#define P3 3*PI/2
#define DR 0.0174533 //one radian degree

int WIDTH = 1024;
int HEIGHT = 512;

float px, py, pdx, pdy, pa; //PLayer position

//Drawing a player and aline denoting the direction, basically are the direction vectors powered by delta vales pdx and pdy
void DrawPlayer()
{
	glColor3f(1,0,0);
	glPointSize(16);
	glBegin(GL_POINTS);
	glVertex2i(px,py);
	glEnd();
	
	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex2i(px,py);
	glVertex2i(px+pdx*5,py+pdy*5);
	glEnd();
	glBegin(GL_LINES);
	glVertex2i(px+1,py+1);
	glVertex2i(px+pdx*5+1,py+pdy*5+1);
	glEnd();
}

//Map 1D array taken as 2D
int mapX=8, mapY=8, mapS=64;
int map[]=
{
	1,1,1,1,1,1,1,1,
	1,0,0,0,0,0,0,1,
	1,0,0,0,1,0,0,1,
	1,0,0,0,1,0,0,1,
	1,0,1,0,0,0,0,1,
	1,0,1,0,0,0,0,1,
	1,0,1,0,0,0,1,1,
	1,1,1,1,1,1,1,1,
};

//Drawing the 2D map using the map[] values and writing quads of size 64 in either Black or White for each value 1 for white(wall) 0 for black(walkable area)
void draw2DMap(){
	int x,y,xo,yo;
	for(y = 0; y < mapY; y++){
		for(x = 0; x < mapX; x++){
			if(map[y*mapX+x]==1){glColor3f(1,1,1);}else{ glColor3f(0,0,0); }
			xo=x*mapS; yo=y*mapS;
			glBegin(GL_QUADS);
			glVertex2i(xo       +1,yo       +1);
			glVertex2i(xo       +1,yo+mapS  -1);
			glVertex2i(xo+mapS  -1,yo+mapS  -1);
			glVertex2i(xo+mapS  -1,yo       +1);
			glEnd();
			
		}
	}
}


//Display functions
void display()
{
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 draw2DMap();
 raycast3d();
 DrawPlayer();
 glutSwapBuffers();
}

float dist(float ax,float ay,float bx,float by,float ang)
{
	return (sqrt((bx-ax)*(bx-ax) + (by-ay)*(by-ay)));
}

//Raycasting Magic
void raycast3d()
{
	int r,mx,my,mp,dof; float rx,ry,ra,xo,yo, disT;
	ra=pa - DR*30; if(ra<0){ ra=ra + 2*PI; } if(ra>2*PI){ ra=ra-2*PI; }
	for(r=0;r<60;r++)
	{
		
		//Check horizontal lines
		dof=0;
		float disH=1000000, hx=px,hy=py;
		float aTan =-1/tan(ra);
		printf("aTan value %f \n",aTan);
		
		if(ra>PI) //Looking down
		{
			ry=(((int)py>>6)<<6)-0.0001; 
			rx=(py-ry)*aTan+px; 
			yo=-64; 
			xo=-yo*aTan;
		} 
		
		if(ra<PI) //Looking up
		{
			ry=(((int)py>>6)<<6)+64;     
			rx=(py-ry)*aTan+px; 
			yo= 64; 
			xo=-yo*aTan;
		} 
		
		if(ra==0 || ra==PI)
		{
			rx=px; 
			ry=py; 
			dof=8; 
		}
		
		while(dof<8)
		{
			mx=(int)(rx)>>6; 
			my=(int)(ry)>>6; 
			mp=my*mapX+mx;
			
			if(mp>0 && mp<mapX*mapY && map[mp]>0){ hx=rx; hy=ry; disH=dist(px,py,hx,hy,ra); dof=8;}//We have hit a wall
			else{ rx+=xo; ry+=yo; dof+=1; }//No we haven't?? Next line lessgoo
		}
	
		
		//Check vertical line
		dof=0;
		float disV=1000000, vx=px,vy=py;
		float nTan =-tan(ra);
		
		if(ra>P2 && ra<P3) //Looking left
		{
			rx=(((int)px>>6)<<6)-0.0001; 
			ry=(px-rx)*nTan+py; 
			xo=-64; 
			yo=-xo*nTan;
		} 
		
		if(ra<P2 || ra>P3) //Looking right
		{
			rx=(((int)px>>6)<<6)+64;     
			ry=(px-rx)*nTan+py; 
			xo= 64; 
			yo=-xo*nTan;
		} 
		
		if(ra==0 || ra==PI)
		{
			rx=px; 
			ry=py;  
			dof=8; 
		}//Looking straight up or down
		
		while(dof<8)
		{
			mx=(int)(rx)>>6; 
			my=(int)(ry)>>6; 
			mp=my*mapX+mx;
			
			if(mp>0 && mp<mapX*mapY && map[mp]>0){ vx=rx; vy=ry; disV=dist(px,py,vx,vy,ra); dof=8;}//We have hit a wall
			else{ rx+=xo; ry+=yo; dof+=1; }//No we haven't?? Next line lessgoo
		}
		if(disV<disH){rx=vx;ry=vy; disT=disV; glColor3f(0.9,0,0);}
		if(disH<disV){rx=hx;ry=hy;disT=disH; glColor3f(0.7,0,0);}
		glLineWidth(3);
		glBegin(GL_LINES);
		glVertex2i(px,py);
		glVertex2i(rx,ry);
		glEnd();
		
		//---Draw the walls-----
		float ca=pa-ra; if(ca<0){ ca=ca + 2*PI; } if(ca>2*PI){ ca=ca-2*PI; } disT=disT*cos(ca); //fixing scaling issue
		float lineH=(mapS*320)/disT; if(lineH>320){lineH=320;}   //drawn line height
		float lineO=160-lineH/2;                                 //giving offset
		glLineWidth(8); glBegin(GL_LINES); glVertex2i(r*8+530,lineO); glVertex2i(r*8+530,lineH+lineO); glEnd();
		ra+=DR;if(ra<0){ ra=ra + 2*PI; } if(ra>2*PI){ ra=ra-2*PI; }
	}
}

//Mapping buttons 
void buttons(unsigned char key, int x, int y)
{
	if(key=='a'){pa-=0.1; if(pa<   0){pa+=2*PI;} pdx=cos(pa)*5; pdy=sin(pa)*5;}
	if(key=='d'){pa+=0.1; if(pa>2*PI){pa-=2*PI;} pdx=cos(pa)*5; pdy=sin(pa)*5;}
	if(key=='w'){px+=pdx; py+=pdy;}
	if(key=='s'){px-=pdx; py-=pdy;}
	printf("px value %f \n",px);
	printf("py value %f \n",py);
	glutPostRedisplay();	
}

//Initialzing the window screen before the display
void init()
{
 glClearColor(0.3,0.3,0.3,0);
 gluOrtho2D(0,WIDTH,HEIGHT,0);
 px=500; py=500; pdx=cos(pa)*5; pdy=sin(pa)*5;
}

//Main function responsible for calling the above functions sequencially
int main(int argc, char** argv[])
{ 
 glutInit(&argc, argv);
 glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
 glutInitWindowSize(WIDTH,HEIGHT);
 glutCreateWindow("Nkn Ley");
 init();
 glutDisplayFunc(display);
 glutKeyboardFunc(buttons);
 glutMainLoop();         
}                        
