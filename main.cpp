#include <opencv2/opencv.hpp>
#include <ccomplex>
#include <iomanip>

#define kWIDTH 800
#define kHEIGHT 800

class Pixel {
public:
    char r, g, b;
    Pixel() : r(0), g(0), b(0) {};
    Pixel(char r, char g, char b) : r(r), g(g), b(b) {};
    void set(char nr, char ng, char nb) {
        r = nr;
        g = ng;
        b = nb;
    }
};

class Frame {
private:
    std::vector<Pixel> buf;
    std::vector<char> data;
    size_t width, height;
public:
    Frame(size_t width, size_t height) : width(width), height(height) {
        this->buf = std::vector<Pixel>(width * height);
    }

    void setPix(size_t x, size_t y, char r, char g, char b) {
        if ((y * width + x) < buf.size()) {
            buf[y * width + x].set(r, g, b);
        }
    }

    Pixel* getPix(size_t x, size_t y) {
        if ((y * width + x) < buf.size()) {
            return &buf[y * width + x];
        }
        return nullptr;
    }

    char* getData() {
        data.clear();
        for (int j = 0; j < width; j++)
            for (int i = 0; i < height; i++) {
                data.push_back(buf[j * width + i].r);
                data.push_back(buf[j * width + i].g);
                data.push_back(buf[j * width + i].b);
            }
        return data.data();
    }

    std::vector<Pixel>* getBuf() { return &buf; }

    size_t getWidth() {
        return width;
    }

    size_t getHeight() {
        return height;
    }

    void copy(Frame* f) {
        width = f->getWidth();
        height = f->getHeight();
        buf.clear();
        for (auto p = f->getBuf()->begin(); p != f->getBuf()->end(); p++) {
            buf.push_back(*p);
        }
    }
};

void fractal(Frame*);

void blur(Frame*, int);

int main() {
    Frame f(kWIDTH, kHEIGHT);
    fractal(&f);
    cv::Mat frac(kWIDTH, kHEIGHT, CV_8UC3, f.getData());
    cv::imshow("Fractal", frac);
    blur(&f, 6);
    cv::Mat blu(kWIDTH, kHEIGHT, CV_8UC3, f.getData());
    cv::imshow("Blur", blu);
    cv::waitKey(0);
    cv::destroyAllWindows();
    return 0;
}

void fractal(Frame* f) {
    std::complex<double> z, c;
    c.real(-0.8);
    c.imag(0.156);
    for (double i = -kWIDTH / 2; i < kWIDTH / 2; i++) {
        for (double j = -kHEIGHT / 2; j < kHEIGHT / 2; j++) {
            z.real(i / 200);
            z.imag(j / 200);
            int k;
            for (k = 0; k < 200; k++) {
                if (sqrt(z.real() * z.real() + z.imag() * z.imag()) > 2) break;
                else z = z * z + c;
            }
            f->setPix((size_t)i + kWIDTH / 2, (size_t)j + kHEIGHT / 2, k, k, k);
        }
        std::cout << std::fixed << std::setprecision(2) << "Fractal: " << 100 * ((i + kWIDTH / 2) / kWIDTH) << "%" << std::endl;
    }
}

void blur(Frame* f, int ra) {
    Frame out(f->getWidth(), f->getHeight());
    for (size_t x = 0; x < f->getWidth(); x++) {
        for (size_t y = 0; y < f->getHeight(); y++) {
            float c = 0;
            float r = 0, g = 0, b = 0;
            Pixel* p;
            for (int i = -ra / 2; i < ra / 2; i++) {
                for (int j = -ra / 2; j < ra / 2; j++) {
                    if (x + i >= 0 && y + j >= 0) {
                        p = f->getPix(x + i, y + j);
                        if (p != nullptr) {
                            c++;
                            r += p->r;
                            g += p->g;
                            b += p->b;
                        }
                    }
                }
            }
            if (c != 0) {
                r /= c;
                g /= c;
                b /= c;
            }
            out.setPix(x, y, r, g, b);
        }
        std::cout << std::fixed << std::setprecision(2) << "Blur: " << 100 * ((double)x / f->getWidth()) << "%" << std::endl;
    }
    f->copy(&out);
}
