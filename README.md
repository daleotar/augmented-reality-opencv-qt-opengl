# augmented-reality-opencv-qt-opengl

This project implements a classic augmented reality project.

It is done using Qt 4.5 and OpenCV 2.1.

It implements the image pre-processing (thresholding, etc) using OpenCV.
The shape (the square/diamond/trapezium) recognition is done OpenCV by finding the vertices.
The calculations needed to transpose the image in order to convert a diamond/trapezium to a square in order to analyze is done manually.
Once I get the square, the pattern recognition is done manually, not using OpenCV or any other library.

The source code doesn't include OpenCV 2.1 libraries so you have to import it by yourself.

The code is not well commented so excuse me because it was aproject I've done many years ago when I was a child.
