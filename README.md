# README #

**n-Puzzle** is a game that's now possible to use with **QT5** (checked with QT 5.11.1). You have to install this framework to launch the application. Also you can modify the source code of the **n-Puzzle** with **Qt-Creator** or/and distribute modified versions of the software under **GPL licence**.

The **n-Puzzle** is a sliding puzzle that consists of a frame of numbered square tiles in random order with one tile missed. If the size is 3×3 tiles, the puzzle is called the 8-puzzle or 9-puzzle, and if 4×4 tiles, the puzzle is called the 15-puzzle or 16-puzzle named, respectively, for the number of tiles and the number of spaces. The object of the puzzle is to place the tiles in order by making sliding moves that use the empty space.

This implementation of a popular game using a picture (under CC0 if the source "Internet" chosen), which divides into pieces with a last piece missed.

The **n-Puzzle** consists of **Client** and **Server**. The Client is based on "Drag and Drop Puzzle Example" of Qt-framework. The **Client** can operate with one image from a local directory, a remote Server, or with random picture from the Internet. The **Server** saves pictures in a SQLite-database and provides them to **Clients**.

The last versions of the software include:

- the AI is added to solve the n-Puzzle with a couple of clicks;
- a feature to download images from the Web. It is the option by default, but a user of the software can change this behavior to load images from **Server** or from the directory on the computer.

![shot178.jpeg](https://bitbucket.org/repo/BgoK8dz/images/2884341233-shot178.jpeg)


How to make from the sources:

$ sudo apt-get install qtdeclarative5-dev

$ qmake client.pro

$ make

Then launch n-Puzzle:

$ bin/n-puzzle
