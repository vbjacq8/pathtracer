#include "parse.h"

#include <cstdlib>
#include <iostream>
#include <string>

static void printUsage(const char* prog) {
    std::cerr
        << "Usage: " << prog << " [options]\n"
        << "  --width, -w N           Image width (default 200)\n"
        << "  --height, -h N          Image height (default: width / aspect)\n"
        << "  --aspect A              Width / height ratio (default 2.0)\n"
        << "  --lookfrom X Y Z        Camera position (default 13 2 3)\n"
        << "  --lookat X Y Z          Camera target (default 0 0 0)\n"
        << "  --vup X Y Z             Camera up vector (default 0 1 0)\n"
        << "  --vfov DEG              Vertical field of view (default 20)\n"
        << "  --aperture A            Lens aperture (default 0.1, 0 for pinhole)\n"
        << "  --focus-dist D          Focus distance (default 10)\n"
        << "  --samples, -s N         Samples per pixel (default 100)\n"
        << "  --depth, -d N           Max ray bounce depth (default 500)\n"
        << "  --help                  Show this help\n"
        << "\nExample:\n"
        << "  " << prog << " --width 400 --samples 50 --lookfrom 13 2 3 --lookat 0 0 0\n";
}

static bool parseDouble(const char* text, double& out) {
    char* end = nullptr;
    out = std::strtod(text, &end);
    return end != text && *end == '\0';
}

static bool parseInt(const char* text, int& out) {
    char* end = nullptr;
    long value = std::strtol(text, &end, 10);
    if (end == text || *end != '\0') {
        return false;
    }
    out = static_cast<int>(value);
    return true;
}

static bool parseVec3(int& i, int argc, char** argv, vec3& out) {
    if (i + 3 >= argc) {
        return false;
    }
    double x = 0;
    double y = 0;
    double z = 0;
    if (!parseDouble(argv[i + 1], x) ||
        !parseDouble(argv[i + 2], y) ||
        !parseDouble(argv[i + 3], z)) {
        return false;
    }
    out = vec3(x, y, z);
    i += 3;
    return true;
}

int parseOptions(int argc, char** argv, RenderOptions& opts) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--help" || arg == "-?") {
            printUsage(argv[0]);
            return 2;
        }
        if (arg == "--width" || arg == "-w") {
            if (++i >= argc || !parseInt(argv[i], opts.width)) {
                return 1;
            }
            continue;
        }
        if (arg == "--height" || arg == "-h") {
            if (++i >= argc || !parseInt(argv[i], opts.height)) {
                return 1;
            }
            continue;
        }
        if (arg == "--aspect") {
            if (++i >= argc || !parseDouble(argv[i], opts.aspect)) {
                return 1;
            }
            continue;
        }
        if (arg == "--lookfrom" || arg == "--lookat" || arg == "--vup") {
            vec3* target = nullptr;
            if (arg == "--lookfrom") {
                target = &opts.lookfrom;
            } else if (arg == "--lookat") {
                target = &opts.lookat;
            } else {
                target = &opts.vup;
            }
            if (!parseVec3(i, argc, argv, *target)) {
                return 1;
            }
            continue;
        }
        if (arg == "--vfov") {
            if (++i >= argc || !parseDouble(argv[i], opts.vfov)) {
                return 1;
            }
            continue;
        }
        if (arg == "--aperture") {
            if (++i >= argc || !parseDouble(argv[i], opts.aperture)) {
                return 1;
            }
            continue;
        }
        if (arg == "--focus-dist") {
            if (++i >= argc || !parseDouble(argv[i], opts.focusDist)) {
                return 1;
            }
            continue;
        }
        if (arg == "--samples" || arg == "-s") {
            if (++i >= argc || !parseInt(argv[i], opts.samples)) {
                return 1;
            }
            continue;
        }
        if (arg == "--depth" || arg == "-d") {
            if (++i >= argc || !parseInt(argv[i], opts.depth)) {
                return 1;
            }
            continue;
        }

        std::cerr << "Unknown option: " << arg << "\n";
        printUsage(argv[0]);
        return 1;
    }

    if (opts.width <= 0 || opts.samples <= 0 || opts.depth <= 0 || opts.aspect <= 0) {
        std::cerr << "width, samples, depth, and aspect must be positive.\n";
        return 1;
    }
    if (opts.height <= 0) {
        opts.height = static_cast<int>(opts.width / opts.aspect);
        if (opts.height <= 0) {
            std::cerr << "Computed height must be positive; check width and aspect.\n";
            return 1;
        }
    }

    return 0;
}
