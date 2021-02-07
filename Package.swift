// swift-tools-version:5.3
import PackageDescription

let package = Package(
    name: "CLifeHash",
    platforms: [
        .iOS(.v13), .macOS(.v11), .tvOS(.v13)
    ],
    products: [
        .library(name: "CLifeHash", targets: ["CLifeHash"])
    ],
    dependencies: [
    ],
    targets: [
        .target(
            name: "CLifeHash", 
            dependencies: [],
            path: "src",
            exclude: [
                "gradients.hpp",
                "grid.hpp",
                "color.hpp",
                "cell-grid.hpp",
                "size.hpp",
                "point.hpp",
                "sha256.hpp",
                "version.hpp",
                "memzero.hpp",
                "color-func.hpp",
                "format-utils.hpp",
                "frac-grid.hpp",
                "change-grid.hpp",
                "Makefile.in",
                "bit-enumerator.hpp",
                "hsb-color.hpp",
                "color-grid.hpp",
                "bit-aggregator.hpp",
                "data.hpp",
                "numeric.hpp",
                "patterns.hpp",
                "lifehash.hpp"
            ],
            publicHeadersPath: "SwiftPublicHeaders"
        )
    ],
    cxxLanguageStandard: .cxx11
)
