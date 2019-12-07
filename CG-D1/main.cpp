#include <locale>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>

#include <GL/glut.h>
#include "extras.h"

using namespace std;

// ============================== BEGIN: ESTRUTURAS DE DADOS ============================== //

// Classe que representa um vértice
class Vertex
{
    private:
        // Coordenadas x, y e z do vértice
        float x, y, z;
    public:
        Vertex()
        {
            // Por padrão, o vértice é definido em (0, 0, 0)
            this->x = 0.0f;
            this->y = 0.0f;
            this->z = 0.0f;
        }

        Vertex(float x, float y, float z)
        {
            // Define a posição inicial do vértice
            this->x = x;
            this->y = y;
            this->z = z;
        }

        float getX()
        {
            // Retorna a coordenada x
            return this->x;
        }

        void setX(float x)
        {
            // Define a coordenada x
            this->x = x;
        }

        float getY()
        {
            // Retorna a coordenada y
            return this->y;
        }

        void setY(float y)
        {
            // Define a coordenada y
            this->y = y;
        }

        float getZ()
        {
            // Retorna a coordenada z
            return this->z;
        }

        void setZ(float z)
        {
            // Define a coordenada z
            this->z = z;
        }
};

// Classe que representa um par de vértices
class VertexPair
{
    private:
        // Vetor para armazenar o par de vértices
        Vertex vertexPair[2];
    public:
        VertexPair(Vertex v0, Vertex v1)
        {
            // Define cada vértice do par
            this->vertexPair[0] = v0;
            this->vertexPair[1] = v1;
        }

        Vertex* getV0()
        {
            // Retorna o vértice v0 do par
            return &this->vertexPair[0];
        }

        Vertex* getV1()
        {
            // Retorna o vértice v1 do par
            return &this->vertexPair[1];
        }
};

// Classe que representa um grupo de pares de vértices
class VertexGroup
{
    private:
        // Identificador do grupo
        string id;
        // Vetor de pares de vértices
        vector<VertexPair> vertexPairList;
    public:
        VertexGroup(string id)
        {
            // Define o identificador do grupo
            this->id = id;
        }

        string getId()
        {
            // Retorna o identificador do grupo
            return this->id;
        }

        void addVertexPair(VertexPair vertexPair)
        {
            // Adiciona um par de vértices ao final do vetor
            this->vertexPairList.push_back(vertexPair);
        }

        VertexPair* getVertexPair(int index)
        {
            // Retorna o par de vértices da posição 'index'
            return &this->vertexPairList.at(index);
        }

        void removeLastVertexPair()
        {
            // Remove o último par de vértices
            this->vertexPairList.pop_back();
        }

        void removeAll()
        {
            // Remove todos os pares de vértices
            this->vertexPairList.clear();
        }

        int groupSize()
        {
            // Quantidade de pares de vértices do grupo
            return this->vertexPairList.size();
        }
};

//Classe que representa um triângulo
class Triangle
{
    private:
        // Vetor com os 3 vértices do triângulo
        Vertex vertexArray[3];
    public:
        Triangle(Vertex v0, Vertex v1, Vertex v2)
        {
            // Define cada vértice do triângulo
            this->vertexArray[0] = v0;
            this->vertexArray[1] = v1;
            this->vertexArray[2] = v2;
        }

        Vertex* getV0()
        {
            // Retorna o vértice v0 do triângulo
            return &this->vertexArray[0];
        }

        void setV0(Vertex v0)
        {
            // Define o vértice v0 do triângulo
            this->vertexArray[0] = v0;
        }

        Vertex* getV1()
        {
            // Retorna o vértice v1 do triângulo
            return &this->vertexArray[1];
        }

        void setV1(Vertex v1)
        {
            // Define o vértice v1 do triângulo
            this->vertexArray[1] = v1;
        }

        Vertex* getV2()
        {
            // Retorna o vértice v2 do triângulo
            return &this->vertexArray[2];
        }

        void setV2(Vertex v2)
        {
            // Define o vértice v2 do triângulo
            this->vertexArray[2] = v2;
        }
};

// ============================== END: ESTRUTURAS DE DADOS ============================== //

// ============================== BEGIN: VARIÁVEIS GLOBAIS ============================== //

// Indica se o programa está em modo depuração
bool debug = false;

// Largura e altura inicial da janela
int initWindowWidth = 1200, initWindowHeight = 600;
// Posição inicial da janela
int initWindowPosX = 0, initWindowPosY = 0;
// Largura e altura atual da janela
int currWindowWidth = initWindowWidth, currWindowHeight = initWindowHeight;
// Posição mediana atual da janela
float middleWindowWidth = (currWindowWidth / 2.0f);
// Indica se o programa está em modo Fullscreen
bool isFullscreenWindow =  false;

// Mínimos e máximos do sistema de coordenadas usado
float minX = 0.0f, maxX = 1.0f, minY = 0.0f, maxY = 1.0f, minZ = 0.0f, maxZ = 1.0f;
// Origem no sistema de coordenadas usado
float x_Origin = ((maxX + minX) / 2.0f), y_Origin = ((maxY + minY) / 2.0f), z_Origin = ((maxZ + minZ) / 2.0f);

// Posição (x, y, z) da câmera
float cam_pos_z = 2 * maxZ;
float cam_pos_x = x_Origin;
float cam_pos_y = y_Origin;

// Rotação no eixo-X e no eixo-Y
float rotationX = 0.0f, rotationY = 0.0f, rotationSpeed = 2.5f;

// Variáveis para armazenar o último x e y do movimento do mouse (Callback motion())
int last_x, last_y;

// Vetor de grupos de par de vértices
vector<VertexGroup> vertexGroupList;

// Marcação usada para definição do identificador de um novo grupo
int groupNameTag = 0;

// Índice do grupo atual
int groupIndex = 0;

// Índice do par de vértices atual
int groupPairIndex = 0;

