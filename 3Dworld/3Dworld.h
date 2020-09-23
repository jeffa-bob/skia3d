#include <math.h>
#include <algorithm>
#include <array>
#include <future>
#include <iostream>
#include <vector>

namespace world {
// 3 floats that represent a point in 3d space
struct _3dvect {
    float x, y, z;
};
// 2 _3dvect that represent a line in 3d space
struct ray {
    _3dvect raypoint[2];
};
// a color of 3 values, red,green, and blue
struct color {
    int r, g, b;
};
// array of three _3dvect that represents a triangle in 3d space, along with color
struct tri {
    _3dvect tri[3];
    color col;
    _3dvect normal[3];
    bool normset = false;
};
// camera view
struct camera {
    int width, height;
    _3dvect pos;
    _3dvect camdir;
    float fov;
};

/*vector of tris and a sphere that is the checks if the mesh should be checked more closely to find
what triangle a ray collides with struct mesh
{
  std::vector<tri> mesh;
};
*/
// sets first color to be average of both color
void mixcolor(color& col1, color col2) {
    col1 = {col1.r + col2.r / 2, col1.g + col2.g / 2, col1.b + col2.b / 2};
};

// returns true if both _3dvect are equal to each other
bool _3dvectequal(_3dvect pos1, _3dvect pos2) {
    return pos1.x == pos2.x && pos1.y == pos2.y && pos1.z == pos2.z;
};

// returns true if both _color are equal to each other
bool colorequal(color col1, color col2) {
    return col1.r == col2.r && col1.g == col2.g && col1.b == col2.b;
}

// returns centrion of tri
_3dvect centeroftri(tri current) {
    _3dvect final;
    final.x = (current.tri[0].x + current.tri[1].x + current.tri[2].x) / 3;
    final.y = (current.tri[0].y + current.tri[1].y + current.tri[2].y) / 3;
    final.z = (current.tri[0].z + current.tri[1].z + current.tri[2].z) / 3;
    return final;
}

// changes first paramenter to the sum of the first and second parameters
void add_3dvect(_3dvect& fir, const _3dvect& sec) {
    fir.x += sec.x;
    fir.y += sec.y;
    fir.z += sec.z;
}

// changes first paramenter to the difference of the first and second parameters
void sub_3dvect(_3dvect& fir, const _3dvect sec) {
    fir.x -= sec.x;
    fir.y -= sec.y;
    fir.z -= sec.z;
}

// changes first paramenter to the product of the first and second parameters
void mul_3dvect(_3dvect& fir, const _3dvect sec) {
    fir.x *= sec.x;
    fir.y *= sec.y;
    fir.z *= sec.z;
}

// returns true if the first _3dvect is greater than the second on all axis
bool greater_3dvect(const _3dvect& fir, const _3dvect& sec) {
    return fir.x > sec.x && fir.y > sec.y && fir.z > sec.z;
}

// scales ray by unit vector of ray multiplied by magnitude; inplace
void rayscaler(ray& rays, const float& mag, const ray& unitray) {
    rays = {{{unitray.raypoint[0]},
             {unitray.raypoint[1].x * mag, unitray.raypoint[1].y * mag,
              unitray.raypoint[1].z * mag}}};
}
// return the magnitute of a ray in a float, decimal will be rounded
float magnitudeofaray(const ray& ray) {
    return (float)sqrt(
            ((ray.raypoint[0].x - ray.raypoint[1].x) * (ray.raypoint[0].x - ray.raypoint[1].x)) +
            ((ray.raypoint[0].y - ray.raypoint[1].y) * (ray.raypoint[0].y - ray.raypoint[1].y)) +
            ((ray.raypoint[0].z - ray.raypoint[1].z) * (ray.raypoint[0].z - ray.raypoint[1].z)));
}
float magnitudeofavect(const _3dvect& vect) {
    return (float)sqrt(((vect.x) * (vect.x)) + ((vect.y) * (vect.y)) + ((vect.z) * (vect.z)));
}
// gives unit vector of a ray as a ray from the first point of the ray
ray unitvectorofray(const ray& rays) {
    float magnitute = magnitudeofaray(rays);
    return {{{rays.raypoint[0]},
             {rays.raypoint[1].x / magnitute, rays.raypoint[1].y / magnitute,
              rays.raypoint[1].z / magnitute}}};
}

_3dvect unitvector_3dvect(const _3dvect& vect) {
    float magnitute = magnitudeofavect(vect);
    return {vect.x / magnitute, vect.y / magnitute, vect.z / magnitute};
}
_3dvect vectscaler(const _3dvect& vect, const float& mag) {
    _3dvect unit = unitvector_3dvect(vect);
    return {unit.x * mag, unit.y * mag, unit.z * mag};
}
// dot product of two vectors
float dotproduct(const _3dvect& n, const _3dvect& tri0) {
    return ((n.x * tri0.x) + (n.y * tri0.y) + (n.z * tri0.z));
}

// cross product of two vectors
_3dvect crossproduct(const _3dvect& U, const _3dvect& V) {
    return {((U.y * V.z) - (U.z * V.y)), ((U.z * V.x) - (U.x * V.z)), ((U.x * V.y) - (U.y * V.x))};
}

// returns a ray of the trianges normal
void trinormal(tri& curtri) {
    _3dvect U = curtri.tri[1];
    sub_3dvect(U, curtri.tri[0]);
    _3dvect V = curtri.tri[2];
    sub_3dvect(V, curtri.tri[0]);
    curtri.normal[0] = crossproduct(U, V);
    U = curtri.tri[2];
    sub_3dvect(U, curtri.tri[1]);
    V = curtri.tri[0];
    sub_3dvect(V, curtri.tri[1]);
    curtri.normal[1] = crossproduct(U, V);
    U = curtri.tri[0];
    sub_3dvect(U, curtri.tri[2]);
    V = curtri.tri[1];
    sub_3dvect(V, curtri.tri[2]);
    curtri.normal[2] = crossproduct(U, V);
    curtri.normset = true;
}

// returns true if a ray intersects a triangle
bool intersecttri(const tri& curtri, const ray& check) {
    _3dvect normal = curtri.normal[0];
    float d = dotproduct(normal, curtri.tri[0]);
    float t = ((dotproduct(normal, check.raypoint[0]) + d) / dotproduct(normal, check.raypoint[1]));
    if (t < 0) {
        return false;
    }
    _3dvect p = check.raypoint[0];
    _3dvect tvect = {t, t, t};
    mul_3dvect(tvect, check.raypoint[1]);
    add_3dvect(p, tvect);
    // std::cout << p.x << " " << p.y << " " << p.z << std::endl;

    // sub_3dvect(normal, curtri.tri[0]);
    _3dvect edge0 = curtri.tri[1];
    sub_3dvect(edge0, curtri.tri[0]);
    _3dvect C0 = p;
    sub_3dvect(C0, curtri.tri[0]);
    _3dvect c = crossproduct(edge0, C0);
    float h = dotproduct(normal, c);
    if (h < 0) return false;

    edge0 = curtri.tri[2];
    sub_3dvect(edge0, curtri.tri[1]);
    C0 = p;
    sub_3dvect(C0, curtri.tri[1]);
    c = crossproduct(edge0, C0);
    h = dotproduct(normal, c);
    if (h < 0) return false;

    edge0 = curtri.tri[0];
    sub_3dvect(edge0, curtri.tri[2]);
    C0 = p;
    sub_3dvect(C0, curtri.tri[2]);
    c = crossproduct(edge0, C0);
    h = dotproduct(normal, c);
    if (h < 0) return false;
    return true;
}
float normalizedot(const _3dvect& u, const _3dvect& v) {
    float x = dotproduct(u, v);
    return abs(x / (magnitudeofaray({{v, {0, 0, 0}}}) * (magnitudeofaray({{u, {0, 0, 0}}}))));
}

bool sort(const tri& a, const tri& b) {
    if (a.tri[0].z < b.tri[0].z) {
        return false;
    }
    if (a.tri[1].z < b.tri[1].z) {
        return false;
    }
    if (a.tri[2].z < b.tri[2].z) {
        return false;
    }
    return true;
}

// class with list of objects to be rendered onto screen; buildarray function renders scene
class currentworld {
public:
    float naer_clipping_plane = 20.0f;
    // vector of all of the meshes in the current world
    std::vector<tri> triworld = {
            {{{0, 0, 1000}, {1000, 0, 1000}, {1000, 1000, 1000}},
             {0, 255, 0},
             {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
             false},
            {{{0, 0, 1000}, {0, 1000, 1000}, {1000, 1000, 1000}},
             {0, 255, 0},
             {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
             false},
            {{{0, 0, 2000}, {1000, 0, 2000}, {1000, 1000, 2000}},
             {0, 255, 0},
             {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
             false},
            {{{0, 0, 2000}, {0, 1000, 2000}, {1000, 1000, 2000}},
             {0, 255, 0},
             {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
             false},
            {{{0, 0, 1000}, {1000, 0, 1000}, {1000, 0, 2000}},
             {0, 255, 0},
             {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
             false},
            {{{1000, 0, 1000}, {1000, 0, 2000}, {0, 0, 2000}},
             {0, 255, 0},
             {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
             false},
            {{{0, 1000, 1000}, {1000, 1000, 1000}, {1000, 1000, 2000}},
             {0, 255, 0},
             {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
             false},
            {{{1000, 1000, 1000}, {1000, 1000, 2000}, {0, 1000, 2000}},
             {0, 255, 0},
             {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
             false},
    };

    int height;
    int width;
    // the camera with width, height(in pixels), field of view, and distance that rays can go
    camera cam = {600, 600, {0, 0, 0}, {0, 0, 0}, 1.396263f};

    _3dvect lightsource = {0, 50, 400};

    float thickness;

    bool isSorted = false;

    _3dvect applycam(const _3dvect& vect) {
        _3dvect final = vect;

        final.x = (final.x * cosf(cam.camdir.y) - final.z * sinf(cam.camdir.y));
        final.z = (final.x * sinf(cam.camdir.y) + final.z * cosf(cam.camdir.y));
        // final.x = (final.x*cosf(cam.camdir.z)-final.y*sinf(cam.camdir.z));
        // final.y = (final.x*sinf(cam.camdir.z)+final.y*cosf(cam.camdir.z));
        // final.y = (final.y*cosf(cam.camdir.x)-final.z*sinf(cam.camdir.x));
        // final.z = (final.y*sinf(cam.camdir.x)+final.z*cosf(cam.camdir.x));

        sub_3dvect(final, cam.pos);
        return final;
    }

    void drawtri(const tri& rti, SkCanvas* window) {
        SkPath path;
        SkPoint* points = new SkPoint[3];
        for (int i = 0; i < 3; i++) {
            _3dvect temp = applycam(rti.tri[i]);
            points[i].fX = (naer_clipping_plane * temp.x / -temp.z) + (width / 2);
            points[i].fY = (naer_clipping_plane * temp.y / -temp.z) + (height / 2);
            // std::cout << points[i].fX << " " << points[i].fY << std::endl;
        }
        // std::cout << "Drew triangle\n";
        path.addPoly(points, 3, true);
        SkPaint p;
        p.setAntiAlias(true);
        _3dvect camdir = cam.camdir;

        sub_3dvect(camdir, cam.pos);
        _3dvect verttolight = rti.tri[0];
        sub_3dvect(verttolight, lightsource);
        _3dvect camtovert = rti.tri[0];
        sub_3dvect(camtovert, cam.pos);
        float dot0 = abs(normalizedot(camtovert, verttolight));
        // std::cout<<dot0<<std::endl;
        p.setARGB(255, rti.col.r * dot0, rti.col.g * dot0, rti.col.b * dot0);
        window->drawPath(path, p);
    }

    // builds the 2d pixel array of colors and displayes it to the screen
    void renderscreen(SkCanvas* window) {
        // std::cout << cam.pos.x << " "<< cam.pos.y << " "<< cam.pos.z << " "<< naer_clipping_plane
        // << "\n" ;
        if (!isSorted) {
            std::sort(triworld.begin(), triworld.end(), sort);
            isSorted = true;
        }
        height = window->getSurface()->height();
        width = window->getSurface()->width();
        //_3dvect adjustdir = cam.camdir.raypoint[1];
        // sub_3dvect(adjustdir, cam.pos);
        // float halfwidth = tan(cam.fov / 2);
        for (auto&& i : triworld) {
            if (!i.normset) trinormal(i);
            drawtri(i, window);
        }
    };

    void setcamangle(float curangle) {
        cam.camdir.y = sinf(curangle);
        cam.camdir.x = cosf(curangle);
        cam.camdir = unitvector_3dvect(cam.camdir);
        add_3dvect(cam.camdir, cam.pos);
    }

    // returns the color of the object the ray collides with else returns black;
    color willraycollide(const ray& rays) {
        ray increm = rays;
        for (tri curtri : triworld) {
            bool tricheck = intersecttri(curtri, rays);
            if (tricheck) {
                _3dvect adjustray = increm.raypoint[1];
                sub_3dvect(adjustray, increm.raypoint[0]);
                _3dvect adjustnorm = curtri.normal[0];
                sub_3dvect(adjustnorm, curtri.tri[0]);
                float greyfact = normalizedot(adjustray, adjustnorm);
                return {(int)(curtri.col.r * greyfact), (int)(curtri.col.g * greyfact),
                        (int)(curtri.col.b * greyfact)};
            }
        }
        return {0, 0, 0};
    }
};
};  // namespace world