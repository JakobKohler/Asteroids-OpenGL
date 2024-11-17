#include "math.h"
#include "geometry.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iostream>
#include <SDL2/SDL.h>
#include <optional>
// Die folgenden Kommentare beschreiben Datenstrukturen und Funktionen
// Die Datenstrukturen und Funktionen die weiter hinten im Text beschrieben sind,
// hängen höchstens von den vorhergehenden Datenstrukturen ab, aber nicht umgekehrt.





// Ein "Bildschirm", der das Setzen eines Pixels kapselt
// Der Bildschirm hat eine Auflösung (Breite x Höhe)
// Kann zur Ausgabe einer PPM-Datei verwendet werden oder
// mit SDL2 implementiert werden.

class Color {
public:
  unsigned char r, g, b;

    static const Color RED;
    static const Color GREEN;
    static const Color BLUE;
    static const Color WHITE;
    static const Color BLACK;
    static const Color PURPLE;

  explicit Color(const unsigned char red = 0, const unsigned char green = 0, const unsigned char blue = 0)
      : r(red), g(green), b(blue) {}

    friend Color operator*(const Color& color, float factor) {
        // Create a new Color object with the multiplied values
        return Color(
                static_cast<unsigned char>(std::min(255.0f, color.r * factor)),
                static_cast<unsigned char>(std::min(255.0f, color.g * factor)),
                static_cast<unsigned char>(std::min(255.0f, color.b * factor))
        );
    }

    friend Color operator+(const Color& color, Color value) {
        // Create a new Color object with the multiplied values
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
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
      return;
    }

