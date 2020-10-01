# Parallel coordinates plugin

A parallel coordinates plugin for the [hdps](https://github.com/hdps/core). 

Based on the [d3.js](https://github.com/d3/d3) library [Parallel Coordinates](https://syntagmatic.github.io/parallel-coordinates/) from [here](https://github.com/syntagmatic/parallel-coordinates).

## TODO
Next steps:
- Resize the plot automatically when the window size changes
- Check if the webview can be used to drag something onto
- Add interaction controls:
  - Make axis dragable (instead of brushing, since this does not work well at the same time when there are many axes)
    - This could be expanded to an automatic axis reordering
  - Several brush options
- For many dimensions, don't print their names over the axis but just number them and provide a legend
- Option to not show all axis ticks but just the min and max values for each axis
- Option to color lines based on values from one dimension

## Known bugs
- After resizing, when plotting a new PC, the axis of the old one are still displayed