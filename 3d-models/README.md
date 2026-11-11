# 3D-printable parts

The HoloShutter mechanical assembly is supplied as five separate STL meshes. Download the original STL files from the project maintainer's distribution package and add them to this directory when publishing a release.

## Model inventory

| STL file | Approx. dimensions (X × Y × Z) | Triangles | Mesh check |
|---|---:|---:|---|
| `shutter leaf 1.0.stl` | 70.00 × 105.45 × 3.40 mm | 3,880 | Watertight |
| `shutter assembly mount 1.0.stl` | 35.73 × 132.90 × 151.69 mm | 6,712 | Watertight |
| `Leaf_gear(1).stl` | 17.60 × 17.60 × 3.40 mm | 3,038 | Watertight |
| `Base - 175 mm.stl` | 175.00 × 140.00 × 16.00 mm | 1,940 | Watertight |
| `base-knob.stl` | 58.00 × 58.00 × 15.00 mm | 12,760 | Watertight |

Dimensions are the bounding-box extents measured from the uploaded STL meshes. Confirm units and scale in your slicer before printing.

## Files and printing notes

- These are separate part files, not a single fused assembly.
- Keep the original meshes as individual STLs so each component can be oriented and sliced independently.
- The correct material, infill, wall count, supports, tolerances, and print orientation have not yet been validated; choose settings based on your printer and mechanical fit.
- Test-fit the printed mechanism with the laser disabled before installation.
- The servo-driven shutter is not a certified laser safety shutter or interlock.

## Source files

The five source meshes were provided by the project creator. A ZIP package containing the original files and a JSON mesh manifest is available in the ChatGPT project handoff. The binary STL assets still need to be uploaded to this GitHub directory to make them directly downloadable from GitHub.
