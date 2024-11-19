#include "math.h"
#include "geometry.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iostream>
#include <SDL2/SDL.h>
#include <optional>


class Color {
public:
  unsigned char r, g, b;

    static const Color RED;
    static const Color GREEN;
    static const Color BLUE;
    static const Color WHITE;
    static const Color BLACK;
    static const Color PURPLE;
    static const Color PINK;


  explicit Color(const unsigned char red = 0, const unsigned char green = 0, const unsigned char blue = 0)
      : r(red), g(green), b(blue) {}

    friend Color operator*(const Color& color, float factor) {
        return Color(
                static_cast<unsigned char>(std::min(255.0f, color.r * factor)),
                static_cast<unsigned char>(std::min(255.0f, color.g * factor)),
                static_cast<unsigned char>(std::min(255.0f, color.b * factor))
        );
    }

    friend Color operator+(const Color& color, Color value) {
        return Color(
                static_cast<unsigned char>(std::min(255, color.r + value.r)),
                static_cast<unsigned char>(std::min(255, color.g + value.r)),
                static_cast<unsigned char>(std::min(255, color.b + value.r))
        );
    }
};

const Color Color::RED(255, 85, 85);
const Color Color::GREEN(80, 250, 123);
const Color Color::BLUE(139, 233, 253);
const Color Color::WHITE(248, 248, 242);
const Color Color::BLACK(40, 42, 54);
const Color Color::PURPLE(189, 147, 249);
const Color Color::PINK(255, 121, 198);


class Screen {
public:
  [[nodiscard]] int getWidth() const
  {
    return width;
  }

  [[nodiscard]] int getHeight() const
  {
    return height;
  }

private:
  int width;
  int height;
  std::vector<Color> pixels;

public:
  Screen(const int w, const int h) : width(w), height(h), pixels(w * h) {}

  void set_pixel(const int x, const int y, const Color& color) {
    if (x < 0 || x >= width || y < 0 || y >= height) {
      throw std::out_of_range("Pixel coordinates are out of bounds");
    }
    pixels[y * width + x] = color;
  }

  [[nodiscard]] Color get_pixel(const int x, const int y) const
  {
    if (x < 0 || x >= width || y < 0 || y >= height) {
      throw std::out_of_range("Pixel coordinates are out of bounds");
    }
    return pixels[y * width + x];
  }

  void renderImage() const
  {
    std::ofstream outFile("out.ppm");
    if (!outFile) {
      std::cerr << "Error opening file for writing." << std::endl;
      return;
    }
    outFile << "P3\n" << this->width << " " << this->height << "\n255\n";

    for (int j = 0; j < this->height; j++) {
      std::clog << "\rScanlines remaining: " << (this->height - j) << ' ' << std::flush;
      for (int i = 0; i < this->width; i++) {
        const Color currentColor = this->get_pixel(i, j);
        const int r = currentColor.r;
        const int g = currentColor.g;
        const int b = currentColor.b;

        outFile << r << ' ' << g << ' ' << b << ' ';
      }
      outFile << '\n';
    }

    outFile.close();
  }
  void renderSDL2() const {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("SDL2 Image Render", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    for (int j = 0; j < height; j++) {
      for (int i = 0; i < width; i++) {
        Color currentColor = get_pixel(i, j);
        SDL_SetRenderDrawColor(renderer, currentColor.r, currentColor.g, currentColor.b, 255);

        SDL_RenderDrawPoint(renderer, i, j);
      }
    }

    SDL_RenderPresent(renderer);
    bool running = true;
    SDL_Event event;
    while (running) {
      while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
          running = false;
        }
      }
      SDL_Delay(10);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
  }
};

class Camera {
private:
  Screen screen;
  const float viewport_width = 2.0;
  const float focal_length = 1.0;
  Vector3df camera_center = {0, 0, 0};
  Vector3df pixel_delta_u = {0,0,0};
  Vector3df pixel_delta_v = {0,0,0};
  Vector3df pixel00_loc = {0,0,0};
public:
  explicit Camera(const Screen& screen) : screen(screen){
    float viewport_height = viewport_width * (static_cast<double>(screen.getHeight())/screen.getWidth());

    Vector3df viewport_u = {viewport_width, 0, 0};
    Vector3df viewport_v = {0, -viewport_height, 0};

    this->pixel_delta_u =  (1.0f / screen.getWidth()) * viewport_u;
    this->pixel_delta_v =  (1.0f / screen.getHeight()) * viewport_v;

    Vector3df cameraToScreenVector = {0,0,focal_length};
    Vector3df viewport_upper_left = camera_center - cameraToScreenVector - 0.5f * viewport_u -  0.5f * viewport_v;

    this->pixel00_loc = viewport_upper_left + 0.5f * (pixel_delta_u + pixel_delta_v);
  }

