#include "modelisation/OpenGL.h"
#include "analyse/EdgeDetection.h"
#include "modelisation/Transformation.h"
#include "physics/AngleModel.h"
#include "physics/CollisionDetection.h"
#include "windows.h"
#include "muse/Muse.h"
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <vector>
#include <locale>
#include <sstream>
#include <conio.h>
#include <chrono>
#pragma comment(lib,"ws2_32.lib")

#define DEFAULT_BUFLEN 2000
using namespace cv;
using namespace std;

CameraStream *cameraStream = nullptr;
OpenGL *window = nullptr;
AngleModel *angleModel = nullptr;
Ball *ball = nullptr;
__int64 diff = -1;

HANDLE myHandle = nullptr;
unsigned int progress = 0;
int iResult;
char buffer[DEFAULT_BUFLEN];

/// Pour afficher les FPS
int frame = 0, myTime, timebase = 0;
double fps = 0.0;

int useless = 0;

/// Prototypes des fonctions de ce fichier
void loop(int);
void setupMaze();
DWORD WINAPI getMuseResult(LPVOID lpProgress);

int main(int argc, char** argv){

    bool anaglyph;
    string name = "aMAZEd Calibration";
    ball = new Ball(0.5, 0.5, 0.02, 50);
    cameraStream = new CameraStream();
    namedWindow(name, WINDOW_OPENGL);
    HWND* hwnd;

    DWORD myThreadID;
    myHandle = CreateThread(0, 0, getMuseResult,  &progress, 0, &myThreadID);

    while(true){
        Mat currentFrame = cameraStream->getCurrentFrame();
        putText(currentFrame, to_string(progress), Point2i(0, currentFrame.rows - 100), FONT_HERSHEY_PLAIN, 2, Scalar(0, 0, 255), 2);
        putText(currentFrame, "Press enter to play 3D mode", Point2i(0, currentFrame.rows - 50), FONT_HERSHEY_PLAIN, 2, Scalar(0, 0, 255), 2);
        putText(currentFrame, "Press space bar to play normal mode", Point2i(0, currentFrame.rows), FONT_HERSHEY_PLAIN, 2, Scalar(0, 0, 255), 2);
        float long12,long03, long01, long23, ratio = 0;
        EdgeDetection ED = EdgeDetection(currentFrame, true);
        vector<Point2i> coordCorner = ED.getCorner(currentFrame);

        long12 = sqrt(pow(coordCorner[1].x-coordCorner[2].x,2)+pow(coordCorner[1].y-coordCorner[2].y,2));
        long03 = sqrt(pow(coordCorner[0].x-coordCorner[3].x,2)+pow(coordCorner[0].y-coordCorner[3].y,2));

        if(long03 > long12*0.9 && long03 < long12*1.1){
            long01 = sqrt(pow(coordCorner[0].x-coordCorner[1].x,2)+pow(coordCorner[0].y-coordCorner[1].y,2));
            long23 = sqrt(pow(coordCorner[2].x-coordCorner[3].x,2)+pow(coordCorner[2].y-coordCorner[3].y,2));

            ratio = long01/long23;
        }
        bool is45 = ratio > 0.73 && ratio < 0.8 ;
        if(is45){
            putText(currentFrame, "O", Point2i(currentFrame.cols-20, 20), FONT_HERSHEY_PLAIN, 2, Scalar(0, 255, 0), 2);
        }else{
            putText(currentFrame, "X", Point2i(currentFrame.cols-20, 20), FONT_HERSHEY_PLAIN, 2, Scalar(0, 0, 255), 2);
        }

        hwnd = (HWND*)(cvGetWindowHandle(name.c_str()));
        if(hwnd == nullptr){
            CloseHandle(myHandle);
            delete cameraStream;
            delete window;
            delete angleModel;
            return 0;
        }
        imshow(name, currentFrame);

        /// Si on appuie sur :
        /// touche espace => normal mode
        /// touche entrée => anaglyph mode
        int key = waitKey(30);
        if(key == 32 && is45){
            anaglyph = false;
            break;
        } else if(key == 13 && is45){
            anaglyph = true;
            break;
        }
    }

    Mat currentFrame = cameraStream->getCurrentFrame();
    double ratio = (double)currentFrame.cols / (double)currentFrame.rows;
    int width = 1000; /// Largeur de la fenêtre

    auto *glutMaster = new GlutMaster();
    window = new OpenGL(glutMaster, width, (int)(width / ratio), 0, 0, (char*)("aMAZEd"), ball, cameraStream, anaglyph, progress);

    setupMaze();
    window->startTimer();

    destroyWindow(name);
    glutMaster->CallGlutMainLoop();

    CloseHandle(myHandle);
    delete cameraStream;
    delete window;
    delete angleModel;


    return 0;
}

