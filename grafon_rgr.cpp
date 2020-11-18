#include <iostream>
#include <locale>
#include <windows.h>
#include <gl/glut.h>
#include <cmath>
#include <vector>
#include <SOIL.h>

#define PI 3.142315665

bool cul_f = 0,     // Флаг для culface
cam_f = 1,     // Флаг для перспективы
wires_f = 0,   // Флаг для отображение скелета
textur_f = 1;  // Флаг для отображения текстуры

int left_click = GLUT_UP,   // Состояние левой клавиши
right_click = GLUT_UP,  // Состояние правой клавиши
xold,                   // Координата х до изменения (для функции мыши)
yold,                   // Координата y до изменения (для функции мыши)
cameraDistance = 0,     // Растояние до камеры
width,                  // Ширина окна
height;                 // Высота окна

float rotate_x = 0, // для вращения
rotate_y = 0;

GLuint texture; // Для текстуры

GLfloat LightPosition[4] = { 32, 32, 32, 1 }; // Позиция источника света GL_LIGHT1

// Массив с цветами
const GLfloat colors[5][4] = {
    { 1.0, 1.0, 1.0, 1.0 }, // Белый
    { 1.0, 0.0, 0.0, 1.0 }, // Красный
    { 0.0, 1.0, 0.0, 1.0 }, // Синий
    { 0.0, 0.0, 1.0, 1.0 }, // Зеленый
    { 1.0, 1.0, 0.0, 1.0 }  // Желтый
};

//Массив с RGB
const GLfloat rgb[3][4] = {
    { 1.0, 0.0, 0.0, 1.0 },
    { 0.0, 1.0, 0.0, 1.0 },
    { 0.0, 0.0, 1.0, 1.0 }
};

/*  УСТАНОВКА ПАРАМЕТРОВ СВЕТА    */
GLfloat LightAmbient[4] = { 0.1f, 0.1f, 0.05f, 1.0f }; // Цвет света
void setlight()
{
    glDisable(GL_LIGHT1);
    GLfloat LightDiffuse[] = { (1.3f + LightAmbient[0]) / 2, (1.3f + LightAmbient[1]) / 2, (1.3f + LightAmbient[2]) / 2, 1.0f }; // цвет рассеивания зависит от цвета падения
    GLfloat LightSpecular[] = { 1.0f, 1.0f, 1.0f, 0.7f }; // цвет отражения просто белый
    glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);     // Падающий цвет
    glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);     // Рассеиващющийся цвет
    glLightfv(GL_LIGHT1, GL_SPECULAR, LightSpecular);   // Отражаемый цвет
    glEnable(GL_LIGHT1); // Включаем
}

/*   УСТАНОВКА ПАРАМЕТРОВ МАТЕРИАЛА   */
void setmaterial()
{
    GLfloat MatAmbient[] = { 0.1f, 0.1f, 0.1f, 1.0f }; // как сверху все
    GLfloat MatDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat MatShininess = 128; // Степень блеска
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MatAmbient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MatDiffuse);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, MatShininess);
}

/*   ОТРИСОВКА ЦИЛИНДРА КАК В МЕТОДИЧКЕ   */
void draw_cylinder(GLfloat radius, GLfloat height)
{
    GLfloat h = -0.5; // Переменная из методички
    GLfloat angle = 0.0; // из методички l
    GLfloat angle_stepsize = 0.1; // шаг изменения угла

    int color = 0;

    // отрисовка поверхности
    glBegin(GL_TRIANGLE_STRIP);
    for (h; h < 0.5; h += 0.25)
    {
        while (angle <= 2 * PI) 
        {
            GLfloat Radius = radius * (1 + std::abs(std::sin(2 * angle)));
           
            GLfloat x = Radius * cos(angle);
            GLfloat y = Radius * sin(angle);

            if (!textur_f) 
            { // окраска
                if (color == 5)
                    color = 0;
                glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, colors[color]);
                color++;
            }
            glTexCoord2f(-x / 5, y / 255); // наложение текстур
            glVertex3f(x, y, height * h);
            glTexCoord2f(-x / 10, y / 15);
            glVertex3f(x, y, height *(h + 0.25));
            angle = angle + angle_stepsize;
        }
        angle = 0.0;
    }
    glEnd();
}

/*   СПЕЦИАЛЬНАЯ ОТРИСОВКА В ЗАВИСИМОСТИ ОТ WIRE_F   */
void figure()
{
    setmaterial();
    setlight();

    if (wires_f) // При условии что нужно создать каркас
    {
        glDepthFunc(GL_LEQUAL); // Для z-buffer
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); // отключение всех цветовых каналов
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Рисование полных полигонов
        draw_cylinder(10, 40);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); // После того как отрисовали невидимые полигоны для z-buffer включаем обратно
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // рисуем теперь каркас
    }
    draw_cylinder(10, 40);
    glFlush();
}

void install_light()
{
    glPointSize(10);
    glBegin(GL_POINTS);
    glColor3d(1, 1, 1);
    glVertex3d(LightPosition[0], LightPosition[1], LightPosition[2]);
    glEnd();
    glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Очистка буфферов
    glLoadIdentity();

    gluLookAt(0, 50, -10, 0, 0, 0, 0, 1, 0); // настройка вида

    glPushMatrix(); 
    {
        // Изменение положения фигуры в пространстве
        glTranslatef(0, cameraDistance, 0);
        glRotatef(-rotate_y, 1, 0, 0);
        glRotatef(-rotate_x, 0, 1, 0);

        glPushMatrix();
        {
            install_light();
            figure();
        } 
        glPopMatrix();

        glRotatef(-rotate_y, 1, 0, 0);
        glRotatef(-rotate_x, 0, 1, 0);
    } 
    glPopMatrix();
    glFlush();
    glutSwapBuffers();
}