  [[nodiscard]] Ray<float, 3> get_ray(const float x, const float y) const
  {
    const auto pixel_center = pixel00_loc + (x * pixel_delta_u) + (y * pixel_delta_v);
    const auto ray_direction = pixel_center - camera_center; //Swap if not working
    const auto ray = Ray<float, 3>(camera_center, ray_direction);
    return ray;
  };
};

class Material {
public:
    Color diffuseColor;
    bool reflective;

    Material(const Color& diffuseColor, const bool reflective)
            : diffuseColor(diffuseColor), reflective(reflective) {}
};

class Shape {
private:
    Material material;
    Sphere3df geometricObject;

public:
    Shape(const Material& material, const Sphere3df& shape)
            : material(material), geometricObject(shape) {}

    [[nodiscard]] Material getMaterial() const {
        return material;
    }

    [[nodiscard]] Sphere3df getGeometricObject() const {
        return geometricObject;
    }
};

class HitContext{
public:
    Shape hit;
    Intersection_Context<float, 3> intersectionContext;

    HitContext(const Shape hit, const Intersection_Context<float, 3> intersectionContext)
            : hit(hit), intersectionContext(intersectionContext) {}
};

class Scene{
private:
    Vector3df lightSource = {0,0,0};
    std::vector<Shape> objects;
    const float BIG_RADIUS = 1e6;
    const float REG_RADIUS = 3;
    const float ROOM_SIZE = 10;
    const float ROOM_DEPTH_FACTOR = 5;

    void generateScene(){
        Sphere3df leftWallSphere = Sphere3df({-(BIG_RADIUS + ROOM_SIZE),0,0}, BIG_RADIUS);
        Shape leftWall = Shape({Color::RED, false}, leftWallSphere);
        objects.push_back(leftWall);

        Sphere3df rightWallSphere = Sphere3df({BIG_RADIUS + ROOM_SIZE,0,0}, BIG_RADIUS);
        Shape rightWall = Shape({Color::GREEN, false}, rightWallSphere);
        objects.push_back(rightWall);

        Sphere3df floorSphere = Sphere3df({0,-(BIG_RADIUS + ROOM_SIZE),0}, BIG_RADIUS);
        Shape floor = Shape({Color::BLUE, false}, floorSphere);
        objects.push_back(floor);

        Sphere3df ceilingSphere = Sphere3df({0,BIG_RADIUS + ROOM_SIZE,0}, BIG_RADIUS);
        Shape ceiling = Shape({Color::WHITE, false}, ceilingSphere);
        objects.push_back(ceiling);

        Sphere3df backSphere = Sphere3df({0,0,-BIG_RADIUS - 5 * ROOM_SIZE}, BIG_RADIUS);
        Shape back = Shape({Color::WHITE, false}, backSphere);
        objects.push_back(back);

        Sphere3df frontSphere = Sphere3df({0,0,BIG_RADIUS}, BIG_RADIUS);
        Shape front = Shape({Color::RED, false}, frontSphere);
        objects.push_back(front);

        Sphere3df sceneSphere1 = Sphere3df({6,-ROOM_SIZE + REG_RADIUS, -25}, REG_RADIUS);
        Shape obj1 = Shape({Color::PURPLE, false}, sceneSphere1);
        objects.push_back(obj1);

        Sphere3df sceneSphere2 = Sphere3df({-6,-ROOM_SIZE + REG_RADIUS, -35}, REG_RADIUS);
        Shape obj2 = Shape({Color::PURPLE, true}, sceneSphere2);
        objects.push_back(obj2);

        Sphere3df sceneSphere3 = Sphere3df({3,-ROOM_SIZE + REG_RADIUS, -40}, REG_RADIUS);
        Shape obj3 = Shape({Color::PURPLE, false}, sceneSphere3);
        objects.push_back(obj3);

        lightSource = {0, ROOM_SIZE - 1, (ROOM_DEPTH_FACTOR -1) * ROOM_SIZE * (-1)};
    }

public:
    Scene() {
        generateScene();
    };

