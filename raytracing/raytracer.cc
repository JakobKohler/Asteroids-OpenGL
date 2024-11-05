#include "math.h"
#include "geometry.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iostream>
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

  explicit Color(const unsigned char red = 100, const unsigned char green = 0, const unsigned char blue = 0)
      : r(red), g(green), b(blue) {}
};

class Screen {
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

  Color get_pixel(const int x, const int y) const
  {
    if (x < 0 || x >= width || y < 0 || y >= height) {
      throw std::out_of_range("Pixel coordinates are out of bounds");
    }
    return pixels[y * width + x];
  }

  void renderImage()
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
};



// Eine "Kamera", die von einem Augenpunkt aus in eine Richtung senkrecht auf ein Rechteck (das Bild) zeigt.
// Für das Rechteck muss die Auflösung oder alternativ die Pixelbreite und -höhe bekannt sein.
// Für ein Pixel mit Bildkoordinate kann ein Sehstrahl erzeugt werden.
class Camera {
private:
  Vector3df position; // Camera position in 3D space

public:
  Camera(const Vector3df& pos) : position(pos) {}

  Ray<float, 3> get_ray(const float x, const float y) const
  {
    const Vector3df direction = position - position; //Nonsense
    const Ray<float, 3> ray(position, direction);
    return ray;
  };
};


// Für die "Farbe" benötigt man nicht unbedingt eine eigene Datenstruktur.
// Sie kann als Vector3df implementiert werden mit Farbanteil von 0 bis 1.
// Vor Setzen eines Pixels auf eine bestimmte Farbe (z.B. 8-Bit-Farbtiefe),
// kann der Farbanteil mit 255 multipliziert  und der Nachkommaanteil verworfen werden.


// Das "Material" der Objektoberfläche mit ambienten, diffusem und reflektiven Farbanteil.
class Material {
public:
    Color ambient;
    Color diffuse;
    Color reflective;

    Material(const Color& ambient, const Color& diffuse, const Color& reflective)
            : ambient(ambient), diffuse(diffuse), reflective(reflective) {}
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

    Material getMaterial() const {
        return material;
    }

    Sphere3df getGeometricObject() const {
        return geometricObject;
    }
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
    Vector3df lightSource;
    std::vector<Shape> objects;


public:

};

// Die rekursive raytracing-Methode. Am besten ab einer bestimmten Rekursionstiefe (z.B. als Parameter übergeben) abbrechen.


int main(void) {
  // Bildschirm erstellen
  // Kamera erstellen
  // Für jede Pixelkoordinate x,y
  //   Sehstrahl für x,y mit Kamera erzeugen
  //   Farbe mit raytracing-Methode bestimmen
  //   Beim Bildschirm die Farbe für Pixel x,y, setzten
    auto aspect_ratio = 16.0 / 9.0;
    int image_width = 400;

    int image_height = int(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    Screen screen = Screen(image_width, image_height);
    return 0;
}

