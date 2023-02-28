# Parallel coordinates plugin [![Actions Status](https://github.com/hdps/ParallelCoordinatesPlugin/workflows/ParallelCoordinatesPlugin/badge.svg)](https://github.com/hdps/ParallelCoordinatesPlugin/actions)

A parallel coordinates plugin for the [hdps](https://github.com/hdps/core) based on the [d3.js](https://github.com/d3/d3) library [Parallel Coordinates](https://syntagmatic.github.io/parallel-coordinates/) from [here](https://github.com/syntagmatic/parallel-coordinates).

**Limitations**: limit of number of Dimensions * number of Points <= 7,000,000. Qt uses an internal json data representation to communicate between the c++ and js side, and this sets some data size limit for use here. This specific number is a bit arbitrary and does not correspond to the exact max Qt json size but it's pretty close. 


## TODO
Possible future features:
- Add interaction controls:
  - Make axis dragable (instead of brushing, since this does not work well at the same time when there are many axes)
    - This could be expanded to an automatic axis reordering
  - Several brush options
- Option to not show all axis ticks but just the min and max values for each axis
- Option to color lines based on values from one dimension
