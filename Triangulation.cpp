/*
*                                   2D TRIANGULATION
AUTHOR: SAMUEL ONABOLU
STUDENT NUMBER: SAMUEL ONABOLU - 6783369
*/
#include <unordered_set>

#include <stdlib.h>

#include <stdio.h>

#include <windows.h>

#include <malloc.h>

#include <freeglut.h>

#include <FreeImage.h>

#include <set>

#include <cstdlib>

#include <iostream>

#include <ctime>

#include <vector>

#include <algorithm>

using namespace std;
int windowHeight = 1000;
int windowWidth = 1000;

// Number of points
int N = 10;

// Creates points structure.
struct Points {
    int x;
    int y;
    bool operator == (const Points& p) const {
        return x == p.x && y == p.y;
    }
};
// Creates points structure.
struct threeDPoints {
    int x;
    int y;
    double z;
    bool operator == (const Points& p) const {
        return x == p.x && y == p.y;
    }
};

enum { MENU_POINTS, MENU_LATTICE, MENU_QUIT };

// Creates Triangle structure.
struct Triangles {
    Points p1, p2, p3;
};

// edge list to hold x and y pairs.
vector < pair < int, int > > edge_list;

// Vector to hold edge lengths that we will ned to sort
vector < int > edge_length;

// edge list to triangle edges
vector < pair < int, int > > tri_edge;

// store the triangles
vector < Triangles > triangles;

// dynamic data structure to store the points
vector < Points > myPoints;

// dynamic data structure to store the 3d points
vector < threeDPoints > myThreeDPoints;


void generatePoints();
int calculateDistance(Points p0, Points p1);
bool intersect(int xa, int ya, int xb, int yb, int xc, int yc, int xd, int yd);
void addEdge();
void makeTriangle();
bool colinear(int x1, int y1, int x2, int y2, int x3, int y3);
int distanceFromPointToLine(Points p0, Points p1, Points p2);
void cleanup();
bool commonEdge(Triangles t, Triangles f, vector < Points >& points);

/*This function generates n amount of unique points. I ensure the points are unique by using the count function and a boolean flag.
If the point generated is not unique, i set the boolean flag to false, and do not add it. If the flag is true, I add the point to my
points vector.I continue to add elements untill the size is equal to the amount of poins n.*/
void generatePoints() {
    srand((int)time(0));
    bool unique = true;
    Points p;
    while (myPoints.size() != N) {
        int x = rand() % windowWidth + 1;
        int y = rand() % windowHeight + 1;
        p.x = x;
        p.y = y;
        // if the count is 1, we will set unique flag to false and not add the point to our vector.
        if (count(myPoints.begin(), myPoints.end(), p)) {
            unique = false;
        }
        if (unique) myPoints.push_back(p);
        else {
            unique = true;
        };

    }
}
/* Generate as nxn lattice of points. I use x_inc and y_inc to map my x and y coordiantes to the grid.*/
void generateLattice() {
    int w, h;
    w = 10;
    h = 10;
    // Sets the distance between the points
    int x_inc = windowWidth / (double)w;
    int y_inc = windowHeight / (double)h;
    // Initiliazes the nxn lattice.
    N = w * h;
    Points p;
    for (int i = 1; i < windowWidth; i += x_inc) {

        for (int j = 1; j < windowHeight; j += y_inc) {

            p.x = i;
            p.y = j;
            myPoints.push_back(p);
        }
    }
}
// Compare if 2 points are equal by checking if their x and y values of the 2 points are the same. Return true if equal, false otherwise.
bool pointsEqual(const Points& a,
    const Points& b) {
    return a.x == b.x && a.y == b.y;
}
/*I use this method to check if the two triangles share a common edge. I take point 1 of triangle 1 and compare its x and y values of each point
 in triangle 2. If found increment the member variable and push the point into our points vector. . I then go to point 2 of triangle 2 and compare
 its x and y values to each point in triangle 2. If found increment the member variable, and push the point into our points vector. The same is done
 for point 3 of triangle 1. At the end of the method I will check if member is greater than 2. If so, I now have 2 points that our shared. For instance,
 Lets take 2 triangles for example defined as Triangle 1 {p1,p3,p2}, Triangle 2{p1,p3,p4}. At the end of the if statements, It would have pushed Points
 p1, and p3  which is saved as index 0, and index 1. Member is greater than 2 here, now we must check for the 2 points that are unique. I do this by using the count
 function and passing in each point in Triangle 1, and Triangle 2. If the point is not inside, then it is 1 of the unique Points. In this example,
 It would find p2, and p4. I then store these in a temporary vector points. These 2 points will be stored at index index 2, and index 3. If member
 is less than 1 we know it does not share an edge.

 Consider our previous example,  Triangle 1 {p1,p3,p2}, Triangle 2{p1,p3,p4} the vector would store indicies as such:
    /*position 0 = shared point 1 (p1)
    position 1 =  shared point 2  (P3)
    position 2 = unique point 3	  (P2)
    poisiton 3 = unique point 4   (P4)
    */
