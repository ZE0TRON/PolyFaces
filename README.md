# PolyFaces
Polygon Reduction Algorithm based on graphs.
Deleting the closest 2 vertices and creating a one in between to reduce the polygon number.
The update the faces accordingly.
Still under progress (Works but need to improve the face updating algorithm)

##Usage
g++ main.c -o poly
./poly <input_file> <output_file> <reduction_percentage>
Works both on linux and mac os for windows try to do the grep process manually then edit the code and run it.