// Valor padrão para altura
float defaultVertexHeight = 0.1f;
// Valor atual para altura
float currVertexHeight = defaultVertexHeight;
// Bloqueia o aumento/diminuição da altura por igual ou apenas do par selecionado
bool lockHeight = true;
// Valor padrão da largura
float defaultVertexWidth = 0.05f;
// Valor atual para a largura
float currVertexWidth = defaultVertexWidth;

// Espaçamento da grid da Viewport dos pontos 2D
float gridInterval = 0.05f;

// Últimos vetores perpendiculares
Vertex last_pup(+currVertexWidth, 0.0f, 0.0f);
Vertex last_pdown(-currVertexWidth, 0.0f, 0.0f);

// ============================== END: VARIÁVEIS GLOBAIS ============================== //

// ============================== BEGIN: CALLBACKS ============================== //

void init(void);
void keyboard(unsigned char key, int x, int y);
void specialKeyboard(int key, int x, int y);
void motion(int x, int y);
void mouse(int button, int state, int x, int y);
void idle(void);
void reshape(int w, int h);
void display(void);

// ============================== END: CALLBACKS ============================== //

int main(int argc, char** argv)
{
    try
    {
        // Define a localização para a mesma do sistema
        setlocale(LC_ALL, "");

        // Inicializando GLUT
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
        glutInitWindowSize(initWindowWidth, initWindowHeight);

        // Inicia a janela centralizada na tela
        initWindowPosX = ((glutGet(GLUT_SCREEN_WIDTH) - initWindowWidth) / 2);
        initWindowPosY = ((glutGet(GLUT_SCREEN_HEIGHT) - initWindowHeight) / 2);
        glutInitWindowPosition(initWindowPosX, initWindowPosY);

        glutCreateWindow("CG - T1");

        // Inicializa demais recursos
        init();

        // Define as callbacks
        glutKeyboardFunc(keyboard);
        glutSpecialFunc(specialKeyboard);
        glutMotionFunc(motion);
        glutMouseFunc(mouse);
        glutIdleFunc(idle);
        glutReshapeFunc(reshape);
        glutDisplayFunc(display);

        cout << "# ==================== INSTRUÇÕES DE USO ==================== #" << endl << endl;
        cout << "1) Clique do botão esquerdo do mouse na região da grid 2D - Cria um novo vértice" << endl;
        cout << "2) Clique do botão direito do mouse na região da grid 2D - Remove o último vértice criado" << endl;
        cout << "3) Scroll up do mouse - Aumenta o zoom no objeto 3D" << endl;
        cout << "4) Scroll down do mouse - Diminui o zoom no objeto 3D" << endl;
        cout << "5) Tecla 'n' - Cria um novo grupo" << endl;
        cout << "6) Tecla 'x' - Remove o grupo atual" << endl;
        cout << "7) Tecla 'c' - Remove todos os vértices do grupo atual" << endl;
        cout << "7) Tecla 'r' - Reseta o programa ao estado inicial" << endl;
        cout << "7) Tecla 'd' - Ativa/desativa o modo de depuração" << endl;
        cout << "10) Teclas '-'/'+' - Diminui/Aumenta a altura atual do par de vértices selecionado" << endl;
        cout << "10) Teclas '.'/',' - Diminui/Aumenta a largura atual do par de vértices selecionado" << endl;
        cout << "10) Tecla 'aspas' - Desbloqueia/Bloqueia a alteração individual da altura do par de vértices" << endl;
        cout << "8) Setas esquerda/direita - Retrocede/avança na seleção de grupo" << endl;
        cout << "9) Setas baixo/cima - Retrocede/avança na seleção do par de vértices do grupo atual" << endl;
        cout << "7) Tecla 'l' - Carrega um modelo salvo" << endl;
        cout << "7) Tecla 's' - Salva o modelo atual" << endl;
        cout << "7) Tecla 'F12' - Entra/sai do modo fullscreen" << endl;
        cout << "7) Tecla 'Esc' - Fecha a janela do programa" << endl << endl;
        cout << "# =========================================================== #" << endl << endl;

        // Loop principal
        glutMainLoop();
    }
    catch (exception& ex)
    {
        cout << endl << ">>>>> ERRO: " << ex.what() << endl;
    }

	return 0;
}

/* Exemplo de cálculo de vetor normal que são definidos a partir dos vértices do triângulo;
  v_2
  ^
  |\
  | \
  |  \       'vn' é o vetor normal resultante
  |   \
  +----> v_1
  v_0
*/
void CalculaNormal(Triangle t, Vertex *vn)
{
    Vertex *v_0 = t.getV0(), *v_1 = t.getV1(), *v_2 = t.getV2();
    Vertex v1, v2;

    double len;

    /* Encontra vetor v1 */
    v1.setX(v_1->getX() - v_0->getX());
    v1.setY(v_1->getY() - v_0->getY());
    v1.setZ(v_1->getZ() - v_0->getZ());

    /* Encontra vetor v2 */
    v2.setX(v_2->getX() - v_0->getX());
    v2.setY(v_2->getY() - v_0->getY());
    v2.setZ(v_2->getZ() - v_0->getZ());

    /* Calculo do produto vetorial de v1 e v2 */
    vn->setX((v1.getY() * v2.getZ()) - (v1.getZ() * v2.getY()));
    vn->setY((v1.getZ() * v2.getX()) - (v1.getX() * v2.getZ()));
    vn->setZ((v1.getX() * v2.getY()) - (v1.getY() * v2.getX()));

    /* normalizacao de n */
    len = sqrt(pow(vn->getX(), 2) + pow(vn->getY(), 2) + pow(vn->getZ(), 2));

    vn->setX(vn->getX() / len);
    vn->setY(vn->getY() / len);
    vn->setZ(vn->getZ() / len);
}

// Compara 2 floats, considerando uma imprecisão (epsilon)
bool float_compare(float a, float b, float epsilon = 0.0001f)
{
    return (fabs(a - b) < epsilon);
}

