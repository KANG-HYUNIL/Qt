#ifndef ENUMS_H
#define ENUMS_H

//mouse status type
enum class MOUSE_EVENT_TYPE{
    NONE, //nothing
    CREATING_CHANGE_SIZE, //change size when create object
    RUNTIME_CHANGE_SIZE, //change size when running
    RUNTIME_CREATE_MAGPOINT, //create line when running
    RUNTIME_CHANGE_POS //change position when running
};


enum class ORIENTION {
    NONE = 0,
    NORTHWEST = 1,
    NORTH,
    NORTHEAST,
    EAST,
    SOUTHEAST,
    SOUTH,
    SOUTHWEST,
    WEST,
    STARTPOINT = 9,
    ENDPOINT = 10
};

enum class LINE_HEAD_TYPE {
    ARROW0 = 0,
    ARROW1,
    ARROW2,
    ARROW3
};

enum class PaintChartType {
    NONE,
    LINE,
};


#endif // ENUMS_H


