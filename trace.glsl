#ifdef VERTEX_SHADER

void main()
{
	gl_Position = gl_Vertex;
}

#else

uniform float r_width;
uniform float r_height;


uniform int r_max_rand_samples;
uniform sampler2DArray r_rand_samples;


#define R_RAND_TEXTURE_WIDTH 64
#define R_RAND_TEXTURE_HEIGHT 64
uniform sampler2D r_rand_texture;
uniform int r_rand_offset_x;
uniform int r_rand_offset_y;


uniform vec3 r_camera_position;
uniform mat3 r_camera_orientation;

uniform int r_samples;

vec3 top_color = vec3(0.5f, 0.7f, 1.0f);
vec3 zenith_color = vec3(1.0, 1.0, 1.0);

struct sphere_t
{
    vec4 center_radius;

	int material_index;
	int align0;
	int align1;
	int align2;
};

struct triangle_t
{
    /*int vertices[3];
    int material_index;*/
    ivec4 vertices_material;
};

struct vertex_t
{
    vec4 position;
    vec4 normal;
};

/*struct triangle_t
{
    vec4 verts[3];

    int material_index;
    int align0;
    int align1;
    int align2;
};*/

#define MAT_MATERIAL_TYPE_LAMBERT 1
#define MAT_MATERIAL_TYPE_DIELECTRIC 2
#define MAT_MATERIAL_TYPE_METAL 3



struct material_t
{
    vec4 base;

    int type;
    float roughness;
    float refraction;
    int align1;
};

struct ray_t
{
    vec3 origin;
    vec3 direction;
};

struct hit_result_t
{
    float t;
    vec3 attenuation;
    vec3 point;
    vec3 normal;

    int material_index;
};

#define MAX_SPHERE_COUNT 16
int sphere_count;
sphere_t spheres[MAX_SPHERE_COUNT];



#define R_MAX_SPHERES 64
uniform int r_sphere_count;
uniform layout(std140) r_spheres_uniform_block
{
    sphere_t r_spheres[R_MAX_SPHERES];
};



#define MAT_MAX_MATERIALS 32
uniform layout(std140) r_material_uniform_block
{
    material_t r_materials[MAT_MAX_MATERIALS];
};



#define R_MAX_TRIANGLES 128
uniform int r_triangle_count;
uniform layout(std140) r_triangle_uniform_block
{
    triangle_t r_triangles[R_MAX_TRIANGLES];
};


#define R_MAX_VERTICES 512
uniform layout(std140) r_vertex_uniform_block
{
    vertex_t r_vertices[R_MAX_VERTICES];
};


int rand_sample_index;

float rand_float()
{

    if(rand_sample_index >= r_max_rand_samples)
    {
        rand_sample_index = 0;
    }

    float s = texture(r_rand_samples, vec3(gl_FragCoord.x / float(R_RAND_TEXTURE_WIDTH), gl_FragCoord.y / float(R_RAND_TEXTURE_HEIGHT), float(rand_sample_index))).r;
    rand_sample_index++;
    return s;
}


/*float rand_float2()
{
    return texture(r_rand_texture, vec2((gl_FragCoord.x + r_rand_offset_x) / float(R_RAND_TEXTURE_WIDTH), (gl_FragCoord.y + r_rand_offset_y) / float(R_RAND_TEXTURE_HEIGHT))).r;
}*/

vec3 rand_point_on_sphere()
{
    vec3 p = vec3(rand_float() * 2.0 - 1.0, rand_float() * 2.0 - 1.0, rand_float() * 2.0 - 1.0);
    return normalize(p);
}


/*
=========================================
=========================================
=========================================
*/

void scatter_lambert(ray_t ray_in, hit_result_t hit_result, out vec3 attenuation, out ray_t scattered)
{
    int material_index = hit_result.material_index;
    vec3 target = hit_result.point + hit_result.normal + rand_point_on_sphere();

    scattered.origin = hit_result.point;
    scattered.direction = target - scattered.origin;
    attenuation = r_materials[material_index].base.rgb * 0.5;
}

void scatter_metal(ray_t ray_in, hit_result_t hit_result, out vec3 attenuation, out ray_t scattered)
{
    int material_index = hit_result.material_index;
    float roughness = r_materials[material_index].roughness;

    //vec3 target = hit_result.point + hit_result.normal + rand_point_on_sphere() * roughness;
    vec3 target = hit_result.point + reflect(ray_in.direction, hit_result.normal) + rand_point_on_sphere() * roughness;

    scattered.origin = hit_result.point;
    scattered.direction = target - scattered.origin;
    attenuation = vec3(1.0);
}