void updateVertexOrientation()
{
    if(vertexGroupList[groupIndex].groupSize() >= 4)
    {
        int pairIndex = groupPairIndex;

        if(pairIndex < 3)
        {
            pairIndex = 3;
        }

        if((pairIndex % 2) == 0)
        {
            pairIndex += 1;
        }

        Vertex centralA, centralB;
        Vertex u, pup, pdown;
        Vertex *v0, *v1, *v2, *v3, *v4, *v5, *v6, *v7;

        v0 = vertexGroupList[groupIndex].getVertexPair(pairIndex - 3)->getV0();
        v1 = vertexGroupList[groupIndex].getVertexPair(pairIndex - 3)->getV1();
        v2 = vertexGroupList[groupIndex].getVertexPair(pairIndex - 2)->getV0();
        v3 = vertexGroupList[groupIndex].getVertexPair(pairIndex - 2)->getV1();

        v4 = vertexGroupList[groupIndex].getVertexPair(pairIndex - 1)->getV0();
        v5 = vertexGroupList[groupIndex].getVertexPair(pairIndex - 1)->getV1();
        v6 = vertexGroupList[groupIndex].getVertexPair(pairIndex)->getV0();
        v7 = vertexGroupList[groupIndex].getVertexPair(pairIndex)->getV1();

        v0->setX(v0->getX() - x_Origin);
        v0->setY(v0->getY() - y_Origin);
        v1->setX(v1->getX() - x_Origin);
        v1->setY(v1->getY() - y_Origin);
        v2->setX(v2->getX() - x_Origin);
        v2->setY(v2->getY() - y_Origin);
        v3->setX(v3->getX() - x_Origin);
        v3->setY(v3->getY() - y_Origin);

        v4->setX(v4->getX() - x_Origin);
        v4->setY(v4->getY() - y_Origin);
        v5->setX(v5->getX() - x_Origin);
        v5->setY(v5->getY() - y_Origin);
        v6->setX(v6->getX() - x_Origin);
        v6->setY(v6->getY() - y_Origin);
        v7->setX(v7->getX() - x_Origin);
        v7->setY(v7->getY() - y_Origin);

        centralA.setX((v0->getX() + v1->getX()) / 2.0f);
        centralA.setY((v0->getY() + v1->getY()) / 2.0f);

        centralB.setX((v4->getX() + v5->getX()) / 2.0f);
        centralB.setY((v4->getY() + v5->getY()) / 2.0f);

        /* Encontra vetor ente os pontos centrais */
        u.setX(centralB.getX() - centralA.getX());
        u.setY(centralB.getY() - centralA.getY());

        /* Encontra dois vetores ortogonais a u */
        pup.setX(-u.getY());
        pup.setY(u.getX());

        pdown.setX(u.getY());
        pdown.setY(-u.getX());

        float val_0 = sqrt(pow(pup.getX(), 2) + pow(pup.getY(), 2));
        float val_1 = sqrt(pow(pdown.getX(), 2) + pow(pdown.getY(), 2));

        if(val_0 == 0)
        {
            val_0 = 1;
        }

        if(val_1 == 0)
        {
            val_1 = 1;
        }

        pup.setX((pup.getX() / val_0) * currVertexWidth);
        pup.setY((pup.getY() / val_0) * currVertexWidth);

        pdown.setX((pdown.getX() / val_1) * currVertexWidth);
        pdown.setY((pdown.getY() / val_1) * currVertexWidth);

        /*Desloca os vértices na direção do vetor perpendicular*/
        v0->setX(centralA.getX() + pup.getX());
        v0->setY(centralA.getY() + pup.getY());
        v1->setX(centralA.getX() + pdown.getX());
        v1->setY(centralA.getY() + pdown.getY());
        v2->setX(centralA.getX() + pup.getX());
        v2->setY(centralA.getY() + pup.getY());
        v3->setX(centralA.getX() + pdown.getX());
        v3->setY(centralA.getY() + pdown.getY());

        v4->setX(centralB.getX() + pup.getX());
        v4->setY(centralB.getY() + pup.getY());
        v5->setX(centralB.getX() + pdown.getX());
        v5->setY(centralB.getY() + pdown.getY());
        v6->setX(centralB.getX() + pup.getX());
        v6->setY(centralB.getY() + pup.getY());
        v7->setX(centralB.getX() + pdown.getX());
        v7->setY(centralB.getY() + pdown.getY());

        v0->setX(v0->getX() + x_Origin);
        v0->setY(v0->getY() + y_Origin);
        v1->setX(v1->getX() + x_Origin);
        v1->setY(v1->getY() + y_Origin);
        v2->setX(v2->getX() + x_Origin);
        v2->setY(v2->getY() + y_Origin);
        v3->setX(v3->getX() + x_Origin);
        v3->setY(v3->getY() + y_Origin);

        v4->setX(v4->getX() + x_Origin);
        v4->setY(v4->getY() + y_Origin);
        v5->setX(v5->getX() + x_Origin);
        v5->setY(v5->getY() + y_Origin);
        v6->setX(v6->getX() + x_Origin);
        v6->setY(v6->getY() + y_Origin);
        v7->setX(v7->getX() + x_Origin);
        v7->setY(v7->getY() + y_Origin);

        last_pup = pup;
        last_pdown = pdown;
    }
}

void loadModel(string filepath)
{
    ifstream modelFile(filepath);

    if (!modelFile.is_open())
    {
        cout << ">>>>> ERRO: Não foi possível abrir o arquivo " << filepath.c_str() << endl;
        return;
    }

    vertexGroupList.clear();

    string tmpLine;
    istringstream lineStream;
    float x, y, z;

    while(getline(modelFile, tmpLine))
    {
        lineStream.clear();
        lineStream.str(tmpLine);

        string groupId;
        int groupSize;

        lineStream >> groupId >> groupSize;

        VertexGroup vertexGroup(groupId);

        for(int i = 0; i < groupSize; i++)
        {
            getline(modelFile, tmpLine);

            lineStream.clear();
            lineStream.str(tmpLine);
            lineStream >> x >> y >> z;

            Vertex v0(x, y, z);

            getline(modelFile, tmpLine);

            lineStream.clear();
            lineStream.str(tmpLine);
            lineStream >> x >> y >> z;

            Vertex v1(x, y, z);

            VertexPair p0(v0, v1);
            vertexGroup.addVertexPair(p0);
        }

        vertexGroupList.push_back(vertexGroup);
        getline(modelFile, tmpLine);
    }

    modelFile.close();
    groupNameTag = vertexGroupList.size();

    cout << ">>>>> Modelo carregado!" << endl;
}