    SDL_Window* window = SDL_CreateWindow("SDL2 Image Render", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    if (!window) {
      std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
      SDL_Quit();
      return;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
      std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
      SDL_DestroyWindow(window);
      SDL_Quit();
      return;
    }

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



// Eine "Kamera", die von einem Augenpunkt aus in eine Richtung senkrecht auf ein Rechteck (das Bild) zeigt.
// Für das Rechteck muss die Auflösung oder alternativ die Pixelbreite und -höhe bekannt sein.
// Für ein Pixel mit Bildkoordinate kann ein Sehstrahl erzeugt werden.
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
  Camera(const Screen& screen) : screen(screen){
    float viewport_height = viewport_width * (static_cast<double>(screen.getHeight())/screen.getWidth());

    Vector3df viewport_u = {viewport_width, 0, 0};
    Vector3df viewport_v = {0, -viewport_height, 0};

    this->pixel_delta_u =  (1.0f/screen.getWidth()) * viewport_u;
    this->pixel_delta_v =  (1.0f/screen.getHeight()) * viewport_v;

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


// Für die "Farbe" benötigt man nicht unbedingt eine eigene Datenstruktur.
// Sie kann als Vector3df implementiert werden mit Farbanteil von 0 bis 1.
// Vor Setzen eines Pixels auf eine bestimmte Farbe (z.B. 8-Bit-Farbtiefe),
// kann der Farbanteil mit 255 multipliziert  und der Nachkommaanteil verworfen werden.


class Material {
public:
    Color diffuseColor;
    bool reflective;

    Material(const Color& diffuseColor, const bool reflective)
            : diffuseColor(diffuseColor), reflective(reflective) {}
};




// Ein "Objekt", z.B. eine Kugel oder ein Dreieck, und dem zugehörigen Material der Oberfläche.
// Im Prinzip ein Wrapper-Objekt, das mindestens Material und geometrisches Objekt zusammenfasst.
// Kugel und Dreieck finden Sie in geometry.h/tcc

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
    Vector3df intersection;

    HitContext(const Shape hit, const Vector3df intersection)
            : hit(hit), intersection(intersection) {}
};

// verschiedene Materialdefinition, z.B. Mattes Schwarz, Mattes Rot, Reflektierendes Weiss, ...
// im wesentlichen Variablen, die mit Konstruktoraufrufen initialisiert werden.


// Die folgenden Werte zur konkreten Objekten, Lichtquellen und Funktionen, wie Lambertian-Shading
// oder die Suche nach einem Sehstrahl für das dem Augenpunkt am nächsten liegenden Objekte,
// können auch zusammen in eine Datenstruktur für die gesammte zu
// rendernde "Szene" zusammengefasst werden.

// Die Cornelbox aufgebaut aus den Objekten
// Am besten verwendet man hier einen std::vector< ... > von Objekten.

// Punktförmige "Lichtquellen" können einfach als Vector3df implementiert werden mit weisser Farbe,
// bei farbigen Lichtquellen müssen die entsprechenden Daten in Objekt zusammengefaßt werden
// Bei mehreren Lichtquellen können diese in einen std::vector gespeichert werden.

// Sie benötigen eine Implementierung von Lambertian-Shading, z.B. als Funktion
// Benötigte Werte können als Parameter übergeben werden, oder wenn diese Funktion eine Objektmethode eines
// Szene-Objekts ist, dann kann auf die Werte teilweise direkt zugegriffen werden.
// Bei mehreren Lichtquellen muss der resultierende diffuse Farbanteil durch die Anzahl Lichtquellen geteilt werden.

// Für einen Sehstrahl aus allen Objekte, dasjenige finden, das dem Augenpunkt am nächsten liegt.
// Am besten einen Zeiger auf das Objekt zurückgeben. Wenn dieser nullptr ist, dann gibt es kein sichtbares Objekt.

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
        Shape leftWall = Shape({Color::RED, true}, leftWallSphere);
        objects.push_back(leftWall);

        Sphere3df rightWallSphere = Sphere3df({BIG_RADIUS + ROOM_SIZE,0,0}, BIG_RADIUS);
        Shape rightWall = Shape({Color::GREEN, true}, rightWallSphere);
        objects.push_back(rightWall);

        Sphere3df floorSphere = Sphere3df({0,-(BIG_RADIUS + ROOM_SIZE),0}, BIG_RADIUS);
        Shape floor = Shape({Color::BLACK, true}, floorSphere);
        objects.push_back(floor);

        Sphere3df ceilingSphere = Sphere3df({0,BIG_RADIUS + ROOM_SIZE,0}, BIG_RADIUS);
        Shape ceiling = Shape({Color::BLACK, true}, ceilingSphere);
        objects.push_back(ceiling);

        Sphere3df backSphere = Sphere3df({0,0,-BIG_RADIUS - 5 * ROOM_SIZE}, BIG_RADIUS);
        Shape back = Shape({Color::BLACK, true}, backSphere);
        objects.push_back(back);

        Sphere3df sceneSphere1 = Sphere3df({6,-ROOM_SIZE + REG_RADIUS, -25}, REG_RADIUS);
        Shape obj1 = Shape({Color::BLUE, true}, sceneSphere1);
        objects.push_back(obj1);

        Sphere3df sceneSphere2 = Sphere3df({-6,-ROOM_SIZE + REG_RADIUS, -35}, REG_RADIUS);
        Shape obj2 = Shape({Color::BLUE, true}, sceneSphere2);
        objects.push_back(obj2);

        Sphere3df sceneSphere3 = Sphere3df({3,-ROOM_SIZE + REG_RADIUS, -40}, REG_RADIUS);
        Shape obj3 = Shape({Color::PURPLE, true}, sceneSphere3);
        objects.push_back(obj3);

        lightSource = {0, ROOM_SIZE - 1, (ROOM_DEPTH_FACTOR -1) * ROOM_SIZE * (-1)};
    }

public:
    Scene() {
        generateScene();
    };

    std::vector<Shape> getShapes(){
        return objects;
    }

    [[nodiscard]] Vector3df getLightSource() const
    {
        return lightSource;
    }

