#ifndef BUFFERSOURCE_H
#define BUFFERSOURCE_H

#include "videosource.h"
#include "veeventlistener.h"
#include "veevent.h"

/**
   Reads images from a buffer that is supplied on creation.
*/
class BufferSource :
    public VideoSource, public VeEventListener {

public:
    /**
       Creates a new BufferSource. 

       @param width,height Dimension of the images in pixels.
       @param frameCount Number of images in the buffer
       @param pixBytes Number of bytes per pixel and plane
       @param planes Number of color planes
       @param buffers pointer to the data pointers.
       */
    BufferSource(int width, int height, int frameCount, int pixBytes, int planes, void** buffers);
    ~BufferSource(void);

    void recieveEvent(VeEvent &e);
private:
    /// Pointer to the data
    void** buffers;
    /// Number of frames in buffer
    int frameCount;
    /// Bytes per pixel and plane
    int pixBytes;
    /// Number of color planes
    int planes;
    /// Current image
    int curImg;
    /// Copies a buffer of bytes
    static char* copyBuffer(char* buffer, int size);
    /**
    	Transforms  a 16-bit 1-plane buffer into a 8-bit 3-plane buffer
    	@param size Size in shorts
    */
    static char* transformBuffer(short* buffer, int size);
};

#endif
