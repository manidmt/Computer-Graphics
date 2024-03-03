#include <GLApp.h>
#include <FontRenderer.h>

class MyGLApp : public GLApp {
public:
  Image image{640,480};
  FontRenderer fr{"helvetica_neue.bmp", "helvetica_neue.pos"};
  std::shared_ptr<FontEngine> fe{nullptr};
  std::string text;

  MyGLApp() : GLApp{800,800,1,"Color Picker"} {}
  
  Vec3 convertPosFromHSVToRGB(float x, float y) {
    // TODO:
    // enter code here that interprets the mouse's
    // x, y position as H ans S (I suggest to set
    // V to 1.0) and converts that tripple to RGB

      //We have to multiply the x by 360:
      x = x * 360;
      
      float value = 1.0f;

      // I am going to apply the formula which converts HSV to RGB that is shown in the next website: 
      // https://www.had2know.org/technology/hsv-rgb-conversion-formula-calculator.html

      float m = 255 * value;
      float t = m * (1 - y);

      float z = (m - t) * (1 - abs(fmod(x / 60, 2) - 1));
      float r, g, b;

      if (x < 60) { r = m; g = z + t; b = t; }
      else if (60 <= x && x < 120) { r = z + t; g = m; b = t; }
      else if (120 <= x && x < 180) { r = t; g = m; b = z + t; }
      else if (180 <= x && x < 240) { r = t; g = z + t; b = m; }
      else if (240 <= x && x < 300) { r = z + t; g = t; b = m; }
      else if (300 <= x && x < 360) { r = m; g = t; b = z + t; }

      // We want values between [0,1.0] so we have to divide r, g, b by 255:

      r = r / 255.0f;
      g = g / 255.0f;
      b = b / 255.0f;

      return Vec3{r, g ,b};

    //return Vec3{x,y,1.0f};
  }
  
  virtual void init() override {
    fe = fr.generateFontEngine();
    for (uint32_t y = 0;y<image.height;++y) {
      for (uint32_t x = 0;x<image.width;++x) {
        const Vec3 rgb = convertPosFromHSVToRGB(float(x)/image.width, float(y)/image.height);
        image.setNormalizedValue(x,y,0,rgb.r); image.setNormalizedValue(x,y,1,rgb.g);
        image.setNormalizedValue(x,y,2,rgb.b); image.setValue(x,y,3,255);
      }
    }
    GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GL(glBlendEquation(GL_FUNC_ADD));
    GL(glEnable(GL_BLEND));
  }
  
  virtual void mouseMove(double xPosition, double yPosition) override {
    Dimensions s = glEnv.getWindowSize();
    if (xPosition < 0 || xPosition > s.width || yPosition < 0 || yPosition > s.height) return;
    const Vec3 hsv{float(360*xPosition/s.width),float(1.0-yPosition/s.height),1.0f};
    const Vec3 rgb = convertPosFromHSVToRGB(float(xPosition/s.width), float(1.0-yPosition/s.height));
    std::stringstream ss; ss << "HSV: " << hsv << "  RGB: " << rgb; text = ss.str();
  }
    
  virtual void draw() override {
    drawImage(image);

    const Dimensions dim{ glEnv.getFramebufferSize() };
    fe->render(text, dim.aspect(), 0.03f, {0,-0.9f}, Alignment::Center, {0,0,0,1});
  }
};

#ifdef _WIN32
#include <Windows.h>
INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
#else
int main(int argc, char** argv) {
#endif
  try {
    MyGLApp myApp;
    myApp.run();
  }
  catch (const GLException& e) {
    std::stringstream ss;
    ss << "Insufficient OpenGL Support " << e.what();
#ifndef _WIN32
    std::cerr << ss.str().c_str() << std::endl;
#else
    MessageBoxA(
      NULL,
      ss.str().c_str(),
      "OpenGL Error",
      MB_ICONERROR | MB_OK
    );
#endif
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
