#include "image_utils.h"
#include "assert.h"
#include <algorithm>

using std::min;

ofImage image_utils::raw_to_of(vector<vector<Color>> image){
    return ofImage(raw_to_ofpix(image));
}

ofPixels image_utils::raw_to_ofpix(vector<vector<Color>> image){
    const int height = image.size();
    const int width = image[0].size();
    ofPixels pixels;
    pixels.allocate(width, height, OF_IMAGE_COLOR);
    for(int i=0; i<height; i++){
        for(int j=0; j<width; j++){
            ofColor color(image[i][j].r, image[i][j].g, image[i][j].b);
            pixels.setColor(j,i,color);
        }
    }
    return pixels;
}

vector<vector<Color>> image_utils::of_to_raw(ofImage image){
    const int height = image.getHeight();
    const int width = image.getWidth();
    vector<vector<Color>> rv;
    ofPixels pixels = image.getPixels();
    for(int i=0; i<height; i++){
        vector<Color> temp;
        for(int j=0; j<width; j++){
            ofColor of_c = pixels.getColor(j,i);
            Color c(of_c.r, of_c.g, of_c.b);
            temp.push_back(c);
        }
        rv.push_back(temp);
    }
    return rv;
}