bool commonEdge(Triangles tri1, Triangles tri2, vector < Points >& points) {
    int member = 0;

    //find which points are shared
    if (pointsEqual(tri1.p1, tri2.p1) || pointsEqual(tri1.p1, tri2.p2) || pointsEqual(tri1.p1, tri2.p3)) {

        points.push_back(tri1.p1);
        member++;
    }
    if (pointsEqual(tri1.p2, tri2.p1) || pointsEqual(tri1.p2, tri2.p2) || pointsEqual(tri1.p2, tri2.p3)) {

        points.push_back(tri1.p2);
        member++;
    }
    if (pointsEqual(tri1.p3, tri2.p1) || pointsEqual(tri1.p3, tri2.p2) || pointsEqual(tri1.p3, tri2.p3)) {

        points.push_back(tri1.p3);
        member++;
    }
    // take each point from our 2 triangls and see if they are in the vector, if not then they are the unique points

    if (member > 1) {

        if (!count(points.begin(), points.end(), tri1.p1)) points.push_back(tri1.p1);
        if (!count(points.begin(), points.end(), tri1.p2)) points.push_back(tri1.p2);
        if (!count(points.begin(), points.end(), tri1.p3)) points.push_back(tri1.p3);
        if (!count(points.begin(), points.end(), tri2.p1)) points.push_back(tri2.p1);
        if (!count(points.begin(), points.end(), tri2.p2)) points.push_back(tri2.p2);
        if (!count(points.begin(), points.end(), tri2.p3)) points.push_back(tri2.p3);
        return true;
    }

    return false;
}
/* This method loops triangles vector, and check if each triangle shares an edge using our common edge method. If so, set our boolean flag
shared to true, and apply the triangulation clean up algorithm. Such as calculating the distance of p1,p3, and distance of p2 and p4
as per the example on the notes.*/
void cleanup() {

    Triangles tri1;
    Triangles tri2;
    int a = 0;
    int b = 0;
    vector < Points > temp;
    bool found = false;

    for (int i = 0; i < triangles.size(); i++) {

        tri1 = triangles[i];
        for (int j = i + 1; j < triangles.size(); j++) {
            bool shared = false;

            tri2 = triangles[j];
            if (commonEdge(tri1, tri2, temp)) {
                shared = true;
                // save indicies of the triangles that need to be cleaned.
                int a = i;
                int b = j;
            }

            if (shared) {

                //calculate distance between shared edge
                int distance1 = calculateDistance(temp[0], temp[1]);
                //calculate distance between unique edge
                int distance2 = calculateDistance(temp[2], temp[3]);

                // calculate distnace of the 2 shared points from the edge that is not shared
                int sign1 = distanceFromPointToLine(temp[0], temp[2], temp[3]);
                int sign2 = distanceFromPointToLine(temp[1], temp[2], temp[3]);
                bool opposite = false;
                //Check for opposite signs, if so set opposite to true, Else it will stay as false.
                if ((sign1 < 0 && sign2 > 0) || (sign1 > 0 && sign2 < 0)) opposite = true;
                /* If opposite signsand absolute val of distance 2, is less than absolute val of distance 1, and signs are opposite,
                perform clean up*/
                if (abs(distance2) < abs(distance1) && opposite) {
                    cout << "Cleaning" << endl;
                    /*We need to remove the second triangle first.Consider a vector as such : {1, 2, 3, 4}. Asssume we want to clean triangles
                    2 and 3 at index 1 and 2. If we remove triangle 2 and index 1 first, our array will now look like this {1, 3, 4}, and
                    triangle 3 is now at index 1 which is incorrect. Thus, we should remove triangle 2 first. If we remove triangle 2 first
                    our vector will look as such {1, 2 , 4}. Our triangle 2 that needs to be removed will still be at index 1, and then we
                    will be pointing to the right triangle thats needs to be removed.
                    */
                    triangles.erase(triangles.begin() + b); // remove triangle 2 first
                    triangles.erase(triangles.begin() + a); // remove triangle 1 next

                    Triangles newTri1, newTri2;
                    newTri1.p1 = temp[0];
                    newTri1.p2 = temp[2];
                    newTri1.p3 = temp[3];

                    newTri2.p1 = temp[2];
                    newTri2.p2 = temp[3];
                    newTri2.p3 = temp[1];
                    triangles.push_back(newTri1);
                    triangles.push_back(newTri2);


                }

            }

        }

    }
}
// This method calculates the distance from point to a line. I removed the absolute value so i can see which side the point is on from the line.
int distanceFromPointToLine(Points p0, Points p1, Points p2) {
    int a, b, c;
    int distance;
    a = p2.y - p1.y;
    b = p1.x - p2.x;
    c = (p2.x * p1.y) - (p1.x * p2.y);
    distance = (a * p0.x) + (b * p0.y) + c;
    return distance;
}

