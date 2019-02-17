<h1 align="center">
​    Labyritnhe
</h1>

Ce projet est codé en C++. Il permet la modélisation d'un labyrinthe en 3D à partir d'une feuille. Ce projet fonctionne uniquement sur Windows 10 avec l'environnement C++ Mingw64.

## Déploiement

### OpenCV

Mettre la bibliothèque OpenCV à la racine du disque (C:/). Prendre le OpenCV 3.4.1-x64 avec les configuration suivante :
- MinGW-x64-4.8.1-release-posix-seh-rev5
- Windows-10-64bit
- CMake-3.12.0

Cliquer sur le lien suivant : [https://github.com/huihut/OpenCV-MinGW-Build](https://github.com/huihut/OpenCV-MinGW-Build)
Il faut ajouter le dossier /bin au path.
### FreeGlut

Télécharger la bibliothèque au lien suivant : [https://www.transmissionzero.co.uk/files/software/development/GLUT/freeglut-MinGW.zip](https://www.transmissionzero.co.uk/files/software/development/GLUT/freeglut-MinGW.zip).
Il faut ajouter la dll x64 dans System32.
Il faut mettre la bibliothèque sur la racine du disque (C:/)
### Environnement C++

L'environnement C++ doit être mingw64, les config suivante :
- La version utilisé est 5.3.0
- Architecture x86_x64
- Thread Posix
- Exception seh
- Build revision 0


## Author

- **Adrien Morel** - *Initial work* - [adrienmorel](https://github.com/adrienmorel)