void loop(int endGame){

    window->setProgress(progress);

    if(endGame == 1){
        CloseHandle(myHandle);
        waitKey(0);
        return;
    }

    /// Affichage FPS
    frame++;
    myTime = glutGet(GLUT_ELAPSED_TIME);
    if (myTime - timebase > 1000) {
        fps = frame * 1000.0 / (myTime - timebase);
        timebase = myTime;
        frame = 0;
    }
    window->setFps(fps);

    vector<Point2i> coordCorner;
    Mat currentFrame = cameraStream->getCurrentFrame();
    EdgeDetection edgeDetection = EdgeDetection(currentFrame, false);
    coordCorner = edgeDetection.getCorner(currentFrame);

    double offSetBall = 0.08;

    /// Si les 4 coins ont été détéctées
    if(coordCorner.size() == 4 && !edgeDetection.isReversed(coordCorner)) {
        Transformation transformation = Transformation(coordCorner, Size(currentFrame.cols, currentFrame.rows), 0.1, 20);
        angleModel->setCurrentTransformation(&transformation);

        vector<Wall> walls;
        if(CollisionDetection::findCollisions(ball, window->getWalls(), walls)){

            /// Detection de la nature de la collision
            bool verticalCollision = false;
            bool horizontalCollision = false;
            for(auto &wall : walls){
                if(!verticalCollision && wall.isVertical()) verticalCollision = true;
                if(!horizontalCollision && !wall.isVertical()) horizontalCollision = true;
            }

            /// Collision verticale on rebondit selon l'axe X
            if(verticalCollision){
                ball->setNextX(ball->getNextX() - ball->getVx());
                //ball->setVx(-ball->getVx());
                if(ball->getVx() >= 0){
                    ball->setVx(-offSetBall);
                }else{
                    ball->setVx(offSetBall);
                }
                ball->setAx(0);
            }

            /// Collision horizontale on rebondit selon l'axe Y
            if(horizontalCollision){
                ball->setNextY(ball->getNextY() - ball->getVy());
                //ball->setVy(-ball->getVy());

                if(ball->getVy() >= 0){
                    ball->setVy(-offSetBall);
                }else{
                    ball->setVy(offSetBall);
                }
                ball->setAy(0);
            }

            ball->updatePosition();

            /// S'il s'agit d'une collision sur le bout du mur
            /*if(CollisionDetection::findCollisions(ball, window->getWalls(), walls)){
                if(verticalCollision){
                    ball->setNextY(ball->getNextY() - ball->getVy() * 2);
                    if(ball->getVy() > 0){
                        ball->setVy(-offSetBall);
                    }else{
                        ball->setVy(offSetBall);
                    }
                    ball->setAy(0);
                }

                if(horizontalCollision){
                    ball->setNextX(ball->getNextX() - ball->getVx() * 2);
                    if(ball->getVx() > 0){
                        ball->setVx(-offSetBall);
                    }else{
                        ball->setVx(offSetBall);
                    }
                    ball->setAx(0);
                }
            }*/

            cout << ball->getR();
            cout << '\n';

        }else{
            ball->setAx(angleModel->getAngleY() / 10);
            ball->setAy(angleModel->getAngleX() / 10);
            ball->updatePosition();
        }

        double p[16];
        double m[16];
        transformation.getProjectionMatrix(p);
        transformation.getModelviewMatrix(m);
        window->setProjectionMatrix(p);
        window->setModelviewMatrix(m);
    }

    glutPostRedisplay();

}