void saveModel(string filepath)
{
    ofstream modelFile(filepath);

    if (!modelFile.is_open())
    {
        cout << ">>>>> ERRO: Não foi possível criar o arquivo " << filepath.c_str() << endl;
        return;
    }

    modelFile << std::fixed << std::setprecision(6);

    VertexGroup *tmpVertexGroup;
    int vertexPairCount;
    VertexPair *tmpVertexPair;
    Vertex *tmpV0, *tmpV1;

    for(int i = 0; i < vertexGroupList.size(); i++)
    {
        tmpVertexGroup = &vertexGroupList[i];

        modelFile << tmpVertexGroup->getId() << " " << tmpVertexGroup->groupSize() << endl;

        vertexPairCount = tmpVertexGroup->groupSize();

        for(int j = 0; j < vertexPairCount; j++)
        {
            tmpVertexPair = tmpVertexGroup->getVertexPair(j);

            tmpV0 = tmpVertexPair->getV0();
            modelFile << tmpV0->getX() << " " << tmpV0->getY() << " " << tmpV0->getZ() << endl;

            tmpV1 = tmpVertexPair->getV1();
            modelFile << tmpV1->getX() << " " << tmpV1->getY() << " " << tmpV1->getZ() << endl;
        }

        modelFile << endl;
    }

    modelFile.close();
    cout << ">>>>> Modelo salvo!" << endl;
}

// Desenha grid da Viewport de pontos 2D
void drawGrid()
{
    // Define como cor da grid o preto
    glColor3f(0.0f, 0.0f, 0.0f);

    // Pontos iniciais
    float x1 = 0.0f, y1 = minY, z1 = 0.0f;
    float x2 = 0.0f, y2 = maxY, z2 = 0.0f;

    // Tamanho da grid de acordo com o intervalo entre as linhas da grid
    int gridSize = maxX / gridInterval;

    // Linhas paralelas ao eixo Y
    for(int i = 0; i <= gridSize; i++)
    {
        // Desenha linhas paralelas ao eixo-Y, com exceção da linha do eixo coordenado
        if(!float_compare(x1, x_Origin))
        {
            // Desenha uma linha de (x1, y1, z1) até (x2, y2, z2)
            glBegin(GL_LINES);
                glVertex3f(x1, y1, z1);
                glVertex3f(x2, y2, z2);
            glEnd();
        }

        x1 += gridInterval;
        x2 += gridInterval;
    }

    x1 = minX, y1 = 0.0f, z1 = 0.0f;
    x2 = maxX, y2 = 0.0f, z2 = 0.0f;

    gridSize = maxY / gridInterval;

    // Linhas paralelas ao eixo X
    for(int i = 0; i <= gridSize; i++)
    {
        // Desenha linhas paralelas ao eixo-X, com exceção da linha do eixo coordenado
        if(!float_compare(y1, y_Origin))
        {
            // Desenha uma linha de (x1, y1, z1) até (x2, y2, z2)
            glBegin(GL_LINES);
                glVertex3f(x1, y1, z1);
                glVertex3f(x2, y2, z2);
            glEnd();
        }

        y1 += gridInterval;
        y2 += gridInterval;
    }
}

// Desenha os eixos coordenados
void drawAxis()
{
    // Desenha o eixo-X
    glBegin(GL_LINES);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(minX, y_Origin, 0.0f);
        glVertex3f(maxX, y_Origin, 0.0f);
    glEnd();

    // Desenha o eixo-Y
    glBegin(GL_LINES);
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(x_Origin, minY, 0.0f);
        glVertex3f(x_Origin, maxY, 0.0f);
    glEnd();

    // Desenha o eixo-Z
    glBegin(GL_LINES);
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(x_Origin, y_Origin, minZ);
        glVertex3f(x_Origin, y_Origin, maxZ);
    glEnd();
}

// Desenha os vértices no plano 2D
void drawGroupPoints()
{
    // Para cada grupo de par de vértices
    for(int g = 0; g < vertexGroupList.size(); g++)
    {
        // Verifica se há algum par de vértices no grupo
        if(vertexGroupList[g].groupSize() > 0)
        {
            // Define o tamanho do ponto
            glPointSize(8.0f);

            // Para cada par de vértices do grupo
            for(int i = 0; i < vertexGroupList[g].groupSize(); i += 2)
            {
                // Obtém o vértice v0 e v1 do par de vértices do grupo
                Vertex *v0 = vertexGroupList[g].getVertexPair(i)->getV0();
                Vertex *v1 = vertexGroupList[g].getVertexPair(i)->getV1();

                // Obtém o ponto central entre v0 e v1
                Vertex v2((v0->getX() + v1->getX()) / 2, (v0->getY() + v1->getY()) / 2, v0->getZ());

                // Desenha o ponto central
                glBegin(GL_POINTS);
                    if((g == groupIndex) && (i == groupPairIndex))
                    {
                        // Caso o par esteja selecionado, colore de vermelho
                        glColor3f(1.0f, 0.0f, 0.0f);
                    }
                    else
                    {
                        // Senão, de azul
                        glColor3f(0.0f, 0.0f, 1.0f);
                    }
                    glVertex3f(v2.getX(), v2.getY(), 0.0f);
                glEnd();

                // Verifica se o programa está em modo depuração
                if(debug)
                {
                    // Desenha os vértices auxiliares
                    glBegin(GL_POINTS);
                        glColor3f(1.0f, 0.0f, 1.0f);
                        glVertex3f(v0->getX(), v0->getY(), 0.0f);
                    glEnd();

                    glBegin(GL_POINTS);
                        glColor3f(1.0f, 0.0f, 1.0f);
                        glVertex3f(v1->getX(), v1->getY(), 0.0f);
                    glEnd();
                }
            }
        }
    }
}

