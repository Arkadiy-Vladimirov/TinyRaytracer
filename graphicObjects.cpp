#ifndef __grObj_cpp__
#define __grObj_cpp__

#include "graphicObjects.hpp"
#include "interactionModels.hpp"
#include <cmath>
#include <stdio.h>

class MediaInteractionModel;

float Camera::render_distance = 100000;
unsigned Camera::sqrt_pix_rays = 1; //1 -- no antialiasing. 2 -- satisfactory, 3 -- high
float Ray::epsilon = 0.01; 
unsigned Ray::max_recursion_depth = 5;
//unsigned DiffuseBall::dispersed_rays_number = 4;
const GraphObject* PolygonMesh::hitted_polygon = NULL;

//_______________Camera_Object_methods_____________________
Camera::Camera(Vec3f f_origin, Vec3f view_dir, double f_fov, Vec2un resolution, int a_channels, Vec2f f_mat_size) : cam_base(f_origin, view_dir), matrix(resolution.x, resolution.y, a_channels)  { //warning: channels!
    fov = f_fov;
    matrix_size = f_mat_size;

    //calculate matrix_origin, pixel_size, x_base and y_base.
    mat_base.orig = cam_base.orig + 1/tan(fov/2)*0.5*matrix_size.x * cam_base.e1 + 0.5*matrix_size.x * cam_base.e2 + 0.5*matrix_size.y * cam_base.e3;
    pixel_size.x = matrix_size.x/resolution.x;
    pixel_size.y = matrix_size.y/resolution.y;

    mat_base.e1 = pixel_size.x * (-cam_base.e2);
    mat_base.e2 = pixel_size.y * (-cam_base.e3); 
};

const Image& Camera::RenderImage(const GrObjCollection& scene) {
    fprintf(stdout, "%s\n", "Rendering started, progress:"); 
    float progress, max_progress = matrix.Width()*matrix.Height(), step = 0;
    for (int x = 0; x < matrix.Width(); ++x) {
    for (int y = 0; y < matrix.Height(); ++y) {
        RenderPixel(scene,x,y);
        step += 1; 
        float progress = 100*step/max_progress;
        fprintf(stdout, "%f", progress);
        fprintf(stdout, "%s\n", "%");
    }
    }
    fprintf(stdout, "%s\n", "Rendering finished succesfully."); 
    return matrix;
};

void Camera::RenderPixel(const GrObjCollection& scene, unsigned x, unsigned y) {
    Vec3f x_step = mat_base.e1 * (1/float(sqrt_pix_rays));
    Vec3f y_step = mat_base.e2 * (1/float(sqrt_pix_rays));
    Vec3f base = GetPixelLocation(x,y);
    Color col(0,0,0);
    float R =  1/float(sqrt_pix_rays*sqrt_pix_rays); //intensivity of a single ray

    for (int x_cnt = 0; x_cnt < sqrt_pix_rays; ++x_cnt) {
    for (int y_cnt = 0; y_cnt < sqrt_pix_rays; ++y_cnt) {
        Ray ray(cam_base.orig, (base + x_cnt*x_step + y_cnt*y_step) - cam_base.orig);
        col = col + R*ray.Cast(scene);
    }
    }
        matrix.PutPixel(x, y, Pixel{static_cast<uint8_t>(round(col.x)), static_cast<uint8_t>(round(col.y)), static_cast<uint8_t>(round(col.z)), 255});
};
//________________________________________________________


//______________GrObjCollection_object_methods______________________
GrObjCollection::~GrObjCollection() {
    for (unsigned i = 0; i < size; ++i) {
        if (data[i] != NULL)
            delete data[i];
    };
    delete[] data;
};

const GraphObject* GrObjCollection::operator[](unsigned idx) const {//to acces existing object
    if (idx < size)
        return data[idx];
    else throw "error: array index out of bounds";
};

GraphObject*& GrObjCollection::operator[](unsigned idx) {           //to create new object
    if (idx < size) {
        throw "error: non constant reference to GrObjCollection object pointer"; }
    if (idx > size) {
        throw "error: non sequential data array initialization"; }
    //if (idx == size) {
        unsigned new_size = idx+1;
        GraphObject** new_data = new GraphObject*[new_size];
        for (int i = 0; i < size; ++i)
            new_data[i] = data[i];
        for (int i = size; i < new_size; ++i)
            new_data[i] = NULL;
        delete[] data;
        data = new_data;
        size = new_size;
        return data[idx];
    //};
};
//_______________________________________________________________________