void setupMaze(){

    /// Calibration des couleurs


    Mat currentFrame = cameraStream->getCurrentFrame();
    EdgeDetection edgeDetection = EdgeDetection(currentFrame, true);

    vector<Point2i> coordCorner;
    vector<Point2i> coordStartEnd;
    vector<vector<Point2i>> lines;

    /// Tant que les 4 coins n'ont pas été détéctées
    do {

        currentFrame = cameraStream->getCurrentFrame();
        coordStartEnd = edgeDetection.startEndDetection(currentFrame);
        coordCorner = edgeDetection.getCorner(currentFrame);

        /// Detection des murs
        lines = edgeDetection.wallsDetection(currentFrame, coordCorner, coordStartEnd);

    }while(coordStartEnd.size() != 2);

    Transformation *transformation = new Transformation(coordCorner, Size(currentFrame.cols, currentFrame.rows), 1, 10);

    ///point d'arrivée sauvegarde
    Point2d *pointModelEnd = new Point2d(transformation->getModelPointFromImagePoint(coordStartEnd[1]));
    window->setEndPoint(pointModelEnd);

    ///set la boule aux coordonnées du départ détectés
    cv::Point2d pointModelStart = transformation->getModelPointFromImagePoint(coordStartEnd[0]);

    ///set la boule aux coordonnées du départ
    ball->setNextX(pointModelStart.x);
    ball->setNextY(pointModelStart.y);

    /// Calcul des coordonées des extrimités des murs
    vector<Wall> walls;
    for (const auto &line : lines) {

        Point2d pointImageA = transformation->getModelPointFromImagePoint(line[0]);
        Point2d pointImageB = transformation->getModelPointFromImagePoint(line[1]);

        Wall wall(pointImageA, pointImageB);

        walls.push_back(wall);
    }

    /// Murs extérieurs
//    walls.emplace_back(Point2d(0, 0), Point2d(0, 1));
//    walls.emplace_back(Point2d(1, 1), Point2d(0, 1));
//    walls.emplace_back(Point2d(1, 1), Point2d(1, 0));
//    walls.emplace_back(Point2d(1, 0), Point2d(0, 0));

    window->setWalls(walls);

    angleModel = new AngleModel(transformation);


}

DWORD WINAPI getMuseResult(LPVOID lpProgress) {

    unsigned int& progress = *((unsigned int*)lpProgress);
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0){
        cout << "WSAStartup failed.\n";
        system("pause");
        return 1;
    }

    string path = "/muse/result";
    string domain = "muse-labyrinthe.herokuapp.com";

    string get_http;
    get_http = "GET " + path + " HTTP/1.1\r\nHost: " + domain + "\r\nConnection: close\r\n\r\n";

    SOCKADDR_IN SockAddr;

    struct hostent *host;
    host = gethostbyname(domain.c_str());
    SockAddr.sin_port=htons(80);
    SockAddr.sin_family=AF_INET;
    SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);

    while(true){
        SOCKET Socket;
        Socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
        if(connect(Socket,(SOCKADDR*)(&SockAddr),sizeof(SockAddr)) != 0){
            cout << "Could not connect";
            //system("pause");
            WSACleanup();
            return 1;
        }

        iResult = send(Socket,get_http.c_str(), strlen(get_http.c_str()),0 );
        if (iResult == SOCKET_ERROR) {
            printf("send failed: %d\n", WSAGetLastError());
            closesocket(Socket);
            WSACleanup();
            return 1;
        }

        int nDataLength;
        while ((nDataLength = recv(Socket,buffer,10000,0)) > 0){
            string statue_str;
            statue_str += buffer[9];
            statue_str += buffer[10];
            statue_str += buffer[11];
            int statue = atoi(statue_str.c_str());
            switch (statue){
                case 404:
                    closesocket(Socket);
                    WSACleanup();
                    cout << "ERROR : ";
                    cout << statue;
                    return 1;
                case 503:
                    closesocket(Socket);
                    WSACleanup();
                    cout << "ERROR : ";
                    cout << statue;
                    return 1;
                default:
                    break;
            }

            int start = 0;
            while(buffer[start] != '{' && (buffer[start] >= 32 || buffer[start] == '\n' || buffer[start] == '\r')) start++;
            int end = start + 1;
            while(buffer[end] != '}' && (buffer[end] >= 32 || buffer[end] == '\n' || buffer[end] == '\r')) end++;

            string data_str;
            string success_str;
            for(int s = start; s <= end; s++){
                data_str += buffer[s];
            }

            size_t firstProgress = data_str.find('"', data_str.find('"', data_str.find('"') + 1) + 1);
            size_t lastProgress = data_str.find('"', firstProgress + 1);

            string progress_str = data_str.substr(firstProgress+1,lastProgress-firstProgress-1);

            size_t firstTime = data_str.find('"', data_str.find('"', data_str.find('"', lastProgress + 1) + 1) + 1);
            size_t lastTime = data_str.find('"', firstTime + 1);

            string time_str = data_str.substr(firstTime+1,lastTime-firstTime-1);


            // __int64 = long long int
            __int64 time = _atoi64(time_str.c_str());
            __int64 now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

            //différence de temps entre la réception du signal sur l'android et la réception de la donnée sur le laby

            if(diff < 0){
                diff = now - time;
            } else {
                diff = (diff + now - time)/2;
            }

            //cout << diff;
            //cout << '\n';

            progress = (unsigned int)atoi(progress_str.c_str());
            //Sleep(300);
        }
    }
}