// Desenha objeto no espaço 3D
void drawGroupObject()
{
    int nextPairIndex;

    // Para cada grupo de par de vértices
    for(int g = 0; g < vertexGroupList.size(); g++)
    {
        // Verifica se existe pelo menos 4 pares de vértices no grupo para desenhar
        if(vertexGroupList[g].groupSize() >= 4)
        {
            // Desenha as faces do objeto
            for(int i = 0; i < (vertexGroupList[g].groupSize() - 2); i += 2)
            {
                VertexPair *pair0 = vertexGroupList[g].getVertexPair(i);

                nextPairIndex = (i + 1);
                VertexPair *pair1 = vertexGroupList[g].getVertexPair(nextPairIndex);

                nextPairIndex = (i + 2);
                VertexPair *pair2 = vertexGroupList[g].getVertexPair(nextPairIndex);

                nextPairIndex = (i + 3);
                VertexPair *pair3 = vertexGroupList[g].getVertexPair(nextPairIndex);

                Vertex *v0 = pair0->getV0();
                Vertex *v1 = pair0->getV1();

                Vertex *v2 = pair1->getV0();
                Vertex *v3 = pair1->getV1();

                Vertex *v4 = pair2->getV0();
                Vertex *v5 = pair2->getV1();

                Vertex *v6 = pair3->getV0();
                Vertex *v7 = pair3->getV1();

                Triangle t[12] = {
                    {*v1, *v0, *v2}, {*v1, *v2, *v3},
                    {*v4, *v5, *v6}, {*v5, *v7, *v6},
                    {*v0, *v4, *v2}, {*v4, *v6, *v2},
                    {*v5, *v1, *v7}, {*v1, *v3, *v7},
                    {*v3, *v2, *v7}, {*v2, *v6, *v7},
                    {*v0, *v1, *v4}, {*v1, *v5, *v4}};

                Vertex vetorNormal;

                glBegin(GL_TRIANGLES);
                    // Desenha cada triângulo
                    for(int i = 0; i < 12; i++)
                    {
                        // Calcula o vetor normal para cada triângulo
                        CalculaNormal(t[i], &vetorNormal);
                        glNormal3f(vetorNormal.getX(), vetorNormal.getY(),vetorNormal.getZ());

                        glVertex3d(t[i].getV0()->getX(), t[i].getV0()->getY(), t[i].getV0()->getZ());
                        glVertex3d(t[i].getV1()->getX(), t[i].getV1()->getY(), t[i].getV1()->getZ());
                        glVertex3d(t[i].getV2()->getX(), t[i].getV2()->getY(), t[i].getV2()->getZ());
                    }
                glEnd();
            }
        }
    }
}

void init(void)
{
    // Inicializa iluminação
    initLight(initWindowWidth, initWindowHeight);
    setMaterials();
    glEnable(GL_DEPTH_TEST);

    // Inicializa o primeiro grupo de par de vértices
    VertexGroup group0("Group_" + std::to_string(groupNameTag++));
	vertexGroupList.push_back(group0);
}

