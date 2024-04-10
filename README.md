# Parallel coordinates plugin [![Actions Status](https://github.com/ManiVaultStudio/ParallelCoordinatesPlugin/workflows/ParallelCoordinatesPlugin/badge.svg)](https://github.com/ManiVaultStudio/ParallelCoordinatesPlugin/actions)

A parallel coordinates plugin for the [ManiVault](https://github.com/ManiVaultStudio/core) visual analytics framework based on [syntagmatic's parallel coordinates](https://github.com/syntagmatic/parallel-coordinates) library written in [d3.js](https://github.com/d3/d3).

<p align="middle">
  <img src="https://github.com/ManiVaultStudio/ParallelCoordinatesPlugin/assets/58806453/82833616-d2e1-4907-a421-52e4a223cc2a" align="middle" width="65%" />
  <img src="https://github.com/ManiVaultStudio/ParallelCoordinatesPlugin/assets/58806453/177bd5e1-96be-4b55-961f-f87c06cec443" align="middle" width="20%" /> </br>
  Parallel coordinates showing the  <a href="https://doi.org/10.24432/C5859H">Auto MPG</a> dataset with a manual selection in the "acceleration" dimension
</p>

## Interaction
- Selecting ranges in each dimension
- Selecting which dimensions to display
- Clamping the shown value range

This plugin does not implement all interactions provided by the underlying library. Have a look [at it's docs](https://syntagmatic.github.io/parallel-coordinates/) for a full set of features.

## Limitations
Qt uses an internal JSON data representation to communicate between the C++ (ManiVault) and JS side (this view plugin), and this introduces some data size limits. 
The maximum number of elements (number of dimensions * number of data points) that the current implementation can handle is roughly the maximum size of a Qt JSON array. 
You're on the safe side with fewer than 7,000,000 elements. 