void scatter_dielectric(ray_t ray_in, hit_result_t hit_result, out vec3 attenuation, out ray_t scattered)
{
    int material_index = hit_result.material_index;
    float refraction = r_materials[material_index].refraction;
    vec3 outward_normal;
    vec3 refracted;
    float ni_over_nt;

    if(dot(ray_in.direction, hit_result.normal) > 0.0)
    {
        /* ray and normal point in the same direction, which
        means it's exiting the dielectric material... */

        outward_normal = -hit_result.normal;
        ni_over_nt = refraction;
    }
    else
    {
        /* the ray is entering the object... */

        outward_normal = hit_result.normal;
        ni_over_nt = 1.0 / refraction;
    }

    refracted = refract(ray_in.direction, outward_normal, ni_over_nt);

    //if(dot(refracted, refracted) == 0.0)
    //{
    //    scattered.direction = reflect(ray_in.direction, hit_result.normal);
   // }
   // else
    {
        scattered.direction = refracted;
    }

    scattered.origin = hit_result.point;
    attenuation = vec3(1.0);
}


void scatter(ray_t ray_in, hit_result_t hit_result, out vec3 attenuation, out ray_t scattered)
{
    switch(r_materials[hit_result.material_index].type)
    {
        case MAT_MATERIAL_TYPE_LAMBERT:
            scatter_lambert(ray_in, hit_result, attenuation, scattered);
        break;

        case MAT_MATERIAL_TYPE_METAL:
            scatter_metal(ray_in, hit_result, attenuation, scattered);
        break;

        case MAT_MATERIAL_TYPE_DIELECTRIC:
            scatter_dielectric(ray_in, hit_result, attenuation, scattered);
        break;
    }
}


/*
=========================================
=========================================
=========================================
*/

#define T_MAX 1000.0
#define T_MIN 0.001
#define MAX_BOUNCES 8

bool intersect_sphere(ray_t ray, out hit_result_t result, in sphere_t sphere, float t_min, float t_max)
{
    vec3 oc = ray.origin - sphere.center_radius.xyz;
    float a = dot(ray.direction, ray.direction);
    float b = dot(oc, ray.direction);
    float c = dot(oc, oc) - sphere.center_radius.w * sphere.center_radius.w;
    float d = b * b - a * c;

    float s;

    if(d > 0.0)
	{
		s = sqrt(d);

		float temp = (-b - s) / a;

		if(temp < t_max && temp > t_min)
		{
            result.t = temp;
            result.point = ray.origin + ray.direction * temp;
			result.normal = (result.point - sphere.center_radius.xyz) / sphere.center_radius.w;
			result.material_index = sphere.material_index;
			return true;
		}

		temp = (-b + s) / a;

		if(temp < t_max && temp > t_min)
		{
            result.t = temp;
            result.point = ray.origin + ray.direction * temp;
			result.normal = (result.point - sphere.center_radius.xyz) / sphere.center_radius.w;
			result.material_index = sphere.material_index;
			return true;
		}

	}

	return false;
}


bool intersect_triangle(ray_t ray, out hit_result_t result, triangle_t triangle, float t_min, float t_max)
{
    vec3 normal;
    vec3 direction;
    float direction_len;
    vec3 point;
    vec3 e0;
    vec3 e1;
    vec3 e2;
    vec3 v0;

    float t;
    float d;

    float a0;
    float a1;
    float a2;
    float at;

    float u;
    float v;
    float w;


    e0 = r_vertices[triangle.vertices_material[1]].position.xyz - r_vertices[triangle.vertices_material[0]].position.xyz;
    e1 = r_vertices[triangle.vertices_material[2]].position.xyz - r_vertices[triangle.vertices_material[1]].position.xyz;
    normal = r_vertices[triangle.vertices_material[0]].normal.xyz;
    //normal = normalize(cross(e0, e1));

    d = dot(ray.direction, normal);

    if(d != 0.0)
    {
        //direction_len = length(ray.direction);

        v0 = r_vertices[triangle.vertices_material[0]].position.xyz - ray.origin;

        t = dot(normal, v0) / d;

        if(t >= t_min && t <= t_max)
        {
            /* ray hits the place that contains the triangle... */
            point = ray.origin + ray.direction * t;
            //e2 = triangle.verts[0].xyz - triangle.verts[2].xyz;

            a0 = dot(normal, cross(e0, point - r_vertices[triangle.vertices_material[1]].position.xyz));
            a1 = dot(normal, cross(e1, point - r_vertices[triangle.vertices_material[2]].position.xyz));
            //a2 = dot(normal, cross(e2, point - triangle.verts[0].xyz)) * 0.5;
            at = dot(normal, cross(e0, e1));

            u = a0 / at;
            v = a1 / at;
            w = 1.0 - u - v;
            //w = a2 / at;

            if(u >= 0.0 && u <= 1.0)
            {
                if(v >= 0.0 && v <= 1.0)
                {
                    if(w >= 0.0 && w <= 1.0)
                    {
                        result.t = t;
                        result.point = point;
                        result.normal = normal;
                        result.material_index = triangle.vertices_material[3];
                        return true;
                    }
                }
            }
        }
    }

    return false;
}