//check if the 2 line segments intersect.
bool intersect(int xa, int ya, int xb, int yb, int xc, int yc, int xd, int yd) {
    double ta, tb;
    int D = ((xb - xa) * (yd - yc)) - ((yb - ya) * (xd - xc));
    if (D == 0) {
        return false; // lines are parallel
    }

    ta = ((xc - xa) * (yd - yc) - (yc - ya) * (xd - xc)) / (double)D;

    if (xd - xd == 0)
        tb = (xa - xc + (xb - xa) * ta) / (double)(xd - xc);

    else
        tb = (xa - xc + (xb - xa) * ta) / (double)(xd - xc);

    if ((ta > 0 && ta < 1) && (tb > 0 && tb < 1)) {
        return true; // 2 lines intersect
    }
    else {
        return false;
    }
}

// This method calculates distance from 2 points
int calculateDistance(Points p1, Points p2) {
    int d = sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
    return d;
}

// Generate edge list, and my tri edge list. I test for intersection in my tri edge list before inserting the edge.
void addEdge() {

    for (int i = 0; i < N; i++) {
        for (int j = i + 1; j < N; j++) {
            edge_list.push_back(make_pair(i, j));
        }
    }

    // Sort the edge list in ascending order based on distance.
    sort(edge_list.begin(), edge_list.end(), [&](pair < int, int > p1, pair < int, int > p2) {
        return calculateDistance(myPoints[p1.first], myPoints[p1.second]) < calculateDistance(myPoints[p2.first], myPoints[p2.second]);
        });

    // initialize tri_edge with first point in edge_list
    tri_edge.push_back(make_pair(edge_list[0].first, edge_list[0].second));

    /*Iterate over each edge in the edge_listand add it to tri_edge if it does not intersect with any edge in tri_edge
    * x0 = x coord of point 1 in our edge list, y0 = y coord of point 1 in our edge list, x1 = x coord of point2 in our edge list
    y1 = y coord of point 2 in our edge list.
    */

    for (int i = 1; i < edge_list.size(); i++) {
        int x0 = myPoints[edge_list[i].first].x;
        int y0 = myPoints[edge_list[i].first].y;
        int x1 = myPoints[edge_list[i].second].x;
        int y1 = myPoints[edge_list[i].second].y;

        /*x2 = x coord of point 1 in our tri_edge list, y2 = y coord of point 1 in our tri_edge list, x3 = x coord of point 2 in our tri_edge list
        y3 = y coord of point 2 in our tri_edge list*/

        bool intersects = false;
        for (int j = 0; j < tri_edge.size(); j++) {
            int x2 = myPoints[tri_edge[j].first].x;
            int y2 = myPoints[tri_edge[j].first].y;
            int x3 = myPoints[tri_edge[j].second].x;
            int y3 = myPoints[tri_edge[j].second].y;

            if (intersect(x0, y0, x1, y1, x2, y2, x3, y3)) {
                intersects = true;
                break;
            }
        }

        if (!intersects) {
            tri_edge.push_back(edge_list[i]);
        }
    }

    // Sort the edge list in ascending order based on distance
    sort(tri_edge.begin(), tri_edge.end(), [&](pair < int, int > p1, pair < int, int > p2) {
        return calculateDistance(myPoints[p1.first], myPoints[p1.second]) <
        calculateDistance(myPoints[p2.first], myPoints[p2.second]);
        });

}

