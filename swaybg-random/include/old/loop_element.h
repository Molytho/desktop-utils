#ifndef BUILD_LOOP_ELEMENT_H
#define BUILD_LOOP_ELEMENT_H

class LoopElement {
public:
    virtual void handle_ready(int events) = 0;
    virtual int get_fd() = 0;
    virtual short get_mask() = 0;
};

#endif //BUILD_LOOP_ELEMENT_H