void keyboard(unsigned char key, int x, int y)
{
    switch (tolower(key))
    {
        case 'd':
        {
            debug = !debug;
            break;
        }
        case 'l':
        {
            string filepath;
            cout << ">>>>> Caminho do arquivo para carregar o modelo: ";
            cin >> filepath;

            loadModel(filepath);
            break;
        }
        case 's':
        {
            string filepath;
            cout << ">>>>> Caminho do arquivo para salvar o modelo: ";
            cin >> filepath;

            saveModel(filepath);
            break;
        }
        case 'n':
        {
            // Adiciona um novo grupo de par de vértices
            VertexGroup group("Group_" + std::to_string(groupNameTag++));
            vertexGroupList.push_back(group);

            // Altera o grupo atualmente selecionado para o novo grupo
            groupIndex = (vertexGroupList.size() - 1);

            // Reseta o par de vértices selecionado
			groupPairIndex = 0;

			// Reseta a altura para o valor padrão
			currVertexHeight = defaultVertexHeight;

			// Reseta a largura para o valor padrão
			currVertexWidth = defaultVertexWidth;

			// Reseta os vetores perpendiculares para o padrão
            last_pup.setX(+currVertexWidth);
            last_pup.setY(0.0f);

            last_pdown.setX(-currVertexWidth);
            last_pdown.setY(0.0f);
            break;
        }
        case 'x':
        {
            // Verifica se há mais de 1 grupo de par de vértices
            if(vertexGroupList.size() > 1)
            {
                // Se sim, remove o grupo atualmente selecionado
                vertexGroupList.erase(vertexGroupList.begin() + groupIndex);

                // Altera o grupo selecionado para o grupo anterior ao removido, de forma circular
                groupIndex = (groupIndex - 1) % vertexGroupList.size();

                // Altera o par de vértices atualmente selecionado para o inicial
				groupPairIndex = 0;

				// Caso o grupo possua um par de vértices
				if (vertexGroupList[groupIndex].groupSize() > 0)
				{
				    // Altera a coordenada Z atual para a coordenada Z do vértice selecionado
                    currVertexHeight = vertexGroupList[groupIndex].getVertexPair(groupPairIndex)->getV0()->getZ();
				}
            }
            break;
        }
        case 'c':
        {
            // Remove todos os pares de vértices do grupo atualmente selecionado
            vertexGroupList[groupIndex].removeAll();

            // Reseta o par de vértices selecionado
            groupPairIndex = 0;

            // Reseta a altura para o valor padrão
            currVertexHeight = defaultVertexHeight;

            // Reseta a largura para o valor padrão
			currVertexWidth = defaultVertexWidth;

			// Reseta os vetores perpendiculares para o padrão
            last_pup.setX(+currVertexWidth);
            last_pup.setY(0.0f);

            last_pdown.setX(-currVertexWidth);
            last_pdown.setY(0.0f);
            break;
        }
        case 'r':
        {
            // Remove todos os pares de vértices do grupo atualmente selecionado
            vertexGroupList.clear();

            // Reseta a tag de identificadores de grupos
            groupNameTag = 0;

            // Reseta o grupo atualmente selecionado
            groupIndex = 0;

            // Reseta o par de vértices atualmente selecionado
            groupPairIndex = 0;

            // Adiciona o grupo inicial
            VertexGroup group0("Group_" + std::to_string(groupNameTag++));
            vertexGroupList.push_back(group0);

            // Reseta a altura para o valor padrão
            currVertexHeight = defaultVertexHeight;

            // Reseta a largura para o valor padrão
			currVertexWidth = defaultVertexWidth;

			// Reseta os vetores perpendiculares para o padrão
            last_pup.setX(+currVertexWidth);
            last_pup.setY(0.0f);

            last_pdown.setX(-currVertexWidth);
            last_pdown.setY(0.0f);
            break;
        }
        case '\'':
        {
            lockHeight = !lockHeight;
            break;
        }
        case '-':
        {
            // Verifica se a coordenada Z atual é maior do que o mínimo definido
            if ((currVertexHeight - 0.01f) >= minZ)
            {
                // Caso seja, decrementa a coordenada Z atual
                currVertexHeight -= 0.01f;
            }

            // Verifica se há par de vértices no grupo atual
            if(vertexGroupList[groupIndex].groupSize() > 0)
            {
                if(lockHeight && (vertexGroupList[groupIndex].groupSize() >= 4))
                {
                    if((groupPairIndex % 2) != 0)
                    {
                        if((currVertexHeight - 0.01f) > vertexGroupList[groupIndex].getVertexPair(groupPairIndex - 1)->getV0()->getZ())
                        {
                            vertexGroupList[groupIndex].getVertexPair(groupPairIndex)->getV0()->setZ(currVertexHeight);
                            vertexGroupList[groupIndex].getVertexPair(groupPairIndex)->getV1()->setZ(currVertexHeight);

                            if(groupPairIndex == 1)
                            {
                                vertexGroupList[groupIndex].getVertexPair(groupPairIndex + 2)->getV0()->setZ(currVertexHeight);
                                vertexGroupList[groupIndex].getVertexPair(groupPairIndex + 2)->getV1()->setZ(currVertexHeight);
                            }
                            else
                            {
                                vertexGroupList[groupIndex].getVertexPair(groupPairIndex - 2)->getV0()->setZ(currVertexHeight);
                                vertexGroupList[groupIndex].getVertexPair(groupPairIndex - 2)->getV1()->setZ(currVertexHeight);
                            }
                        }
                    }
                    else
                    {
                        vertexGroupList[groupIndex].getVertexPair(groupPairIndex)->getV0()->setZ(currVertexHeight);
                        vertexGroupList[groupIndex].getVertexPair(groupPairIndex)->getV1()->setZ(currVertexHeight);

                        if(groupPairIndex == 0)
                        {
                            vertexGroupList[groupIndex].getVertexPair(groupPairIndex + 2)->getV0()->setZ(currVertexHeight);
                            vertexGroupList[groupIndex].getVertexPair(groupPairIndex + 2)->getV1()->setZ(currVertexHeight);
                        }
                        else
                        {
                            vertexGroupList[groupIndex].getVertexPair(groupPairIndex - 2)->getV0()->setZ(currVertexHeight);
                            vertexGroupList[groupIndex].getVertexPair(groupPairIndex - 2)->getV1()->setZ(currVertexHeight);
                        }
                    }
                }
                else
                {
                    if((groupPairIndex % 2) != 0)
                    {
                        if((currVertexHeight - 0.01f) > vertexGroupList[groupIndex].getVertexPair(groupPairIndex - 1)->getV0()->getZ())
                        {
                            vertexGroupList[groupIndex].getVertexPair(groupPairIndex)->getV0()->setZ(currVertexHeight);
                            vertexGroupList[groupIndex].getVertexPair(groupPairIndex)->getV1()->setZ(currVertexHeight);
                        }
                    }
                    else
                    {
                        vertexGroupList[groupIndex].getVertexPair(groupPairIndex)->getV0()->setZ(currVertexHeight);
                        vertexGroupList[groupIndex].getVertexPair(groupPairIndex)->getV1()->setZ(currVertexHeight);
                    }
                }
            }

            break;
        }
        case '+':
        {
            // Caso seja, incrementa a coordenada Z atual
            currVertexHeight += 0.01f;

            // Verifica se há par de vértices no grupo atual
            if(vertexGroupList[groupIndex].groupSize() > 0)
            {
                if(lockHeight && (vertexGroupList[groupIndex].groupSize() >= 4))
                {
                    if((groupPairIndex % 2) == 0)
                    {
                        if((currVertexHeight + 0.01f) < vertexGroupList[groupIndex].getVertexPair(groupPairIndex + 1)->getV0()->getZ())
                        {
                            vertexGroupList[groupIndex].getVertexPair(groupPairIndex)->getV0()->setZ(currVertexHeight);
                            vertexGroupList[groupIndex].getVertexPair(groupPairIndex)->getV1()->setZ(currVertexHeight);

                            if(groupPairIndex == 0)
                            {
                                vertexGroupList[groupIndex].getVertexPair(groupPairIndex + 2)->getV0()->setZ(currVertexHeight);
                                vertexGroupList[groupIndex].getVertexPair(groupPairIndex + 2)->getV1()->setZ(currVertexHeight);
                            }
                            else
                            {
                                vertexGroupList[groupIndex].getVertexPair(groupPairIndex - 2)->getV0()->setZ(currVertexHeight);
                                vertexGroupList[groupIndex].getVertexPair(groupPairIndex - 2)->getV1()->setZ(currVertexHeight);
                            }
                        }
                    }
                    else
                    {
                        vertexGroupList[groupIndex].getVertexPair(groupPairIndex)->getV0()->setZ(currVertexHeight);
                        vertexGroupList[groupIndex].getVertexPair(groupPairIndex)->getV1()->setZ(currVertexHeight);

                        if(groupPairIndex == 1)
                        {
                            vertexGroupList[groupIndex].getVertexPair(groupPairIndex + 2)->getV0()->setZ(currVertexHeight);
                            vertexGroupList[groupIndex].getVertexPair(groupPairIndex + 2)->getV1()->setZ(currVertexHeight);
                        }
                        else
                        {
                            vertexGroupList[groupIndex].getVertexPair(groupPairIndex - 2)->getV0()->setZ(currVertexHeight);
                            vertexGroupList[groupIndex].getVertexPair(groupPairIndex - 2)->getV1()->setZ(currVertexHeight);
                        }
                    }
                }
                else
                {
                    if((groupPairIndex % 2) == 0)
                    {
                        if((currVertexHeight + 0.01f) < vertexGroupList[groupIndex].getVertexPair(groupPairIndex + 1)->getV0()->getZ())
                        {
                            vertexGroupList[groupIndex].getVertexPair(groupPairIndex)->getV0()->setZ(currVertexHeight);
                            vertexGroupList[groupIndex].getVertexPair(groupPairIndex)->getV1()->setZ(currVertexHeight);
                        }
                    }
                    else
                    {
                        vertexGroupList[groupIndex].getVertexPair(groupPairIndex)->getV0()->setZ(currVertexHeight);
                        vertexGroupList[groupIndex].getVertexPair(groupPairIndex)->getV1()->setZ(currVertexHeight);
                    }
                }
            }

            break;
        }
        case '.':
        {
            // Verifica se a largura é maior do que o mínimo definido
            if ((currVertexWidth - 0.01f) > minX)
            {
                // Diminui a largura
                currVertexWidth -= 0.01f;
                updateVertexOrientation();
            }
            break;
        }
        case ',':
        {
            // Aumenta a largura
            currVertexWidth += 0.01f;
            updateVertexOrientation();
            break;
        }
        case 27:
        {
            exit(0);
            break;
        }
    }
}