    std::optional<HitContext> findeNearestShape(Ray3df ray){
        std::optional<HitContext> hitContext;
        bool shapeFound = false;
        float minimal_t = INFINITY;
        for(const Shape shape: objects){
            const float t = shape.getGeometricObject().intersects(ray);
            Vector3df intersectionPoint = ray.origin + t * ray.direction;
            if(t != 0 && t < minimal_t && intersectionPoint.vector[2] < 0){
                constexpr float epsilon = 0.000015f;
                minimal_t = t;
                shapeFound = true;
                hitContext = HitContext(shape, intersectionPoint);
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
    static bool isShapeBetweenPoints(const HitContext& point1, const Vector3df point2, Scene scene)
    {
        const float acneCorrection = (point1.hit.getGeometricObject().getRadius() > 1000) ? 7e-8f : 5e-4f;
        const Vector3df shifterOrigin = point1.intersection + acneCorrection  * (point1.intersection - point1.hit.getGeometricObject().getCenter());
        Vector3df dir = point2 - shifterOrigin;
        dir.normalize();

        const auto lineBetween = Ray(shifterOrigin, dir);
        const float ogHitLightDistance = (point1.intersection - point2).square_of_length();

        for(const Shape shape: scene.getShapes()){
            const float t = shape.getGeometricObject().intersects(lineBetween);
            if(t != 0){
                const Vector3df intersectionPoint = lineBetween.origin + t * lineBetween.direction;
                const float hitHitDistance = (intersectionPoint - point1.intersection).square_of_length();
                const float hitLightDistance = (point2 - intersectionPoint).square_of_length();
                if ((hitHitDistance <= ogHitLightDistance) && (hitLightDistance <= ogHitLightDistance)){
                    return true;
                };
            }
        }
        return false;
    }
public:
    static Color getColorLambertian(const HitContext& hitContext, Scene scene){
        // const float acneCorrection = (hitContext.hit.getGeometricObject().getRadius() > 1000) ? 0.000002f : 0.00002f;
        // const float acneCorrection = (hitContext.hit.getGeometricObject().getRadius() > 1000) ? 0.00000005f : 0.00000005f;
        constexpr float ambientLight = 0.25f;

        Vector3df sphereSurfaceVector = hitContext.intersection - hitContext.hit.getGeometricObject().getCenter();
        sphereSurfaceVector.normalize();
        Vector3df lightSourceNormal = scene.getLightSource() - hitContext.intersection;
        lightSourceNormal.normalize();

        /*// std::optional<HitContext> shadowRayHC = scene.findeNearestShape(Ray(hitContext.intersection + acneCorrection  * (hitContext.intersection - hitContext.hit.getGeometricObject().getCenter()),(1.0f) *lightSourceNormal)); //Hier bei intersection koennte die Akne entstehen. Punkt am normalen Vektor verschieben
        // if(shadowRayHC.has_value()){
        //     const Vector3df lightSourceVector = scene.getLightSource() - hitContext.intersection;
        //     const Vector3df hitLightVector = scene.getLightSource() - shadowRayHC.value().intersection;
        //     Vector3df hitHitVector = hitContext.intersection - shadowRayHC.value().intersection;
        //
        //     if(hitHitVector.length() < lightSourceVector.length() && hitLightVector.length() < lightSourceVector.length()){ //Might be incorrect, also getNearestObject might be incorrect in this case because its dependend on origin and not correct in this case
        //         return hitContext.hit.getMaterial().diffuseColor * ambientLight;
        //     }
        // }
        //*/
        if(isShapeBetweenPoints(hitContext, scene.getLightSource(), scene))
        {
            return hitContext.hit.getMaterial().diffuseColor * ambientLight;
        }


        const float intensity = ambientLight + std::max(0.0f, sphereSurfaceVector * lightSourceNormal);
        return hitContext.hit.getMaterial().diffuseColor * intensity;
    }

    static void render(Scene s){
        constexpr auto aspect_ratio = 16.0/9.0;
        constexpr int image_width = 1600;

        constexpr int image_height = static_cast<int>(image_width / aspect_ratio);

        auto screen = Screen(image_width, image_height);
        const auto camera = Camera(screen);
        for (int y = 0; y < image_height; y++){
            for (int x = 0; x < image_width; x++){
                // if(x != 641 && y != 485)
                // {
                //     continue;
                // }

                const Ray ray = camera.get_ray(x, y);
                std::optional<HitContext> hitContext = s.findeNearestShape(ray);
                if(hitContext.has_value()){
                    Color color = getColorLambertian(hitContext.value(), s);
                    screen.set_pixel(x,y, color);
                }
            }
        }
        screen.renderSDL2();
    };
};

// Die rekursive raytracing-Methode. Am besten ab einer bestimmten Rekursionstiefe (z.B. als Parameter übergeben) abbrechen.


int main(void) {
  // Bildschirm erstellen
  // Kamera erstellen
  // Für jede Pixelkoordinate x,y
  //   Sehstrahl für x,y mit Kamera erzeugen
  //   Farbe mit raytracing-Methode bestimmen
  //   Beim Bildschirm die Farbe für Pixel x,y, setzten

    auto s = Scene();

    Raytracer::render(s);
    return 0;
}