void keyboardFunc(unsigned char key, int x, int y)
{
    std::string a, choose;
    double r, g, b;
    while (true)
    {
        
        std::cout << "Введите действие:\n1. смена цвета света\n2. смена цвета заднего плана(окна)\n3. переключение между текстурой и зарисовкой\n4. Удаление передней или задней стороны\n5. Установка положения источника света\n6. установка свойства материала(блестящий или нет)\n7. переключение между каркасом и сплошной фигурой\n";
        std::cin >> a;
        if (a == "1")
        {
            std::cout << "Введите r, g, b:\n";
            std::cin >> r >> g >> b;
            LightAmbient[0] = r;
            LightAmbient[1] = g;
            LightAmbient[2] = b;
            break;
        }
        if (a == "2")
        {
            std::cout << "Введите r, g, b:\n";
            std::cin >> r >> g >> b;
            glClearColor(r, g, b, 1);
            break;
        }
        if (a == "3")
        {
            if (textur_f)
            {
                textur_f = false;
                glDisable(GL_TEXTURE_2D);
                break;
            }
            else
            {
                textur_f = true;
                glEnable(GL_TEXTURE_2D);
                break;
            }
        }

        if (a == "4")
        {
            if (cul_f)
            {
                glDisable(GL_CULL_FACE);
                cul_f = false;
                break;
            }
            else
            {
                glEnable(GL_CULL_FACE);
                glCullFace(GL_BACK);
                cul_f = true;
                break;
            }
        }

        if (a == "5")
        {
            std::cout << "Введите x, y, z:\n";
            int x, y, z;
            std::cin >> x >> y >> z;
            float LightDirection[4] = { x, y, z, 1.0f };
            glPointSize(10);
            glBegin(GL_POINTS);
            glColor3d(1, 1, 1);
            glVertex3d(x, y, z); // первая точка
            glEnd();
            glLightfv(GL_LIGHT1, GL_POSITION, LightDirection);
            break;
        }

        if(a == "6")
        {
            while (1)
            {
                std::cout << "1. блестящий\n2. не блестящий\n";
                std::cin >> choose;
                if (choose == "1")
                {
                    float MatSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
                    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, MatSpecular);
                    break;
                }
                if (choose == "2")
                {
                    float MatSpecular[] = { 0.0f, 0.0f, 0.0f, 0.0f };
                    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, MatSpecular);
                    break;
                }
                else 
                {
                    std::cout << "Выберите досутпный режим!" << std::endl;
                }
            }
        }
        if (a == "7")
        {
            if (wires_f)
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                wires_f = false;
                break;
            }
            else
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                wires_f = true;
                break;
            }
        }
        else
        {
            std::cout << "Выберите одну из доступных функций" << std::endl;
            break;
        }
    }
    system("cls");
}

void mouseFunc(int button, int state, int x, int y) { // |
    if (button == GLUT_LEFT_BUTTON)                   // |
        left_click = state;                           // |
    if (button == GLUT_RIGHT_BUTTON)                  // |
        right_click = state;                          // |
                                                      // |
    xold = x;                                         // |
    yold = y;                                         // |
}                                                     // |
                                                      // | Изменение положения фигуры в пространстве
void motionFunc(int x, int y) {                       // |
    if (left_click == GLUT_DOWN) {                    // |
        rotate_y = rotate_y + (y - yold) / 5.f;       // |
        rotate_x = rotate_x + (x - xold) / 5.f;       // |
        glutPostRedisplay();                          // |
    }                                                 // |
    if (right_click == GLUT_DOWN)                     // |
    {                                                 // |
        cameraDistance -= (y - yold) * 0.2f;          // |
        yold = y;                                     // |
        glutPostRedisplay();                          // |
    }                                                 // |
                                                      // |
    xold = x;                                         // |
    yold = y;                                         // |
}                                                     // |


void reshapeFunc(int new_width, int new_height) {
    width = new_width;
    height = new_height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);

    //загрузка текстуры
    texture = SOIL_load_OGL_texture
    (
        "Image.bmp",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
    );

    //glBindTexture(GL_TEXTURE_2D, texture); // Привязка текстуры
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // повтор текстуры
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if (cam_f)
        gluPerspective(90, width / height, 1, 200);
    else  glOrtho(-45, 45, -45, 45, 1, 200);

    glMatrixMode(GL_MODELVIEW);

    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    setlocale(LC_ALL, "Ru");
    // Создаем окно
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH); // Используем 4 канала, с двумя буферами, с буфером глубины
    glutInitWindowSize(900, 600);

    glutCreateWindow("Четырехлистный цилиндр ");

    std::string choose;
    while (1)
    {
        std::cout << "1. перспективная проекция\n2. аксонометрическая\n";
        std::cin >> choose;
        if (choose == "1")
        {
            cam_f = 1;
            break;
        }
        if (choose == "2")
        {
            cam_f = 0;
            break;
        }
        else std::cout << "Пожалуйста, выберите доступный режим!" << std::endl;

    }

    glEnable(GL_DEPTH_TEST); // Для z-buffer

    glEnable(GL_POLYGON_SMOOTH); // для корректного отображения полигонов
    glEnable(GL_LINE_SMOOTH); //  и линий

    glEnable(GL_TEXTURE_2D); // использоавние 2д текстур

    glEnable(GL_LIGHTING); // использоавние света
    glShadeModel(GL_SMOOTH); // гладкие  тени

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glutDisplayFunc(&display);
    glutReshapeFunc(&reshapeFunc);
    glutMouseFunc(&mouseFunc);
    glutKeyboardFunc(&keyboardFunc);
    glutMotionFunc(&motionFunc);

    glutMainLoop();

    return 0;
}