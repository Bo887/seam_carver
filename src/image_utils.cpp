#include "image_utils.h"

ofImage ImageUtils::raw_to_of(vector<vector<Color>> image){
    const int height = image.size();
    const int width = image[0].size();
    ofPixels pixels;
    pixels.allocate(width, height, OF_IMAGE_COLOR);
    for(int i=0; i<height; i++){
        for(int j=0; j<width; j++){
            ofColor color(image[i][j].r, image[i][j].g, image[i][j].b);
            pixels.setColor(i,j,color);
        }
    }
    return ofImage(pixels);
}

vector<vector<Color>> ImageUtils::of_to_raw(ofImage image){
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

ofImage ImageUtils::draw_seams(vector<vector<Color>> image, vector<vector<int>> h_seams, vector<vector<int>> v_seams){
    int height = image.size();
    int width = image[0].size();
    for(auto seam : h_seams){
        assert(seam.size() == width);
    }
    for(auto seam : v_seams){
        assert(seam.size() == height);
    }
    for(auto seam : h_seams){
        for(int col=0; col<width; col++){
            int row = seam[col];
            image[row][col] = Color(255, 0, 0);
        }
    }
    for(auto seam : v_seams){
        for(int row=0; row<height; row++){
            int col = seam[row];
            image[row][col] = Color(255, 0, 0);
        }
    }
    return raw_to_of(image);
}
