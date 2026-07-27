#include "parse.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>

static void printUsage(const char* prog) {
    std::cerr
        << "Usage: " << prog << " [options]\n"
        << "  --width, -w N           Image width (default 200)\n"
        << "  --height, -h N          Image height (default: width / aspect)\n"
        << "  --aspect A              Width / height ratio (default 16/9)\n"
        << "  --lookfrom X Y Z        Camera position (default 13 2 3)\n"
        << "  --lookat X Y Z          Camera target (default 0 0 0)\n"
        << "  --vup X Y Z             Camera up vector (default 0 1 0)\n"
        << "  --vfov DEG              Vertical field of view (default 20)\n"
        << "  --aperture A            Lens aperture (default 0, pinhole; try 0.1 for DOF)\n"
        << "  --focus-dist D          Focus distance (default 10)\n"
        << "  --samples, -s N         Batch only: total samples per pixel (default 100)\n"
        << "  --depth, -d N           Max ray bounce depth (default 50)\n"
        << "  --display-width N       Window width (default: render width)\n"
        << "  --display-height N      Window height (default: render height)\n"
        << "  --gamma G               Display gamma (default 2.2)\n"
        << "  --fps                   Include FPS in the window title (off by default)\n"
        << "  --fullscreen            Fullscreen desktop; upscale render to display\n"
        << "  --help                  Show this help\n"
        << "\nExample:\n"
        << "  " << prog << " --width 400 --samples 50 --lookfrom 13 2 3 --lookat 0 0 0\n"
        << "Note: use spaces between vec3 components (commas are optional: 13, 2, 3).\n";
}

/** Strips commas so tokens like "478," from copy-pasted RTIOW args still parse. */
static std::string sanitizeNumberToken(const char* text) {
    std::string cleaned(text);
    cleaned.erase(std::remove(cleaned.begin(), cleaned.end(), ','), cleaned.end());
    return cleaned;
}

static bool parseDouble(const char* text, double& out) {
    const std::string cleaned = sanitizeNumberToken(text);
    char* end = nullptr;
    out = std::strtod(cleaned.c_str(), &end);
    return end != cleaned.c_str() && *end == '\0';
}

static bool parseInt(const char* text, int& out) {
    const std::string cleaned = sanitizeNumberToken(text);
    char* end = nullptr;
    long value = std::strtol(cleaned.c_str(), &end, 10);
    if (end == cleaned.c_str() || *end != '\0') {
        return false;
    }
    out = static_cast<int>(value);
    return true;
}

static bool parseVec3(int& i, int argc, char** argv, vec3& out, const char* flag) {
    if (i + 3 >= argc) {
        std::cerr << "Option " << flag << " needs three numbers (got fewer).\n";
        return false;
    }
    double x = 0;
    double y = 0;
    double z = 0;
    if (!parseDouble(argv[i + 1], x) ||
        !parseDouble(argv[i + 2], y) ||
        !parseDouble(argv[i + 3], z)) {
        std::cerr << "Option " << flag << " expects three numbers, got: '"
                  << argv[i + 1] << "' '" << argv[i + 2] << "' '" << argv[i + 3] << "'\n";
        return false;
    }
    out = vec3(x, y, z);
    i += 3;
    return true;
}

static bool requireInt(int& i, int argc, char** argv, int& out, const char* flag) {
    if (++i >= argc || !parseInt(argv[i], out)) {
        std::cerr << "Option " << flag << " needs an integer"
                  << (i < argc ? std::string(" (got '") + argv[i] + "')" : "") << ".\n";
        return false;
    }
    return true;
}

static bool requireDouble(int& i, int argc, char** argv, double& out, const char* flag) {
    if (++i >= argc || !parseDouble(argv[i], out)) {
        std::cerr << "Option " << flag << " needs a number"
                  << (i < argc ? std::string(" (got '") + argv[i] + "')" : "") << ".\n";
        return false;
    }
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
            if (!requireInt(i, argc, argv, opts.width, arg.c_str())) {
                return 1;
            }
            continue;
        }
        if (arg == "--height" || arg == "-h") {
            if (!requireInt(i, argc, argv, opts.height, arg.c_str())) {
                return 1;
            }
            continue;
        }
        if (arg == "--aspect") {
            if (!requireDouble(i, argc, argv, opts.aspect, arg.c_str())) {
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
            if (!parseVec3(i, argc, argv, *target, arg.c_str())) {
                printUsage(argv[0]);
                return 1;
            }
            continue;
        }
        if (arg == "--vfov") {
            if (!requireDouble(i, argc, argv, opts.vfov, arg.c_str())) {
                return 1;
            }
            continue;
        }
        if (arg == "--aperture") {
            if (!requireDouble(i, argc, argv, opts.aperture, arg.c_str())) {
                return 1;
            }
            continue;
        }
        if (arg == "--focus-dist") {
            if (!requireDouble(i, argc, argv, opts.focusDist, arg.c_str())) {
                return 1;
            }
            continue;
        }
        if (arg == "--samples" || arg == "-s") {
            if (!requireInt(i, argc, argv, opts.samples, arg.c_str())) {
                return 1;
            }
            continue;
        }
        if (arg == "--depth" || arg == "-d") {
            if (!requireInt(i, argc, argv, opts.depth, arg.c_str())) {
                return 1;
            }
            continue;
        }
        if (arg == "--display-width") {
            if (!requireInt(i, argc, argv, opts.displayWidth, arg.c_str())) {
                return 1;
            }
            continue;
        }
        if (arg == "--display-height") {
            if (!requireInt(i, argc, argv, opts.displayHeight, arg.c_str())) {
                return 1;
            }
            continue;
        }

        if (arg == "--gamma") {
            if (!requireDouble(i, argc, argv, opts.gamma, arg.c_str())) {
                return 1;
            }
            continue;
        }

        if (arg == "--fullscreen") {
            opts.fullscreen = true;
            continue;
        }

        if (arg == "--fps") {
            opts.showFps = true;
            continue;
        }

        std::cerr << "Unknown option: " << arg << "\n";
        printUsage(argv[0]);
        return 1;
    }

    if (opts.width <= 0 || opts.samples <= 0 || opts.depth <= 0 || opts.aspect <= 0 || opts.gamma <= 0) {
        std::cerr << "width, samples, depth, aspect, and gamma must be positive.\n";
        return 1;
    }
    if (opts.height <= 0) {
        opts.height = static_cast<int>(opts.width / opts.aspect);
        if (opts.height <= 0) {
            std::cerr << "Computed height must be positive; check width and aspect.\n";
            return 1;
        }
    }

    if (opts.displayWidth < 0 || opts.displayHeight < 0) {
        std::cerr << "display-width and display-height must be non-negative.\n";
        return 1;
    }
    if (!opts.fullscreen) {
        if (opts.displayWidth == 0 && opts.displayHeight == 0) {
            opts.displayWidth = opts.width;
            opts.displayHeight = opts.height;
        } else if (opts.displayWidth == 0) {
            opts.displayWidth = static_cast<int>(
                opts.displayHeight * static_cast<double>(opts.width) / opts.height);
        } else if (opts.displayHeight == 0) {
            opts.displayHeight = static_cast<int>(
                opts.displayWidth * static_cast<double>(opts.height) / opts.width);
        }
        if (opts.displayWidth <= 0 || opts.displayHeight <= 0) {
            std::cerr << "display-width and display-height must be positive.\n";
            return 1;
        }
    }

    return 0;
}
