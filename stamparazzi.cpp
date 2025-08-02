#include <GL/glut.h>
#include <math.h>

#define PI acos(-1.0)

int w=800, h=600;
int frameNumber=0, dir_camera=0;
bool up, down, left, right, zoom_in, zoom_out;
float vermelho[3][4]={{0.2,0,0,1},{0.7,0,0,1},{1,1,1,1}},
		verde[3][4]={{0,0.2,0,1},{0,0.7,0,1},{1,1,1,1}},
		azul[3][4]={{0,0,0.2,1},{0,0,0.7,1},{1,1,1,1}},
		amarelo[3][4]={{0.2,0.2,0,1},{0.7,0.7,0,1},{1,1,1,1}},
		magenta[3][4]={{0.2,0,0.2,1},{0.7,0,0.7,1},{1,1,1,1}},
		ciano[3][4]={{0,0.2,0.2,1},{0,0.7,0.7,1},{1,1,1,1}};
float comp_esp = 50.0, xpos=0, ypos=3, zpos=4;

struct XYZ
{
    float x;
    float y;
    float z;
};

#define NI 15
#define NJ 6
XYZ inp[NI+1][NJ+1];
#define RESOLUTIONI 3*NI
#define RESOLUTIONJ 3*NJ
XYZ outp[RESOLUTIONI][RESOLUTIONJ];

double BezierBlend(int k,double mu, int n)
{
   int nn,kn,nkn;
   double blend=1;

   nn = n;
   kn = k;
   nkn = n - k;

   while (nn >= 1) {
      blend *= nn;
      nn--;
      if (kn > 1) {
         blend /= (double)kn;
         kn--;
      }
      if (nkn > 1) {
         blend /= (double)nkn;
         nkn--;
      }
   }
   if (k > 0)
      blend *= pow(mu,(double)k);
   if (n-k > 0)
      blend *= pow(1-mu,(double)(n-k));

   return(blend);
}



void generateControlPoint()
{
	   //int nMax = 2;
	   //int nMin = -2;
	   int i,j;
	   //srandom(time(NULL));
	   for (i=0;i<=NI;i++) {
	      for (j=0;j<=NJ;j++) {
	         inp[i][j].x = i;
	         inp[i][j].y = j;
	         //inp[i][j].z = random()%((nMax+1)-nMin) + nMin;
	        inp[i][j].z = 4*sin(i);
	      }
	   }
}

void Surface()
{
   int i,j,ki,kj;
   double mui,muj,bi,bj;
   /* Create a random surface */
   for (i=0;i<RESOLUTIONI;i++) {
      mui = i / (double)(RESOLUTIONI-1);
      for (j=0;j<RESOLUTIONJ;j++) {
         muj = j / (double)(RESOLUTIONJ-1);
         outp[i][j].x = 0;
         outp[i][j].y = 0;
         outp[i][j].z = 0;
         for (ki=0;ki<=NI;ki++) {
            bi = BezierBlend(ki,mui,NI);
            for (kj=0;kj<=NJ;kj++) {
               bj = BezierBlend(kj,muj,NJ);
               outp[i][j].x += (inp[ki][kj].x * bi * bj);
               outp[i][j].y += (inp[ki][kj].y * bi * bj);
               outp[i][j].z += (inp[ki][kj].z * bi * bj);
            }
         }
      }
   }

}

XYZ calculaNormal(XYZ u, XYZ v) {
    XYZ normal;
    normal.x = u.y * v.z - u.z * v.y;
    normal.y = u.z * v.x - u.x * v.z;
    normal.z = u.x * v.y - u.y * v.x;


    // Normaliza
    float length = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
    normal.x /= length;
    normal.y /= length;
    normal.z /= length;

    return normal;
}

void escolheMaterial(float array[][4]){
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,array[0]);
	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,array[1]);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,array[2]);
	glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,comp_esp);
}

void keyboard(int key, int x, int y){
	if(key == GLUT_KEY_UP) up=true;
	if(key == GLUT_KEY_DOWN) down=true;
	if(key == GLUT_KEY_LEFT) left=true;
	if(key == GLUT_KEY_RIGHT) right=true;
	if(key == GLUT_KEY_F1) zoom_in=true;
	if(key == GLUT_KEY_F2) zoom_out=true;
	if(key == GLUT_KEY_F3) xpos=0, ypos=3, zpos=4;
}

void key_release(int key, int x, int y){
	if(key == GLUT_KEY_UP) up=false;
	if(key == GLUT_KEY_DOWN) down=false;
	if(key == GLUT_KEY_LEFT) left=false;
	if(key == GLUT_KEY_RIGHT) right=false;
	if(key == GLUT_KEY_F1) zoom_in=false;
	if(key == GLUT_KEY_F2) zoom_out=false;
}