// Check if triangle is colinear using area of a triangle formula. x1, y1 corresponds to p1. x2, y2 corresponds to p2. x3, y3 corresponds to p3.
bool colinear(int x1, int y1, int x2, int y2, int x3, int y3) {
    int area = abs((x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2)) / 2.0);
    if (area == 0) return true;
    else return false;

}

// Method to check if the triangle im about to insert is already in the triangles structure.
bool duplicate(Triangles tri, vector < Triangles > triangles) {

    for (int i = 0; i < triangles.size(); i++) {
        int count = 0;
        if (pointsEqual(tri.p1, triangles[i].p1) || pointsEqual(tri.p1, triangles[i].p2) || pointsEqual(tri.p1, triangles[i].p3)) {
            count++;
        }
        if (pointsEqual(tri.p2, triangles[i].p1) || pointsEqual(tri.p2, triangles[i].p2) || pointsEqual(tri.p2, triangles[i].p3)) {
            count++;
        }
        if (pointsEqual(tri.p3, triangles[i].p1) || pointsEqual(tri.p3, triangles[i].p2) || pointsEqual(tri.p3, triangles[i].p3)) {
            count++;
        }

        if (count == 3) return true;
    }
    return false;
}

/* Extracts the triangles from the tri edge list.*/
void makeTriangle() {
    Triangles tri;

    for (int i = 0; i < tri_edge.size(); i++) {
        int ind = 0;

        // Pa, and Pb are 2 points of our 1st edge.
        int Pa = tri_edge[i].first;
        int Pb = tri_edge[i].second;

        // boolean to check when we found a point c to find another point d
        bool findD = false;

        for (int c = 0; c < tri_edge.size(); c++) {
            int Pc = c;
            // search for shared edges and special cases
            pair < int, int > p1 = {
              Pb,
              Pc
            };
            pair < int, int > p2 = {
              Pc,
              Pa
            };
            pair < int, int > p3 = {
              Pc,
              Pb
            };
            pair < int, int > p4 = {
              Pa,
              Pc
            };

            // Increment the member once a edge is found
            int member = 0;
            if (int count_p1 = count(tri_edge.begin(), tri_edge.end(), p1)) {
                member++;
            }
            if (int count_p2 = count(tri_edge.begin(), tri_edge.end(), p2)) {
                member++;
            }
            if (int count_p3 = count(tri_edge.begin(), tri_edge.end(), p3)) {
                member++;
            }
            if (int count_p4 = count(tri_edge.begin(), tri_edge.end(), p4)) {
                member++;
            }

            // If member greater 1 we push into our tri edge as our 1st triangle, and set find point d flag to true to find point d.
            if (member > 1) {
                tri.p1.x = myPoints[Pa].x;
                tri.p1.y = myPoints[Pa].y;
                tri.p2.x = myPoints[Pb].x;
                tri.p2.y = myPoints[Pb].y;
                tri.p3.x = myPoints[p1.second].x;
                tri.p3.y = myPoints[p2.first].y;
                // check if triangle is not already in our triangle vector.
                if (!duplicate(tri, triangles)) {
                    triangles.push_back(tri);
                    findD = true;

                    break;
                }
                else continue;
            }
        }
        // if find D flag is false go to next iteration of finding point c.
        if (!findD) continue;

        // For loop for finding point D, using same logic as for finding point c.
        for (int d = 0; d < tri_edge.size(); d++) {

            bool add = true;
            int Pd = d;
            pair < int, int > p1 = {
              Pb,
              Pd
            };
            pair < int, int > p2 = {
              Pd,
              Pa
            };
            pair < int, int > p3 = {
              Pd,
              Pb
            };
            pair < int, int > p4 = {
              Pa,
              Pd
            };
            int member = 0;

            if (int count_p1 = count(tri_edge.begin(), tri_edge.end(), p1)) {
                member++;
            }
            if (int count_p2 = count(tri_edge.begin(), tri_edge.end(), p2)) {
                member++;
            }
            if (int count_p3 = count(tri_edge.begin(), tri_edge.end(), p3)) {
                member++;
            }
            if (int count_p4 = count(tri_edge.begin(), tri_edge.end(), p4)) {
                member++;
            }

            if (member > 1) {

                tri.p1.x = myPoints[Pa].x;
                tri.p1.y = myPoints[Pa].y;
                tri.p2.x = myPoints[Pb].x;
                tri.p2.y = myPoints[Pb].y;
                tri.p3.x = myPoints[p1.second].x;
                tri.p3.y = myPoints[p2.first].y;
                // check if triangle is not already in our triangle vector.
                if (!duplicate(tri, triangles)) {
                    triangles.push_back(tri);
                    findD = false;
                    break;
                }
                else continue;

            }
        }
    }

}

