#include <GLApp.h>
#include <cmath>
#include <optional>

class MyGLApp : public GLApp {
public:
  Image image{1024,1024};
  
  MyGLApp() : GLApp{1024,1024} {}
    
  std::optional<Vec3> raySphereIntersect(const Vec3& sphereCenter, const float& radius, const Vec3& rayOrigin, const Vec3& pixelPos) {
    
    /*
    * @author Manuel Díaz-Meco Terrés
    * Computer Graphics 2022
    * Assigment 2, excercise 2
    * 
    Explanation: being (a,b,c) the sphere's center and r its radius we know that the sphere's implicit equation is:
                    (x-a)^2 + (y-b)^2 + (z-c)^2 = r^2
                 given the rayOrigin (Ox,Oy,Oz) and the pixelPoint (Px,Py,Pz) the ray's parametric equation is:
                    x = Ox + t(Px-Ox)
                    y = Oy + t(Py-Oy)
                    z = Oz + t(Pz-Oz)
                 where t is a real number.

                 So the intersection point will be the solution to the equation formed by substituting the 
                 parametric equation of the ray in the implicit one of the sphere:

                    ((Ox + t(Px-Ox)-a)^2 + ((Oy + t(Py-Oy))-b)^2 + ((Oz + t(Pz-Oz))-c)^2 = r^2

                 If we expand the equation we get the following:

                    t^2[(Px-Ox)^2 + (Py-Oy)^2 + (Pz-Oz)^2] +
                    2*t[Ox(Px-Ox) + Oy(Py-Oy) + Oz(Pz-Oz) - a(Px-Ox) - b(Py-Oy) - c(Pz-Oz)] +
                       [a + b + c + Ox^2 + Oy^2 + Oz^2 - r^2 - 2*[aOx + bOy + cOz] = 0
    */


      double t1, t2, index_t2, index_t1, index_t0;
      double vx = (pixelPos.x - rayOrigin.x); double vy = (pixelPos.y - rayOrigin.y); double vz = (pixelPos.z - rayOrigin.z);

      index_t2 = pow(vx, 2) + pow(vy, 2) + pow(vz, 2);
      index_t1 = 2*(rayOrigin.x * vx + rayOrigin.y * vy + rayOrigin.z * vz - sphereCenter.x * vx - sphereCenter.y * vy - sphereCenter.z * vz);
      index_t0 = sphereCenter.x + sphereCenter.y + sphereCenter.z + pow(rayOrigin.x, 2) + pow(rayOrigin.y, 2) + pow(rayOrigin.z, 2)
          - pow(radius, 2) - 2 * (sphereCenter.x * rayOrigin.x + sphereCenter.y * rayOrigin.y + sphereCenter.z * rayOrigin.z);

      double d = sqrt(pow(index_t1, 2) - (4 * index_t2 * index_t0));
      if (d > 0) 
      {
          t1 = ((index_t1 * -1) + (d)) / (2 * index_t0);
          t2 = ((index_t1 * -1) - (d)) / (2 * index_t0);

          float x1 = rayOrigin.x + t1 * vx;
          float y1 = rayOrigin.y + t1 * vy;
          float z1 = rayOrigin.z + t1 * vz;

          float x2 = rayOrigin.x + t2 * vx;
          float y2 = rayOrigin.y + t2 * vy;
          float z2 = rayOrigin.z + t2 * vz;

          double module1 = sqrt(pow(x1 - rayOrigin.x, 2) + pow(y1 - rayOrigin.y, 2) + pow(z1 - rayOrigin.z, 2));
          double module2 = sqrt(pow(x2 - rayOrigin.x, 2) + pow(y2 - rayOrigin.y, 2) + pow(z2 - rayOrigin.z, 2));

          if (module1 < module2)
          {
              Vec3 v(x1, y1, z1);
              return v;
          }
          else
          {
              Vec3 v(x2, y2, z2);
              return v;
          }
      }
      else return{};
  }
  
  Vec3 computeLighting(const Vec3& rayOrigin, const Vec3& lightPos, const Vec3& intersectionPoint, const Vec3& normal,
                       const Vec3& specularColor, const Vec3& diffuseColor, const Vec3& ambientColor) {
    const Vec3 viewDir  = Vec3::normalize(rayOrigin-intersectionPoint);
    const Vec3 lightDir = Vec3::normalize(lightPos-intersectionPoint);
    const Vec3 reflectedDir = normal * 2.0f * Vec3::dot(normal, lightDir) - lightDir;
    
    const Vec3 specular = specularColor * pow(std::max(0.0f,Vec3::dot(reflectedDir, viewDir)),16.0f);
    const Vec3 diffuse  = diffuseColor * std::max(0.0f,Vec3::dot(normal, lightDir));
    const Vec3 ambient  = ambientColor;
    return specular + diffuse + ambient;
  }
    
  virtual void init() {
    glEnv.setTitle("Intersection Demo");
    GL(glDisable(GL_CULL_FACE));
    GL(glClearColor(0,0,0,0));
    
    const Vec3 lightPos{0.0f,4.0f,0.0f};
    const Vec3 sphereCenter{0.0f, 0.0f, -4.0f};
    const float radius = 2.0f;
    const Vec3 rayOrigin{0.0f, 0.0f, 4.0f};
    const Vec3 topLeftCorner{-2.0f, 2.0f, 0.0f}, topRightCorner{2.0f, 2.0f, 0.0f};
    const Vec3 bottomLeftCorner{-2.0f, -2.0f, 0.0f}, bottomRightCorner{2.0f, -2.0f, 0.0f};
    const Vec3 deltaX = (topRightCorner-topLeftCorner)/ float(image.width);
    const Vec3 deltaY = (topRightCorner-bottomRightCorner)/ float(image.height);
    
    for (uint32_t y = 0;y< uint32_t(image.height);++y) {
      for (uint32_t x = 0;x< uint32_t(image.width);++x) {
        const Vec3 pixelPos = bottomLeftCorner + deltaX*float(x) + deltaY*float(y);
        const std::optional<Vec3> intersection = raySphereIntersect(sphereCenter, radius, rayOrigin, pixelPos);
        Vec3 color;
        if (intersection) {
          const Vec3 normal=(*intersection - sphereCenter) / radius;
          color = computeLighting(rayOrigin, lightPos, *intersection, normal,
                                             Vec3{1.0f,1.0f,1.0f}, Vec3{1.0f,0.0f,0.0f}, Vec3{0.1f,0.0f,0.0f});
        } else {
          color = Vec3{0.0f,0.0f,0.0f};
        }
        image.setNormalizedValue(x,y,0,color.r);
        image.setNormalizedValue(x,y,1,color.g);
        image.setNormalizedValue(x,y,2,color.b);
        image.setValue(x,y,3,255);
      }
    }
  }
    
  virtual void draw() {
    GL(glClear(GL_COLOR_BUFFER_BIT));
    drawImage(image);
  }

} myApp;

int main(int argc, char ** argv) {
  myApp.run();
  return EXIT_SUCCESS;
}  
