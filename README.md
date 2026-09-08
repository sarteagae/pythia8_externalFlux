# Pythia 8 External Photon Flux

This repository contains modifications to **Pythia 8.317** to implement an external photon flux for ultraperipheral heavy-ion collisions using a tabulated Pb photon flux.

The external photon flux is provided through a numerical table and incorporated into the Pythia photon-emission/kinematics machinery.

## Repository structure

```text
pythia8_externalFlux/
├── include/
│   ├── Pythia8/
│   │   └── PartonDistributions.h
│   └── flux/
│       └── PbFluxFromTable.h
├── src/
│   ├── PartonDistributions.cc
│   ├── GammaKinematics.cc
│   └── PhaseSpace.cc
├── share/
│   └── Pythia8/
│       └── xmldoc/
│           └── PDFSelection.xml
├── examples/
│   ├── main01.cc
│   ├── test03.cc
│   └── data/
│       └── pb_flux/
│           └── pb_xgamma_spline.dat
└── README.md
```

## External Pb photon flux

The file

```text
examples/data/pb_flux/pb_xgamma_spline.dat
```

contains the tabulated Pb photon-flux information used by the implementation.

The header

```text
include/flux/PbFluxFromTable.h
```

provides the interface for reading and using the tabulated flux.

## Modified Pythia files

The external-flux implementation requires modifications to selected Pythia 8 files:

* `include/Pythia8/PartonDistributions.h`
* `src/PartonDistributions.cc`
* `src/GammaKinematics.cc`
* `src/PhaseSpace.cc`
* `share/Pythia8/xmldoc/PDFSelection.xml`

These files should be applied to a compatible **Pythia 8.317** installation.

## Examples

The `examples/` directory contains the relevant example programs:

* `main01.cc`
* `test03.cc` JPsi->mumu using vmd model

The `main01.cc` example is included to demonstrate the external photon-flux implementation.

## Requirements

* Pythia 8.317
* A C++ compiler compatible with Pythia 8.317
* Standard build tools used by Pythia

## Notes

This repository contains only the files associated with the external photon-flux modification and selected examples/data. It does not contain a complete Pythia installation or build products.

The original Pythia 8 source code and documentation are available from the Pythia project.

