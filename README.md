# augmented-reality-opencv-qt-opengl

This project implements a classic augmented reality project.

It is done using Qt 4.5 and OpenCV 2.1.

It implements the image pre-processing (thresholding, etc) using OpenCV.
The shape (the square/diamond/trapezium) recognition is done OpenCV by finding the vertices.
The calculations needed to transpose the image in order to convert a diamond/trapezium to a square in order to analyze is done manually.
Once I get the square, the pattern recognition is done manually, not using OpenCV or any other library.
The 3D object reading and rendering is also done manually because I wanted to know how to do it by myself instead of using a library.

The source code doesn't include OpenCV 2.1 libraries so you have to import it by yourself.

The patterns are in 'patterns' folder. You can create any pattern using the 'mesh.png' in the folder as a reference. This is, you can create any pattern by filling the outer squares in the mesh with black and making any combination of black/white squares on the inner 9 squares. The pattern must have the filename set to P##.png. As you can guess, the pattern is defined by the combination of the 9 central squares.

Once you create a pattern, you must create in the 'obj' folder two files with the name Patt##.obj and Patt##.mtl associated with the pattern in the 'patterns' folder. The existing files as you can see are created with 3ds Max OBJ exporter. The code only supports vertices and faces and .mtl file only support colors.

The 'database' folder are just pictures of me testing the pattern recognition.

The code is not well commented so excuse me because it was aproject I've done many years ago when I was a child.