bool intersect_world(ray_t ray, out hit_result_t result, float t_min, float t_max)
{
    int i;

    bool hit = false;

    hit_result_t temp_result;

    temp_result.t = T_MAX;
    result.t = T_MAX;

   /* for(i = 0; i < r_sphere_count; i++)
    {
        if(intersect_sphere(ray, temp_result, r_spheres[i], t_min, t_max))
        {
            if(temp_result.t < result.t)
            {
                result = temp_result;
                hit = true;
            }
        }
    }*/

    for(i = 0; i < r_triangle_count; i++)
    {
        if(intersect_triangle(ray, temp_result, r_triangles[i], t_min, t_max))
        {
            if(temp_result.t < result.t)
            {
                result = temp_result;
                hit = true;
            }
        }
    }

    return hit;
}


/*
=========================================
=========================================
=========================================
*/





hit_result_t results[MAX_BOUNCES + 1];
ray_t rays[MAX_BOUNCES + 1];

vec3 ray_color(ray_t ray)
{
    hit_result_t result;
    hit_result_t temp_result;

    vec3 color = vec3(1.0);
    vec3 accum = vec3(0.0);

    int i;
    int bounces = 0;

    bool hit = false;

    float t;

    rays[0] = ray;

    for(bounces = 0; bounces < MAX_BOUNCES; bounces++)
    {
        hit = intersect_world(rays[bounces], results[bounces], T_MIN, T_MAX);

        if(!hit)
        {
            vec3 dir = normalize(rays[bounces].direction);
            t = 0.5 * (dir[1] + 1.0);
            results[bounces].attenuation = top_color * t + zenith_color * (1.0 - t);
            bounces++;
            break;
        }

        scatter(rays[bounces], results[bounces], results[bounces].attenuation, rays[bounces + 1]);

    }

    if(bounces > 0)
    {
        for(bounces--; bounces >= 0; bounces--)
        {
            color = color * results[bounces].attenuation;
        }

        return color;
    }

    return vec3(0.0);

    //return
}

#define SAMPLES 5

void main()
{
    vec2 uv = vec2(gl_FragCoord.x / r_width, gl_FragCoord.y / r_height);


    vec3 right = r_camera_orientation[0] * r_width * 0.001;
    vec3 up = r_camera_orientation[1] * r_height * 0.001;
    vec3 look_at = r_camera_orientation[2];
    vec3 llc = -right * 0.5 - up * 0.5;


    /*vec3 right = vec3(r_width * 0.001, 0.0, 0.0);
    vec3 up = vec3(0.0, r_height * 0.001, 0.0);
    vec3 llc = vec3(-r_width * 0.5 * 0.001, -r_height * 0.5 * 0.001, 0.0);
    vec3 look_at = vec3(0.0, 0.0, -1.0);*/


    vec3 dir = llc + up * uv.y + right * uv.x - look_at;
    vec3 color = vec3(0.0);

    ray_t ray;

    int i;

    ray.origin = r_camera_position;

    for(i = 0; i < r_samples; i++)
    {
        ray.direction = dir + vec3(rand_float() * 2.0 - 1.0, rand_float() * 2.0 - 1.0, 0.0) * 0.0005;
        color += ray_color(ray);
    }

    color /= r_samples;

    gl_FragColor = vec4(color, 1.0);
}


#endif