void mouse(int button, int state, int x, int y){
	if(button == GLUT_LEFT_BUTTON)
		if(state == GLUT_DOWN) dir_camera=1;
	if(button == GLUT_RIGHT_BUTTON)
		if(state == GLUT_DOWN) dir_camera=0;
}

void reshape(int w, int h){
	if(!h) h=1;
	glViewport(0,0,w,h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(60,1,0.1,20);
}

void display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(xpos,ypos,zpos,0,0,0,0,1,0);

	if(!dir_camera) {
		escolheMaterial(vermelho);
		glPushMatrix();
			glTranslatef(1,1,1);
			glRotatef(frameNumber,0,0,1);
			glutSolidCube(0.5);
		glPopMatrix();
	} else {
		escolheMaterial(amarelo);
		glPushMatrix();
			glTranslatef(1,1,1);
			glRotatef(frameNumber,0,0,1);
			glutSolidTorus(0.15,0.25,30,30);
		glPopMatrix();
	}

	if(!dir_camera) {
		escolheMaterial(verde);
		glPushMatrix();
			glRotatef(frameNumber,0,1,0);
			glutSolidSphere(0.5,30,30);
		glPopMatrix();
	} else {
		escolheMaterial(magenta);
		glPushMatrix();
			glRotatef(frameNumber,0,1,0);
			glutSolidTeapot(0.5);
		glPopMatrix();
	}


	if(!dir_camera) {
		escolheMaterial(azul);
		glPushMatrix();
			glTranslatef(-2,-2,-2);
			glRotatef(frameNumber,1,0,0);
			glutSolidCone(1,1,30,30);
		glPopMatrix();
	} else {
		escolheMaterial(ciano);
		glPushMatrix();
			glTranslatef(-4,-4,-4);
			glScalef(0.2,0.2,0.2);
			Surface();

			for(int i=0; i<RESOLUTIONI-1; i++){
				for(int j=0; j<RESOLUTIONJ-1; j++){
					XYZ u = {outp[i+1][j].x - outp[i][j].x, outp[i+1][j].y - outp[i][j].y, outp[i+1][j].z - outp[i][j].z};
					XYZ v = {outp[i+1][j+1].x - outp[i][j].x, outp[i+1][j+1].y - outp[i][j].y, outp[i+1][j+1].z - outp[i][j].z};
					XYZ normal = calculaNormal(u,v);
					glNormal3f(normal.x,normal.y,normal.z);
					glBegin(GL_QUADS);
						glVertex3f(outp[i][j].x,outp[i][j].y,outp[i][j].z);
						glVertex3f(outp[i][j+1].x,outp[i][j+1].y,outp[i][j+1].z);
						glVertex3f(outp[i+1][j+1].x,outp[i+1][j+1].y,outp[i+1][j+1].z);
						glVertex3f(outp[i+1][j].x,outp[i+1][j].y,outp[i+1][j].z);
					glEnd();
				}
			}
		glPopMatrix();
	}

	glutSwapBuffers();
	glFlush();
}

void init(){
	glClearColor(0,0,0,1);

	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	float luz_ambiente[4]={0.2,0.2,0.2,1},
			luz_difusa[4]={0.7,0.7,0.7,1},
			luz_especular[4]={1,1,1,1},
			luz_posicao[4]={0,5,0,1};
	glLightfv(GL_LIGHT0,GL_AMBIENT,luz_ambiente);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,luz_difusa);
	glLightfv(GL_LIGHT0,GL_SPECULAR,luz_especular);
	glLightfv(GL_LIGHT0,GL_POSITION,luz_posicao);

	glEnable(GL_LIGHT0);
}

void anima(int v){
	frameNumber++;

	for(int i=0; i<=NI; i++){
		for(int j=0; j<=NJ; j++){
			inp[i][j].z=3*sin(0.05*frameNumber+i);
		}
	}

	if(left and xpos>-10) xpos--;
	if(right and xpos<10) xpos++;
	if(up and ypos<10) ypos++;
	if(down and ypos>-10) ypos--;
	if(zoom_in and zpos>-10) zpos--;
	if(zoom_out and zpos<10) zpos++;

	glutPostRedisplay();
	glutTimerFunc(20,anima,0);
}

int main(int argc, char **argv){
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(80,80);
	glutInitWindowSize(w,h);
	glutCreateWindow("Teste para P2");

	init();
	generateControlPoint();
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutSpecialFunc(keyboard);
	glutSpecialUpFunc(key_release);
	glutMouseFunc(mouse);
	glutTimerFunc(20,anima,0);
	glutMainLoop();
	return 0;
}