//____________________Ray_Object_methods_______________________________
Color Ray::Cast(const GrObjCollection& scene) const {
    Color resCol; //default color (ray reached maximum render distance, or maximum recursion level)
    if (recursion_depth <= max_recursion_depth) {
        Vec3f hit_point;
        const GraphObject* hitted_obj_ptr = HittedObjectPtr(scene, hit_point);
        if (hitted_obj_ptr != NULL) {
            resCol = hitted_obj_ptr->Hit(*this, hit_point, scene); 
        };
    };
    return resCol;
};

const GraphObject* Ray::HittedObjectPtr(const GrObjCollection& scene, Vec3f& hit_point) const {//probably has to substitute render distance with skySphere object hit
              hit_point = orig + Camera::GetRenderDistance() * direction; //inaccurate
    Vec3f sus_hit_point;
    const GraphObject* obj_ptr = NULL; 

    for (int i = 0; i < scene.GetSize(); ++i) {
        if (scene[i]->CheckHit(*this, sus_hit_point)) { 
            if (norm(sus_hit_point - orig) < norm(hit_point - orig)) {
                hit_point = sus_hit_point;
                obj_ptr = scene[i];
            };
        };
    };

    return obj_ptr;
};
//______________________________________________________________________


//_____________________Polygon_methods__________________________________
    Polygon::Polygon(const Vec3f* vert) {
        vertices[0] = vert[0]; vertices[1] = vert[1]; vertices[2] = vert[2];
        location.orig = vert[0];
        location.e1 = vertices[1]-location.orig;
        location.e2 = vertices[2]-location.orig;
        location.e3 = normalize(cross(location.e1,location.e2)); //outer normal
    };

    bool Polygon::CheckHit(const Ray& ray, Vec3f& hit_point) const {
        Vec3f E1 = location.e1,       E2 = location.e2;
        Vec3f O  = ray.GetOrigin(),   D = ray.GetDirection();
        Vec3f T  = O - location.orig; 
        Vec3f P = cross(D,E2),        Q = cross(T,E1);

        float det = scalar(P,E1);
        if (det < 1e-8 && det > -1e-8) {
            return false;
        }
        float inv_det = 1/det;

        float t = inv_det * scalar(Q,E2);
        float u = inv_det * scalar(P,T);
        float v = inv_det * scalar(Q,D);

        if ((t > 0) && (u >= 0) && (v >= 0) && (u+v <= 1)) {
            hit_point = (1-u-v)*vertices[0] + u*vertices[1] + v*vertices[2];
            return true;
        }

        return false;
    };

    Color Polygon::Hit(const Ray& ray, const Vec3f& hit_point, const GrObjCollection& scene) const {
        return Color();
    };
//______________________________________________________________________


//______________________Polygon_Mesh_methods____________________________
PolygonMesh::PolygonMesh(const Repere& f_loc, const MediaInteractionModel* mat, Vec3f* vert_buf, unsigned buf_size) : GraphObject(f_loc), material(mat) {
        Vec3f polygon_vert[3];
        unsigned N = buf_size/3;
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < 3; ++j) {
                polygon_vert[j] = vert_buf[3*i + j] + location.orig;
                //may add more transformations 
            }
            polygons[i] = new Polygon(polygon_vert);
        }
    };

bool PolygonMesh::CheckHit(const Ray& ray, Vec3f& hit_point) const {
    const GraphObject* hitted_polygon_ptr = ray.HittedObjectPtr(polygons, hit_point);
    if (hitted_polygon_ptr == NULL) {
        return false;
    }

    SetHittedPolygon(hitted_polygon_ptr);
    return true;
};

Color PolygonMesh::Hit(const Ray& ray, const Vec3f& hit_point, const GrObjCollection& scene) const {
    Vec3f normal = GetHittedPolygon()->GetLocation().e3;
    return material->Interact(ray, hit_point, normal, scene);
    //return GetHittedPolygon()->Hit(ray,hit_point,scene);
};
//______________________________________________________________________


//__________________________Balls'_methods______________________________
bool Ball::CheckHit(const Ray& ray, Vec3f& hit_point) const {
    //solve quadratic equation
    Vec3f a = ray.GetOrigin(), b = GetLocation().orig, l = ray.GetDirection();
    float r = GetRadius();

    float B = 2*scalar(a-b,l), C = pow(norm(a-b),2) - pow(r,2);
    float discriminant = pow(B,2) - 4*C; //A == 1
    float k;

    if (discriminant < 0) {//no intersection
        return false;
    }

    k = (-B -sqrt(discriminant)) / 2;
    if (k >= 0) {
        hit_point = a + k*l;
        return true;
    }

    k = (-B +sqrt(discriminant)) / 2;
    if (k >= 0) {
        hit_point = a + k*l;
        return true;
    }
    return false; //interection behind the ray
};

