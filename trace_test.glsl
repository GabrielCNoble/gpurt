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


uniform vec3 r_camera_position;
uniform mat3 r_camera_orientation;

uniform int r_samples;

//vec3 right = vec3(r_width * 0.001, 0.0, 0.0);
//vec3 up = vec3(0.0, r_height * 0.001, 0.0);
//vec3 llc = vec3(-r_width * 0.5 * 0.001, -r_height * 0.5 * 0.001, 0.0);
//vec3 look_at = vec3(0.0, 0.0, -1.0);

vec3 top_color = vec3(0.5f, 0.7f, 1.0f);
vec3 zenith_color = vec3(1.0, 1.0, 1.0);

struct sphere_t
{
    vec4 center_radius;
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
};

#define MAX_SPHERE_COUNT 16
int sphere_count;
sphere_t spheres[MAX_SPHERE_COUNT];



#define R_MAX_SPHERES 64
uniform int r_sphere_count;
uniform layout(std140) r_spheres_uniform_buffer
{
    sphere_t r_spheres[R_MAX_SPHERES];
};


int rand_sample_index;

float rand_float()
{
    //float sample = 0.0;

    if(rand_sample_index >= r_max_rand_samples)
    {
        rand_sample_index = 0;
    }

    float s = texture(r_rand_samples, vec3(gl_FragCoord.x / r_width, gl_FragCoord.y / r_height, float(rand_sample_index))).r;
    rand_sample_index++;
    return s;

	//return 0.0;
}

vec3 rand_point_on_sphere()
{
    vec3 p = vec3(rand_float() * 2.0 - 1.0, rand_float() * 2.0 - 1.0, rand_float() * 2.0 - 1.0);
    return normalize(p);
}


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
			//result.material = m_material;
			return true;
		}

		temp = (-b + s) / a;

		if(temp < t_max && temp > t_min)
		{
            result.t = temp;
            result.point = ray.origin + ray.direction * temp;
			result.normal = (result.point - sphere.center_radius.xyz) / sphere.center_radius.w;
			//result.material = m_material;
			return true;
		}

	}

	return false;
}


#define T_MAX 1000.0
#define T_MIN 0.0001
#define MAX_BOUNCES 8

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

    //float t = 0.5 * (ray.direction[1] + 1.0);

    float t;

    rays[0] = ray;

    for(bounces = 0; bounces < MAX_BOUNCES; bounces++)
    {
        results[bounces].t = T_MAX;
        //results[bounces].attenuation = vec3(1.0);

        hit = false;

        for(i = 0; i < r_sphere_count; i++)
        {
            if(intersect_sphere(rays[bounces], temp_result, r_spheres[i], T_MIN, T_MAX))
            {
                if(temp_result.t < results[bounces].t)
                {
                    results[bounces] = temp_result;
                    results[bounces].attenuation = vec3(0.5);
                    hit = true;
                }
            }
        }

        if(!hit)
        {
            vec3 dir = normalize(rays[bounces].direction);
            t = 0.5 * (dir[1] + 1.0);
            results[bounces].attenuation = top_color * t + zenith_color * (1.0 - t);
            bounces++;
            break;
        }

        rays[bounces + 1].origin = results[bounces].point;
        vec3 target = results[bounces].point + results[bounces].normal + rand_point_on_sphere();
        rays[bounces + 1].direction = target - results[bounces].point;
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

#define SAMPLES 8

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

    /*sphere_count = 0;
    spheres[0].center_radius = vec4(0.0, -0.5, -8.0, 1.0);
    spheres[1].center_radius = vec4(0.0, -101.2, 0.0, 100.0);*/
    /*spheres[0].center = vec3(0.0, -0.5, -8.0);
    spheres[0].radius = 1.0;
    spheres[1].center = vec3(0.0, -101.2, 0.0);
    spheres[1].radius = 100.0;*/

    //sphere_count = 2;



    //ray.origin = vec3(0.0);
    ray.origin = r_camera_position;
    //ray.direction = dir;

    for(i = 0; i < r_samples; i++)
    {
        ray.direction = dir + vec3(rand_float() * 2.0 - 1.0, rand_float() * 2.0 - 1.0, 0.0) * 0.0005;
        //ray.direction = dir;
        color += ray_color(ray);
    }

    color /= r_samples;

    gl_FragColor = vec4(color, 1.0);
}


#endif