void specialKeyboard(int key, int x, int y)
{
    switch(key)
    {
        case GLUT_KEY_LEFT: // Seta direcional para esquerda
        {
            // Verifica se o índice de grupo é maior que zero
			if (groupIndex > 0)
			{
                // Caso seja, altera o índice do grupo selecionado para o anterior
				groupIndex--;

                // Altera o par de vértices atualmente selecionado para o inicial
				groupPairIndex = 0;

				// Caso o grupo possua um par de vértices
				if (vertexGroupList[groupIndex].groupSize() > 0)
				{
					// Altera a coordenada Z atual para a coordenada Z do vértice selecionado
					currVertexHeight = vertexGroupList[groupIndex].getVertexPair(groupPairIndex)->getV0()->getZ();
				}
				else
				{
                    currVertexHeight = defaultVertexHeight;
                    currVertexWidth = defaultVertexWidth;
				}
			}
            break;
        }
        case GLUT_KEY_RIGHT: // Seta direcional para direita
        {
            // Verifica se o índice de grupo é menor que o índice do último grupo
			if (groupIndex < (vertexGroupList.size() - 1))
			{
                // Caso seja, altera o índice do grupo selecionado para o próximo
				groupIndex++;

				// Altera o par de vértices atualmente selecionado para o inicial
				groupPairIndex = 0;

				// Caso o grupo possua um par de vértices
				if (vertexGroupList[groupIndex].groupSize() > 0)
				{
					// Altera a coordenada Z atual para a coordenada Z do vértice selecionado
					currVertexHeight = vertexGroupList[groupIndex].getVertexPair(groupPairIndex)->getV0()->getZ();
				}
				else
				{
                    currVertexHeight = defaultVertexHeight;
                    currVertexWidth = defaultVertexWidth;
				}
			}
            break;
        }
        case GLUT_KEY_UP: // Seta direcional para cima
        {
            // Verifica se o índice de par de vértices é menor que o último par
			if (groupPairIndex < (vertexGroupList[groupIndex].groupSize() - 1))
			{
                // Caso seja, altera o par de vértices atualmente selecionado para o próximo
				groupPairIndex++;

				// Altera a coordenada Z atual para a coordenada Z do vértice selecionado
				currVertexHeight = vertexGroupList[groupIndex].getVertexPair(groupPairIndex)->getV0()->getZ();
			}
            break;
        }
        case GLUT_KEY_DOWN: // Seta direcional para baixo
        {
            // Verifica se o índice de par de vértices é maior que o primeiro par
			if (groupPairIndex > 0)
			{
                // Caso seja, altera o par de vértices atualmente selecionado para o anterior
				groupPairIndex--;

				// Altera a coordenada Z atual para a coordenada Z do vértice selecionado
				currVertexHeight = vertexGroupList[groupIndex].getVertexPair(groupPairIndex)->getV0()->getZ();
			}
            break;
        }
        case GLUT_KEY_F12:
        {
            if(!isFullscreenWindow)
            {
                glutFullScreen();
            }
            else
            {
                glutReshapeWindow(initWindowWidth, initWindowHeight);
                glutPositionWindow(initWindowPosX, initWindowPosY);
            }

            isFullscreenWindow = !isFullscreenWindow;
            break;
        }
    }
}

void motion(int x, int y)
{
    // Limita o movimento de rotação apenas aos limites da Viewport do objeto 3D
    if((x < (currWindowWidth / 2)) || (x > currWindowWidth) || (y < 0.0f) || (y > currWindowHeight))
    {
        return;
    }

    // Aumenta/diminui a rotação no eixo-X ou eixo-Y
    if(x > last_x)
    {
        rotationX += rotationSpeed;
    }
    else if(x < last_x)
    {
        rotationX -= rotationSpeed;
    }

    if(y > last_y)
    {
        rotationY += rotationSpeed;
    }
    else if(y < last_y)
    {
        rotationY -= rotationSpeed;
    }

    last_x = x;
    last_y = y;
}