Color Ball::Hit(const Ray& ray, const Vec3f& hit_point, const GrObjCollection& scene) const {
    return material->Interact(ray, hit_point, GetNormal(hit_point), scene); 
};


/*Color RefractiveBall::Hit(const Ray& ray, const Vec3f& hit_point, const GrObjCollection& scene) const {
    //determine where are we
    float n1, n2; //refractive indices 1 - before surface, 2 - after surface.
    float alpha;  //normal angle in rad < pi/2.
    Vec3f n;      //surface normal, direction: 2->1
    Vec3f r = ray.GetDirection();
    SetConfig(r,hit_point,n,n1,n2,alpha);

    //count directions(Snell's law)
    float beta = asin(n1/n2 * sin(alpha));
    Vec3f tau = r+cos(alpha)*n; if (norm(tau) != 0) { tau = normalize(tau); } 
    Vec3f a = cos(alpha)*n   + sin(alpha)*tau;
    Vec3f b = cos(beta)*(-n) + sin(beta) *tau;
    
    //count intensivities
    //Schlick's approximation
        float R0 = pow(((n1-n2)/(n1+n2)),2);
        float R = R0 + (1-R0)*pow((1-cos(alpha)),5); //intensivity of reflected ray 

    //create rays
    Ray reflected_ray(hit_point + Ray::GetEpsilon()*a, a, ray.GetRecursionDepth() + 1);
    Ray refracted_ray(hit_point + Ray::GetEpsilon()*b, b, ray.GetRecursionDepth() + 1);
    return R*reflected_ray.Cast(scene) + (1-R)*refracted_ray.Cast(scene);
};

void RefractiveBall::SetConfig(const Vec3f& dir, const Vec3f& dot, Vec3f& normal, float& n1, float& n2, float& alpha) const {
    normal = GetNormal(dot);
    float cos_alpha = cos(normal,dir);
    if (cos_alpha <= 0) {//ray is in extetrior
        n1 = outer_refractive_index; 
        n2 = inner_refractive_index;
        //normal - o.k;
        alpha = angle(-normal,dir);
    } else {             //ray is in interior
       n1 = inner_refractive_index;
       n2 = outer_refractive_index;
       normal = -normal;
       alpha = angle(-normal,dir); 
    }
    //now we are in n1, hitting n2, normal - on us, alpha - acute normal angle.
};

Color DiffuseBall::Hit(const Ray& ray, const Vec3f& hit_point, const GrObjCollection& scene) const {
    Vec3f n = GetNormal(hit_point), r = ray.GetDirection();
    float alpha = angle(-n,r);
    Vec3f tau = r + cos(alpha) * n;
    Repere tan_bas(hit_point,n,tau,true); //e1 - normal vector, e2 - tangent vector, e3 - cross product

    Color resCol(0,0,0); //black(no light)
    unsigned N = GetDispersedRaysNumber();
    Color RS = GetReflectanceSpectrum();
    
    Vec3f disp_dirs[N]; float COS[N]; //cos beetwen normal and disp_dir[i]
    float R = GetDispersedDirections(tan_bas,alpha,disp_dirs,COS,N);

    for (int i = 0; i < N ; ++i) {
        Ray reflected_ray(tan_bas.orig + Ray::GetEpsilon()*disp_dirs[i], disp_dirs[i], ray.GetRecursionDepth() + 1);
        resCol = resCol + R*COS[i]*RS*reflected_ray.Cast(scene);
    }

    return resCol;
};

float DiffuseBall::GetDispersedDirections(const Repere& local_basis, float incident_angle, Vec3f* dir_arr, float* cos_arr, unsigned size) const {
    float phi, theta;
    float cos_sum = 0; 
    for (int i = 0; i < size; ++i) {
         phi  = float(std::rand());  phi  =  phi  / RAND_MAX * 2*PI;   // [0,pi], E(phi) = pi/2 - alpha 
        theta = float(std::rand()); theta = theta / RAND_MAX * (PI/2);   // [0,pi], E(thetha) = pi/2
        //polar transform: e1 = z, e2 = x, e3 = y;
        dir_arr[i] = sin(theta)*cos(phi)*local_basis.e2 + sin(theta)*sin(phi)*local_basis.e3 + cos(theta)*local_basis.e1;
        cos_arr[i] = cos(local_basis.e1,dir_arr[i]);
        cos_sum += cos_arr[i];
    }
    return 1/cos_sum;
}; 

Color EmittingBall::Hit(const Ray& ray, const Vec3f& hit_point, const GrObjCollection& scene) const {
    return emission;
};*/

//_____________________________________________________________________

#endif //__grObj_cpp__