void threeDimensional() {
    double z;
    for (int i = 0; i < triangles.size(); i++) {
        threeDPoints p;
        z = sin(triangles[i].p1.x * triangles[i].p1.y) * cos(triangles[i].p1.x - triangles[i].p1.y);
        z = sin(triangles[i].p2.x * triangles[i].p2.y) * cos(triangles[i].p2.x - triangles[i].p2.y);
        z = sin(triangles[i].p3.x * triangles[i].p3.y) * cos(triangles[i].p3.x - triangles[i].p3.y);
        p.x = myPoints[i].x;
        p.y = myPoints[i].y;
        p.z = z;
        myThreeDPoints.push_back(p);
    }
}

void display() {

    glClear(GL_COLOR_BUFFER_BIT);
    glPointSize(5);

    // Render points
    glBegin(GL_POINTS);
    for (int i = 0; i < myPoints.size(); i++) {

        glVertex2f(myPoints[i].x, myPoints[i].y);

    }
    glEnd();

    //Render triangles

    srand((int)time(0));
    cout << "triangle size: " << triangles.size();
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < triangles.size(); i++) {

        glColor3f((rand() % 255) / 255.0, (rand() % 255) / 255.0, (rand() % 255) / 255.0);
        glVertex2f(triangles[i].p1.x, triangles[i].p1.y);
        glVertex2f(triangles[i].p2.x, triangles[i].p2.y);
        glVertex2f(triangles[i].p3.x, triangles[i].p3.y);

    }

    glEnd();

    // Render edges

    glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 1.0);
    for (int i = 0; i < tri_edge.size(); i++) {
        glVertex2f(myPoints[tri_edge[i].first].x, myPoints[tri_edge[i].first].y);
        glVertex2f(myPoints[tri_edge[i].second].x, myPoints[tri_edge[i].second].y);
    }
    glEnd();
    glFlush();

}
void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 0x1B:
    case'q':
    case 'Q':
        exit(0);
        break;
    case'l':
    case'L':
        generateLattice();
        addEdge();
        makeTriangle();
        cleanup();
        glutDisplayFunc(display);
        break;
    case 'p':
    case 'P':
        glClear(GL_COLOR_BUFFER_BIT);
        generatePoints();
        addEdge();
        makeTriangle();
        cleanup();
        glutDisplayFunc(display);
        break;
    }
}
int main(int argc, char** argv) {

    int numOfPoints = 0;
    char input = ' ';
    glutInit(&argc, argv);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);

    cout << " L: LATTICE MODE \n P : RANDOM POINTS MODE \n Q : Quit \n";
    cin >> input;
    if (input == 'q' || input == 'Q') return (0);
    cout << "How many points? \n";
    cin >> numOfPoints;
    cout << "Loading..." << endl;
    N = numOfPoints;

    if (input == 'p' || input == 'P') {
        generatePoints();
        addEdge();
        makeTriangle();
        cleanup();
    }
    else if (input == 'l' || input == 'L')
    {
        generateLattice();
        addEdge();
        makeTriangle();
        cleanup();
    }
    else if (input == 'q' || input == 'Q')
    {
        return(0);
    }

    glutCreateWindow("2D TRIANGULATION DISPLAY");
    glutDisplayFunc(display);
    // Print the sorted tri edge list
    cout << "# of edges created : " << tri_edge.size() << endl;
    cout << "The triangle size is : " << triangles.size() << endl;
    glShadeModel(GL_SMOOTH);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0, windowWidth, 0, windowHeight);
    glMatrixMode(GL_MODELVIEW);
    glutMainLoop();

    return 0;
}