void mouse(int button, int state, int x, int y)
{
    if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) // Botão esquerdo do mouse
    {
        // Limita o clique apenas para a Viewport dos pontos 2D
        if(x < middleWindowWidth)
        {
            // Coordenadas X, Y e Z do vértice
            float posX = maxX * (x / middleWindowWidth);
            float posY = maxY - (y / (float)currWindowHeight);
            float posMinZ = minZ;
            float posMaxZ = currVertexHeight;

            // Cria um novo par de vértices
            Vertex v0(posX + last_pup.getX(), posY + last_pup.getY(),  posMinZ);
            Vertex v1(posX + last_pdown.getX(), posY + last_pdown.getY(),  posMinZ);

            Vertex v2(v0.getX(), v0.getY(),  posMaxZ);
            Vertex v3(v1.getX(), v1.getY(),  posMaxZ);

            VertexPair p0(v0, v1);
            VertexPair p1(v2, v3);

            vertexGroupList[groupIndex].addVertexPair(p0);
            vertexGroupList[groupIndex].addVertexPair(p1);

            // Altera o par de vértices atualmente selecionado para o novo par
            groupPairIndex = (vertexGroupList[groupIndex].groupSize() - 1);

            // Altera a coordenada Z atual para a coordenada Z do vértice selecionado
            currVertexHeight = vertexGroupList[groupIndex].getVertexPair(groupPairIndex)->getV0()->getZ();

            updateVertexOrientation();
        }
    }

    if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN)) // Botão direito do mouse
    {
        // Verifica se há par de vértices no grupo
        if(vertexGroupList[groupIndex].groupSize() > 0)
        {
            // Se sim, remove os 2 últimos pares de vértices
            vertexGroupList[groupIndex].removeLastVertexPair();
            vertexGroupList[groupIndex].removeLastVertexPair();

            // Verifica se não há mais pares de vértices
            if(vertexGroupList[groupIndex].groupSize() == 0)
            {
                // Se sim, altera o par de vértices atualmente selecionado para o inicial
                groupPairIndex = 0;

                // Altera a coordenada Z atual para o padrão
                currVertexHeight = defaultVertexHeight;

                currVertexWidth = defaultVertexWidth;

                last_pup.setX(+currVertexWidth);
                last_pup.setY(0.0f);

                last_pdown.setX(-currVertexWidth);
                last_pdown.setY(0.0f);
            }
            else
            {
                // Caso contrário, seleciona o último par de vértices
                groupPairIndex = (vertexGroupList[groupIndex].groupSize() - 1);

                // Altera a coordenada Z atual para a coordenada Z do vértice selecionado
                currVertexHeight = vertexGroupList[groupIndex].getVertexPair(groupPairIndex)->getV0()->getZ();
            }
        }
    }

    if(button == 3) // Scroll up
    {
        // Aumenta o zoom da câmera
		cam_pos_z -= 0.1f;
    }

    if(button == 4) // Scroll Down
    {
        // Diminui o zoom da câmera
		cam_pos_z += 0.1f;
    }
}

//Exibir informaçoes na barra de titulo
void idle(void)
{
    // Verifica se há par de vértices no grupo
    if(vertexGroupList[groupIndex].groupSize() > 0)
    {
        // Altera a coordenada Z atual para a coordenada Z do vértice selecionado
        currVertexHeight = vertexGroupList[groupIndex].getVertexPair(groupPairIndex)->getV0()->getZ();
    }

    // Define o título da janela
	string windowTitle = string(vertexGroupList[groupIndex].getId()
                             + " | Pair = " + to_string(groupPairIndex)
                             + " | Height = " + to_string(currVertexHeight) + " (" + (lockHeight ? "Locked" : "Unlocked") + ")"
                             + " | Width = " + to_string(currVertexWidth));
	glutSetWindowTitle(windowTitle.c_str());

    glutPostRedisplay();
}

void reshape(int w, int h)
{
    currWindowWidth = w;
    currWindowHeight = h;
    middleWindowWidth = (currWindowWidth / 2.0f);
}

void display(void)
{
    // Define a Viewport dos pontos 2D
    glViewport(0, 0, middleWindowWidth, currWindowHeight);

    // Limpa o fundo da Viewport com a cor branca
	glScissor(0, 0, middleWindowWidth, currWindowHeight);
	glEnable(GL_SCISSOR_TEST);
	glClearDepth(1.0f);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Define a projeção ortogonal
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(minX, maxX, minY, maxY, minZ, maxZ);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Desabilita/habilita a iluminação
    glDisable(GL_LIGHTING);

    // Dsenha a grid, os eixos coordenados e os vértices dos grupos
    drawGrid();
    drawAxis();
    drawGroupPoints();

    // Define a Viewport do modelo 3D
	glViewport(middleWindowWidth, 0, middleWindowWidth, currWindowHeight);

	// Limpa o fundo da Viewport com a cor preta
	glScissor(middleWindowWidth, 0, middleWindowWidth, currWindowHeight);
	glEnable(GL_SCISSOR_TEST);
	glClearDepth(1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Define a projeção em perspectiva
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90.0f, (middleWindowWidth / currWindowHeight), 0.01f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Define a posição da câmera
    gluLookAt(cam_pos_x, cam_pos_y, cam_pos_z, x_Origin, y_Origin, z_Origin, 0.0f, 1.0, 0.0f);

    // Habilita a iluminação
    glEnable(GL_LIGHTING);

    // Desenha o objeto 3D
    glPushMatrix();
		glTranslatef(x_Origin, y_Origin, z_Origin);
        glRotatef(rotationY, 1.0f, 0.0f, 0.0f);
        glRotatef(rotationX, 0.0f, 1.0f, 0.0f);
		glTranslatef(-x_Origin, -y_Origin, -z_Origin);
        drawGroupObject();
    glPopMatrix();

    glutSwapBuffers();
}