    [[nodiscard]] Vector3df getLightSource() const
    {
        return lightSource;
    }

    std::optional<HitContext> findeNearestShape(Ray3df ray){
        std::optional<HitContext> hitContext;
        Intersection_Context<float, 3> intersectionContext;

        bool shapeFound = false;
        float minimal_t = INFINITY;
        for(const Shape shape: objects){
            const bool intersects = shape.getGeometricObject().intersects(ray, intersectionContext);
            if(intersects && intersectionContext.t > 0 && intersectionContext.t < minimal_t){
                minimal_t = intersectionContext.t;
                shapeFound = true;
                hitContext = HitContext(shape, intersectionContext);
            }
        }

        if(shapeFound){
            return hitContext;
        }else{
            return std::nullopt;
        };
    }
};

class Raytracer{
private:

    static Color trace(const Ray3df& ray, Scene scene, const int remainingDepth)
    {
        auto color = Color(0,0,0);
        const std::optional<HitContext> hitContext = scene.findeNearestShape(ray);

        if(hitContext.has_value() && remainingDepth != 0){
            if(hitContext.value().hit.getMaterial().reflective){
                const float acneCorrection = 1e-4;
                Intersection_Context intersectionContext = hitContext.value().intersectionContext;

                const Vector3df shiftedOrigin = intersectionContext.intersection + acneCorrection  * intersectionContext.normal;
                float dotProduct = ray.direction * intersectionContext.normal;
                Vector3df reflectedDirection = ray.direction - 2.0f * dotProduct * intersectionContext.normal;

                Ray reflectedRay(shiftedOrigin, reflectedDirection);
                color = trace(reflectedRay, scene, remainingDepth - 1) + Color::WHITE * 0.2;
            } else {
                color = getColorLambertian(hitContext.value(), scene);
            }
        }
        return color;
    }

    static bool isShapeBetweenPoints(const Vector3df collisionPoint, const Vector3df point2, Scene scene)
    {
        Vector3df dir = point2 - collisionPoint;
        dir.normalize();

        const auto shadowRay = Ray(collisionPoint, dir);
        const float ogHitLightDistance = (collisionPoint - point2).length();

        std::optional<HitContext> nearestShape = scene.findeNearestShape(shadowRay);

        if(nearestShape.has_value()){
            Intersection_Context intersectionContext = nearestShape.value().intersectionContext;
            return intersectionContext.t < ogHitLightDistance;
        }
        return false;
    }
public:
    static Color getColorLambertian(const HitContext& hitContext, const Scene& scene){
        Intersection_Context intersectionContext = hitContext.intersectionContext;
        constexpr float ambientLight = 0.3f;

        Vector3df lightSourceNormal = scene.getLightSource() - intersectionContext.intersection;
        lightSourceNormal.normalize();

        float intensity = ambientLight + std::max(0.0f, intersectionContext.normal * lightSourceNormal);
        const float acneCorrection = (hitContext.hit.getGeometricObject().getRadius() > 1000) ? 6e-2f : 5e-4f;

        Vector3df shiftedCollision = intersectionContext.intersection + acneCorrection  * intersectionContext.normal;

        if(isShapeBetweenPoints(shiftedCollision, scene.getLightSource(), scene))
        {
            intensity = ambientLight; 
        }
        return hitContext.hit.getMaterial().diffuseColor * intensity;
    }

    static void render(const Scene& s, Screen screen){
        const auto camera = Camera(screen);

        for (int y = 0; y < screen.getHeight(); y++){
            for (int x = 0; x < screen.getWidth(); x++){
                const Ray ray = camera.get_ray(x, y);
                Color tracedColor = trace(ray, s, 3);
                screen.set_pixel(x,y, tracedColor);
            }
        }
        screen.renderSDL2();
    };
};

int main() {
    constexpr auto aspect_ratio = 16.0/9.0;
    constexpr int image_width = 1000;
    constexpr int image_height = static_cast<int>(image_width / aspect_ratio);

    const auto scene = Scene();
    auto screen = Screen(image_width, image_height);

    Raytracer::render(scene, screen);
    return 0;
}

